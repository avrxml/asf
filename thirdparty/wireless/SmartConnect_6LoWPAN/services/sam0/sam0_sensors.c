#include <stdint.h>
#include <inttypes.h>
#include "asf.h"
#include "sam0_sensors.h"

/*
 * Configuration:
 *   ADC_USE_STANDBY = 1: ADC is sampling in STANDBY mode, due to noise reduction
 *   ADC_USE_STANDBY = 0: ADC is sampling while MCU is running
 */
#define ADC_USE_STANDBY (0)
/*
 * Note: In order to use ADC IRQ with Atmel CMSIS, the following patch should be applied.
 * Issue: originally #ifdefs are applied to test enum values, arghhh!
 *
 * Change in file src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c:
 * -----------------------------------------------------
 * #ifdef ADC_IRQn
 *  (void*) ADC_Handler,
 * -----------------------------------------------------
 * To:
 * -----------------------------------------------------
 * #if defined(ADC_IRQn) || defined(ID_ADC)
 * (void*) ADC_Handler,
 * -----------------------------------------------------
 */


#define WAIT_ADC_SYNC() while(ADC->STATUS.reg & ADC_STATUS_SYNCBUSY)

static uint16_t adc_get_value(void);
static void samr21_adc_init(sensor_type_t sensor);
static int8_t get_temperature(sensor_result_t *res);
#if BOARD==SAMR21_XPLAINED_PRO
static int8_t get_io_supply(sensor_result_t *res);
static int8_t get_analog_pin(sensor_result_t *res);
#endif

/*=== external functions =========================================================================*/
int8_t get_sensor_value(sensor_type_t sensor, sensor_result_t *res)
{
    int8_t ret;

    switch (sensor)
    {
        case TEMPERATURE:
            ret = get_temperature(res);
            break;
#if BOARD==SAMR21_XPLAINED_PRO
        case IO_SUPPLY:
            ret = get_io_supply(res);
            break;

        case ANALOG_PIN:
            ret = get_analog_pin(res);
            break;
#endif
        default:
            ret = -1;
            break;
    }

    return ret;
}

/*=== internal functions =========================================================================*/
static void samr21_adc_init(sensor_type_t sensor)
{
    uint8_t refsel1, refsel2;

    refsel1 = ADC->REFCTRL.bit.REFSEL;
    // generic Clock selection ID.. 0x1E = GCLK_ADC (page103 - Datasheet)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_ADC | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);

     //Power Manager - enable ADC (Page 122 - Datasheet)
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;

    ADC->CTRLA.reg = ADC_CTRLA_ENABLE;
    WAIT_ADC_SYNC();

    switch (sensor)
    {
        case TEMPERATURE:
            // 1 Volt Reference for Temperature Measurement
            ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INT1V;
            // sync-asynchronicity of CLK_ADC_APB and GLK_ADC=>some registers need sync
            WAIT_ADC_SYNC();
            // case  Temperature-measurement - select Temp-sensor-Inputs
            ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND | ADC_INPUTCTRL_MUXPOS_TEMP;
            WAIT_ADC_SYNC();
            // Temperature Sensor Output in VREF-Register enable
            SYSCTRL->VREF.reg = (SYSCTRL->VREF.reg & ~0x03) | SYSCTRL_VREF_TSEN;
            WAIT_ADC_SYNC();
            break;
#if BOARD==SAMR21_XPLAINED_PRO
        case IO_SUPPLY:
            ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INT1V;
            WAIT_ADC_SYNC();
            ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND | ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC;
            WAIT_ADC_SYNC();
            SYSCTRL->VREF.bit.BGOUTEN = 0;
            SYSCTRL->VREF.bit.TSEN = 0;
            WAIT_ADC_SYNC();
            break;

        case ANALOG_PIN:
            PORT->Group[0].DIRCLR.reg = PORT_PA07;
            PORT->Group[0].PINCFG[7].reg |= PORT_PINCFG_PMUXEN;
            PORT->Group[0].PMUX[7 / 2].bit.PMUXO |= PORT_PMUX_PMUXO_B_Val;
            ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INTVCC1;
            WAIT_ADC_SYNC();
            ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_IOGND | ADC_INPUTCTRL_MUXPOS_PIN7;
            WAIT_ADC_SYNC();
            SYSCTRL->VREF.bit.BGOUTEN = 1;
            SYSCTRL->VREF.bit.TSEN = 0;
            break;
#endif
        default:
            /* Invalid sensor value */
            return;
    }

    // Pre scaler & Resolution Selection (8MHz / 8 = 1Mhz used at page 1071 for temp. meas.)
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV8 | ADC_CTRLB_RESSEL_12BIT;

    // adjusting Result + Average Control 1...1024 samples possible
    ADC->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(4) | ADC_AVGCTRL_SAMPLENUM_32;
    WAIT_ADC_SYNC();

    // sampling Time length Control
    ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(4);
    WAIT_ADC_SYNC();

    // calibration values
    uint32_t lin0 = (*((uint32_t *)ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    uint32_t lin1 = (*((uint32_t *)ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos;
    uint32_t bias = (*((uint32_t *)ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
    ADC->CALIB.bit.LINEARITY_CAL = (lin1 << 5) | lin0;
    ADC->CALIB.bit.BIAS_CAL = bias;

    refsel2 = ADC->REFCTRL.bit.REFSEL;

    if (refsel1 != refsel2)
    {
        /*
         * refer to datasheet:
         *  30.6.2 Basic Operation
         *  30.6.2.1 Initialization
         *  Before enabling the ADC, the asynchronous clock source must be selected and enabled, and
         *  the ADC reference must be configured. The first conversion after the reference is changed
         * must not be used.
         */
        adc_get_value();
    }
}

#if ADC_USE_STANDBY
/*
 * ADC IRQ handler function
 */
void ADC_Handler(void)
{
    LED_Toggle(LED0);
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
}
#endif

static uint16_t adc_get_value(void)
{
    uint16_t adc = 0xffff;

    /* Step 1: Enable ADC */
#if ADC_USE_STANDBY
    ADC->INTENSET.reg = ADC_INTENSET_RESRDY;
    /* clear pending IRQs */
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    ADC->CTRLA.reg = ADC_CTRLA_RUNSTDBY | ADC_CTRLA_ENABLE;
    WAIT_ADC_SYNC();
    NVIC_EnableIRQ(ADC_IRQn);
#else
    ADC->CTRLA.reg = ADC_CTRLA_ENABLE;
    WAIT_ADC_SYNC();
#endif

    /* Step 2: Trigger ADC measurement */
    ADC->SWTRIG.reg = ADC_SWTRIG_START;
    WAIT_ADC_SYNC();

    /* Step 3: Get ADC value */
#if ADC_USE_STANDBY
    /* sleep during conversion */
    system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
    cpu_irq_enable();
    system_sleep();
    adc = ADC->RESULT.reg;
    NVIC_DisableIRQ(ADC_IRQn);
#else
    /* wait until conversion is ready */
    while (0 == (ADC->INTFLAG.reg & ADC_INTFLAG_RESRDY));
    adc = ADC->RESULT.reg;
#endif

    /* switch off ADC */
    ADC->CTRLA.reg = 0;
    WAIT_ADC_SYNC();

    return adc;
}

static int8_t get_temperature(sensor_result_t *res)
{
    samr21_adc_init(TEMPERATURE);
    res->adc_value = adc_get_value();
    res->error = ADC->INTFLAG.reg & ADC_INTFLAG_OVERRUN ? ADC_OVERRUN : SUCCESS;
    res->sensor_value = ((float) res->adc_value / (float) 4095 - (float) 0.667 ) / (float)0.0024 + 25;
    res->unit = 'C';
    return res->error;
}
#if BOARD==SAMR21_XPLAINED_PRO
static int8_t get_io_supply(sensor_result_t *res)
{
    samr21_adc_init(IO_SUPPLY);
    res->adc_value = adc_get_value();

    res->error = ADC->INTFLAG.reg & ADC_INTFLAG_OVERRUN ? ADC_OVERRUN : SUCCESS;
    // SCALED_IO_VCC = 1/4 VCC, 1V Reference means 4095 = 1V
    res->sensor_value =   4 * ((float) res->adc_value / 4095.0);
    res->unit = 'V';
    return res->error;
}

static int8_t get_analog_pin(sensor_result_t *res)
{
    /*
     * measure voltage at PA07;
     * the measured voltage needs to be between 0 and Vddana/2
     *
     */
     /* Or use a the potentiometer:
           VCC
            |
            +
           | |  R1 = 100k
           | |
            +
            |
            +
           | | R2 = 100k
           | o---> PA07
           | |
            +
            |
           GND

           Reference is VDDANA/2
    */
    samr21_adc_init(ANALOG_PIN);
    res->adc_value = adc_get_value();
    res->error = ADC->INTFLAG.reg & ADC_INTFLAG_OVERRUN ? ADC_OVERRUN : SUCCESS;

    res->sensor_value =  100 * (float) res->adc_value / 4095.0;
    res->unit = '%';
    return res->error;
}
#endif