#include <stdint.h>

// ---------------- Base Addresses ----------------
#define SYSCTL_BASE       0x400FE000
#define GPIO_PORTF_BASE   0x40025000
#define NVIC_BASE         0xE000E100
#define SYSTICK_BASE      0xE000E010

// ---------------- Registers (offsets added) ----------------
// SYSCTL
#define SYSCTL_RCGCGPIO   (*((volatile uint32_t *)(SYSCTL_BASE + 0x608)))

// GPIO Port F
#define GPIO_PORTF_DATA   (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x3FC)))
#define GPIO_PORTF_DIR    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x400)))
#define GPIO_PORTF_DEN    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x51C)))
#define GPIO_PORTF_PUR    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x510)))
#define GPIO_PORTF_LOCK   (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x520)))
#define GPIO_PORTF_CR     (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x524)))
#define GPIO_PORTF_IS     (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x404)))
#define GPIO_PORTF_IBE    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x408)))
#define GPIO_PORTF_IEV    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x40C)))
#define GPIO_PORTF_ICR    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x41C)))
#define GPIO_PORTF_IM     (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x410)))
#define GPIO_PORTF_RIS    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x414)))

// NVIC
#define NVIC_EN0          (*((volatile uint32_t *)(NVIC_BASE + 0x000)))

// SysTick
#define NVIC_ST_CTRL      (*((volatile uint32_t *)(SYSTICK_BASE + 0x00)))
#define NVIC_ST_RELOAD    (*((volatile uint32_t *)(SYSTICK_BASE + 0x04)))
#define NVIC_ST_CURRENT   (*((volatile uint32_t *)(SYSTICK_BASE + 0x08)))

// ---------------- Functions ----------------
void gpioInit(void) {
    // 1. Enable clock for Port F
    SYSCTL_RCGCGPIO |= 0x20;
    while((SYSCTL_RCGCGPIO & 0x20) == 0);

    // 2. Unlock PF0 (needed for SW2)
    GPIO_PORTF_LOCK = 0x4C4F434B;
    GPIO_PORTF_CR   = 0x1F;   // allow PF0–PF4

    // 3. Set directions: PF1, PF2, PF3 outputs (LEDs); PF0, PF4 inputs (switches)
    GPIO_PORTF_DIR  = 0x0E;

    // 4. Enable pull-ups on PF0, PF4
    GPIO_PORTF_PUR  = 0x11;

    // 5. Enable digital on PF0–PF4
    GPIO_PORTF_DEN  = 0x1F;

    // --------- GPIO Interrupt setup ---------
    GPIO_PORTF_IS  &= ~0x11;   // PF0, PF4 edge-sensitive
    GPIO_PORTF_IBE &= ~0x11;   // not both edges
    GPIO_PORTF_IEV &= ~0x11;   // falling edge
    GPIO_PORTF_ICR  =  0x11;   // clear any prior interrupt
    GPIO_PORTF_IM  |=  0x11;   // unmask PF0, PF4 interrupts

    // NVIC enable for Port F (IRQ 30)
    NVIC_EN0 |= (1 << 30);
}

void SysTick_Init(uint32_t ticks) {
    NVIC_ST_CTRL = 0;                // disable SysTick
    NVIC_ST_RELOAD = ticks - 1;      // reload value
    NVIC_ST_CURRENT = 0;             // clear current
    NVIC_ST_CTRL = 0x07;             // enable SysTick INTEN + CLK_SRC + ENABLE
}

// ---------------- Interrupt Handlers ----------------
void SysTick_Handler(void) {
    GPIO_PORTF_DATA ^= 0x08;   // Toggle PF3 (Green LED)
}

void GPIOF_Handler(void) {
    // SW1 (PF4 pressed)
    if (GPIO_PORTF_RIS & 0x10) {
        GPIO_PORTF_ICR = 0x10;       // clear flag
        GPIO_PORTF_DATA ^= 0x04;     // toggle Blue LED
    }
    // SW2 (PF0 pressed)
    if (GPIO_PORTF_RIS & 0x01) {
        GPIO_PORTF_ICR = 0x01;       // clear flag
        GPIO_PORTF_DATA ^= 0x02;     // toggle Red LED
    }
}

// ---------------- Main ----------------
int main(void) {
    gpioInit();

    // SysTick every 0.5s (for 16 MHz system clock)
    SysTick_Init(8000000);

    while(1) {
        // main loop does nothing — LEDs are controlled by interrupts
    }
}
