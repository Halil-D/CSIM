# ===== FREKANS VE SURE TANIMLARI =====
ADDI x5,  x0, 196   # G3
ADDI x6,  x0, 220   # A3
ADDI x7,  x0, 247   # B3
ADDI x8,  x0, 294   # D4
ADDI x9,  x0, 330   # E4
ADDI x10, x0, 392   # G4
ADDI x11, x0, 440   # A4
ADDI x12, x0, 494   # B4
ADDI x23, x0, 400   # Kisa Sure (x30 yerine)
ADDI x24, x0, 800   # Uzun Sure (x31 yerine)
ADDI x25, x0, 1600  # Cok Uzun Sure (x32 yerine)
# ----- 1. OLCU: B3 D4 G4 -----
SW   x23, 1008(x0)
SW   x7,  1004(x0)
SW   x23, 1008(x0)
SW   x8,  1004(x0)
SW   x24, 1008(x0)
SW   x10, 1004(x0)
# ----- 2. OLCU: A4 G4 D4 -----
SW   x23, 1008(x0)
SW   x11, 1004(x0)
SW   x23, 1008(x0)
SW   x10, 1004(x0)
SW   x24, 1008(x0)
SW   x8,  1004(x0)
# ----- 3. OLCU: B3 D4 G4 A4 -----
SW   x23, 1008(x0)
SW   x7,  1004(x0)
SW   x23, 1008(x0)
SW   x8,  1004(x0)
SW   x23, 1008(x0)
SW   x10, 1004(x0)
SW   x23, 1008(x0)
SW   x11, 1004(x0)
# ----- 4. OLCU: B4 Uzun Kapanis -----
SW   x25, 1008(x0)
SW   x12, 1004(x0)
# ----- 5. OLCU: G4 D4 E4 -----
SW   x23, 1008(x0)
SW   x10, 1004(x0)
SW   x23, 1008(x0)
SW   x8,  1004(x0)
SW   x24, 1008(x0)
SW   x9,  1004(x0)
# ----- 6. OLCU: G4 A4 B4 A4 -----
SW   x23, 1008(x0)
SW   x10, 1004(x0)
SW   x23, 1008(x0)
SW   x11, 1004(x0)
SW   x23, 1008(x0)
SW   x12, 1004(x0)
SW   x23, 1008(x0)
SW   x11, 1004(x0)
# ----- 7. OLCU: G4 D4 B3 -----
SW   x23, 1008(x0)
SW   x10, 1004(x0)
SW   x23, 1008(x0)
SW   x8,  1004(x0)
SW   x24, 1008(x0)
SW   x7,  1004(x0)
# ----- 8. OLCU: A3 B3 G3 Finale -----
SW   x23, 1008(x0)
SW   x6,  1004(x0)
SW   x23, 1008(x0)
SW   x7,  1004(x0)
SW   x25, 1008(x0)
SW   x5,  1004(x0)
HALT