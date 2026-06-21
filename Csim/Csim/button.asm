ADDI x21, x0, 1
ADDI x30, x0, 330  # Buzzer duration
SW   x30, 1008(x0) # Load buffer duration 
LW x20, 200(x0)    # Read LED button state (DEFUNCT)
SW x20, 100(x0)    # Update LED state (DEFUNCT)
LW x22, 400(x0)    # Read close button state
BEQ x22, x0, 1
HALT