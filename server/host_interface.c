/*
 * This is the protocol used to communicate with the
 * system.
 */

/*
 * [preamble][length][CMD][message]
 * [preamble]	"~~"
 * [msg type]	1 byte
 * [command]	2 bytes
 * [value/extra]
 *   [Separator Token between string] "~"
 * [postamble]	\r\n"
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "host_interface.h"
#include "dio_functions.h"

#include "logger.h"

#define MAX_MESSAGE_LENGTH	100
#define PIN_NAME_LENGTH 6

char temp_buffer[MAX_MESSAGE_LENGTH];


int decode_message(const char *buffer)
{
	int len, msg_type, command;
	char * msg;

	if (buffer == NULL)
		return -1;

	len = strlen(buffer);

	if ((len == 0) || (len > MAX_MESSAGE_LENGTH))
		return -1;

	strncpy(temp_buffer, buffer, len);

	/* Test preamble ("~~") */
	msg = strchr(buffer, '~');
	if (msg != NULL)
	{
		if (strchr(msg + 1, '~') == NULL)
		{
			printf("Error - Preamble not found!\n");
			return -1;
		}
	}
	else
	{
		printf("Error - Preamble not found!\n");
		return -1;
	}

	/* Test postamble("\r\n") */
	msg = strchr(buffer, '\r');
	if (msg != NULL)
	{
		if (strchr(msg + 1, '\n') == NULL)
		{
			printf("Error - Postamble not found!\n");
			return -1;
		}
	}
	else
	{
		printf("Error - Postamble not found!\n");
		return -1;
	}

	strncpy(temp_buffer, buffer + 2, 1); /* Offset preamble and copy msg type */
	temp_buffer[1] = '\0';
	msg_type = atoi(temp_buffer);

	switch (msg_type)
	{
	case MSG_GET:
		printf("MSG GET selected\n");

		strncpy(temp_buffer, buffer + 3, 2); /* Offset and copy command */
		temp_buffer[2] = '\0';
		command = atoi(temp_buffer);

		strncpy(temp_buffer, buffer + 5, len - 5); /* offset and copy rest of msg */

		execute_command(command, (const char *)strtok(temp_buffer, "\r"));
		break;

	case MSG_SET:
		printf("MSG SET selected\n");

		strncpy(temp_buffer, buffer + 3, 2); /* Offset and copy command */
		temp_buffer[2] = '\0';
		command = atoi(temp_buffer);

		strncpy(temp_buffer, buffer + 5, len - 5); /* offset */

		execute_command(command, (const char *)strtok(temp_buffer, "\r"));
		break;

	default: /* Invalid msg_type */
		return -1;
		break;
	}

	return 0;
}


char msg[20];

int execute_command(int cmd_type, const char *message)
{
	int pin_id, value, reason_id;
	char pin_name[PIN_NAME_LENGTH + 1];

	if (message == NULL)
		return -1;

	printf("execute_command() cmd_type=%d message=%s \n", cmd_type, message);

	if ((cmd_type < CMD_SET_DIO) || (cmd_type > CMD_MAX))
		return -1;

	strncpy(temp_buffer, message, strlen(message) + 1);

	switch(cmd_type)
	{
		case CMD_READ_DIO:
		{
			msg[0] = temp_buffer[0];
			msg[1] = temp_buffer[1];
			msg[3] = '\0';

			pin_id = atoi(msg);

			sprintf(pin_name, "BCM_%d", pin_id);

			printf("CMD_READ_DIO Pin_Name=%s value=%d \n", pin_name, readPin(pin_name));
		}
		break;

		case CMD_SET_DIO:
		{
			msg[0] = temp_buffer[0]; /* Pin_id */
			msg[1] = temp_buffer[1];
			msg[3] = '\0';

			pin_id = atoi(msg);

			msg[0] = temp_buffer[2]; /* Value - 1 or 0 */
			msg[1] = '\0';

			value = atoi(msg);

            printf("CMD_SET_DIO pin_id=%d value=%d\n", pin_id, value);

			switch(pin_id)
			{
				case 17:
					force_output("BCM_17", value);
				break;

				case 27:
					force_output("BCM_27", value);
				break;

				default:
					printf("Execute action() - action invalid! \n");
				break;
			}
		}
		break;

		case CMD_STOP_REASON:
		{
			printf("CMD_STOP_REASON Stop Reason selected \n");

			msg[0] = temp_buffer[0]; /* Stop Reason */
			msg[1] = temp_buffer[1];
			msg[3] = '\0';

			value = atoi(msg);

			printf("CMD_STOP_REASON Stop Reason_id=%d \n", value);

			/* temp_buffer offsets stop reason */
			set_last_stop_reason(value, temp_buffer + 3);
		}
		break;

		case CMD_SET_AIO:
			printf("CMD_SET_AIO Set AIO selected \n");
		break;

		default:
			printf("\nError! Command not valid!\n");
		break;
	}
}
