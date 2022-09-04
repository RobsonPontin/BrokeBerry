#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logger.h"

const char log_level_text[LOG_LEVEL_MAX+1][16] = { \
		"ERROR: ", \
		"Warn: ", \
		"Info: ", \
		"Cfg: ", \
		"Debug: ", \
		"Comm: ", \
		"Trace: ", \
		"" };


void Log(int level, const char *message)
{
	if ((level > LOG_LEVEL_MAX) || (level < LOG_LEVEL_ERR))
		return;

	FILE *file;

	if (LogCreated)
	{
		file = fopen(LOGFILE, "w")
		LogCreated = true;
	} else
	{
		file = fopen(LOGFILE, "a");

		if (file == NULL)
		{
			if (LogCreated)
				LogCreated = false;

			return;
		} else
		{
			time_t now;
			time(&now);

			char *temp_msg = malloc(strlen(log_level_text[level]) + strlen(message) + 1); /* + 1 for null term*/
			strcpy(temp_msg, ctime(&now));
			strcat(temp_msg, log_level_text[level]);
			strcat(temp_msg, message);

			fputs(message, file);
			fclose(file);
		}
	}

	if (file)
			fclose(file);
}