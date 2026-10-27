# 1. Configure CMake with the ARM toolchain and Ninja generator
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake

# 2. Compile and link build/minirtos.elf (must finish with 0 warnings)
cmake --build build

# 3. Launch QEMU paused (-S) with a GDB server on port 1234 (-s)
qemu-system-arm -M mps2-an385 -nographic -kernel build/minirtos.elf -s -S

# 4. Start GDB
gdb-multiarch build/minirtos.elf -ex "target remote :1234"