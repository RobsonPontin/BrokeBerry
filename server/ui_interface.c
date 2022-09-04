#include <stdio.h>
#include <string.h>

#include <mosquitto.h>

#include "ui_interface.h"

#define CLIENT_PREAMBLE "~~"
#define CLIENT_POSTAMBLE "\r\n"

#define MAX_MESSAGE_LENGTH 256


int send_ui_message(int type, const char * msg)
{
	char message[300];

	if ((type <= MESSAGE_UNKNOWN) || (type >= MESSAGE_MAX))
		return -1;

	if ((msg == NULL) || (strlen(msg) == 0))
		return -1;

	if (strlen(msg) >= MAX_MESSAGE_LENGTH)
		return -1;

	/* add message pre-amble */
	message[0] = '~';
	message[1] = '~';

	/* add message type (0 to 9) + '0' -> 48 in ASCII */
	message[2] = type + '0';

	strncpy(message + 3, msg, strlen(msg));

	/* add message post-amble */
	message[strlen(msg) + 3] = '\r';
	message[strlen(msg) + 4] = '\n';
	message[strlen(msg) + 5] = '\0';

	printf("message: [%s] msg: [%s] msg \n", message, msg);

	return client_publish(message);
}