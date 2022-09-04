/* The Main GPIO Connector
 * wiringPi | GPIO.BCM
 * 0            17
 * 1            18
 * 2            Rv1:21 Rev2:27
 * 3            22
 * 4            23
 * 5            24
 * 6            25
 * 7             4
 * 8            Rv1:0 - Rv2:2
 * 9            Rv1:1 - Rv2:3
 * 10			 8
 * 11			 7
 * 12           10
 * 13			 9
 * 14			11
 * 15			14 - NOTE: This pin (TXD) might be used by others process and change states
 * 16			15
 * 17			28 - Rev 2 only
 * 18			29 - Rev 2 only
 * 19			30 - Rev 2 only
 * 20			31 - Rev 2 only
 */
typedef enum
{
	BCM_4 = 7,
	BCM_8 = 10,
	BCM_9 = 13,
	BCM_10 = 12,
	BCM_11 = 14,
	BCM_14 = 15,
	BCM_15 = 16,
	BCM_17 = 0,
	BCM_18 = 1,
	BCM_22 = 3,
	BCM_23 = 4,
	BCM_24 = 5,
	BCM_25 = 6,
	BCM_28 = 17,
	BCM_27 = 2,
	BCM_29 = 18,
	BCM_30 = 19,
	BCM_31 = 20
} DIO_NUMBER_RV2;

/* Struct responsible for converting a string to enum */
const static struct
{
	DIO_NUMBER_RV2 value;
	const char *str;
} conversion[] =
{
	{BCM_4, "BCM_4"},
	{BCM_8, "BCM_8"},
	{BCM_9, "BCM_9"},
	{BCM_10, "BCM_10"},
	{BCM_11, "BCM_11"},
	{BCM_14, "BCM_14"},
	{BCM_15, "BCM_15"},
	{BCM_17, "BCM_17"},
	{BCM_18, "BCM_18"},
	{BCM_22, "BCM_22"},
	{BCM_23, "BCM_23"},
	{BCM_24, "BCM_24"},
	{BCM_25, "BCM_25"},
	{BCM_27, "BCM_27"},
	{BCM_28, "BCM_28"},
	{BCM_29, "BCM_29"},
	{BCM_30, "BCM_30"},
	{BCM_31, "BCM_31"},
};

enum DIO_MODE
{
	IN = 0,
	OUT = 1
};

int init_dio(void);
int setPinMode(char* pin, int mode);
int setResetPin(char* pin, int level);
int readPin(char* pin);
int str2Enum(const char *str);

