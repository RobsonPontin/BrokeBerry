int force_output(char* pin, int level);
void blink_start(char* pin, int time);
int init_dio_functions(void);
int denit_dio_functions(void);
int process_dio_functions(void);
int set_last_stop_reason(int reason_id, const char* buffer);
int toggle_maintenance_mode();

typedef enum
{
	REASON_UNKNOWN = -1,
	ABASTECIMENTO_ARAME = 10,
	CONSERTO_TELA,
	SENSOR_ROLO,
	MANUTENCAO,
	AJUSTE_MAQUINA,
	INTERVALO,
	REASON_MAX
} stop_reasons_t;

const static struct
{
	stop_reasons_t value;
	const char* str;
} conversionReasons[] =
{
	{REASON_UNKNOWN, "Desconhecido"},
	{ABASTECIMENTO_ARAME, "Abastecimento Arame"},
	{CONSERTO_TELA, "Conserto Tela"},
	{SENSOR_ROLO, "Rolo Defeito"},
	{MANUTENCAO, "Manutencao"},
	{AJUSTE_MAQUINA, "Ajuste Maquina"},
	{INTERVALO, "Intervalo"}
};

#define GET_REASON_DEFINITION \
static const char* getReasonStr(stop_reasons_t stopReason) \
{ int i; \
if ((stopReason < REASON_UNKNOWN) || (stopReason >= REASON_MAX)) \
return NULL; \
for (i = 0; i < sizeof(conversionReasons); i++) \
{ if (conversionReasons[i].value == stopReason) \
{ return conversionReasons[i].str; } } \
return NULL; }
