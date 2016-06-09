.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
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

fib:
move $t0 $a0
move $t1 $a1
move $t2 $a2
move $t3 $a3
lw $t4 0($sp)
add $t5, $t0, $t1
add $t6, $t5, $t2
add $t7, $t6, $t3
sw $t2, 8($sp)
add $t2, $t7, $t4
move $v0, $t2
jr $ra

main:
sw $ra, 0($sp)
addi $sp, $sp, 4
jal read
addi $sp, $sp, -4
lw $ra, 0($sp)
move $t0, $v0
move $t1, $t0
sw $ra, 8($sp)
sw $t0, 0($sp)
sw $t1, 4($sp)
addi $sp, $sp, 12
jal read
addi $sp, $sp, -12
lw $ra, 8($sp)
lw $t0, 0($sp)
lw $t1, 4($sp)
move $t2, $v0
move $t3, $t2
sw $ra, 16($sp)
sw $t0, 0($sp)
sw $t1, 4($sp)
sw $t2, 8($sp)
sw $t3, 12($sp)
addi $sp, $sp, 20
jal read
addi $sp, $sp, -20
lw $ra, 16($sp)
lw $t0, 0($sp)
lw $t1, 4($sp)
lw $t2, 8($sp)
lw $t3, 12($sp)
move $t4, $v0
move $t5, $t4
sw $ra, 24($sp)
sw $t0, 0($sp)
sw $t1, 4($sp)
sw $t2, 8($sp)
sw $t3, 12($sp)
sw $t4, 16($sp)
sw $t5, 20($sp)
addi $sp, $sp, 28
jal read
addi $sp, $sp, -28
lw $ra, 24($sp)
lw $t0, 0($sp)
lw $t1, 4($sp)
lw $t2, 8($sp)
lw $t3, 12($sp)
lw $t4, 16($sp)
lw $t5, 20($sp)
move $t6, $v0
move $t7, $t6
sw $ra, 32($sp)
sw $t0, 0($sp)
sw $t1, 4($sp)
sw $t2, 8($sp)
sw $t3, 12($sp)
sw $t4, 16($sp)
sw $t5, 20($sp)
sw $t6, 24($sp)
sw $t7, 28($sp)
addi $sp, $sp, 36
jal read
addi $sp, $sp, -36
lw $ra, 32($sp)
lw $t0, 0($sp)
lw $t1, 4($sp)
lw $t2, 8($sp)
lw $t3, 12($sp)
lw $t4, 16($sp)
lw $t5, 20($sp)
lw $t6, 24($sp)
lw $t7, 28($sp)
sw $t4, 16($sp)
move $t4, $v0
sw $t1, 4($sp)
move $t1, $t4
sw $ra, 40($sp)
sw $t5, 20($sp)
lw $t5, 4($sp)
move $a0, $t5
move $a1, $t3
sw $t4, 32($sp)
lw $t4, 20($sp)
move $a2, $t4
move $a3, $t7
sw $t1, 44($sp)
sw $t0, 0($sp)
sw $t1, 36($sp)
sw $t2, 8($sp)
sw $t3, 12($sp)
sw $t4, 20($sp)
sw $t5, 4($sp)
sw $t6, 24($sp)
sw $t7, 28($sp)
addi $sp, $sp, 44
jal fib
addi $sp, $sp, -44
lw $ra, 40($sp)
lw $t0, 0($sp)
lw $t1, 36($sp)
lw $t2, 8($sp)
lw $t3, 12($sp)
lw $t4, 20($sp)
lw $t5, 4($sp)
lw $t6, 24($sp)
lw $t7, 28($sp)
sw $t4, 20($sp)
move $t4, $v0
sw $t7, 28($sp)
move $t7, $t4
sw $ra, 48($sp)
move $a0, $t7
sw $t0, 0($sp)
sw $t1, 36($sp)
sw $t2, 8($sp)
sw $t3, 12($sp)
sw $t4, 40($sp)
sw $t5, 4($sp)
sw $t6, 24($sp)
sw $t7, 44($sp)
addi $sp, $sp, 52
jal write
addi $sp, $sp, -52
lw $ra, 48($sp)
lw $t0, 0($sp)
lw $t1, 36($sp)
lw $t2, 8($sp)
lw $t3, 12($sp)
lw $t4, 40($sp)
lw $t5, 4($sp)
lw $t6, 24($sp)
lw $t7, 44($sp)
sw $t5, 4($sp)
li $t5, 0
move $v0, $t5
jr $ra
