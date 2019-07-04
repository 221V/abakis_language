; define cpu intel windows ; for windows
define cpu intel kolibri   ; for kolibri

; CD fix ''		   ; current directory
 CD fix '../..' 	  ; up 2 directories
; CD fix '/usbhd0/1'	     ; kolibri SD card

rept 1 { include CD#'/include/a.inc' }