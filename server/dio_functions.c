#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

/* Local libs */
#include "broker.h"
#include "dio_functions.h"
#include "dio.h"
#include "ui_interface.h"

#define MAX_DIO_EVENT_HANDLERS	5
#define MAX_DIO_NAME_LENGTH		32
#define MAX_GPIO				5
#define PIN_NAME_LENGTH			6
#define NAME_LENGTH				20
#define LOW						0
#define HIGH					1

#define TESTING_ID				999
#define CURRENT_TIME			time(null)

#define MAX_BUFFER_LEN 256
#define SEPARATOR_CHAR_TOKEN "~"

GET_REASON_DEFINITION

typedef struct
{
	char name[NAME_LENGTH + 1];
	int configured;
	char pin[PIN_NAME_LENGTH + 1];
	int mode;
	int lastState;
	struct tm time_lastState;

	int (*callback)(int id, char* pin, int data);
}gpio_function_t;

typedef struct
{
	char name[NAME_LENGTH + 1]; /* Machine Name TODO: All this could be gathered from a config file */
	char user[NAME_LENGTH + 1]; /* User/Operator Name */
	char lastStopReason[NAME_LENGTH + 1];/* Stop reason given by the Operator */
	int id;					/* machine id */
	int shift_id;
	int product_id;

	int isRunning;					/* Machine status flag according to the input configured */
	int allowRecord; 				/* Record flag to allow record to the csv file after a input change */
	int isAlarming;   				/* Alarm flag when the system has an alarm */
	int isStopReasonNeeded;			/* Stop reason flag to save when operator entered a reason for the last stop*/
	int maintenance_mode;			/* flag to tell when machine is in maintenance mode */

	time_t lastStopTime; 			 /* Last time machine stopped */
	unsigned long maxStopTime;	  	 /* Max time machine can be stopped and not running in seconds */
}machine_t; /* Used when more than 1 machine per
			 * raspberry pi is implemented.
			 * NOTE: it would be necessary add a remote IO
			 * or something else to accomodate the max current output. */

/* Gloabals */
static gpio_function_t gpio_function_list[MAX_GPIO];
static machine_t machine;
time_t current_time;

/* Prototype Functions */
int function_sensor_rolo(int id, char* pin, int data);
int function_alarm(int id, char* pin, int data);
int function_enable_production(int id, char* pin, int data);
int function_machine_running(int id, char* pin, int data);
int function_product_ready(int id, char* pin, int data);
int force_output(char* pin, int level);
char* get_pin_by_function_name(char* name);


/* Functions */
int init_dio_functions(void)
{
	int i;

	printf("init_dio_functions() started...\n");

	/* Initialization Block */
	for (i = 0; i < MAX_GPIO; i++)
	{
		gpio_function_list[i].configured = 0;
		gpio_function_list[i].callback = NULL;
		gpio_function_list[i].lastState = -1;
	}

	machine.isRunning = 0;
	machine.allowRecord = 0;
	machine.isAlarming = 0;
	machine.isStopReasonNeeded = 1; /* Stop reason needed on startup */
	machine.maintenance_mode = 0;
	machine.lastStopTime = time(NULL);
	machine.maxStopTime = 30; /* TODO: review how long */
	machine.id = 999;
	machine.shift_id = 999;
	machine.product_id = 999;

	strncpy(machine.name, "Prototipo", strlen("Prototipo"));
	machine.name[NAME_LENGTH] = '\0';

	strncpy(machine.user, "ND", strlen("ND"));
	machine.user[NAME_LENGTH] = '\0';

	strncpy(machine.lastStopReason, "desconhecido", strlen("desconhecido"));
	machine.lastStopReason[NAME_LENGTH] = '\0';

	printf("init_dio_functions() - machine.lastStopTime: %s \n", ctime(&machine.lastStopTime));

	/* Manually set all pins and functions
	 * Set pin modes
	 * BCM_17 - Saida - Alarme Maquina Parada
     * BCM_27 - Saida - Habilita Producao
	 * BCM_23 - Entrada - Maquina Produzindo
	 * BCM_24 - Entrada - Enrolador Rodando
	 * BCM_25 - Entrada - Sensor Rolo
	 */

	strncpy(gpio_function_list[0].name, "FUNC_ALARM", NAME_LENGTH);
	gpio_function_list[0].name[NAME_LENGTH] = '\0';
	gpio_function_list[0].configured = 1;
	strncpy(gpio_function_list[0].pin, "BCM_17", strlen("BCM_17"));
	gpio_function_list[0].mode = OUT;
	gpio_function_list[0].callback = function_alarm;
	setPinMode(gpio_function_list[0].pin, gpio_function_list[0].mode);

	strncpy(gpio_function_list[1].name, "FUNC_ENABLE_PRODUCTION", NAME_LENGTH);
	gpio_function_list[1].name[NAME_LENGTH] = '\0';
	gpio_function_list[1].configured = 1;
	strncpy(gpio_function_list[1].pin, "BCM_27", strlen("BCM_27"));
	gpio_function_list[1].mode = OUT;
	gpio_function_list[1].callback = function_enable_production;
	setPinMode(gpio_function_list[1].pin, gpio_function_list[1].mode);

	strncpy(gpio_function_list[2].name, "FUNC_MACHINE_RUNNING", NAME_LENGTH);
	gpio_function_list[2].name[NAME_LENGTH] = '\0';
	gpio_function_list[2].configured = 1;
	strncpy(gpio_function_list[2].pin, "BCM_23", strlen("BCM_23"));
	gpio_function_list[2].mode = IN;
	gpio_function_list[2].callback = function_machine_running;
	setPinMode(gpio_function_list[2].pin, gpio_function_list[2].mode);

	strncpy(gpio_function_list[3].name, "FUNC_SENSOR_ROLO", NAME_LENGTH);
	gpio_function_list[3].name[NAME_LENGTH] = '\0';
	gpio_function_list[3].configured = 1;
	strncpy(gpio_function_list[3].pin, "BCM_25", strlen("BCM_25"));
	gpio_function_list[3].mode = IN;
	gpio_function_list[3].callback = function_sensor_rolo;
	setPinMode(gpio_function_list[3].pin, gpio_function_list[3].mode);

	/* Enrolador function? Does it need a relay? */
	//strncpy(gpio_function_list[3].name, "FUNC_ENROLADOR", NAME_LENGTH);
	//gpio_function_list[3].name[NAME_LENGTH] = '\0';
	//gpio_function_list[3].configured = 1;
	//strncpy(gpio_function_list[3].pin, "", 6);
	//gpio_function_list[3].mode = IN;
	//gpio_function_list[3].callback = function_product_ready;

	return 0;
}


int denit_dio_functions(void)
{
	int i;

	for (i = 0; i < MAX_GPIO; i++)
	{
		if (gpio_function_list[i].mode == OUT)
			setResetPin(gpio_function_list[i].pin, 0);
	}
}


/* Proccess all GPIO that are set and have a callback */
int process_dio_functions(void)
{
	int i;

	/* Maintenance mode disables all functions from running
	 * and maintains Habilita Producao always ON */
	if (machine.maintenance_mode)
		return 0;

	for (i = 0; i < MAX_GPIO; i++)
	{
		if(gpio_function_list[i].configured)
		{
			if (gpio_function_list[i].callback != NULL)
			{
				if (gpio_function_list[i].callback(i, gpio_function_list[i].pin, 0) == -1)
				{
					printf("process_dio_functions() Callback Failed!\n");
				}
			}
		}
	}

	return 0;
}


char* get_pin_by_function_name(char* name)
{
	int i;

	if (name == NULL)
		return NULL;

	printf("get_pin_by_function_name() name=%s\n", name);

	for (i = 0; i < MAX_GPIO; i++)
	{
		if (gpio_function_list[i].configured)
		{
			if (strncmp(gpio_function_list[i].name, name, NAME_LENGTH) == 0)
			{
				printf("Name=%s has output=%s\n", name, gpio_function_list[i].pin);

				return gpio_function_list[i].pin;
			}
		}
	}

	/* Nothing found */
	return NULL;
}


bool rolo_pronto_salvo = false;
/*
* Rolo Pronto - Maquina rodando e Sinal High
*/
int function_sensor_rolo(int id, char* pin, int data)
{
	int pin_state = 0;

	if (pin == NULL)
		return -1;

	pin_state = readPin(pin);

	/* Rolo bom sendo retirado */
	if (machine.isRunning)
	{
		if (pin_state == 1)
		{
			if (rolo_pronto_salvo == false)
			{
				send_ui_message(MACHINE_STATUS, "Rolo Pronto");

				rolo_pronto_salvo = true;

				current_time = time(NULL);
				csv_write(machine.name, "Rolo Pronto", machine.user, machine.shift_id, machine.product_id, machine.id, &current_time);
			}
			return 0;
		}
	}

	rolo_pronto_salvo = false;

	return 0;
}


/* alarm must go off when the machine is stopped for more than x Seconds */
int function_alarm(int id, char* pin, int data)
{
	int pin_state = 0;

	if (pin == NULL)
		return -1;

	pin_state = readPin(pin);

	if (machine.isStopReasonNeeded == 0)
	{
		if (pin_state == 1)
		{
			printf("function_alarm() setting pin=%s to Low\n", pin);
			setResetPin(pin, 0);
		}
	}
	else if (((long)machine.lastStopTime + machine.maxStopTime) < time(NULL))
	{
		if (pin_state == 0)
		{
			send_ui_message(MACHINE_STATUS, "Alarme Ativo");

			printf("function_alarm() setting pin=%s to High\n", pin);
			setResetPin(pin, 1);
		}
	}

	return 0;
}


/* Habilita Producao - saida digital para iniciar producao da Maquina
 * quando nao ha a necessidade de justificar a parada de maquina
 * pin - saida habilita producao
 */
int function_enable_production(int id, char* pin, int data)
{
	int pin_state;

	if (pin == NULL)
		return -1;

	if ((id < 0) || (id >= MAX_GPIO))
		return -1;

	pin_state = readPin(pin);

	if (pin_state == -1)
	{
		printf("function_enable_production() output %s (id=%d) not configured\n", pin, id);
		return -1;
	}

	if (machine.isStopReasonNeeded)
	{
		if (pin_state == 0)
			return 0;

		setResetPin(pin, 0);
	}
	else
	{
		if (pin_state == 1)
			return 0;

		setResetPin(pin, 1);
	}

	return 0;
}


/* Verifica se maquina esta rodando ou parada
 * pin - sinal de maquina rodando ( 1 - rodando, 0 - parada )
 * TODO: add a 1 sec filter
 */
int function_machine_running(int id, char* pin, int data)
{
	int pin_state;

	if (pin == NULL)
		return -1;

	if ((id < 0) || (id >= MAX_GPIO))
		return -1;

	pin_state = readPin(pin);

	if (pin_state == -1)
	{
		printf("function_machine_running() input %s (id=%d) not configured\n", pin, id);
		return -1;
	}

	if (pin_state == gpio_function_list[id].lastState)
		return 0;

	printf("function_machine_running() pin=%s changed to: %d\n", pin, pin_state);

	current_time = time(NULL);

	if (pin_state)
	{
		printf("\tinit_dio_functions() - Machine running at %s \n", ctime(&current_time));
		send_ui_message(MACHINE_STATUS, "Maquina rodando");

		/* If pinState == 1 without a reason been added, We assumed
		 * the system was bypassed
		 */
		if (machine.isStopReasonNeeded)
		{
			machine.isStopReasonNeeded = 0;
			csv_write(machine.name, "By-passed", machine.user, machine.shift_id, machine.product_id, machine.id, &current_time);
		}

		csv_write(machine.name, "Maquina rodando", machine.user, machine.shift_id, machine.product_id, machine.id, &current_time);
	}
	else
	{
		machine.lastStopTime = current_time; /* NOTE: salvamos horario do evento. */
											/* TODO: return Unix epoch time in seconds */

		printf("\tinit_dio_functions() - machine.lastStopTime: %s \n", ctime(&machine.lastStopTime));
		send_ui_message(MACHINE_STATUS, "Maquina parada");

		machine.isStopReasonNeeded = 1;
		strcpy(machine.lastStopReason, "desconhecido");

		csv_write(machine.name, "Maquina parada", machine.user, machine.shift_id, machine.product_id, machine.id, &current_time);
	}

	gpio_function_list[id].lastState = pin_state;
	machine.isRunning = pin_state;

	return 0;
}


/* Liga/Desliga do Enrolador
 * It will add a new line to the csv when the product is ready
 * TODO: future use?
 */
int function_product_ready(int id, char* pin, int data)
{
	if (pin == NULL)
		return -1;

	if ((id < 0) || (id >= MAX_GPIO))
		return -1;

	return 0;
}


char* token_msg;
char temp_buffer[MAX_BUFFER_LEN + 1];

/* Recebe motivo da parada do cliente e dispara geracao relatorio */
int set_last_stop_reason(int reason_id, const char* buffer)
{
	int rc = 0;

	if ((reason_id <= REASON_UNKNOWN) || (reason_id >= REASON_MAX))
		return -1;

	if (reason_id == MANUTENCAO)
		toggle_maintenance_mode();

	/* buffer constains all machine and operator info
	 * for now just operator name */
	if (buffer != NULL)
	{
		strncpy(temp_buffer, buffer, MAX_BUFFER_LEN);
		temp_buffer[MAX_BUFFER_LEN] = '\0';

		token_msg = strtok(temp_buffer, SEPARATOR_CHAR_TOKEN);

		if (strcmp(token_msg, machine.user))
		{
			printf("set_last_stop_reason() Operator changed from %s to %s\n", machine.user, token_msg);

			strncpy(machine.user, token_msg, NAME_LENGTH);
			machine.user[NAME_LENGTH] = '\0';
		}
	}

	printf("setLastStopReson() - reason_id: %d\n", reason_id);

	if (machine.isStopReasonNeeded)
	{
		current_time = time(NULL);

		strncpy(machine.lastStopReason, getReasonStr(reason_id), NAME_LENGTH);
		machine.lastStopReason[NAME_LENGTH] = '\0';

		/* TODO: create a struct to handle all info to print */
		rc = csv_write(machine.name, machine.lastStopReason, machine.user,
			machine.shift_id, machine.product_id, machine.id, &current_time);

		machine.isStopReasonNeeded = 0;

		/* TODO: print more info about what was printed: time date, reason, operator, etc... */
		switch (rc)
		{
			case 0:
				send_ui_message(REPORT, "Parada de maquina salva com sucesso!");
				break;
			case -1:
				send_ui_message(REPORT, "Erro no salvamento da parada de maquina!");
				break;
		}
	}

	return rc;
}


/* Note: when maintenance mode is activated it will prevent the system from running
the function until maintenance mode is deactivated */
int toggle_maintenance_mode()
{
	machine.maintenance_mode = !machine.maintenance_mode;

	if (machine.maintenance_mode)
	{
		send_ui_message(MACHINE_STATUS, "maintenance on");
	}
	else
	{
		send_ui_message(MACHINE_STATUS, "maintenance off");
	}

	/* Turn on Habilita Producao */
	setResetPin(get_pin_by_function_name("FUNC_ENABLE_PRODUCTION"), 1);
}


/* --------- Functions for test only ---------- */
int force_output(char* pin, int level)
{
	if (pin == NULL)
		return -1;

	printf("force_output() pin=%s level=%d\n", pin, level);

	return setResetPin(pin, level);
}


void blink_start(char* pin, int time)
{
	if (pin == NULL)
		return;

    if (time == 0)
    {
      printf("Error! No blink time\n");
      return;
    }

    setPinMode(pin, OUT);

    printf("Start blinking...\n");

    int i = 0;
    while(i < 5)
    {
		force_output(pin, 1);
    	delay(time);
    	force_output(pin, 0);
    	delay(time);
    	i++;
    }
    printf("Blink finished.\n");
}
