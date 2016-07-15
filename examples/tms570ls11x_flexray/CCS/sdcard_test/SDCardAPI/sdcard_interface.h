/**
 * @file sdcard_interface.h
 * @author Pradeepa Senanayake (pradeepa.kck@gmail.com)
 * @date 31 Jan 2016
 * @brief Function declarations for sdcard_interface.c
 *
 *
 * Version v1.0.0
 */

#ifndef _SDCARD_INTERFACE_H
#define _SDCARD_INTERFACE_H
#endif

#include "sdcard_err_codes.h"

#ifdef TEST
#define STATIC static
#else
#define STATIC
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef struct _ITSI_LOG_EVENT{
	char length;
	char day;
	char month;
	char year;
	char hour;
	char minute;
	char second;
	char id3;
	char id2;
	char id1;
	char event_no;
	char crc_msb;
	char crc_lsb;
} ITSI_LOG_EVENT;


#define DEFAULT_EVENT_LOG	("EVENT_Log.txt")
#define MAX_FILENAME_SIZE 255

enum _READ_TYPE{
	FULL_READ = 0,
	LAST_100,
	N_FROM_BEGINING,
	N_FROM_LAST
};

typedef enum _READ_TYPE READ_TYPE;

int SDCardIF_Initialize();
int SDCardIF_SetAudioFileBuffer(char *p_buffer, int buf_size);
int SDCardIF_PlayAudioFile(const char *filename);
int SDCardIF_SetLogFile(const char* filename);
int SDCardIF_DeleteLogFile(const char* filename);
int SDCardIF_LogEvent(ITSI_LOG_EVENT *event);
int SDCardIF_ReadEventLog(const char* filename, ITSI_LOG_EVENT *event, READ_TYPE read_type, int *no_of_events, int offset);
int SDCardIF_GetCurrentLogFile(char *filename, int *filename_size);
int SDCardIF_ReadFirmwareFile(char *filename, int offset, char *buffer, int *buf_size, int *file_size);
int SDCardIF_CreateFirmwareFile(const char *filename);
int SDCardIF_DeleteFirmwareFile(const char *filename);
int SDCardIF_AppendFirmwareData(const char *filename, char* data, int data_size);
void SDCardIF_Reset();
