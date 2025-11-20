#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"

#define PWM_FREQUENCY       50
#define SERVO_MIN_PULSE     1000
#define SERVO_MAX_PULSE     2000
#define SERVO_CENTER        1500

#define LIGHT_THRESHOLD     100
#define SERVO_STEP          2
#define UPDATE_DELAY        100

uint32_t pwm_load;
uint32_t adc_left;
uint32_t adc_right;
int32_t servo_angle = 90;

void InitializePWM(void);
void InitializeADC(void);
void SetServoAngle(int angle);
void ReadLightSensors(void);
void Delay_ms(uint32_t milliseconds);

int main(void)
{
    int32_t light_difference;

    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |
                   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    InitializePWM();
    InitializeADC();

    SetServoAngle(90);
    Delay_ms(1000);

    while(1)
    {
        ReadLightSensors();
        light_difference = (int32_t)adc_left - (int32_t)adc_right;

        if(light_difference > LIGHT_THRESHOLD)
        {
            if(servo_angle > 0)
            {
                servo_angle -= SERVO_STEP;
                if(servo_angle < 0)
                    servo_angle = 0;
                SetServoAngle(servo_angle);
            }
        }
        else if(light_difference < -LIGHT_THRESHOLD)
        {
            if(servo_angle < 180)
            {
                servo_angle += SERVO_STEP;
                if(servo_angle > 180)
                    servo_angle = 180;
                SetServoAngle(servo_angle);
            }
        }

        Delay_ms(UPDATE_DELAY);
    }
}

void InitializePWM(void)
{
    uint32_t pwm_clock;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);

    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    pwm_clock = SysCtlClockGet() / 64;

    pwm_load = (pwm_clock / PWM_FREQUENCY) - 1;

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, pwm_load);

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,
                     (pwm_load * SERVO_CENTER) / 20000);

    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

void InitializeADC(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    ADCSequenceDisable(ADC0_BASE, 3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
}

void SetServoAngle(int angle)
{
    uint32_t pulse_width;

    if(angle < 0)
        angle = 0;
    if(angle > 180)
        angle = 180;

    pulse_width = SERVO_MIN_PULSE +
                  ((angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) / 180);

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,
                     (pwm_load * pulse_width) / 20000);
}

void ReadLightSensors(void)
{
    uint32_t adc_buffer[1];

    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
                            ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false));
    ADCSequenceDataGet(ADC0_BASE, 3, adc_buffer);
    adc_left = adc_buffer[0];
    ADCSequenceDisable(ADC0_BASE, 3);

    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
                            ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false));
    ADCSequenceDataGet(ADC0_BASE, 3, adc_buffer);
    adc_right = adc_buffer[0];
    ADCSequenceDisable(ADC0_BASE, 3);
}

void Delay_ms(uint32_t milliseconds)
{
    SysCtlDelay((SysCtlClockGet() / 3000) * milliseconds);
}
