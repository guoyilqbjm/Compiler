.data
_prompt: .asciiz "Enter an integer:"
_reg: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra

write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra

main:
li $t0, 1
li $t1, 2
move $t2, $t0
move $t3, $t1
li $t4, 2
add $t5, $t0, $t1
add $t6, $t5, $t2
add $t7, $t6, $t3
add $t7, $t7, $t4
move 0, $t7
move $v0, 0
jr $ra
