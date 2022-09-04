#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <time.h> /*TODO: Remove it - testing only*/

#include "broker.h"
#include "host_interface.h"
#include "dio_functions.h"
#include "dio.h"
#include "csv_generation.h"
#include "daemon.h"
#include "ui_interface.h"

/* Prototype Functions */
void signal_handler(int sig);

/* Globals */
static volatile int done = 0;	/* volatile tells the compiler the value
								 * of the var will change without action from the code */

/* NOTE: activate it to get cycle processing time */
/*clock_t loopTime;*/

/* Linux signal - Signals are a way to delive async events to the application
 * SIGINT - signal generate on ctrl+c
 * SIGTERM - generic signal used to cause program termination
*/
void signal_handler(int sig)
{
	switch(sig)
	{
		case SIGINT:
		case SIGTERM:
			done = 1;
			break;
	}

    return;
}

/* Functions */
int main (int argc, char *argv[]) {

int opt;

signal(SIGTERM, signal_handler); /* catch termination signal */
signal(SIGINT,  signal_handler); /* catch ctrl-c signal */

while ((opt = getopt(argc, argv, "rnbhmtcspf:")) != -1)
{
	switch(opt) {
	case 'r':
        printf("R selected");
        printf("\n");

        break;

	case 'n':
        printf("N selected");
        printf("\n");

        break;

    case 'b':
    	printf("Starting blinking test process...\n");
    	delay(500);
    	init_dio();
        blink_start("BCM_17", 500); /* Default blink test on pin BCM_17 */

        break;

    case 'h':
        printf("Help Section. Type:\n");
        printf("\t -b time : Start blinking LED on BCM_17. Argument time in ms\n");
        printf("\t -f : Force a specific output using the following arguments: -f GPIO# level. i.e.: -f 17 1 or -f 17 0 \n");
        printf("\t -m : Start mosquitto message broker\n");
      	printf("\t -s : Start BrokeBerry process using Daemon\n");
        printf("\n");

        break;

	case 'm':
	    printf("MQTT Broker selected...\n");
		break;

	case 't':
		printf("Start testing...\n");
		init_dio_functions();
		break;

	case 'c':
	    printf("Testing IO File...\n");
	    csv_test();
		break;

	case '?':
		printf("Option not avaliable\n");
        printf("\n");

		break;

	case 's':
		printf("Start program selected\n");
		break;

	case 'p':
		init_mqtt();
		printf("Publish msg test\n");
		send_ui_message(RASP_STATUS, "Publishing testing!");
		return 0;

	case 'f':
		init_dio();
		setPinMode("BCM_27", OUT);

		force_output("BCM_27", atoi(optarg));

		delay(2000);
		return 0;

	default:
		printf("No Valid Option!\n");
		return -1;
}

if ((opt != 'm') && (opt != 's'))
	exit(0);

if (opt == 's')
	daemonize(); /*Start DAEMON*/

/* Initialization */
init_dio();

init_dio_functions();

/* Start MQQT */
init_mqtt();

/* PROGRAM MAIN LOOP */
while (!done)
{
	/*NOTE: clock() to measure the time used to process*/
	/*loopTime = clock();*/

	process_mosquitto();

	/* Execute Functions */
	process_dio_functions();

	/*NOTE: clock() to measure the time used to process*/
	/*loopTime = clock() - loopTime;*/
	/*double time_taken = ((double)loopTime) / CLOCKS_PER_SEC;*/
	/*printf("Processing Time taken: %f\n", time_taken);*/
}

/* PROGRAM TERMINATION */
deinit_mosquitto();
denit_dio_functions();

printf("The program was Gracefully Terminated\n");

}
return 0;
}
