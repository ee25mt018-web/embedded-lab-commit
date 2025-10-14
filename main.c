#include <stdint.h>
#include <stdio.h>
uint32_t fadd(uint32_t a, uint32_t b)
{
    // Extract exponent (8 bits) and mantissa (23 bits)
    uint32_t exp_a  = (a >> 23) & 0xFF;
    uint32_t exp_b  = (b >> 23) & 0xFF;
    uint32_t mant_a = a & 0x7FFFFF;
    uint32_t mant_b = b & 0x7FFFFF;

    // Add the hidden '1' bit for normalized numbers
    mant_a |= 0x800000;
    mant_b |= 0x800000;

    // Align exponents
    if (exp_a > exp_b)
    {
        mant_b >>= (exp_a - exp_b);
        exp_b = exp_a;
    }
    else if (exp_b > exp_a)
    {
        mant_a >>= (exp_b - exp_a);
        exp_a = exp_b;
    }

    // Add mantissas (both positive)
    uint32_t mant_sum = mant_a + mant_b;
    uint32_t exp_sum = exp_a;

    // Normalize if carry bit overflows
    if (mant_sum & 0x1000000)
    {
        mant_sum >>= 1;
        exp_sum += 1;
    }
    mant_sum &= 0x7FFFFF;

    //floating point generation
    uint32_t result = (exp_sum << 23) | mant_sum;
    return result;
}
struct
{
    uint32_t u;     // 32-bit IEEE format computed by fadd
    float f;        // reference result computed by c = a + b
} buf[0x100];
int index;

void fadd_test(float a, float b)
{
    union
    {
        uint32_t u;     // access as 32-bit raw bits
        float f;        // access as a floating-point number
    } A, B, C;

    float c;

    A.f = a;
    B.f = b;

    C.u = fadd(A.u, B.u);       // output of asm code
    c = a + b;                  // reference output by C code

    if (index < 0x100)
    {
        buf[index].u = C.u;     // asm result
        buf[index].f = c;       // C result
        index++;
    }

    return;
}

int main(void)
{
    fadd_test(100.0f, 0.25f);       // 100.25f  (0x42C88000)
    fadd_test(1.5f, 1.5f);          // 3.0f     (0x40400000)
    fadd_test(2.75f, 1.5f);         // 4.25f    (0x40880000)
    fadd_test(1024.0f, 1.0f);       // 1025.0f  (0x44802000)
    fadd_test(1.0f, 0.125f);        // 1.125f   (0x3F900000)
    fadd_test(65536.0f, 256.0f);    // 65792.0f (0x47808000)

    while (1);

    return 0;
}
