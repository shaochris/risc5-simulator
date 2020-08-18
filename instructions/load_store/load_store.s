lui x1, 16
lui x2, 2
lui x3, 0x0300
lui x15, 0x0400
lui x16, 0x0500
lui x17, 0x060

sb x1, 0(x3)
lb x4, 0(x3)
lbu x5, 0(x3)

sh x1, 0(x15)
lh x6, 0(x15)
lhu x7, 0(x15)

sw x1, 0(x16)
lw x8, 0(x16)
lwu x9, 0(x16)

sd x1, 0(x17)
ld x10, 0(x17)

ebreak

