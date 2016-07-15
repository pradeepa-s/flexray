/**
 * @file sdcard_interface.c
 * @author Pradeepa Senanayake (pradeepa.kck@gmail.com)
 * @date 31 Jan 2016
 * @brief SDCard API function implementation
 *
 * This is a specific implementation which exposes more
 * higher level abstraction of low level sd card file
 * manipulations.
 *
 * Always use @@SDCardIF_Initialize function to initialize
 * the library before use.
 *
 * All the function comments has indepth details of each
 * function operations.
 *
 * Version v1.0.0
 */

#include "sdcard_interface.h"
#include "plat_arm.h"
#include <stdio.h>
#include <string.h>

STATIC char sdcardif_initialized = FALSE;

STATIC char *audio_buffer = NULL;
STATIC int audio_buffer_size = 0;

STATIC char event_log_file[MAX_FILENAME_SIZE];

static char IsNotInitialized();
static void DecodeEvents(ITSI_LOG_EVENT *event, char *line_buffer);

/**
 * @brief Function initializes the sdcard API
 *
 * @return None
 * @note Make sure to call this function before calling any other API functions. 
 * 
 * @return	SDCARD_IF_OP_SUCCESS				-	Success
 * @return 	SDCARD_IF_ERR_NOT_INITIALIZED		-	Init function has not been called
 * @return 	SDCARD_IF_ERR_INVALID_PARAM			-	Invalid input parameter 
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE	-	Failed to create  new file 
 * @return	Refer to sdcard_err_codes.h
 */

int SDCardIF_Initialize()
{
	int ret = SDCARD_IF_OP_SUCCESS;
		
	audio_buffer = NULL;
	audio_buffer_size = 0;
	
	ret = FileIF_Initialize();
	
	if(SDCARD_IF_OP_SUCCESS == ret){
		sdcardif_initialized = TRUE;
		
		ret = SDCardIF_SetLogFile(DEFAULT_EVENT_LOG);
	}

	if(SDCARD_IF_OP_SUCCESS != ret){
		sdcardif_initialized = FALSE;
	}
	
	return ret;
}


/**
 * @brief Function to set the buffer used by the audio file.
 * 
 * @param buffer[out] 	Pointer to an allocated buffer
 * @param buf_size[in] 	Size of the allocated buffer
 * @return 	SDCARD_IF_OP_SUCCESS			Operation success
 * 			SDCARD_IF_ERR_NOT_INITIALIZED	Init function not called
 * 			SDCARD_IF_ERR_INVALID_PARAM		Invalid parameters
 */

int SDCardIF_SetAudioFileBuffer(char *buffer, int buf_size)
{
	int ret = SDCARD_IF_OP_SUCCESS;	
	
	if(IsNotInitialized()){
		ret = SDCARD_IF_ERR_NOT_INITIALIZED;
	}
	else if((NULL == buffer)){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else if(buf_size <= 0){
		ret = SDCARD_IF_ERR_INVALID_BUFFER_SIZE;
	}
	else{
		audio_buffer = buffer;
		audio_buffer_size = buf_size;
	}
	
	return ret;
}

/**
 * @brief Function to read audio file from SDCARD to the audio file buffer
 * 
 * @param filename[in] 	Filename of the audio file
 * @return 	SDCARD_IF_OP_SUCCESS			Operation success 
 * @return	Refer to sdcard_err_codes.h
 */
int SDCardIF_PlayAudioFile(const char *filename)
{
	int ret = SDCARD_IF_OP_SUCCESS;
	int amount_read = audio_buffer_size;
	int file_size;
	
	if(IsNotInitialized()){
		ret = SDCARD_IF_ERR_NOT_INITIALIZED;
	}
	else if((NULL == audio_buffer) || (0 == audio_buffer_size)){
		ret = SDCARD_IF_ERR_AUDIO_BUFFER_NOT_SET;
	}
	else{
		ret = FileIF_CopyFileToBuffer(filename, 0, audio_buffer, &amount_read, &file_size);
	}
	
	return ret;
}

/**
 * @brief Function to set the current log file
 * 
 * If file is not available, an empty file will be created
 * 
 * @param filename[in] 	Filename of the log file
 * 
 * @return 	SDCARD_IF_OP_SUCCESS				Operation success 
 * @return 	SDCARD_IF_ERR_NOT_INITIALIZED		Init function has not been called
 * @return 	SDCARD_IF_ERR_INVALID_PARAM			Invalid input parameter 
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE	Failed to create  new file 
 * @return	Refer to sdcard_err_codes.h
 * 
 * @note None
 * @warning Maximum filename size is MAX_FILENAME_SIZE
 */

int SDCardIF_SetLogFile(const char* filename)
{
	int ret = SDCARD_IF_OP_SUCCESS;
	
	if(IsNotInitialized()){
		ret = SDCARD_IF_ERR_NOT_INITIALIZED;
	}	
	else if(NULL == filename){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else if(strlen(filename) > MAX_FILENAME_SIZE){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else{
		ret = FileIF_IsFileAvailable(filename);
		
		if(FILEIF_ERR_FILE_NOT_AVAILABLE == ret){
			ret = FileIF_CreateFile(filename);
		}
		
		memset(event_log_file,0x00,strlen(event_log_file));
		memcpy(event_log_file, filename, strlen(filename));
	}
	
	return ret;
}

/**
 * @brief Function to set delete the current log file
 * 
 * If the currently selected log file is deleted, the selected log file
 * will be changed back to the default value DEFAULT_EVENT_LOG.
 * 
 * @param filename[in] 	Filename of the log file
 * 
 * @return 	SDCARD_IF_OP_SUCCESS				Operation success
 * @return 	SDCARD_IF_ERR_INVALID_PARAM			Invalid input parameter 
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE	Failed to delete the file 
 * @return	Refer to sdcard_err_codes.h
 * 
 * @note No need to initialize the API to use this.
 * @warning Maximum filename size is MAX_FILENAME_SIZE
 */
int SDCardIF_DeleteLogFile(const char* filename)
{
	int ret = 0;
	
	if(NULL == filename){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else if(FILEIF_ERR_FILE_NOT_AVAILABLE == FileIF_IsFileAvailable(filename)){
		ret = SDCARD_IF_ERR_FILE_NOT_AVAILABLE;
	}
	else{
		ret = FileIF_DeleteFile(filename);
		
		if((FILEIF_OP_SUCCESS == ret) && (0 == strcmp(filename, event_log_file))){
			memset(event_log_file, 0x00, sizeof(event_log_file));
			memcpy(event_log_file, DEFAULT_EVENT_LOG, strlen(DEFAULT_EVENT_LOG));
		}
	}
	
	return ret;
}

/**
 * @brief Function to get the current log file name
 * 
 * There can be multiple log files in the system. This API can be used to
 * check what is the current log file in use. 
 * 
 * @param filename[in] 				Buffer to copy the filename
 * @param filename_size[inout] 	Buffer size
 * 
 * @return 	SDCARD_IF_OP_SUCCESS				Operation success
 * @return 	SDCARD_IF_ERR_NOT_INITIALIZED		Init function was not called
 * @return 	SDCARD_IF_ERR_INVALID_PARAM			Invalid input parameter/buffer size  
 * @return	Refer to sdcard_err_codes.h
 * 
 * @see SDCardIF_SetLogFile
 * 
 * @note 	Init function must be called before calling this function.
 * 			It is advisable to allocate MAX_FILENAME_SIZE for buf_size to
 * 			store the filename.
 * 
 * @warning None
 */
int SDCardIF_GetCurrentLogFile(char *filename, int *filename_size)
{
	int ret;
	
	if(IsNotInitialized()){
		ret = SDCARD_IF_ERR_NOT_INITIALIZED;
	}
	else if(NULL == filename){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else if((*filename_size < strlen(event_log_file))|| (*filename_size <= 0)) {
		ret = SDCARD_IF_ERR_INVALID_BUFFER_SIZE;
	}
	else{
		memset(filename, 0x00, *filename_size);
		memcpy(filename, event_log_file, strlen(event_log_file));
		ret = SDCARD_IF_OP_SUCCESS;
	}
	
	return ret;
}


/**
 * @brief Function to read a firmware file to a buffer
 * 
 * Firmware file needs to be read from the SD CARD to a buffer provided
 * by the firmware developer. This function is designed to facilitate 
 * that task.
 * 
 * Function can use @offset parameter to apply an offset to the start 
 * location of the firmware. 
 * 
 * @param filename[in] 				Firmware file name
 * @param offset[in] 				Offset to start reading
 * @param buffer[out] 				User allocated buffer
 * @param buf_size[inout] 			Buffer size as input, amount copied as output
 * @param file_size[out] 			Size of the filename
 * 
 * @return 	SDCARD_IF_OP_SUCCESS				Operation success
 * @return 	SDCARD_IF_ERR_INVALID_BUFFER_SIZE	Buffer size is zero or negetive 
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE	File is not accessible
 * @return 	SDCARD_IF_ERR_INVALID_PARAM			Invalid input parameter
 * @return 	SDCARD_IF_ERR_BUFFER_OFFSET			Offset is negetive or more than filesize
 * @return 	SDCARD_IF_WARN_BUFFER_SIZE_SMALL	Warning buffer size is smaller than filesize
 * @return 	SDCARD_IF_WARN_BUFFER_SIZE_LARGE	Warning buffer size is larger than filesize
 * @return	Refer to sdcard_err_codes.h
 * 
 * 
 * @note 	If a offset is provided data copying will start after applying
 * 			the specified amount of offset to the data in the firmware 
 * 			file
 * 
 * @note	If the @filename and @file_size parameters are valid and the file
 * 			is available. The @file_size will contain the size of the file
 * 			irrespective of the validity of other files.
 * 
 * @warning Do not treat warnings as errors.
 */
int SDCardIF_ReadFirmwareFile(char *filename, int offset, char *buffer, int *buf_size, int *file_size)
{
	int ret;	
	
	/* Read file size */
	ret = FileIF_GetFileSize(filename, file_size);
	
	if((NULL == buffer) || (NULL == buf_size)){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else if(*buf_size <= 0){
		ret = SDCARD_IF_ERR_INVALID_BUFFER_SIZE;		
	}
	else if(offset < 0){
		ret = SDCARD_IF_ERR_BUFFER_OFFSET;
	}
	else{
		ret = FileIF_CopyFileToBuffer(filename, offset, buffer, buf_size, file_size);
	}
	
	return ret;
}

/**
 * @brief Function to log an event to the file
 * 
 * Function will take the @event and log it in a new line in the 
 * log file set by @SDCardIF_SetLogFile API. 
 * 
 * @param event[in]		Pointer to the event structure whcih holds the event
 * 
 * @return 	SDCARD_IF_OP_SUCCESS				Operation success
 * @return 	SDCARD_IF_ERR_NOT_INITIALIZED		Init function has not been called
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE	File is not accessible
 * @return 	SDCARD_IF_ERR_INVALID_PARAM			Invalid input parameter
 * @return	Refer to sdcard_err_codes.h
 * 
 * @see 	SDCardIF_SetLogFile
 * @see 	SDCardIF_Initialize
 * 
 * @note 	@SDCardIF_Initialize function must be called prior to 
 * 			using this API.
 */
int SDCardIF_LogEvent(ITSI_LOG_EVENT *event)
{
	int ret = SDCARD_IF_OP_SUCCESS;
	char value[100];
	
	if(IsNotInitialized()){
		ret = SDCARD_IF_ERR_NOT_INITIALIZED;
	}
	else if(NULL == event){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else{
		memset(value, 0x00, sizeof(value));
		
		sprintf(value, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",\
							event->length, 
							event->day,
							event->month,
							event->year,
							event->hour,
							event->minute,
							event->second,
							event->id3,
							event->id2,
							event->id1,
							event->event_no,
							event->crc_msb,
							event->crc_lsb);
							
		ret = FileIF_AppendString(event_log_file,value);
						
	}
	
	return ret;
}

/**
 * @brief Function to read event log
 * 
 * Function will read the specified event log file and provide the
 * available events to the user. 
 * 
 * @param filename[in]			Filename of the log file
 * @param event[out]			Buffer to store the events
 * @param read_type[in]			Can be LAST_100, N_FROM_BEGINING, N_FROM_LAST or FULL_READ
 * @param no_of_events[inout]	No of event that can be stored in the buffer
 * @param offset[in]			Offset used for N_FROM_LAST and N_FROM_BEGINING
 * 
 * @return 	SDCARD_IF_OP_SUCCESS					Operation success
 * @return 	SDCARD_IF_ERR_INVALID_PARAM				Invalid input parameters 
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE		File is not accessible 
 * @return 	SDCARD_IF_ERR_EVENT_COUNT				Provided @no_of_events parameter is invalid
 * @return 	SDCARD_IF_ERR_EVENT_NOT_FOUND			Error in fetching the event 
 * @return 	SDCARD_IF_ERR_BUFFER_OFFSET				Offset used for reading is invalid
 * @return 	SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL	Required no of events is more than available
 * @return	Refer to sdcard_err_codes.h
 * 
 * @see 	SDCardIF_LogEvent
 * 
 * @note 	When the SDCARD_IF_ERR_EVENT_COUNT is returned, refer to the 
 * 			@no_of_events parameter to find out the available number of events.
 * 
 * @note	If required amount of events are not available, whatever 
 * 			available data will be copied to the buffer, subsequently
 * 			the return value will be SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL.
 * 
 * @note	Upon successful completion of the function the @no_of_events
 * 			parameter will contain the copied number of events.
 * 
 */

int SDCardIF_ReadEventLog(const char* filename, ITSI_LOG_EVENT *event, READ_TYPE read_type, int *no_of_events, int offset)
{
	int ret = SDCARD_IF_OP_SUCCESS;
	int event_count = 0;
	int event_count_total = 0;
	int expected_count = 0;
	int read_count = 0;
	int start_line = 1;
	int i = 0;	
	char line_buffer[64];
	int buffer_size = sizeof(line_buffer);
	
	/* Validate parameters */
	if((NULL == filename) || (NULL == no_of_events) || (NULL == event) || \
			((read_type < FULL_READ) || (read_type > N_FROM_LAST))){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}else{
		ret = FileIF_IsFileAvailable(filename);		
	}
		
	if(FILEIF_OP_SUCCESS == ret){
		
		/* Get the available events in the file */
		ret = FileIF_GetNoOfLines(filename, &event_count);										
				
		if(FILEIF_OP_SUCCESS == ret){
				
			event_count_total = event_count;
			
			/* Handle the offset parameter */		
			if((N_FROM_LAST == read_type) || (N_FROM_BEGINING == read_type)){
				
				/* For N_FROM_BEGINING and N_FROM_LAST verify bounds of offset */
				if((offset > event_count) || (offset < 0)){
					ret = SDCARD_IF_ERR_BUFFER_OFFSET;
				}			
				else{
					/* Because of offset event_count will decrease by that amount */
					event_count -= offset;
				}						
			}						
		}
		
		if(FILEIF_OP_SUCCESS == ret){	
		
			/* If buffer is not enough, return error */
			if(((FULL_READ == read_type) && (event_count > *no_of_events)) || \
					((0 >= *no_of_events) && (read_type != LAST_100))){
						
				*no_of_events = event_count;
				ret = SDCARD_IF_ERR_EVENT_COUNT;
			}
		}
		
		if(FILEIF_OP_SUCCESS == ret){				
			
			/* If LAST_100 is the read type we are expecting 100 events */
			if(LAST_100 == read_type){
				expected_count = 100;
			}
			else{
				expected_count = *no_of_events;
			}						
			
			/* Check whether required events are available */
			if(event_count < expected_count){				
				ret = SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL;
				read_count 	 = event_count;			
			}
			else{
				read_count = expected_count;						
			}
			
			/* Find the starting event */
			if(LAST_100 == read_type){
				start_line = event_count_total - read_count + 1;					
			}
			else if(N_FROM_LAST == read_type){
				start_line = event_count_total - read_count + 1 - offset;	
			}
			else if(N_FROM_BEGINING == read_type){
				start_line = 1 + offset;
			}
				
			/* Copy events to the structure */		
			for(i = 0; i<read_count; i++){
					
				buffer_size = sizeof(line_buffer);
				memset(line_buffer, 0x00, buffer_size);
								
				if(SDCARD_IF_OP_SUCCESS == FileIF_ReadLine(filename, i+start_line, line_buffer, &buffer_size)){
					DecodeEvents(&event[i], line_buffer);																				
				}
			}
			
			*no_of_events = read_count;
								
		}		
	}
	
	return ret;
}


/**
 * @brief Function to create an empty firmware file
 * 
 * This function creates an empty file provided that a file
 * with the same name does not exist in the provided location.
 * 
 * @param filename[in]			Filename of the firmware file
 * 
 * @return 	SDCARD_IF_OP_SUCCESS					Operation success
 * @return 	SDCARD_IF_ERR_INVALID_PARAM				Invalid input parameters 
 * @return 	SDCARD_IF_ERR_FILE_ACCESS				File creation failed
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE		File creation failed
 * @return	Refer to sdcard_err_codes.h
 * 
 * @note 	This function can work without initilizing the library.
 * 			ie: without calling @SDCardIF_Initialize
 */

int SDCardIF_CreateFirmwareFile(const char *filename)
{
	int ret = SDCARD_IF_OP_SUCCESS;
	
	if(NULL == filename){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	else if(SDCARD_IF_OP_SUCCESS == FileIF_IsFileAvailable(filename)){
		ret = SDCARD_IF_ERR_FILE_ACCESS;
	}
	else{
		ret = FileIF_CreateFile(filename);
	}
	
	return ret;
}

/**
 * @brief Function to delete a firmware file
 * 
 * This function delets an empty file. 
 * If the file is not there, the function will return SUCCESS.
 * 
 * @param filename[in]			Filename of the firmware file
 * 
 * @return 	SDCARD_IF_OP_SUCCESS					Operation success
 * @return 	SDCARD_IF_ERR_INVALID_PARAM				Invalid input parameters  
 * @return 	SDCARD_IF_ERR_FILE_NOT_AVAILABLE		File deletion failed
 * @return	Refer to sdcard_err_codes.h
 * 
 * @note 	This function can work without initilizing the library.
 * 			ie: without calling @SDCardIF_Initialize
 */

int SDCardIF_DeleteFirmwareFile(const char *filename)
{
	int ret = SDCARD_IF_OP_SUCCESS;
	
	if(NULL == filename){
		ret = SDCARD_IF_ERR_INVALID_PARAM;
	}	
	else{
		ret = FileIF_DeleteFile(filename);
		
		if(SDCARD_IF_ERR_FILE_NOT_AVAILABLE == ret){
			ret = SDCARD_IF_OP_SUCCESS;
		}
	}
	
	return ret;
}

/**
 * @brief Append the firmware data to a file
 * 
 * Append provided data to the firmware file
 * 
 * @param filename[in]			Filename of the firmware file
 * @param data[in]				Firmware data buffer
 * @param data_size[in]			Size of the data buffer
 * 
 * @return 	SDCARD_IF_OP_SUCCESS					Operation success
 * @return 	SDCARD_IF_ERR_INVALID_PARAM				Invalid input parameters  
 * @return	Refer to sdcard_err_codes.h
 * 
 */
 
int SDCardIF_AppendFirmwareData(const char *filename, char* data, int data_size)
{		
	int ret = SDCARD_IF_OP_SUCCESS;
	
	if(	(NULL == filename)	||
		(NULL == data)	||
		(data_size <= 0)){
			ret = SDCARD_IF_ERR_INVALID_PARAM;
	}
	
	if(SDCARD_IF_OP_SUCCESS == ret){
		ret = FileIF_IsFileAvailable(filename);
	}
	
	if(SDCARD_IF_OP_SUCCESS == ret){
		ret = FileIF_CopyBufferToFile(filename, data, data_size);
	}	
	
	return ret;
}


/**
 * @brief Function to reset all the static and global buffers
 * 
 */
void SDCardIF_Reset()
{
	sdcardif_initialized = FALSE;
	audio_buffer = NULL;
	audio_buffer_size = 0;	
	memset(event_log_file, 0x00, sizeof(event_log_file));
}

static char IsNotInitialized()
{
	return (FALSE == sdcardif_initialized);
}

static void DecodeEvents(ITSI_LOG_EVENT *event, char *line_buffer)
{
	int event_arr[13];
	/* potentially unsafe code. if string contains invalid characters */
										
	if(event && line_buffer){
		sscanf((const char*)line_buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					&event_arr[0],
					&event_arr[1],
					&event_arr[2],
					&event_arr[3],
					&event_arr[4],
					&event_arr[5],
					&event_arr[6],
					&event_arr[7],
					&event_arr[8],
					&event_arr[9],
					&event_arr[10],
					&event_arr[11],
					&event_arr[12]);
						
		event->length = event_arr[0];
		event->day = event_arr[1];
		event->month = event_arr[2];
		event->year = event_arr[3];
		event->hour = event_arr[4];
		event->minute = event_arr[5];
		event->second = event_arr[6];
		event->id3 = event_arr[7];
		event->id2 = event_arr[8];
		event->id1 = event_arr[9];
		event->event_no = event_arr[10];
		event->crc_msb = event_arr[11];
		event->crc_lsb = event_arr[12];	
	}
}
