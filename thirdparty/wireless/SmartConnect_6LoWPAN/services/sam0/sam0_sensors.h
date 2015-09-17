

typedef enum sensor_type_tag
{
    TEMPERATURE,
#if BOARD==SAMR21_XPLAINED_PRO
	IO_SUPPLY,
    ANALOG_PIN
#endif
} sensor_type_t;

typedef enum sensor_error_tag
{
    SUCCESS,
    ADC_OVERRUN,
    DEVICE_ON_FIRE
} sensor_error_t;


typedef struct sensor_result_tag
{
    uint16_t adc_value;
    float sensor_value;
    char unit;
    sensor_error_t error;
} sensor_result_t;


int8_t get_sensor_value(sensor_type_t sensor, sensor_result_t *res);



