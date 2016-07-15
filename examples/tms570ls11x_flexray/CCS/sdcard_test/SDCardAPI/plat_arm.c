/**
 * @file plat_arm.c
 * @author Pradeepa Senanayake (pradeepa.kck@gmail.com)
 * @date 31 Jan 2016
 * @brief File contains arm compiler specific file access implementation.
 *
 * This file contains generic functions to do file manipulation using
 * FAT file system version R0.11a.
 *
 * The file can be easyly ported to any compiler to include compiler
 * specific functions.
 *
 * Ex: Currently this file uses f_open, f_read type functions which
 * are implemented in ff.h and ff.c files. If the file needs to be
 * ported to different architecture/file system, the developer
 * only needs to modify those. Link to Fat Filesystem is available
 * below.
 *
 * @see http://elm-chan.org/fsw/ff/00index_e.html
 *
 * Version v1.0.0 
 */

#include <stdio.h>
#include <string.h>
#include "plat_arm.h"
#include "ff.h"
enum {INITIALIZED = 1, NOT_INITIALIZED = 0};

static char initialized = NOT_INITIALIZED;
static FATFS g_sFatFsObj;

static int ff_return_code_translate(int return_code);
static int CheckInitialization(void);
static int check_size(unsigned int utilization);
static int get_disk_utilization(unsigned int *utilization);

/**
 * @brief Function to initialize the file system access layer
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 */
int FileIF_Initialize(void)
{
	//
	// Mount the file system, using logical disk 0.
	//
	//  iFResult = f_mount(&g_sFatFs,"",1);


	int iFResult = f_mount(&g_sFatFsObj, "", 1);

	if (iFResult != FR_OK) {
		return (FILEIF_ERR_UNINIT);
	}
	else{
		initialized = INITIALIZED;
		return FILEIF_OP_SUCCESS;
	}
}

/**
 * @brief Function to open a file and copy the content to a buffer
 * 
 * Function will attempt to open the file with the given filename and copy
 * the content of the file to the buffer.
 * 
 * If the buffer size is more than file size, file content will be copied and 
 * the rest of the buffer will have 0x00 value.
 * 
 * If the buffer size is less than the file size, this will only copy the amount 
 * of data that can be accommodated by the buffer.
 * 
 * If an @offset is specified the data copying will start after applying the
 * offset to the file buffer.
 * 
 * If the function succeed the @buf_size parameter will contain the amount
 * of data copied.
 * 
 * If @filename and @file_size are valid, the @file_size parameter will 
 * contain the size of the pointed file when function returns.
 * 
 * @param filename[in] 		Filename to use
 * @param offset[in] 		Offset to apply to file data when start reading
 * @param buffer[out] 		Buffer pointer to copy data
 * @param buf_size[inout] 	Size of the buffer. Upon proper execution of the function the copied data amount will be saved in this variable pointer.
 * @param file_size[out]	Pointer to an integer which to get the pointed file size
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_BUFFER_SIZE			Invalid buffer size (zero or negative)
 * @return 	FILEIF_ERR_FILE_OFFSET			Offset is negative or more than file size
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be accessed
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * @return 	FILEIF_WARN_BUFFER_SIZE_SMALL	Provided buffer is small (warning)
 * @return 	FILEIF_WARN_BUFFER_SIZE_LARGE	Provided buffer is large (warning)
 * 
 * @note Do not treat warnings as errors
 *
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 * @warning Warning.
 */

int FileIF_CopyFileToBuffer(const char *filename, int offset, char *buffer, int *buf_size, int *file_size)
{
	int ret = FILEIF_OP_SUCCESS;
	int warn = FILEIF_OP_SUCCESS;
	int f_size = 0;
	int amount_to_read = 0;	
	UINT f_read_size = 0;
	FILE f;
	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	
	ret = FileIF_GetFileSize(filename, file_size);
	
	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	else if((NULL == buf_size) || (NULL == buffer)){
		ret = FILEIF_ERR_INVALID_PARAM;
	}
	else if(*buf_size <= 0) {
		ret = FILEIF_ERR_BUFFER_SIZE;
	}
	else if(offset < 0){
		ret = FILEIF_ERR_FILE_OFFSET;
	}
	else{
			
		FileIF_GetFileSize(filename, &f_size);
		*file_size = f_size;
		
		ret = f_open(&f, filename, FA_READ);
		
		if(ret == FR_OK){
			
			/* Set the buffer content to 0x00 */
			memset(buffer, 0x00, *buf_size);
			
			amount_to_read = *buf_size;
			
			/* Check the offset */
			if(offset > f_size){
				ret = FILEIF_ERR_FILE_OFFSET;
			}
			else{
										
				/* Check the buf size */
				if(f_size < *buf_size){
					warn = FILEIF_WARN_BUFFER_SIZE_LARGE;
					amount_to_read = f_size;			
				}
				else if(f_size > *buf_size){
					warn = FILEIF_WARN_BUFFER_SIZE_SMALL;
				}
				
				ret = f_lseek(&f, offset);
				
				if(FR_OK == ret){
#if 0				
					ret = f_read(&f, buffer, amount_to_read, &f_read_size);
					
					if(FR_OK == ret){
						*buf_size = f_read_size;
					}
				}
#else	

					int amount_read = 256;
					int final_read_size = 0;
					
					while(1){
						if(amount_to_read > 256){
							amount_read = 256;
						}
						else{
							amount_read = amount_to_read;
						}

						amount_to_read -= amount_read;

						ret = f_read(&f, buffer, amount_read, &f_read_size);

						buffer += amount_read;

						if(FR_OK == ret){
							final_read_size += f_read_size;
						}
						else{
							break;
						}

						if(0 == amount_to_read){
							break;
						}
					}

					*buf_size = final_read_size;
				}
#endif				
				f_close(&f);
			}
		}
		
		/* If file is not available translate  */
		if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
		else{
			ret = ff_return_code_translate(ret);
		}
	}
	
	if(FILEIF_OP_SUCCESS == ret){
		unsigned int uti;
		if(FILEIF_OP_SUCCESS == get_disk_utilization(&uti)){
			ret = check_size(uti);
		}
	}

	// Check whether a warning is there.
	if((FILEIF_OP_SUCCESS == ret) && (warn != FILEIF_OP_SUCCESS)){
		ret = warn;
	}

	return ret;
}

/**
 * @brief Function to check whether a file is present in the system
 * 
 * 
 * @param filename 	Filename 
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be accessed
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 *
 */
int FileIF_IsFileAvailable(const char *filename)
{
	int ret;
	FILE f;
	
	/* Check whether initialization is done */
	ret = CheckInitialization();
	
	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	if(NULL == filename){
		ret = FILEIF_ERR_INVALID_PARAM;
	}
	else{
		
		ret = f_open(&f,filename,FA_READ);
		
		if(ret == FR_OK){
			ret = FILEIF_OP_SUCCESS;	
			f_close(&f);
		}
		else{
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
	}
	
	return ret;
}

/**
 * @brief Function to create a new file in the system
 * 
 * Function will attempt to create a new file. If file is already available,
 * function will return success.
 * 
 * @param filename 	Filename 
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be accessed/created
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 
 * @note If the file is already available function will return success.
 */

int FileIF_CreateFile(const char *filename)
{
	int ret;
	FILE f;
	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	ret = FileIF_IsFileAvailable(filename);
	
	if(FILEIF_ERR_FILE_NOT_AVAILABLE == ret){
		ret = f_open(&f,filename, FA_WRITE | FA_OPEN_ALWAYS);
		
		if(ret == FR_OK){
			ret = f_lseek(&f,f_size(&f));

			if(ret == FR_OK){
				ret = FILEIF_OP_SUCCESS;
				f_close(&f);
			}
		}
		
		/* If file is not available translate  */
		if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
		else{
			ret = ff_return_code_translate(ret);
		}

	}

	if(FILEIF_OP_SUCCESS == ret){
		unsigned int uti;
		if(FILEIF_OP_SUCCESS == get_disk_utilization(&uti)){
			ret = check_size(uti);
		}
	}
	
	return ret;
}

/**
 * @brief Function to delete a file from the system
 * 
 * @param filename 	Filename 
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be found or cannot be deleted
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 * 
 * @warning None
 */
int FileIF_DeleteFile(const char *filename)
{		
	int ret;

	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}

	ret = FileIF_IsFileAvailable(filename);
	
	if(FILEIF_OP_SUCCESS == ret){	
		ret = f_unlink(filename);
		
		/* If file is not available translate  */
		if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
		else{
			ret = ff_return_code_translate(ret);
		}
	}
	
	return ret;
}

/**
 * @brief Function to delete a file from the system
 * 
 * @param filename[in] 		Filename 
 * @param file_size[out] 	Integer pointer to store the filesize 
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be found or cannot be accessed
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 * 
 * @warning None
 */
int FileIF_GetFileSize(const char *filename, int *file_size)
{
	int ret;
	FILE f;

	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	if(NULL == filename || NULL == file_size){
		ret = FILEIF_ERR_INVALID_PARAM;
	}
	else{

		ret = f_open(&f,filename, FA_READ);
			
		if(ret == FR_OK){
			/* Get the file size */
			*file_size = f_size(&f);
			
			ret = FILEIF_OP_SUCCESS;
		}
		
		/* If file is not available translate  */
		if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
		else{
			ret = ff_return_code_translate(ret);
		}
			
	}
	
	return ret;
}

/**
 * @brief Function to append a string to the end of a file
 * 
 * @param filename[in] 		Filename 
 * @param string[in] 		Null terminated string to append
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be found or cannot be accessed
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 * 
 * @warning None
 */
int FileIF_AppendString(const char *filename, const char *string)
{
	int ret = FILEIF_OP_SUCCESS;
	FILE f;
	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	if((NULL == filename) || (NULL == string)){
		ret = FILEIF_ERR_INVALID_PARAM;
	}
	else{
		ret = FileIF_IsFileAvailable(filename);
		
		if(FILEIF_OP_SUCCESS == ret){
			ret = f_open(&f, filename, FA_WRITE);

			if(FR_OK == ret){
				f_lseek(&f, f_size(&f));
				f_puts(string,&f);
				f_close(&f);
				ret = FILEIF_OP_SUCCESS;
			}
			
			/* If file is not available translate  */
			if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
				ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
			}
			else{
				ret = ff_return_code_translate(ret);
			}
		}			
	}

	if(FILEIF_OP_SUCCESS == ret){
		unsigned int uti;
		if(FILEIF_OP_SUCCESS == get_disk_utilization(&uti)){
			ret = check_size(uti);
		}
	}
	return ret;
}

/**
 * @brief Function to get number of lines in a file
 * 
 * This function can be used to get number of lines available in a
 * text file.
 * 
 * @param filename[in] 		Filename 
 * @param no_of_lines[out] 	No of lines in the file
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be found or cannot be accessed
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 * 
 * @warning None
 */
int FileIF_GetNoOfLines(const char *filename,int *no_of_lines)
{
	int ret;
	FILE f;
	char ch[2];
	int line_count = 0;
	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}

	ret = FileIF_IsFileAvailable(filename);
	if(FILEIF_OP_SUCCESS == ret){
		if(NULL == no_of_lines){
			ret = FILEIF_ERR_INVALID_PARAM;
		}
		else{
			ret = f_open(&f,filename, FA_READ);
			if(ret == FR_OK){
				while(!f_eof(&f)){
					f_gets(ch,2,&f);
					if(ch[0] == '\n'){
						line_count++;
					}
				}
				
				f_close(&f);
				*no_of_lines = line_count;
			}
			
			/* If file is not available translate  */
			if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
				ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
			}
			else{
				ret = ff_return_code_translate(ret);
			}
		}		
	}
	
	return ret;
}

/**
 * @brief Function to read a line of text from a file
 * 
 * This function can be used to read a line of text from a file.
 * 
 * @param filename[in] 		Filename 
 * @param line_no[in] 		Line number to read
 * @param line_buffer[out] 	Buffer to store data
 * @param buf_size[inout] 	Buffer length
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be found or cannot be accessed
 * @return 	FILEIF_ERR_BUFFER_SIZE			Buffer size is invalid
 * @return 	FILEIF_ERR_LINE_NO				Line number is invalid
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 *
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 * @note 	If FILEIF_ERR_BUFFER_SIZE is returned then the @buf_size 
 * 			will contain the required data amount
 * 
 * @warning None
 */

int FileIF_ReadLine(const char *filename, int line_no, char *line_buffer, int *buf_size)
{
	int ret = FILEIF_OP_SUCCESS;
	FILE f;
	int line_index = 0;
	int line_length = 0;
	char line_found = 0;
	char temp_line[256];

	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}

	/* Parameter validation */
	if((NULL == filename) || (NULL == line_buffer) || (NULL == buf_size)){
		ret = FILEIF_ERR_INVALID_PARAM;
	}
	else if(line_no <= 0){
		ret = FILEIF_ERR_LINE_NO;
	}
	else{
		ret = FileIF_IsFileAvailable(filename);
	}
	
	if(FILEIF_OP_SUCCESS == ret){
		
		ret = f_open(&f,filename,FA_READ);
		
		if(ret == FR_OK){
			/* Search for the line */
			while(NULL != f_gets(temp_line, sizeof(temp_line), &f)){
				
				line_index++;				
				if(line_no == line_index){					
					line_found = 1;
					break;
				}				
			}
			
			if(line_found){
				/* Verify line size */
				memset(line_buffer, 0x00, *buf_size);
				
				line_length = strlen(temp_line);
				
				if(line_length <= *buf_size){
					/* Copy line content */					
					memcpy(line_buffer, temp_line, line_length);
				}
				else{					
					ret = FILEIF_ERR_BUFFER_SIZE;
				}
				
				/* Copy line size */
				*buf_size = line_length;
			}
			else{
				ret = FILEIF_ERR_LINE_NO;
			}			
			
			f_close(&f);
		}
		
		/* If file is not available translate  */
		if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
		else{
			ret = ff_return_code_translate(ret);
		}
	}
	
	return ret;
}


/**
 * @brief Function to copy buffer content to file
 * 
 * This function appends buffer content to a file 
 * 
 * @param filename[in] 		Filename 
 * @param buffer[in] 		Data to append
 * @param buf_size[in] 		Data amount
 * 
 * @return 	FILEIF_OP_SUCCESS				Operation success 
 * @return 	FILEIF_ERR_INVALID_PARAM		Function parameters are invalid
 * @return 	FILEIF_ERR_FILE_NOT_AVAILABLE	File cannot be found
 * @return 	FILEIF_ERR_FILE_ACCESS			File cannot be accessed
 * @return 	FILEIF_ERR_UNINIT				Failed to initialize
 * 
 * @note	If called in uninitialized state, the function internally will
 * 			initialize the library and continue.
 *
 * @warning None
 */

int FileIF_CopyBufferToFile(const char *filename, char *buffer, int buf_size)
{
	int ret = FILEIF_OP_SUCCESS;
	
	FILE f;
	unsigned int copied_amount = 0;
	/* Check whether initialization is done */
	ret = CheckInitialization();

	if(FILEIF_OP_SUCCESS != ret){
		return ret;
	}
	if(	(NULL == filename)	||
		(NULL == buffer)	||
		(buf_size <= 0)){
		ret = FILEIF_ERR_INVALID_PARAM;
	}
	else{
		ret = FileIF_IsFileAvailable(filename);
	}
	
	if(FILEIF_OP_SUCCESS == ret){
		ret = f_open(&f,filename,FA_READ | FA_WRITE);
		
		if(FR_OK == ret){
			
			// Go to the end
			ret = f_lseek(&f,f_size(&f));

			if(FR_OK == ret){
				ret = f_write(&f, buffer, buf_size, &copied_amount);

				if(FR_OK == ret){
					if(copied_amount != buf_size){
						ret = FILEIF_ERR_FILE_ACCESS;
					}
				}
			}
			
			f_close(&f);
		}
		
		/* If file is not available translate  */
		if((FR_NO_FILE == ret) || (FR_NO_PATH == ret)){
			ret = FILEIF_ERR_FILE_NOT_AVAILABLE;
		}
		else if(ret > 0){
			ret = ff_return_code_translate(ret);
		}
	}

	if(FILEIF_OP_SUCCESS == ret){
		unsigned int uti;
		if(FILEIF_OP_SUCCESS == get_disk_utilization(&uti)){
			ret = check_size(uti);
		}
	}

	return ret;
}

/**
 * @brief	Function to reset the states to original
 *
 * This function is a debug function which can make the FileIf to
 * move to uninitialized state.
 *
 * @param 	None
 *
 * @return 	None
 *
 */
void FileIF_Uninit(void)
{
	initialized = NOT_INITIALIZED;
}

/************************** Static functions **************************/

int CheckInitialization(void)
{
	int ret = FILEIF_OP_SUCCESS;

	if(NOT_INITIALIZED == initialized){
		ret = FileIF_Initialize();

		if(FILEIF_OP_SUCCESS != ret){
			return ret;
		}
	}

	return ret;
}

/**
 * @brief Translate return fat file system return codes
 * 
 * 
 * @param return_code[in] 		Input return code 
 * 
 * @return	Translated return code
 */

static int ff_return_code_translate(int return_code)
{
	int ret = FILEIF_OP_SUCCESS;
	
	switch(return_code){		
		case FR_OK:						/* (0) Succeeded */
			break;
		
		case FR_DISK_ERR:				/* (1) A hard error occurred in the low level disk I/O layer */
			ret = FILEIF_WARN_FF_DISK_ERROR;
			break;

		case FR_INT_ERR:				/* (2) Assertion failed */
			ret = FILEIF_WARN_FF_INTERNAL_ERROR;
			break;
					
		case FR_NOT_READY:				/* (3) The physical drive cannot work */
			ret = FILEIF_WARN_FF_NOT_READY;
			break;
			
		case FR_NO_FILE:				/* (4) Could not find the file */
			ret = FILEIF_WARN_FF_NO_FILE;
			break;
			
		case FR_NO_PATH:				/* (5) Could not find the path */
			ret = FILEIF_WARN_FF_NO_PATH;
			break;
			
		case FR_INVALID_NAME:			/* (6) The path name format is invalid */
			ret = FILEIF_WARN_FF_INVALID_NAME;
			break;
			
		case FR_DENIED:					/* (7) Access denied due to prohibited access or directory full */
			ret = FILEIF_WARN_FF_DENIED;
			break;
			
		case FR_EXIST:					/* (8) Access denied due to prohibited access */
			ret = FILEIF_WARN_FF_EXIST;
			break;
			
		case FR_INVALID_OBJECT:			/* (9) The file/directory object is invalid */
			ret = FILEIF_WARN_FF_INVALID_OBJECT;
			break;
			
		case FR_WRITE_PROTECTED:		/* (10) The physical drive is write protected */
			ret = FILEIF_WARN_FF_WRITE_PROTECTED;
			break;
			
		case FR_INVALID_DRIVE:			/* (11) The logical drive number is invalid */
			ret = FILEIF_WARN_FF_INVALID_DRIVE;
			break;
			
		case FR_NOT_ENABLED:			/* (12) The volume has no work area */
			ret = FILEIF_WARN_FF_NOT_ENABLED;
			break;
			
		case FR_NO_FILESYSTEM:			/* (13) There is no valid FAT volume */
			ret = FILEIF_WARN_FF_NO_FILESYSTEM;
			break;
			
		case FR_MKFS_ABORTED:			/* (14) The f_mkfs() aborted due to any parameter error */
			ret = FILEIF_WARN_FF_MKFS_ABORTED;
			break;
			
		case FR_TIMEOUT:				/* (15) Could not get a grant to access the volume within defined period */
			ret = FILEIF_WARN_FF_TIMEOUT;
			break;
			
		case FR_LOCKED:					/* (16) The operation is rejected according to the file sharing policy */
			ret = FILEIF_WARN_FF_LOCKED;
			break;
		
		case FR_NOT_ENOUGH_CORE:		/* (17) LFN working buffer could not be allocated */
			ret = FILEIF_WARN_FF_NOT_ENOUGH_CORE;
			break;
			
		case FILEIF_WARN_FF_TOO_MANY_OPEN_FILES:	/* (18) Number of open files > _FS_LOCK */
			ret = FILEIF_WARN_FF_DISK_ERROR;
			break;
			
		case FILEIF_WARN_FF_INVALID_PARAM:		/* (19) Given parameter is invalid */
			ret = FILEIF_WARN_FF_DISK_ERROR;
			break;
			
		default:
			ret = FILEIF_WARN_FF_UNKNOWN;
			break;
	}
	
	return ret;
}


static int get_disk_utilization(unsigned int *utilization)
{
	int ret = FILEIF_OP_SUCCESS;
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;


	/* Get volume information and free clusters of drive 1 */
	ret = f_getfree("/", &fre_clust, &fs);
	if (FR_OK == ret){
		/* Get total sectors and free sectors */
		tot_sect = (fs->n_fatent - 2) * fs->csize;
		fre_sect = fre_clust * fs->csize;

		if(utilization){
			*utilization = (100 * (tot_sect - fre_sect) / tot_sect);
		}
		else{
			ret = FILEIF_ERR_INVALID_PARAM;
		}
	}
	else{
		ret = ff_return_code_translate(ret);
	}

	return ret;
}


static int check_size(unsigned int utilization)
{
	if(utilization >= 80){
		return FILEIF_WARN_EIGHTY_PERCENT;
	}
	else{
		return FILEIF_OP_SUCCESS;
	}
}
