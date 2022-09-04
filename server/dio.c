/*
 * Map and handle all IO requests for set and reset
 * Responsible for reading status of Analog inputs and outputs
 */
#include <wiringPi.h>
#include <stdio.h>

#include "dio.h"

#define MAX_GPIO_PINS 40


int init_dio(void)
{
	wiringPiSetup();

	return 0;
}


int str2Enum(const char *str)
{
	int i;

	if (str == NULL)
		return -1;

	for (i = 0; i < sizeof(conversion) / sizeof(conversion[0]); ++i)
	{
		if (strcmp(str, conversion[i].str) == 0)
			return conversion[i].value;
	}

	return -1;
}


int setPinMode(char* pin, int mode)
{
	if (pin == NULL)
		return -1;

	if ((mode < IN) || (mode > OUT))
		return -1;

	int id = str2Enum(pin);
	if (id == -1)
	{
		printf("setPinMode() Failed\n");
		return -1;
	}

	if ((id < 0) || (id > MAX_GPIO_PINS))
		return -1;

	pinMode(id, mode);

	return 0;
}


int setResetPin(char* pin, int value)
{
	int id;

	if (pin == NULL)
		return -1;

	if (value)
		value = 1;

	id = str2Enum(pin);
	if (id == -1)
	{
		printf("Error! setResetPin() - Invalid pin number!\n");
		return -1;
	}

	printf("setResetPin() - pin: %s dio_port=%d value=%d \n", pin, id, value);
	digitalWrite(id, value);
}

int readPin(char* pin)
{
	if (pin == NULL)
		return -1;

	int id = str2Enum(pin);
	if (id == -1)
	{
		printf("readPin() failed\n");
		return -1;
	}

	return digitalRead(id);
}
