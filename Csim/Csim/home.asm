ADDI x5,  x0, 262 # C4 - do
ADDI x6,  x0, 294 # D4 - re
ADDI x7,  x0, 330 # E4 - mi
ADDI x8,  x0, 349 # F4 - fa
ADDI x9,  x0, 392 # G4 - sol
ADDI x10, x0, 440 # A4 - la
ADDI x11, x0, 494 # B4 - si
ADDI x12, x0, 523 # C5 - do tiz
ADDI x13, x0, 587 # D5 - re tiz
ADDI x14, x0, 659 # E5 - mi tiz
ADDI x15, x0, 698 # F5 - fa tiz
ADDI x16, x0, 784 # G5 - sol tiz
ADDI x17, x0, 196 # G3 - sol pes
ADDI x18, x0, 220 # A3 - la pes
ADDI x19, x0, 247 # B3 - si pes
ADDI x24, x0, 200 # kisa nota suresi
ADDI x25, x0, 400 # uzun nota suresi
SW   x24, 1008(x0) # 1. olcu
SW   x5,  1004(x0)
SW   x24, 1008(x0)
SW   x7,  1004(x0)
SW   x24, 1008(x0)
SW   x9,  1004(x0)
SW   x25, 1008(x0)
SW   x12, 1004(x0)
SW   x24, 1008(x0)
SW   x11, 1004(x0)
SW   x25, 1008(x0)
SW   x9,  1004(x0)
SW   x24, 1008(x0) # 2. olcu
SW   x6,  1004(x0)
SW   x24, 1008(x0)
SW   x8,  1004(x0)
SW   x24, 1008(x0)
SW   x10, 1004(x0)
SW   x25, 1008(x0)
SW   x13, 1004(x0)
SW   x24, 1008(x0)
SW   x12, 1004(x0)
SW   x25, 1008(x0)
SW   x10, 1004(x0)
SW   x24, 1008(x0) # 3. olcu
SW   x7,  1004(x0)
SW   x24, 1008(x0)
SW   x9,  1004(x0)
SW   x24, 1008(x0)
SW   x11, 1004(x0)
SW   x25, 1008(x0)
SW   x14, 1004(x0)
SW   x24, 1008(x0)
SW   x13, 1004(x0)
SW   x25, 1008(x0)
SW   x12, 1004(x0)
SW   x25, 1008(x0) # 4. olcu
SW   x16, 1004(x0)
SW   x24, 1008(x0)
SW   x15, 1004(x0)
SW   x24, 1008(x0)
SW   x14, 1004(x0)
SW   x25, 1008(x0)
SW   x12, 1004(x0)
SW   x24, 1008(x0)
SW   x11, 1004(x0)
SW   x25, 1008(x0)
SW   x9,  1004(x0)
SW   x24, 1008(x0) # 5. olcu
SW   x14, 1004(x0)
SW   x24, 1008(x0)
SW   x13, 1004(x0)
SW   x24, 1008(x0)
SW   x12, 1004(x0)
SW   x24, 1008(x0)
SW   x11, 1004(x0)
SW   x25, 1008(x0)
SW   x9,  1004(x0)
SW   x25, 1008(x0)
SW   x12, 1004(x0)
SW   x25, 1008(x0) # 6. olcu
SW   x17, 1004(x0)
SW   x24, 1008(x0)
SW   x18, 1004(x0)
SW   x24, 1008(x0)
SW   x19, 1004(x0)
SW   x25, 1008(x0)
SW   x5,  1004(x0)
HALT