# bare metal arm specification
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# cross-compiler executables
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# skips the compilation/linking of dummy executable program -> "Check for working C compiler: /usr/bin/arm-none-eabi-gcc - skipped"
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) 

# only search for headers/libraries in the target toolchain directories, not the host PC
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)