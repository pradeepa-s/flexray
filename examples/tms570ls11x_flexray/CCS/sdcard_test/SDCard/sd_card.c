//*****************************************************************************
//
// sd_card.c - Example program for reading files from an SD card.
//
// Copyright (c) 2011-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
//*****************************************************************************

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gio.h"
#include "uartstdio.h"
#include "sci.h"
#include "ff.h"
#include "diskio.h"
#include "cmdline.h"
#include "sd_defs.h"
#include "sdcard_interface.h"
#include "plat_arm.h"

extern void disk_timerproc (void);

static char g_audio_buffer[50000];
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>SD card using FAT file system (sd_card)</h1>
//!
//! This example application demonstrates reading a file system from an SD
//! card.  It makes use of FatFs, a FAT file system driver.  It provides a
//! simple command console via a serial port for issuing commands to view and
//! navigate the file system on the SD card.
//!
//! The first UART, which is connected to the USB debug virtual serial port on
//! the evaluation board, is configured for 115,200 bits per second, and 8-N-1
//! mode.  When the program is started a message will be printed to the
//! terminal.  Type ``help'' for command help.
//!
//! For additional details about FatFs, see the following site:
//! http://elm-chan.org/fsw/ff/00index_e.html
//
//*****************************************************************************

#ifdef LCD
extern int Cmd_load(int argc, char *argv[]);
#endif

//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code, and a
// string representation.  FRESULT codes are returned from the FatFs FAT file
// system driver.
//
//*****************************************************************************
typedef struct {
    FRESULT iFResult;
    char *pcResultStr;
} tFResultString;

//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and it's
// name as a string.  This is used for looking up error codes for printing to
// the console.
//
//*****************************************************************************
tFResultString g_psFResultStrings[] = {
FRESULT_ENTRY(FR_OK),
FRESULT_ENTRY(FR_DISK_ERR),
FRESULT_ENTRY(FR_INT_ERR),
FRESULT_ENTRY(FR_NOT_READY),
FRESULT_ENTRY(FR_NO_FILE),
FRESULT_ENTRY(FR_NO_PATH),
FRESULT_ENTRY(FR_INVALID_NAME),
FRESULT_ENTRY(FR_DENIED),
FRESULT_ENTRY(FR_EXIST),
FRESULT_ENTRY(FR_INVALID_OBJECT),
FRESULT_ENTRY(FR_WRITE_PROTECTED),
FRESULT_ENTRY(FR_INVALID_DRIVE),
FRESULT_ENTRY(FR_NOT_ENABLED),
FRESULT_ENTRY(FR_NO_FILESYSTEM),
FRESULT_ENTRY(FR_MKFS_ABORTED),
FRESULT_ENTRY(FR_TIMEOUT),
FRESULT_ENTRY(FR_LOCKED),
FRESULT_ENTRY(FR_NOT_ENOUGH_CORE),
FRESULT_ENTRY(FR_TOO_MANY_OPEN_FILES),
FRESULT_ENTRY(FR_INVALID_PARAMETER), };

//*****************************************************************************
//
// A macro that holds the number of result codes.
//
//*****************************************************************************
#define NUM_FRESULT_CODES       (sizeof(g_psFResultStrings) /                 \
                                 sizeof(tFResultString))

//*****************************************************************************
//
// This function returns a string representation of an error code that was
// returned from a function call to FatFs.  It can be used for printing human
// readable error messages.
//
//*****************************************************************************
const char *
StringFromFResult(FRESULT iFResult) {
    uint_fast8_t ui8Idx;

    //
    // Enter a loop to search the error code table for a matching error code.
    //
    for (ui8Idx = 0; ui8Idx < NUM_FRESULT_CODES; ui8Idx++) {
        //
        // If a match is found, then return the string name of the error code.
        //
        if (g_psFResultStrings[ui8Idx].iFResult == iFResult) {
            return (g_psFResultStrings[ui8Idx].pcResultStr);
        }
    }

    //
    // At this point no matching code was found, so return a string indicating
    // an unknown error.
    //
    return ("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a timer tick
// every 10 ms for internal timing purposes.
//
//*****************************************************************************
void SysTickHandler(void) {  // todo: implement
    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();
}

//*****************************************************************************
//
// This function implements the "ls" command.  It opens the current directory
// and enumerates through the contents, and prints a line for each item it
// finds.  It shows details such as file attributes, time and date, and the
// file size, along with the name.  It shows a summary of file sizes at the end
// along with free space.
//
//*****************************************************************************
int Cmd_ls(int argc, char *argv[]) {
    unsigned int ui32TotalSize;
    unsigned int ui32FileCount;
    unsigned int ui32DirCount;
    FRESULT iFResult;
    FATFS *psFatFs;
    char *pcFileName;
#if _USE_LFN
    char pucLfn[_MAX_LFN + 1];
    g_sFileInfo.lfname = pucLfn;
    g_sFileInfo.lfsize = sizeof(pucLfn);
#endif

    //
    // Open the current directory for access.
    //
    iFResult = f_opendir(&g_sDirObject, g_pcCwdBuf);

    //
    // Check for error and return if there is a problem.
    //
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    ui32TotalSize = 0;
    ui32FileCount = 0;
    ui32DirCount = 0;

    //
    // Give an extra blank line before the listing.
    //
    UARTprintf("\n");

    //
    // Enter loop to enumerate through all directory entries.
    //
    for (;;) {
        //
        // Read an entry from the directory.
        //
        iFResult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if (iFResult != FR_OK) {
            return ((int) iFResult);
        }

        //
        // If the file name is blank, then this is the end of the listing.
        //
        if (!g_sFileInfo.fname[0]) {
            break;
        }

        //
        // If the attribue is directory, then increment the directory count.
        //
        if (g_sFileInfo.fattrib & AM_DIR) {
            ui32DirCount++;
        }

        //
        // Otherwise, it is a file.  Increment the file count, and add in the
        // file size to the total.
        //
        else {
            ui32FileCount++;
            ui32TotalSize += g_sFileInfo.fsize;
        }

#if _USE_LFN
        pcFileName = ((*g_sFileInfo.lfname)?g_sFileInfo.lfname:g_sFileInfo.fname);
#else
        pcFileName = g_sFileInfo.fname;
#endif
        //
        // Print the entry information on a single line with formatting to show
        // the attributes, date, time, size, and name.
        //
        UARTprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
                (g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
                (g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
                (g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
                (g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
                (g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
                (g_sFileInfo.fdate >> 9) + 1980, (g_sFileInfo.fdate >> 5) & 15,
                g_sFileInfo.fdate & 31, (g_sFileInfo.ftime >> 11),
                (g_sFileInfo.ftime >> 5) & 63, g_sFileInfo.fsize, pcFileName);
    }

    //
    // Print summary lines showing the file, dir, and size totals.
    //
    UARTprintf("\n%4u File(s),%10u bytes total\n%4u Dir(s)", ui32FileCount,
            ui32TotalSize, ui32DirCount);

    //
    // Get the free space.
    //
    iFResult = f_getfree("/", (DWORD *) &ui32TotalSize, &psFatFs);

    //
    // Check for error and return if there is a problem.
    //
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    //
    // Display the amount of free space that was calculated.
    //
    UARTprintf(", %10uK bytes free\n",
            (ui32TotalSize * psFatFs->free_clust / 2));

    //
    // Made it to here, return with no errors.
    //
    return (0);
}

//*****************************************************************************
//
// This function implements the "cd" command.  It takes an argument that
// specifies the directory to make the current working directory.  Path
// separators must use a forward slash "/".  The argument to cd can be one of
// the following:
//
// * root ("/")
// * a fully specified path ("/my/path/to/mydir")
// * a single directory name that is in the current directory ("mydir")
// * parent directory ("..")
//
// It does not understand relative paths, so dont try something like this:
// ("../my/new/path")
//
// Once the new directory is specified, it attempts to open the directory to
// make sure it exists.  If the new path is opened successfully, then the
// current working directory (cwd) is changed to the new path.
//
//*****************************************************************************
int Cmd_cd(int argc, char *argv[]) {
    uint_fast8_t ui8Idx;
    FRESULT iFResult;

    //
    // Copy the current working path into a temporary buffer so it can be
    // manipulated.
    //
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    //
    // If the first character is /, then this is a fully specified path, and it
    // should just be used as-is.
    //
    if (argv[1][0] == '/') {
        //
        // Make sure the new path is not bigger than the cwd buffer.
        //
        if (strlen(argv[1]) + 1 > sizeof(g_pcCwdBuf)) {
            UARTprintf("Resulting path name is too long\n");
            return (0);
        }

        //
        // If the new path name (in argv[1])  is not too long, then copy it
        // into the temporary buffer so it can be checked.
        //
        else {
            strncpy(g_pcTmpBuf, argv[1], sizeof(g_pcTmpBuf));
        }
    }

    //
    // If the argument is .. then attempt to remove the lowest level on the
    // CWD.
    //
    else if (!strcmp(argv[1], "..")) {
        //
        // Get the index to the last character in the current path.
        //
        ui8Idx = strlen(g_pcTmpBuf) - 1;

        //
        // Back up from the end of the path name until a separator (/) is
        // found, or until we bump up to the start of the path.
        //
        while ((g_pcTmpBuf[ui8Idx] != '/') && (ui8Idx > 1)) {
            //
            // Back up one character.
            //
            ui8Idx--;
        }

        //
        // Now we are either at the lowest level separator in the current path,
        // or at the beginning of the string (root).  So set the new end of
        // string here, effectively removing that last part of the path.
        //
        g_pcTmpBuf[ui8Idx] = 0;
    }

    //
    // Otherwise this is just a normal path name from the current directory,
    // and it needs to be appended to the current path.
    //
    else {
        //
        // Test to make sure that when the new additional path is added on to
        // the current path, there is room in the buffer for the full new path.
        // It needs to include a new separator, and a trailing null character.
        //
        if (strlen(g_pcTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcCwdBuf)) {
            UARTprintf("Resulting path name is too long\n");
            return (0);
        }

        //
        // The new path is okay, so add the separator and then append the new
        // directory to the path.
        //
        else {
            //
            // If not already at the root level, then append a /
            //
            if (strcmp(g_pcTmpBuf, "/")) {
                strcat(g_pcTmpBuf, "/");
            }

            //
            // Append the new directory to the path.
            //
            strcat(g_pcTmpBuf, argv[1]);
        }
    }

    //
    // At this point, a candidate new directory path is in chTmpBuf.  Try to
    // open it to make sure it is valid.
    //
    iFResult = f_opendir(&g_sDirObject, g_pcTmpBuf);

    //
    // If it can't be opened, then it is a bad path.  Inform the user and
    // return.
    //
    if (iFResult != FR_OK) {
        UARTprintf("cd: %s\n", g_pcTmpBuf);
        return ((int) iFResult);
    }

    //
    // Otherwise, it is a valid new path, so copy it into the CWD.
    //
    else {
        strncpy(g_pcCwdBuf, g_pcTmpBuf, sizeof(g_pcCwdBuf));
    }

    //
    // Return success.
    //
    return (0);
}

//*****************************************************************************
//
// This function implements the "pwd" command.  It simply prints the current
// working directory.
//
//*****************************************************************************
int Cmd_pwd(int argc, char *argv[]) {
    //
    // Print the CWD to the console.
    //
    UARTprintf("%s\n", g_pcCwdBuf);

    //
    // Return success.
    //
    return (0);
}

//uint16_t bswap16(uint16_t a)
//{
//  a = ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8);
//  return a;
//}

//uint32_t bswap32(uint32_t a)
//{
//  a = ((a & 0x000000FF) << 24) |
//      ((a & 0x0000FF00) <<  8) |
//      ((a & 0x00FF0000) >>  8) |
//      ((a & 0xFF000000) >> 24);
//  return a;
//}

//uint16_t Color565(uint8_t r, uint8_t g, uint8_t b)
//{
//  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
//}

//*****************************************************************************
//
// This function implements the "cat" command.  It reads the contents of a file
// and prints it to the console.  This should only be used on text files.  If
// it is used on a binary file, then a bunch of garbage is likely to printed on
// the console.
//
//*****************************************************************************
int Cmd_cat(int argc, char *argv[]) {
    FRESULT iFResult;
    unsigned int ui32BytesRead;

    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
        UARTprintf("Resulting path name is too long\n");
        return (0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if (strcmp("/", g_pcCwdBuf)) {
        strcat(g_pcTmpBuf, "/");
    }

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_pcTmpBuf, argv[1]);

    //
    // Open the file for reading.
    //
    iFResult = f_open(&g_sFileObject, g_pcTmpBuf, FA_READ);

    //
    // If there was some problem opening the file, then return an error.
    //
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    //
    // Enter a loop to repeatedly read data from the file and display it, until
    // the end of the file is reached.
    //
    do {
        //
        // Read a block of data from the file.  Read as much as can fit in the
        // temporary buffer, including a space for the trailing null.
        //
        iFResult = f_read(&g_sFileObject, g_pcTmpBuf, sizeof(g_pcTmpBuf) - 1,
                (UINT *) &ui32BytesRead);

        //
        // If there was an error reading, then print a newline and return the
        // error to the user.
        //
        if (iFResult != FR_OK) {
            UARTprintf("\n");
            return ((int) iFResult);
        }

        //
        // Null terminate the last block that was read to make it a null
        // terminated string that can be used with printf.
        //
        g_pcTmpBuf[ui32BytesRead] = 0;

        //
        // Print the last chunk of the file that was received.
        //
        UARTprintf("%s", g_pcTmpBuf);
    } while (ui32BytesRead == sizeof(g_pcTmpBuf) - 1);

    //
    // Return success.
    //
    return (0);
}


//*****************************************************************************
//
// This function implements the "del" command.  It is used to delete files
// and folders.
//
//*****************************************************************************
int Cmd_del(int argc, char *argv[]) {
    int iFResult;
    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
        UARTprintf("Resulting path name is too long\n");
        return (0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if (strcmp("/", g_pcCwdBuf)) {
        strcat(g_pcTmpBuf, "/");
    }

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_pcTmpBuf, argv[1]);

    //
    // Open the file for reading.
    //
    iFResult = SDCardIF_DeleteLogFile(g_pcTmpBuf);

    if(iFResult == SDCARD_IF_OP_SUCCESS){
    	UARTprintf("Successfully deleted : %s\n",g_pcTmpBuf);
    }
	else{
				UARTprintf("Error in operation: %d \n",iFResult);
	}

    return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "del" command.  It is used to delete files
// and folders.
//
//*****************************************************************************
int Cmd_init_sd(int argc, char *argv[]) {
    int iFResult = SDCARD_IF_OP_SUCCESS;

    //
    // Initialize the SD CARD.
    //
    SDCardIF_Initialize();

    if(iFResult == SDCARD_IF_OP_SUCCESS){
    	UARTprintf(	"Successfully initialized\n"
    				"The EVENT_Log.TXT file should be created in the SD CARD now.\n"
    				"UNLESS it was already there.");
    }
	else{
		UARTprintf("Error in operation: %d \n",iFResult);
	}

    return ((int) iFResult);
}

//*****************************************************************************
//
// This function implements the "play_audio" command.  It is used to copy the
// file content to audio buffer
//
//*****************************************************************************
int Cmd_play_audio(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 2){
		 //
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		//
		// Copy the file content to the audio buffer. Buffer is set using SDCardIF_SetAudioFileBuffer
		//
		iFResult = SDCardIF_PlayAudioFile(g_pcTmpBuf);

		if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
			UARTprintf("Buffer content: \n");

			SDCardIF_DeleteFirmwareFile("play_audio.wav");
			SDCardIF_CreateFirmwareFile("play_audio.wav");
			SDCardIF_AppendFirmwareData("play_audio.wav", g_audio_buffer, sizeof(g_audio_buffer));
			UARTprintf("play_audio.wav file is created. Which contains audio data.\n");
#if 0
			int i = 0;

			for(i = 0; i < sizeof(g_audio_buffer); i++){

				if(i % 32 == 0){
					UARTprintf("\n[0x%x]\t:", i);
				}

				UARTprintf(" 0x%x",g_audio_buffer[i]);

			}
#endif

			iFResult = 0;
		}
		else{
					UARTprintf("Error in operation: %d \n",iFResult);
		}

	}

	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "read_firmware" command.  Used to read firmware
// file to a buffer.
//
//*****************************************************************************
int Cmd_read_firmware(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 4){
		 //
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		// Configure the offset/buffer size/filename_size
		int offset = atoi(argv[2]);
		int buf_size = atoi(argv[3]);
		int filename_size;

		char *buffer = (char*)malloc(buf_size);

		//
		// Read firmware data
		//
		iFResult = SDCardIF_ReadFirmwareFile(g_pcTmpBuf, offset, buffer, &buf_size, &filename_size);

		if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
			UARTprintf("Operation finished with condition: %d\n",iFResult);
			UARTprintf("Copied amount of data: %d\n",buf_size);
			UARTprintf("Buffer content: \n");

			int i = 0;
			for(i = 0; i<buf_size; i++){
				if(i % 16 == 0){
					UARTprintf("\n[%d]: ",i);
				}

				UARTprintf("0x%x ",buffer[i]);
			}


			iFResult = 0;
		}
		else{
			UARTprintf("Required filename size: %d\n",buf_size);
			UARTprintf("Error in operation: %d \n",iFResult);
		}

		if(buffer){
			free(buffer);
		}

	}

	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "create_fw" command.  Used to create a dummy
// firmware file.
//
//*****************************************************************************
int Cmd_create_firmware(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 2){
		 //
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		//
		// Create firmware data
		//
		iFResult = SDCardIF_CreateFirmwareFile(g_pcTmpBuf);

		if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
			UARTprintf("Operation finished with condition: %d\n",iFResult);
			UARTprintf("File %s should be created.\n",g_pcTmpBuf);

			iFResult = 0;
		}
		else{
			UARTprintf("Error in operation: %d \n",iFResult);
		}
	}

	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "delete_fw" command.  Used to delete the
// firmware file.
//
//*****************************************************************************
int Cmd_delete_firmware(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 2){
		 //
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		//
		// Delete firmware data
		//
		iFResult = SDCardIF_DeleteFirmwareFile(g_pcTmpBuf);

		if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
			UARTprintf("Operation finished with condition: %d\n",iFResult);
			UARTprintf("File %s should be deleted.\n",g_pcTmpBuf);

			iFResult = 0;
		}
		else{
			UARTprintf("Error in operation: %d \n",iFResult);
		}
	}

	return ((int) iFResult);
}


//****************************************************************************************
//
// This function implements the "append_fw" command.  Used to append a pre configured
// data pattern to firmware file.
//
//*****************************************************************************************
int Cmd_append_firmware(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 3){
		 //
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		int data_pattern = atoi(argv[2]);
		char buffer[10];
		int i;
		for(i = 0; i < sizeof(buffer); i++){
			buffer[i] = data_pattern;
		}

		//
		// Append firmware data
		//
		iFResult = SDCardIF_AppendFirmwareData(g_pcTmpBuf, buffer, sizeof(buffer));

		if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
			UARTprintf("Operation finished with condition: %d\n",iFResult);
			UARTprintf("File %s should be modified. Use read_firmware to confirm.\n",g_pcTmpBuf);

			iFResult = 0;
		}
		else{
			UARTprintf("Error in operation: %d \n",iFResult);
		}
	}

	return ((int) iFResult);
}



//****************************************************************************************
//
// This function implements the "append_test" command.  Used to create a new file by
// copying the contents of another file
//
//*****************************************************************************************
int Cmd_append_test(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 4){
		char pcFile1[80];
		char pcFile2[80];

		memset(pcFile1, 0x00, sizeof(pcFile1));
		memset(pcFile2, 0x00, sizeof(pcFile2));

		//
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		strcpy(pcFile1,g_pcTmpBuf);

		//
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//

		memset(g_pcTmpBuf, 0x00, sizeof(g_pcTmpBuf));

		if (strlen(g_pcCwdBuf) + strlen(argv[2]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[2]);

		strcpy(pcFile2,g_pcTmpBuf);

		int transfer_size = atoi(argv[3]);

		UARTprintf("Starting to copy %s to %s with each transfer size of %d. Please wait...\n",pcFile1,pcFile2,transfer_size);

		char *buffer = (char*)malloc(transfer_size * sizeof(char));
		int buffer_size = transfer_size;
		int file_size = 0;
		int i =0;
		if(buffer){

			//
			// Get File1 size
			//

			iFResult = SDCardIF_ReadFirmwareFile(	pcFile1,
													0,
													buffer,
													&buffer_size,
													&file_size);

			if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){

				for(i = 0; i < file_size;){
					buffer_size = transfer_size;

					iFResult = SDCardIF_ReadFirmwareFile(pcFile1,i,buffer,&buffer_size,&file_size);

					if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
						iFResult = SDCardIF_AppendFirmwareData(pcFile2,buffer,buffer_size);

						if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
							i += buffer_size;
							UARTprintf(".");
						}
						else{
							UARTprintf("Error in operation: %d \n",iFResult);
							break;
						}
					}
					else{
						UARTprintf("Error in operation: %d \n",iFResult);
						break;
					}
				}
			}

			UARTprintf("\n");

			if((SDCARD_IF_OP_SUCCESS == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_LARGE == iFResult) || (SDCARD_IF_WARN_BUFFER_SIZE_SMALL == iFResult)){
				UARTprintf("Operation finished with condition: %d\n",iFResult);

				iFResult = 0;
			}
			else{
				UARTprintf("Error in operation: %d \n",iFResult);
			}

			free(buffer);
		}
		else{
			UARTprintf("Error in operation: Malloc failed \n");
		}
	}

	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "Cmd_set_log" command.  It is used to set the
// default log file.
//
//*****************************************************************************
int Cmd_set_log(int argc, char *argv[]) {

	int iFResult = -1;

	if(argc == 2){
		 //
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		//
		// Set log file
		//
		iFResult = SDCardIF_SetLogFile(g_pcTmpBuf);

		if(SDCARD_IF_OP_SUCCESS == iFResult){
			UARTprintf("Log file is set to: %s \n",g_pcTmpBuf);
			UARTprintf("Now the file should be created in the SD card.\n"
						"Use the 'ls' command to check it");
			iFResult = 0;
		}
		else{
			UARTprintf("Error in operation: %d \n",iFResult);
		}

	}

	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "Cmd_log_event" command.  It is used to set the
// write an event to the selected log file.
//
//*****************************************************************************
int Cmd_log_event(int argc, char *argv[]) {

	int iFResult = -1;
	static char event_no_static = 0;

	if(argc == 1){

		//
		// Log event
		//
		ITSI_LOG_EVENT event;

		event.length = 13;
		event.day = 31;
		event.month = 1;
		event.year = 15;
		event.hour = 19;
		event.minute = 20;
		event.second = 10;
		event.id3 = 1;
		event.id2 = 2;
		event.id3 = 3;
		event.event_no = event_no_static++;
		event.crc_lsb = 11;
		event.crc_msb = 22;

		// Log the event
		iFResult = SDCardIF_LogEvent(&event);

		if(SDCARD_IF_OP_SUCCESS == iFResult){
			UARTprintf("Successfully logged the entry to the log file");
			iFResult = 0;
		}
		else{
			UARTprintf("Error in operation: %d \n",iFResult);
		}

	}

	return ((int) iFResult);
}

//*****************************************************************************
//
// This function implements the "set_audio" command.  It is used to set the audio
// file buffer.
//
//*****************************************************************************
int Cmd_set_audio(int argc, char *argv[]) {
	int iFResult = -1;

	if(argc == 2){
		int size = atoi(argv[1]);

		if(size > 0){
			//if(g_audio_buffer){
				//free(g_audio_buffer);
			//}

			//g_audio_buffer = (char *)malloc(size);

			//
			// Set audio file buffer
			//
			iFResult = SDCardIF_SetAudioFileBuffer(g_audio_buffer, size);;

			if(iFResult == SDCARD_IF_OP_SUCCESS){
				UARTprintf("Successfully assigned the buffer");
			}
			else{
				UARTprintf("Error reported : %d\n",iFResult);
			}
		}
	}

	return ((int) iFResult);
}

//*****************************************************************************
//
// This function implements the "Cmd_current_log" command.  It is used to get
// the name of current log file.
//
//*****************************************************************************
int Cmd_current_log(int argc, char *argv[]) {
	int iFResult = -1;

	if(argc == 1){
		char log_file[100];
		int size = sizeof(log_file);

		// Get current log filename
		iFResult = SDCardIF_GetCurrentLogFile(log_file,&size);

		if(iFResult == SDCARD_IF_OP_SUCCESS){
			UARTprintf("Current filename is: %s",log_file);
		}
		else{
			UARTprintf("Error reported : %d\n",iFResult);
		}

	}

	return ((int) iFResult);
}

//*****************************************************************************
//
// This function implements the "read_last_100" command.  It is used to
// read the last 100 events of a file
//
//*****************************************************************************
int Cmd_read_last_100(int argc, char *argv[]) {
	int iFResult = -1;
	int i;
	if(argc == 2){

		//
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		// Buffer to store events
		ITSI_LOG_EVENT events[100];
		int no_of_events = 100;

		// Read events (LAST_100)
		iFResult = SDCardIF_ReadEventLog(g_pcTmpBuf,events,LAST_100,&no_of_events, 0);

		if(	(iFResult == SDCARD_IF_OP_SUCCESS) ||
			(iFResult == SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL)){

			UARTprintf("Completed with status: %d\n",iFResult);
			UARTprintf("Amount of events read: %d\n",no_of_events);

			for(i=0; i<no_of_events; i++){
				UARTprintf("#%d: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
								i,
								events[i].length,
								events[i].day,
								events[i].month,
								events[i].year,
								events[i].hour,
								events[i].minute,
								events[i].second,
								events[i].id3,
								events[i].id2,
								events[i].id1,
								events[i].event_no,
								events[i].crc_msb,
								events[i].crc_lsb);
			}

			iFResult = FR_OK;
		}
		else{
			UARTprintf("Error reported : %d\n",iFResult);
		}
	}


	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "read_full" command.  It is used to
// read the total events in a file.
//
//*****************************************************************************
int Cmd_read_full(int argc, char *argv[]) {
	int iFResult = -1;
	int i;
	if(argc == 2){

		//
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		ITSI_LOG_EVENT events[100];
		int no_of_events = 100;

		// Read events (FULL_READ)
		iFResult = SDCardIF_ReadEventLog(g_pcTmpBuf,events,FULL_READ,&no_of_events, 0);

		if(	(iFResult == SDCARD_IF_OP_SUCCESS) ||
			(iFResult == SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL)){

			UARTprintf("Completed with status: %d\n",iFResult);
			UARTprintf("Amount of events read: %d\n",no_of_events);

			for(i=0; i<no_of_events; i++){
				UARTprintf("#%d: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
								i,
								events[i].length,
								events[i].day,
								events[i].month,
								events[i].year,
								events[i].hour,
								events[i].minute,
								events[i].second,
								events[i].id3,
								events[i].id2,
								events[i].id1,
								events[i].event_no,
								events[i].crc_msb,
								events[i].crc_lsb);
			}

			iFResult = FR_OK;
		}
		else{
			UARTprintf("Error reported : %d\n",iFResult);
		}
	}


	return ((int) iFResult);
}

//*****************************************************************************
//
// This function implements the "read_begining_n" command.  It is used to
// read the N number of events from the begining.
//
//*****************************************************************************
int Cmd_read_begining_n(int argc, char *argv[]) {
	int iFResult = -1;
	int i;
	if(argc == 4){

		//
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		ITSI_LOG_EVENT *events = NULL;
		int no_of_events = atoi(argv[2]);
		int offset = atoi(argv[3]);

		events = (ITSI_LOG_EVENT *)malloc(sizeof(ITSI_LOG_EVENT) * no_of_events);

		// Read events
		iFResult = SDCardIF_ReadEventLog(g_pcTmpBuf,events,N_FROM_BEGINING,&no_of_events,offset);

		if(	(iFResult == SDCARD_IF_OP_SUCCESS) ||
			(iFResult == SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL)){

			UARTprintf("Completed with status: %d\n",iFResult);
			UARTprintf("Amount of events read: %d\n",no_of_events);

			for(i=0; i<no_of_events; i++){
				UARTprintf("#%d: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
								i,
								events[i].length,
								events[i].day,
								events[i].month,
								events[i].year,
								events[i].hour,
								events[i].minute,
								events[i].second,
								events[i].id3,
								events[i].id2,
								events[i].id1,
								events[i].event_no,
								events[i].crc_msb,
								events[i].crc_lsb);
			}

			iFResult = FR_OK;

			if(events){
				free(events);
			}
		}
		else{
			UARTprintf("Error reported : %d\n",iFResult);
		}


	}


	return ((int) iFResult);
}

//*****************************************************************************
//
// This function implements the "read_last_n" command.  It is used to
// read the N number of events from the begining.
//
//*****************************************************************************
int Cmd_read_last_n(int argc, char *argv[]) {
	int iFResult = -1;
	int i;
	if(argc == 4){

		//
		// First, check to make sure that the current path (CWD), plus the file
		// name, plus a separator and trailing null, will all fit in the temporary
		// buffer that will be used to hold the file name.  The file name must be
		// fully specified, with path, to FatFs.
		//
		if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
			UARTprintf("Resulting path name is too long\n");
			return (0);
		}

		//
		// Copy the current path to the temporary buffer so it can be manipulated.
		//
		strcpy(g_pcTmpBuf, g_pcCwdBuf);

		//
		// If not already at the root level, then append a separator.
		//
		if (strcmp("/", g_pcCwdBuf)) {
			strcat(g_pcTmpBuf, "/");
		}

		//
		// Now finally, append the file name to result in a fully specified file.
		//
		strcat(g_pcTmpBuf, argv[1]);

		ITSI_LOG_EVENT *events = NULL;
		int no_of_events = atoi(argv[2]);
		int offset = atoi(argv[3]);
		events = (ITSI_LOG_EVENT *)malloc(sizeof(ITSI_LOG_EVENT) * no_of_events);

		// Read events
		iFResult = SDCardIF_ReadEventLog(g_pcTmpBuf,events,N_FROM_LAST,&no_of_events, offset);

		if(	(iFResult == SDCARD_IF_OP_SUCCESS) ||
			(iFResult == SDCARD_IF_WARN_LESS_NO_Of_EVENTS_AVAIL)){

			UARTprintf("Completed with status: %d\n",iFResult);
			UARTprintf("Amount of events read: %d\n",no_of_events);

			for(i=0; i<no_of_events; i++){
				UARTprintf("#%d: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
								i,
								events[i].length,
								events[i].day,
								events[i].month,
								events[i].year,
								events[i].hour,
								events[i].minute,
								events[i].second,
								events[i].id3,
								events[i].id2,
								events[i].id1,
								events[i].event_no,
								events[i].crc_msb,
								events[i].crc_lsb);
			}

			iFResult = FR_OK;
		}
		else{
			UARTprintf("Error reported : %d\n",iFResult);
		}
	}


	return ((int) iFResult);
}


//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list of the
// available commands with a brief description.
//
//*****************************************************************************
int Cmd_help(int argc, char *argv[]) {
    tCmdLineEntry *psEntry;

    //
    // Print some header text.
    //
    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    //
    // Point at the beginning of the command table.
    //
    psEntry = &g_psCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The end of the
    // table has been reached when the command name is NULL.
    //
    while (psEntry->pcCmd) {
        //
        // Print the command name and the brief description.
        //
        UARTprintf("%16s: %s\n", psEntry->pcCmd, psEntry->pcHelp);

        //
        // Advance to the next entry in the table.
        //
        psEntry++;
    }

    //
    // Return success.
    //
    return (0);
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions, and
// brief description.
//
//*****************************************************************************
tCmdLineEntry g_psCmdTable[] = {
        { "help", Cmd_help, "Display list of commands" },
		{ "h", Cmd_help, "alias for help" },
		{ "?", Cmd_help, "alias for help" },
		{ "ls",Cmd_ls, "Display list of files" },
		{ "chdir", Cmd_cd, "Change directory" },
		{ "cd", Cmd_cd, "alias for chdir" },
		{ "pwd", Cmd_pwd, "Show current working directory" },
		{ "cat", Cmd_cat, "Show contents of a text file" },
		{ "del", Cmd_del, "Delete available file/directory"},
		{ "init_sd", Cmd_init_sd, "Initialize SD Card"},
		{ "set_audio", Cmd_set_audio, "Set audio file buffer. (Parameter is the size)"},
		{ "play_audio", Cmd_play_audio, "Copy audio file to the buffer. (Parameter is the filename)"},
		{ "set_log", Cmd_set_log, "Set the current log file. (Parameter is filename)"},
		{ "log_event", Cmd_log_event, "Log a sample event to the log file."},
		{ "read_last_100", Cmd_read_last_100, "Read last 100 events."},
		{ "read_full", Cmd_read_full, "Read all events."},
		{ "read_begining", Cmd_read_begining_n, "Read number of events from begining, \n\t\t Ex:read_begining <filename> <events> <offset>."},
		{ "read_last", Cmd_read_last_n, "Read number of events from last, \n\t\t Ex:read_last <filename> <events> <offset>."},
		{ "current_log", Cmd_current_log, "Get current log file"},
		{ "read_firmware", Cmd_read_firmware, "Read firmware file to a buffer. \n\t\t Ex: read_firmware <filename> <offset> <amount>"},
		{ "create_fw", Cmd_create_firmware, "Create a new firmware file with default data \n\t\t Ex: create_fw <filename>"},
		{ "delete_fw", Cmd_delete_firmware, "Delete the firmware file \n\t\t Ex: delete_fw <filename>"},
		{ "append_fw", Cmd_append_firmware, "Append pre configured data pattern to the firmware file \n\t\t Ex: append_fw <filename> <data_pattern>"},
		{ "sdcard_append_test", Cmd_append_test, "Copy file1 to the end of file2, each iteration copies 'transfer_size' number of bytes.\n\t\t Ex: append_fw <file1> <file2> <transfer_size>"},
#ifdef LCD
        {    "load", Cmd_load, "Load a bmp file"},                  // Load_bmp.c
#endif
        { 0, 0, 0 } };

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned int ui32Line)
{
}
#endif



/*------------------------------------------------------------/
/ Open or create a file in append mode
/------------------------------------------------------------*/

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}


//*****************************************************************************
//
// The program main function.  It performs initialization, then runs a command
// processing loop to read commands from the console.
//
//*****************************************************************************
int SD_Test(void) {
    int nStatus;
    FRESULT iFResult;



    // todo jc write soimething
#define TEST_AUDIO_FILE    "TEST_AUDIO.WAV"
    FRESULT res;                /* FatFs function common result code */

    if(FILEIF_ERR_UNINIT == FileIF_Initialize()){
    	UARTprintf("SD CARD not found. Program will halt!!!");
    	while(1);
    }
    // write some info
    FIL fsrc;                /* File objects */

      /* Open  the file for append */
      res = open_append(&fsrc, TEST_AUDIO_FILE);
      if (res != FR_OK) {
          /* Error. Cannot create the file */
          while(1);
      }

      // if file empty, write header
      if (! f_size(&fsrc)) {
          res = f_printf(&fsrc, "This string should contain 100 bytes of data.\n"
        		  	  	  	  	  "This is created to test the audio file read function.\n"
        		  	  	  	  	  "123456789012345678901234567890123456789012345678901234567890\n"
        		  	  	  	  	  "123456789012345678901234567890123456789012345678901234567890\n"
        		  	  	  	  	  "123456789012345678901234567890123456789012345678901234567890\n");
        if (res <0) {
            /* Error. Cannot write header */
            while(1);
        }
      }

      /* Close the file */
      res = f_close(&fsrc);
      if (res != FR_OK)
      {
        /* Error. Cannot close the file */
        while(1);
      }


    //
    // Enter an infinite loop for reading and processing commands from the
    // user.
    //
    while (1) {
        //
        // Print a prompt to the console.  Show the CWD.
        //
        UARTprintf("\n%s> ", g_pcCwdBuf);

        //
        // Get a line of text from the user.
        //
        UARTgets(g_pcCmdBuf, sizeof(g_pcCmdBuf));
//        sciReceive(sciREG1, sizeof(g_pcCmdBuf), g_pcCmdBuf);
        //
        // Pass the line from the user to the command processor.  It will be
        // parsed and valid commands executed.
        //

        nStatus = CmdLineProcess(g_pcCmdBuf);

        //
        // Handle the case of bad command.
        //
        if (nStatus == CMDLINE_BAD_CMD) {
            UARTprintf("Bad command!\n");
        }

        //
        // Handle the case of too many arguments.
        //
        else if (nStatus == CMDLINE_TOO_MANY_ARGS) {
            UARTprintf("Too many arguments for command processor!\n");
        }

        //
        // Otherwise the command was executed.  Print the error code if one was
        // returned.
        //
        else if (nStatus != 0) {
            UARTprintf("Command returned error code %s\n",
                    StringFromFResult((FRESULT) nStatus));
        }
    }
}

