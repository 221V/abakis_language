; ABAKIS: MINIMAL FPU

include 'a.inc'

float f, g, e
float a=1.234567, b=6.543210, c=6.300004
float d=270.0, r=4.7, z=7.654321

text t(128), f1.t='FPU EXAMPLE:',\
 f2.t='CONVERT:', f3.t='MATH:'

function test.i 	  ; constants
  log f1.t, 'r'
  % f=1.0, g=0.0, e=pi()
  log.fp f, g, e
  log.r
endf

function test.convert
  locals i
  . i=1024
  log f2.t, 'r'
  % f=i2f(i), i=f2i(f)	  ; integer, float
  log.n i
  log.fp f
  % f=d2r(d), g=r2d(f)	  ; degrees, radians
  log.fp f, g
  log.r
endf

function test.math
  locals i
  log f3.t, 'r'
  % f=a+b, g=f-b, e=a*c   ; arithmetic
  log.fp f, g, e
  . i=8
  % e=i2f(i), f=sqrt(e)   ; square root
  log.fp f
  . i=120		  ; angle
  log.n i
  % e=i2f(i), e=d2r(e)	  ; convert
  % f=sin(e), g=cos(e)	  ; sine, cosine
  log.fp f, g
endf

function main
  test.i
  test.convert
  test.math
  show.log
endf