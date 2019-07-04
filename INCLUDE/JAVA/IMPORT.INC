; $$$$$$$$$$$$ JASM : JAVA ASSEMBLER $$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ????????????????? IMPORT.INC ???????????????????

macro import name, package, [methods] {
 common
  utf8 _#name=`package#'/'#`name
  cclass name=_#name
 forward
  match m(x), methods \{
   m\#.method constant_nameandtype \
   _\#m, x \}
 forward
  match m(x), methods \{
   name\#.\#m constant_methodref \
   name, m\#.method \}
}

macro import.field class, member, type {
 class#.#member constant_fieldref \
  class, class#.#member#.field
 class#.#member#.field constant_nameandtype \
  _#member, type
 utf8 _#member=`member
}

import Object, 'java/lang', init(_VV)

import Integer, 'java/lang', init(_VV)

import String, 'java/lang', init(_VV)

import Files, 'java/nio', init(_VV)

import Color, 'java/awt', init(_VV)

import.field Color, red, _CT
import.field Color, blue, _CT

import Graphics, 'java/awt', init(_VV),\
 paint(_GV), drawString(_SIIV)

utf8 _paint='paint', _drawString='drawString'

import Applet, 'java/applet', init(_VV)

setBackground.method constant_nameandtype \
 _setBackground, _CV
Applet.setBackground constant_methodref \
 Applet, setBackground.method

utf8 _setBackground='setBackground'

import JFrame, 'javax/swing', init(_VV),\
 setVisible(_ZV), setSize(_IIV), setTitle(_SV)

utf8 _setVisible='setVisible',\
 _setSize='setSize', _setTitle='setTitle'

import System, 'java/lang', init(_VV)

import.field System, out, _PS

utf8 _PS='Ljava/io/PrintStream;'
import PrintStream, 'java/io', init(_VV)
PrintStream_println constant_methodref \
 PrintStream, println_method
println_method constant_nameandtype \
 _println, _SV
utf8 _println='println'