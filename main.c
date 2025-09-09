#include <stdint.h>

#define SYSCTL_RCGC2_R      (*((volatile uint32_t *)0x400FE108))
#define GPIO_PORTF_DATA_R   (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R    (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R    (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_LOCK_R   (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R     (*((volatile uint32_t *)0x40025524))
#define GPIO_PORTF_PUR_R    (*((volatile uint32_t *)0x40025510))
#define GPIO_LOCK_KEY       0x4C4F434B

#define STCTRL              (*((volatile uint32_t *)0xE000E010))
#define STRELOAD            (*((volatile uint32_t *)0xE000E014))
#define STCURRENT           (*((volatile uint32_t *)0xE000E018))

#define RED_LED     (1 << 1)
#define BLUE_LED    (1 << 2 )
#define SW1         (1 << 4 )

int main(void) {
    SYSCTL_RCGC2_R |= 0x20;

    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= (RED_LED | BLUE_LED | SW1);
    GPIO_PORTF_DIR_R |= (RED_LED | BLUE_LED);
    GPIO_PORTF_DIR_R &= ~ (SW1);
    GPIO_PORTF_DEN_R |= (RED_LED | SW1 | BLUE_LED);
    GPIO_PORTF_PUR_R |= (SW1);



    STRELOAD = 16000000-1;
    STCURRENT = 0;
    STCTRL = (1 << 2) | (1 << 0);

    while (1) {
        if(STCTRL & (1 << 16)){
        GPIO_PORTF_DATA_R ^= RED_LED;
        }
        if((GPIO_PORTF_DATA_R & SW1) == 0){
                 GPIO_PORTF_DATA_R |= BLUE_LED;
             }
             else{
                 GPIO_PORTF_DATA_R &= ~ BLUE_LED;
         }
}
}
