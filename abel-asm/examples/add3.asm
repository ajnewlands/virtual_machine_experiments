; Equivalent to the hardcoded add3 program in abel's main.cpp

A EQU 4
B EQU 3
C EQU 2
D EQU 1

section .data

section .text

function add3(a, b, c, d):
    load @a
    load @b
    add
    load @c
    add
    load @d
    add
    ret
end

function _main():
    var a, b, c, d
    push A
    store @a
    push B
    store @b
    push C
    store @c
    push D
    store @d
    load @a
    load @b
    load @c
    load @d
    call add3
    pop
    ret
end
