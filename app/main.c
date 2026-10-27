#include <stdint.h>

/* .rodata, FLASH */
const uint32_t flash_const = 0xCAFEBABE;

/* .data, FLASH -> RAM */
volatile uint32_t data_var = 0xDEADBEEF;

/* .bss, initialized in RAM */
volatile uint32_t bss_var;

/* .bss because initialized to 0 */
volatile uint32_t loop_counter = 0;

int main(void)
{
    // taking &flash_const forces the linker to keep flash_const in Flash 
    const volatile uint32_t *p = &flash_const;
    data_var = *p + 1; /* becomes 0xCAFEBABF */

    if (flash_const == 0xCAFEBABE) {
        data_var += 1; /* becomes 0xCAFEBAC0 */
        bss_var +=1;
    }

    while (1) {
        loop_counter++;
    }

    return 0;
}