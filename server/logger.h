/* TODO: define log file location */
#define LOGFILE "gl.log"

typedef enum
{
	LOG_LEVEL_ERR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_CONFIG,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_COMM,
	LOG_LEVEL_TRACE,
	LOG_LEVEL_MAX
} log_level_t;

void Log(int level, const char *message);
