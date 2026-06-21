ADDI x20, x0, 65 # Letter A
ADDI x21, x0, 0  # Counter
ADDI x22, x0, 4  # Upper limit
ADDI x23, x0, 90 # Z
ADDI x24, x0, 0
ADDI x25, x0, 10 # New line
SW   x25, 50(x0) # Put lew line char to memory

SLLI x24, x24, 8
OR   x24, x24, x20
ADDI x20, x20, 1
ADDI x21, x21, 1
BLT  x21, x22, 5
SW x24, 100(x0)   # Print 4 Letters 
ADDI x17, x0, 1,  # Stdout
ADDI x11, x0, 100 # Memory Location
ADDI x12, x0, 4   # Byte count
ECALL
ADDI x21, x0, 0   # Reset char counter
ADDI x11, x0, 50 # Print New Line
ADDI x12, x0, 1   # Byte count
ECALL
BLT  x20, x23, 6

ADDI x21, x0, 0

HALT