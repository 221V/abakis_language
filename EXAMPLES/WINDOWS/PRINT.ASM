; ABAKIS EXAMPLE. SEE \INCLUDE\

; print t, f, ... ; print formatted text

include 'a.inc'

integer age=37

text t(64), name='Zia'

text f=+'Hi, %t. You are %n years old ',\
 'which is %hh/%bb'

text magic=+'I''m the magic genie. ',\
 'Ask a question then click OK'

text answers[]=\
 'Yes', 'No', 'Maybe', 'Definitely'

function main
  print t, f, name, age, age, age
  say t
  say magic
  random answers.$-1
  say [answers+r0*4]
endf