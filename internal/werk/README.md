# (change me) WERK service internal

WERK = werelaxe + rkhapov = Wery drEvnii Russkii Komputer

## Архитектура

### Словарь
- vd = vm descriptor - уникальное число, идентифицирующее виртуальную машину 

### Устройства
- экран монохромный 320*240
- serial - поток символов (или hex кодов, в зависимости от режима), доступный только автору вм
и использующийся для отладки программ
- memory - `[0x0000...0xffff]`
- настраиваемый таймер, вызывает обработчик прерывания по истечению времени

### Memory mapping
`0x0000-0x000D` - meta (писать\читать в неё напрямую или через стек запрещено)
`0x0000-0x0007` - vd владельца памяти

`0x0008-0x000D` - interrupts vector

`0x000E-0x258D` - video memory
`0x3000-...` - область, используемая ВМ

### Interrupts

- int0 - таймер
- int1 - serial out
- int2 - serial in
- hlt - сон до появления прерывания

### Registers

- V[0..F] - регистры общего назначения, каждый имеет младшую и старшую части size = 2 (V1H, V1L)
- PC - указатель на след. инструкцию size = 2
- SP - указатель вершины стека size = 2
- F - регистр флагов для условных переходов size = 1
- X, Y - регистры указания координат для операций с экраном, size = 2

### Instruction set
```
R - register (V, X, Y)
С - constant value

w - word
b - byte

movw R1, R2 # R2 = R1
movw C1, R1 # R1 = C1
movb C1, R1 # R1L\R1H = C1

loadw $ADDR, R1
loadb $ADDR, R1
storew R1, $ADDR
storeb R1, $ADDR

addw R1, R2, R3 # R3 = R1 + R2
addb R1, R2, R3 # R3 = R1 + R2
sub, mul, div(?) etc...
setint C, $ADDR

setpixel C # screen[y, x] = C (1/0)
setpixel R1 # screen[y, x] = R1 (1/0)
clearscreen C
clearscreen R
# так же можно писать напрямую в отображаемую память экрана, но это сложнее

out C1 # print C1 to serial as symbol value
out R1 # print R1 to serial as symbol value
hexout C1 # print C1 to serial as hex value
hexout R1 # print R1 to serial as hex value

cmpb
cmpw
cmp C1, R1
cmp R1, C1
cmp R1, R2
cmp C1, C2
# set F according to OP1 - OP2

je $ADDR
jne $ADDR
jl $ADDR
jg $ADDR
jle $ADDR
jge $ADDR

```