#include <stdint.h>              // Standard integer definitions
#include "tm4c123gh6pm.h"        // TM4C123 register definitions

int main(void) {

    SYSCTL_RCGC2_R = 0x00000020;     // Enable clock for Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock Port F (required for PF0)
    GPIO_PORTF_CR_R = 0x03;          // Allow changes on PF0 (switch) and PF1 (LED)
    GPIO_PORTF_DIR_R = 0x02;         // Set PF1 as output (Red LED), PF0 as input
    GPIO_PORTF_DEN_R = 0x03;         // Enable digital function for PF0 and PF1
    GPIO_PORTF_PUR_R = 0x01;         // Enable pull-up resistor on PF0 (switch input)

    while (1) {
        if ((GPIO_PORTF_DATA_R & 0x01) == 0) {   // If switch (PF0) is pressed (active low)
            GPIO_PORTF_DATA_R = 0x02;            // Turn ON Red LED (PF1 high)
        }
        else {                                   // If switch not pressed
            GPIO_PORTF_DATA_R &= ~0x02;          // Turn OFF Red LED (PF1 low)
        }
    }
}
