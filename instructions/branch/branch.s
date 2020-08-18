lui x1, 5
lui x2, 3
lui x3, 1

_test2:
  bne x1, x3, _test

_test:
  sub x1, x1, x3
  beq x1, x3, _test3
  bne x1, x3, _test

_test3:
  lui x10, 20
  ebreak