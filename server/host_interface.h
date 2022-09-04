enum command_type
{
	CMD_SET_DIO = 20,
	CMD_SET_AIO = 21,
	CMD_READ_DIO = 30,
	CMD_STOP_REASON = 40,
	CMD_MAX
};

enum message_type
{
	MSG_GET = 0,
	MSG_SET = 1,
};

int decode_message(const char *buffer);