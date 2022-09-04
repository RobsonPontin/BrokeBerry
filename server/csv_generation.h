/* Time Stamp - Operator - Shift ID - Product ID - Machine ID */
int csv_write(const char* machine_name, const char* stop_reason, const char* user_name, int shift_id, int product_id, int machine_id, time_t* lastStopTime);
int csv_test(void);
