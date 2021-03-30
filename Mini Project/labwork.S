  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line

.global hexasc
.global delay
.global time2string
.global enable_interrupt
.data
.text

enable_interrupt:
    ei
    jr      $ra
    nop

hexasc:
    andi    $a0, $a0, 0xF
    li    $t2, 0x37
    li    $t1, 9
    ble    $a0, $t1, number
    add    $v0, $t2, $a0
    jr    $ra
    nop

number:
    addi    $t2, $0, 0x30
    add    $v0, $t2, $a0
    jr    $ra
    nop

delay:
addi    $t2,$0,4711       #ändra här för delay

while:
ble      $a0,$0,exit    # when ms = 0, exit
nop
addi    $a0,$a0,-1    # ms-1
addi    $t1,$0,0    #i=0

forloop:
bge     $t1,$t2, while
nop
addi    $t1,$t1,1
nop
j    forloop
nop

exit:
jr    $ra
nop


time2string:
PUSH    $ra
PUSH    $s0
PUSH    $s1
PUSH    $s2
PUSH    $s3
PUSH    $a0

andi     $a1, $a1, 0xFFFF
move    $s0, $a1        # store 1653 i s0
srl    $s1, $a1, 4        # store 165 i s1
srl    $s2, $a1, 8        # store 16 i s2
srl    $s3, $a1, 12        # store 1 i s3

move    $a0, $s0
jal    hexasc            #hexasc maskar ut 3
nop
move    $s0,$v0            #store 3 (1653) i S0

move    $a0, $s1
jal    hexasc            #hexasc maskar ut 5
nop
move    $s1,$v0            #store 5 (1653) i s1

move    $a0, $s2
jal    hexasc            #hexasc maskar ut 6
nop
move    $s2,$v0            #store 6 (1653) i s2

move    $a0, $s3
jal    hexasc            #hexasc maskar ut 1
nop
move    $s3,$v0            #store 1 (1653) i s3

#    s0 = 3, s1 = 5, s2 = 6, s3 = 1

POP    $a0

beq    $s0, 0x30, suprice


store:
sb    $s0, 4($a0)
sb    $s1, 3($a0)
addi    $v0, $0, 0x3A
sb    $v0, 2($a0)
sb    $s2, 1($a0)
sb    $s3, 0($a0)
sb    $0, 5($a0)

j    end
nop


suprice:
addi     $t3, $0,0x58
beq    $s0, $s1, hepp
j    store

hepp:
sb    $s0, 4($a0)
sb    $s1, 3($a0)
addi    $v0, $0, 0x3A
sb    $v0, 2($a0)
sb    $s2, 1($a0)
sb    $s3, 0($a0)
sb    $t3, 5($a0)
sb    $0, 6($a0)

j    end
nop

end:    POP    $s3
POP    $s2
POP    $s1
POP    $s0
POP    $ra

jr    $ra
nop
