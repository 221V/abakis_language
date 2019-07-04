; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? COLOR.INC ???????????????????

?BPP=15
?RGB='bgr'

;;;;;;;;;;;;;;;;; STANDARD COLORS ;;;;;;;;;;;;;;;;

BLACK=0

IF ?BPP=15
 WHITE   = 111111111111111b
 GRAY25  = 001110011100111b
 GRAY    = 011110111101111b
 GRAY75  = 110001100011000b
 LT.GRAY = 111001110011100b
 IF ?RGB='rgb'
  RED    = 111110000000000b
  GREEN  = 000001111100000b
  BLUE   = 000000000011111b
 ELSE IF ?RGB='bgr'
  RED    = 000000000011111b
  GREEN  = 000001111100000b
  BLUE   = 111110000000000b
 END IF
END IF

CYAN    = GREEN+BLUE
MAGENTA = RED+BLUE
YELLOW  = RED+GREEN