#include <stdint.h>
#include "tm4c123gh6pm.h"
volatile int duty = 160; // Start with 50%
void Timer0A_PWM_Init(void) {
SYSCTL_RCGCTIMER_R |= 1; // Enable Timer0
SYSCTL_RCGCGPIO_R |= 0x02; // Enable clock for Port B
GPIO_PORTB_AFSEL_R |= 0x40; // PB6
GPIO_PORTB_PCTL_R &= ~0x0F000000;
GPIO_PORTB_PCTL_R |= 0x07000000; //PB6 used as Timer for PWM generation
GPIO_PORTB_DEN_R |= 0x40;
TIMER0_CTL_R &= ~0x01; // Disable Timer0A
TIMER0_CFG_R = 0x04; // 16-bit mode
TIMER0_TAMR_R = 0x0A; // PWM mode
TIMER0_TAILR_R = 319; // Period = 320 ticks
TIMER0_TAMATCHR_R = duty; // Duty cycle start (50%)
TIMER0_CTL_R |= 0x01; // Enable Timer0A
}
void GPIOF_Init(void) {
SYSCTL_RCGCGPIO_R |= 0x20; // Enable Port F
GPIO_PORTF_LOCK_R = 0x4C4F434B;
GPIO_PORTF_CR_R = 0x1F;
GPIO_PORTF_DIR_R &= ~0x11; // PF0, PF4 as input
GPIO_PORTF_DEN_R |= 0x11;
GPIO_PORTF_PUR_R |= 0x11; // Pull-ups for switches
}
int main(void) {
GPIOF_Init();
Timer0A_PWM_Init();
while(1) {
if((GPIO_PORTF_DATA_R & 0x10) == 0) { // SW1 pressed (PF4)
if(duty < 319) duty += 16; // Increase duty (max ~100%)
TIMER0_TAMATCHR_R = duty;
while((GPIO_PORTF_DATA_R & 0x10) == 0);
}
if((GPIO_PORTF_DATA_R & 0x01) == 0) { // SW2 pressed (PF0)
if(duty > 0) duty -= 16; // Decrease duty (min 0%)
TIMER0_TAMATCHR_R = duty;
while((GPIO_PORTF_DATA_R & 0x01) == 0);
}
}
}
