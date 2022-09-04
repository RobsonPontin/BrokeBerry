#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "logger.h"
#include "csv_generation.h"

#define WRITE "w"
#define READ "r"
#define APPEND "a"
#define PATH_MAX 256
#define MAX_STRING_LEN 256

/*Prototype*/
FILE* openFile(const char* name);
FILE* createFile(const char* name);
int is_file_created(char* fileName);

/* The name of the csv file must be date_machineNumber.csv */
char csv_fileName[PATH_MAX+1];


int csv_test(void)
{
	FILE *f;
	int product_id = 32, machine_id = 7, shift_id = 2;
	char name[] = "operador 3";
	char machine[] = "trefilador 1";

	f = fopen("csv_test.csv", WRITE);
	fprintf(f, "%s,%s,%d,%d,%d", name, machine, product_id, machine_id, shift_id);

	fclose(f);
}


/*TODO: send this function to a proper file/lib */
char* convertUnixTStampToDateTime(time_t unixTimeStamp)
{
	printf("convertUnixTStampToDateTime() - time logic\n");

	char* dateTime = malloc(sizeof(char) * 32); /* 32 char should be enough for date and time */
	struct tm* timeInfo;

	timeInfo = localtime(&unixTimeStamp);

	strftime(dateTime, 32, "%d/%m/%Y %H:%M:%S", timeInfo);

	return dateTime;
}


char temp_machine_name[MAX_STRING_LEN + 1];
char temp_stop_reason[MAX_STRING_LEN + 1];
char temp_user_name[MAX_STRING_LEN + 1];

/* TODO: Make it generic. Gets buffer with token and print each value per collumn */
int csv_write(const char* machine_name, const char* stop_reason, const char* user_name, int shift_id, int product_id, int machine_id, time_t* last_stop_time)
{
	FILE* file;
	struct tm* timeInfo;

	if ((machine_name == NULL)	||
		(stop_reason == NULL)	||
		(user_name == NULL)		||
		(last_stop_time == NULL))
	{
		return -1;
	}

	strncpy(temp_machine_name, machine_name, MAX_STRING_LEN);
	temp_machine_name[MAX_STRING_LEN] = '\0';

	strncpy(temp_stop_reason, stop_reason, MAX_STRING_LEN);
	temp_stop_reason[MAX_STRING_LEN] = '\0';

	strncpy(temp_user_name, user_name, MAX_STRING_LEN);
	temp_user_name[MAX_STRING_LEN] = '\0';

	printf("csv_write() - writing to the csv file. stop_reason: %s\n", temp_stop_reason);
	printf("%s,%s,%s,%d,%d,%d\n", ctime(last_stop_time), temp_machine_name, temp_user_name, shift_id, product_id, machine_id);

	timeInfo = localtime(last_stop_time);

	/* NOTE: due tm_year returning the # years since 1900, We add 1900 */
	sprintf(csv_fileName, "/share/%d_%d_%d.csv", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);

	if (is_file_created(csv_fileName))
	{
		file = openFile(csv_fileName);
	}
	else
	{
		file = createFile(csv_fileName);
	}

	if(file)
	{
		printf("Writing to the csv file...\n");

		fprintf(file, "%02d/%02d/%d, %d:%02d:%02d,%s,%s,%s,%d,%d,%d\n",
				timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900,
				timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, temp_machine_name,
				temp_stop_reason, temp_user_name, shift_id, product_id,	machine_id);

		fclose(file);
	}
	else
	{
		printf("csv_write() open file failed!\n");
		return -1;
	}

	return 0;
}


int is_file_created(char* fileName)
{
	if (access( fileName, F_OK) != -1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


FILE* openFile(const char* name)
{
	if (name == NULL)
		return NULL;

	printf("csv_openFile() Opening file...\n");

	FILE *file = NULL;

	file = fopen(csv_fileName, APPEND);

	if (file == NULL)
	{
		printf("openFile() - Failed! File is NULL");
		return NULL;
	}

	return file;
}


FILE* createFile(const char* name)
{
	if (name == NULL)
		return NULL;

	return fopen(name, APPEND);
}
