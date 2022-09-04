#include <stdio.h>
#include <string.h>

#include <mosquitto.h>

#include "logger.h"

#include "broker.h"
#include "host_interface.h"

#define MQTT_HOST			"localhost"
#define MQTT_PORT			1883
#define MQTT_USER			"root"
#define MQTT_PASSWORD		"toor"

#define TOPIC_SUBSCRIPTION	"/dev/mosq_subscriber_client"
#define TOPIC_PUBLISHER		"/dev/publisher"
#define TOPIC_CLIENT		"/dev/mosq_publish_client"
#define MQTT_LOOP_TIMEOUT	100

#define CLIENT_ID_SIZE 24

/* Globals */
struct mosquitto *mosq;
char client_id[CLIENT_ID_SIZE];

/* Prototype Functions */
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
void connect_callback(struct mosquitto *mosq, void *obj, int result);


/* Functions */
int init_mqtt()
{
    int rc = 0;
	int subStatus;
	char *userName = MQTT_USER;
	char *password = MQTT_PASSWORD;

    /* it must be called before any other mosquitto function */
	mosquitto_lib_init();

	/* void *str, int c, size_t n - copy 0 to all 24 chars in clientid */
	memset(client_id, 0, CLIENT_ID_SIZE);

	/* char *s, size_t n, const char *format - calling process ID will be stored in clientid */
	snprintf(client_id, 23, "temp_file_%d", getpid());

    printf("client_id=%s\n", client_id);

	/* const char *id, clean session, *obj */
	mosq = mosquitto_new(client_id, true, 0);
	if(mosq)
	{
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

        mosquitto_username_pw_set(mosq, userName, password);

        /* NOTE: For some weird reason rc is returning -1, but all the errors
         * should be greater than 0. So abort connection only if rc != MOSQ_ERR_SUCCESS
         *		-1  : MOSQ_ERR_CONN_PENDING
         * Returns:
         *		0  : MOSQ_ERR_SUCCESS
         *		3  : MOSQ_ERR_INVAL
         *		14 : MOSQ_ERR_ERRNO
         */
		rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
		if (rc != MOSQ_ERR_SUCCESS)
		{
			printf("init_mqtt() - Connection Failed. ErrNo=%d\n", rc);
			deinit_mosquitto();
			return -1;
		}

        /* struct mosquitto *mosq, int *mid, const char *sub, int qos */
		subStatus = mosquitto_subscribe(mosq, NULL, TOPIC_SUBSCRIPTION, 0);

        printf("Connection Subscription status: %d\n", subStatus);
	}

	return rc;
}


void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	if (mosq == NULL)
		return;

	if (obj == NULL)
		return;

	printf("Connect Callback, rc=%d \n", result);
}


/* Handle messages being sent by clients */
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	int rc;
	bool match_result = 0;

	if (mosq == NULL)
		return;

	if (obj == NULL)
		return;

	if (message == NULL)
		return;

	rc = mosquitto_topic_matches_sub(TOPIC_SUBSCRIPTION, message->topic, &match_result);
	if (rc != MOSQ_ERR_SUCCESS)
		return;

	if (match_result == 0)
		return;

	printf("	Message from client:\n"
		"		payload=%s"
		"		payloadlen=%d\n"
		"		topic=%s\n",
		(char*)message->payload,
		message->payloadlen,
		message->topic);

	decode_message(message->payload);
}


/* This must be called frequently in order to check for incoming data
   Make sure all setups and callbacks are set */
int process_mosquitto(void)
{
	int rc = 0;

	/* mosq, timeout, max_packets */
	rc = mosquitto_loop(mosq, MQTT_LOOP_TIMEOUT, 1);

	switch(rc)
	{
		case MOSQ_ERR_SUCCESS:
			return 0;

		case MOSQ_ERR_INVAL:
			printf("if the input parameters were invalid.\n");

			return -1;

		case MOSQ_ERR_NOMEM:
			printf("if an out of memory condition occurred.\n");

			return -1;

		case MOSQ_ERR_NO_CONN:
			printf("\nConnection error!");

			sleep(10);
			mosquitto_reconnect(mosq);

			return -1;

		case MOSQ_ERR_CONN_LOST:
			printf("if the connection to the broker was lost.\n");

			return -1;

		case MOSQ_ERR_PROTOCOL:
			printf("if there is a protocol error communicating with the broker.\n");

			return -1;

		case MOSQ_ERR_ERRNO:
			printf("if a system call returned an error.\n");

			return -1;
	}

	return 0;
}


/* When the program stops it should be called to destroy and release memory */
int deinit_mosquitto(void)
{
	printf("deinit_mosquitto() called\n");
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}


/* TODO: function to publish messages to the clients */
int client_publish(const char * message)
{
	int rc;

	if ((message == NULL) || (strlen(message) == 0))
		return;

	/* TODO: need another mosq instance? */
	rc = mosquitto_publish(mosq, NULL, TOPIC_CLIENT, strlen(message), message, 0, 0);

	switch (rc)
	{
		case MOSQ_ERR_SUCCESS:
			printf("Success!\n");
			return 0;

		case MOSQ_ERR_INVAL:
			printf("The input parameters were invalid.\n");

			return -1;

		case MOSQ_ERR_NOMEM:
			printf("if an out of memory condition occurred.\n");

			return -1;

		case MOSQ_ERR_NO_CONN:
			printf("\nConnection error!");

			sleep(10);
			mosquitto_reconnect(mosq);

			return -1;

		case MOSQ_ERR_CONN_LOST:
			printf("if the connection to the broker was lost.\n");

			return -1;

		case MOSQ_ERR_PROTOCOL:
			printf("if there is a protocol error communicating with the broker.\n");

			return -1;

		case MOSQ_ERR_ERRNO:
			printf("if a system call returned an error.\n");

			return -1;
	}

	return 0;
}
