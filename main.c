#include <stdint.h>   // Standard integer definitions

// ---------------- Register Definitions ----------------
#define SYSCTL_RCGC2_R     (*((volatile uint32_t *)0x400FE108)) 
#define GPIO_PORTF_DATA_R  (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R   (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R   (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_LOCK_R  (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R    (*((volatile uint32_t *)0x40025524))
#define GPIO_PORTF_PUR_R   (*((volatile uint32_t *)0x40025510))

#define GPIO_UNLOCK_KEY    0x4C4F434B   // Unlock value for PF0

// ---------------- Pin Masks ----------------
#define LED_RED    (1 << 1)   // PF1
#define LED_BLUE   (1 << 2)   // PF2
#define LED_GREEN  (1 << 3)   // PF3
#define SWITCH1    (1 << 4)   // PF4
#define SWITCH2    (1 << 0)   // PF0

int main(void) {
    // Enable clock to Port F
    SYSCTL_RCGC2_R |= (1 << 5);

    // Unlock PF0 and commit changes
    GPIO_PORTF_LOCK_R = GPIO_UNLOCK_KEY;
    GPIO_PORTF_CR_R   |= (LED_RED | LED_BLUE | LED_GREEN | SWITCH1 | SWITCH2);

    // Configure LEDs as output
    GPIO_PORTF_DIR_R  |= (LED_RED | LED_BLUE); 
    // Configure switches (PF0, PF4) and green LED as input
    GPIO_PORTF_DIR_R  &= ~(SWITCH1 | SWITCH2 | LED_GREEN);

    // Enable digital I/O
    GPIO_PORTF_DEN_R  |= (LED_RED | LED_BLUE | LED_GREEN | SWITCH1 | SWITCH2);

    // Activate pull-up resistors on the switches
    GPIO_PORTF_PUR_R  |= (SWITCH1 | SWITCH2);

    while (1) {
        // Handle Switch 1 (PF4 ? controls BLUE LED)
        if (!(GPIO_PORTF_DATA_R & SWITCH1)) {     
            GPIO_PORTF_DATA_R |= LED_BLUE;       // Pressed ? ON
        } else {
            GPIO_PORTF_DATA_R &= ~LED_BLUE;      // Released ? OFF
        }

        // Handle Switch 2 (PF0 ? controls RED LED)
        if (!(GPIO_PORTF_DATA_R & SWITCH2)) {    
            GPIO_PORTF_DATA_R |= LED_RED;        // Pressed ? ON
        } else {
            GPIO_PORTF_DATA_R &= ~LED_RED;       // Released ? OFF
        }
    }
}
