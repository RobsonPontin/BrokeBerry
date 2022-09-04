int init_mqtt(void);
int deinit_mosquitto(void);
int process_mosquitto(void);

int client_publish(const char * message);