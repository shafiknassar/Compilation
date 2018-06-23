DEBUG: func header
DEBUG: func name: foo
DEBUG: func header
DEBUG: befor call
DEBUG: usedRegs: 1
DEBUG: check if print
DEBUG: pass arg
DEBUG: reg name: $t0 reg type size: 1
DEBUG: 0
DEBUG: after call
DEBUG: befor call
DEBUG: usedRegs: 1
DEBUG: check if print
DEBUG: pass arg
DEBUG: reg name: $t0 reg type size: 1
DEBUG: 0
DEBUG: after call
.data
str_0: .asciiz "Error division by zero"
str_1: .asciiz "Error index out of bounds"
str_2: .asciiz "\n"
.text
printi:
    lw $a0,4($sp)
    li $v0,1
    syscall
    jr $ra
.end printi

print:
    lw $a0,4($sp)
    li $v0,4
    syscall
    jr $ra
.end print

div_by_zero_handler:
    la $a0,str_0
    li $v0,4
    syscall
    la $a0,str_2
    li $v0,4
    syscall
    li $v0,10
    syscall
.end div_by_zero_handler

index_out_of_bounds_handler:
    la $a0,str_1
    li $v0,4
    syscall
    la $a0,str_2
    li $v0,4
    syscall
    li $v0,10
    syscall
.end index_out_of_bounds_handler

foo:
    addu $sp, $sp, -4
    sw   $fp, ($sp)
    addu $fp, $sp, 4
    lw $t0, -4($fp)
    li $t1, 1
    add $t0, $t0, $t1
    move $v0, $t0
    move $sp,$fp
    sub $sp,$sp,4
    lw $fp,($sp)
    addu $sp,$sp,4
    jr $ra
.end foo

main:
    addu $sp, $sp, -4
    sw   $fp, ($sp)
    addu $fp, $sp, 4
    li $t0, 1
    sub $sp,$sp,4
    sw $t0,0($sp)
    sub $sp, $sp, 4
    sw $ra, 0($sp)
    lw $t0, 0($t0)
    sub $sp, $sp, 4
    sw $t0, 0($sp)
    jal foo
    addu $sp, $sp, 4
    lw $ra, 0($sp)
    addu $sp, $sp, 4
    lw $t0,0($sp)
    addu $sp, $sp, 4
    sw $zero, ($sp)
    addu $sp, $sp, 4
    sw $v0, 4($fp)
label_67:
    lw $t0, 0($fp)
    sub $sp,$sp,4
    sw $t0,0($sp)
    sub $sp, $sp, 4
    sw $ra, 0($sp)
    lw $t0, 0($t0)
    sub $sp, $sp, 4
    sw $t0, 0($sp)
    jal printi
    addu $sp, $sp, 4
    lw $ra, 0($sp)
    addu $sp, $sp, 4
    lw $t0,0($sp)
    addu $sp, $sp, 4
    li $v0,10
    syscall
.end main

