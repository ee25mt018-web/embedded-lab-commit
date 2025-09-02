#include <stdint.h>              // Standard integer definitions
#include tm4c123gh6pm.h        // Header file with register addresses for TM4C123

void delay() {
    int i, j;
    for (i = 0; i < 50; i++) {       // Outer loop
        for (j = 0; j < 3180; j++) { // Inner loop to generate software delay
        }
    }
}

int main(void) {
    SYSCTL_RCGC2_R = 0x20;           // Enable clock for Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock Port F (to allow changes on PF0)
    GPIO_PORTF_CR_R = 0x1F;          // Commit register: allow changes on PF0–PF4
    GPIO_PORTF_DIR_R = 0x0E;         // Set PF1, PF2, PF3 as outputs (LEDs), PF0 & PF4 as inputs (switches)
    GPIO_PORTF_DEN_R = 0x1F;         // Enable digital function on PF0–PF4
    GPIO_PORTF_PUR_R = 0x11;         // Enable internal pull-up resistors on PF0 and PF4 (switches)

    int count = 0;                   // Counter variable for LED sequence

    while (1) {
        if ((GPIO_PORTF_DATA_R & 0x10) == 0) { // Check if SW1 (PF4) is pressed (active low)
            if (count == 0) {
                GPIO_PORTF_DATA_R = 0x02;      // Turn ON Red LED (PF1)
                delay();                       // Delay for debouncing and visibility
                count++;                       // Move to next state
            }
            else if (count == 1) {
                GPIO_PORTF_DATA_R = 0x08;      // Turn ON Green LED (PF3)
                delay();
                count++;
            }
            else if (count == 2) {
                GPIO_PORTF_DATA_R = 0x04;      // Turn ON Blue LED (PF2)
                delay();
                count++;
            }
            else {
                GPIO_PORTF_DATA_R = 0x00;      // Turn OFF all LEDs
                delay();
                count = 0;                     // Reset state back to start
            }
        }
    }
}

