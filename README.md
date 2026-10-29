# 1. Configure CMake with the ARM toolchain and Ninja generator
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake

# 2. Compile and link build/minirtos.elf (must finish with 0 warnings)
cmake --build build

# 3. Launch QEMU paused (-S) with a GDB server on port 1234 (-s)
qemu-system-arm -M mps2-an385 -nographic -kernel build/minirtos.elf -s -S

# 4. Start GDB
gdb-multiarch build/minirtos.elf -ex "target remote :1234"

| Register | Address (Offset) | Access | Hardware Signals & Bit-by-Bit Behavior |
| :--- | :--- | :--- | :--- |
| **DATA** | 0x40004000 (+0x00) | Read / Write | **Bits [7:0]:** Although this is one memory address, in hardware it is wired to two separate 1-byte buffers:<br><br>• **WRITE:** Your byte goes into the TX Holding Buffer to be transmitted out on the TXD pin.<br>• **READ:** You read the byte sitting inside the RX Holding Buffer that arrived from the RXD pin. |
| **STATE** | 0x40004004 (+0x04) | Read / Write | Reports the live hardware status of the TX and RX buffers:<br><br>• **Bit 0 (TX_FULL):** 1 = The TX Holding Buffer is currently full (still busy shifting out the previous character). If the CPU writes to DATA while TX_FULL == 1, the byte is lost! 0 = Ready to accept a new byte.<br>• **Bit 1 (RX_FULL):** 1 = A complete byte has arrived from the RXD pin and is waiting in DATA to be read.<br>• **Bit 2 (TX_OVERRUN):** 1 = CPU wrote to DATA while TX_FULL was 1.<br>• **Bit 3 (RX_OVERRUN):** 1 = A new byte arrived on RXD before the CPU read the previous byte from DATA. |
| **CTRL** | 0x40004008 (+0x08) | Read / Write | Switches on the UART's internal state machines and interrupt wires:<br><br>• **Bit 0 (TX_EN):** 1 = Enables the transmitter logic (TXD pin).<br>• **Bit 1 (RX_EN):** 1 = Enables the receiver logic (RXD pin).<br>• **Bit 2 (TX_IRQ_EN):** 1 = Asserts the UART0TXINTR interrupt wire when TX finishes.<br>• **Bit 3 (RX_IRQ_EN):** 1 = Tells UART0 to pull the UART0RXINTR electrical wire HIGH whenever RX_FULL becomes 1 (a byte arrives). |
| **INTCLEAR** | 0x4000400C (+0x0C) | Write | Clears active interrupt signals on the UART peripheral:<br><br>• **Bit 0 (RX_INT_CLR):** Writing 1 here drops the UART0RXINTR wire back to LOW. (If you forget to write 1 here inside `UART0RX_Handler`, the interrupt wire stays HIGH and the CPU will re-enter `UART0RX_Handler` in an infinite loop forever!)<br>• **Bit 1 (TX_INT_CLR):** Writing 1 clears the TX interrupt signal. |
| **BAUDDIV** | 0x40004010 (+0x10) | Read / Write | **Bits [19:0]:** Clock divider that sets the transmission speed. On mps2-an385, the peripheral clock (PCLK) runs at 25 MHz (25,000,000 Hz). Setting BAUDDIV = 217 divides 25,000,000 / 217 ≈ 115,200 bits per second (115200 baud). |


0x00000000 - 0x1FFFFFFF  ──► Routed to FLASH (Code & Constants)
0x20000000 - 0x3FFFFFFF  ──► Routed to SRAM (Variables & Stack)
0x40000000 - 0x5FFFFFFF  ──► Routed to PERIPHERALS (UART0 at 0x40004000, Timers, GPIO)
0xE0000000 - 0xFFFFFFFF  ──► Routed to SYSTEM CONTROL (NVIC at 0xE000E100, SysTick)