; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ;;;;;;;;;;;;;;;;;;;; BMP ;;;;;;;;;;;;;;;;;;;;;;;

; see IMAGE.INC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; load 24PP .BMP, store as .IMAGE 15/16/24/32.
; for OSs, ROMs. warning: causes slow compile
; with 1+ MB worth of images. to compile fast
; without images, comment "; IMAGE name='abc'"

macro inject.image folder, name, bpp {
  local i, p, a, r, g, b,\
   x, y, w, h, wb
  virtual at 0
    p:: file name#'.BMP'
     ; CD#'/media/'#folder#'/'#name#'.BMP'
     ; '../../media/'#folder#'/'#name#'.BMP'
  END virtual
  IF ~bpp in <15,16,24,32>
    'Invalid BPP' name, bpp
  END IF
  load a hword from p:0
  IF a<>'BM'
    'Invalid signature' name
  END IF
  load a byte from p:1Ch
  IF a<>24
    'Must be 24BPP' name
  END IF
  load w word from p:12h
  load h word from p:16h
  db 'I', 0
  dh w, h
  db bpp, 0
  a=((3-((w*3)+3)) and 3)
  wb=(w*3)+a
  y=h
  WHILE y>0
    o=36h+((y-1)*wb)
    x=0
    WHILE x<w
      i=o+(x*3)
      load b byte from p:i
      load g byte from p:i+1
      load r byte from p:i+2
      IF bpp=32
        dw (r shl 16) or (g shl 8) or b
      ELSE IF bpp=24
        db r, g, b   ; or b, g, r
      ELSE IF bpp=16
        r=((r and 11111b)/8) shl 11
        g=((g and 111111b)/4) shl 5
        b=((b and 11111b)/8)
        dh r or g or b
      ELSE IF bpp=15
        r=((r and 11111b)/8) shl 10
        g=((g and 11111b)/8) shl 5
        b=((b and 11111b)/8)
        dh r or g or b
      END IF
      x=x+1
    END WHILE
    y=y-1
  END WHILE
}

; insert 8BPP .BMP as .IMAGE with palette.
; note: must use special .8 drawing

macro inject.image.8 folder, name {
  local i, p, a, b, c,\
   x, y, w, h, wb
  virtual at 0
    p:: file name#'.BMP'
     ; CD#'/media/'#folder#'/'#name#'.BMP'
     ; '../../media/'#folder#'/'#name#'.BMP'
  END virtual
  load a hword from p:0
  IF a<>'BM'
    'Invalid signature' name
  END IF
  load a byte from p:1Ch
  IF a<>8
    'Must be 8BPP' name
  END IF
  load w word from p:12h
  load h word from p:16h
  db 'I', 0
  dh w, h
  db 8, 0
  i=0
  WHILE i<256
    o=36h+(i*4)
    load b byte from p:o
    load g byte from p:o+1
    load r byte from p:o+2
    db b, g, r, 0
    i=i+1
  END WHILE
  a=((3-(w+3)) and 3)
  wb=w+a
  y=h
  WHILE y>0
    o=436h+((y-1)*wb)
    x=0
    WHILE x<w
      load c byte from p:o+x
      db c
      x=x+1
    END WHILE
    y=y-1
  END WHILE
}