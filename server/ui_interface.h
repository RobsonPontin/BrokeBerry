enum MESSAGE_TYPE
{
	MESSAGE_UNKNOWN = 0,
	RASP_STATUS,
	MACHINE_STATUS,
	REPORT,
	MESSAGE_MAX
};

int send_ui_message(int type, const char * msg);