// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
@R2
M=0
(LOOP)
@R1
D=M
@END
D;JLE
D=D-1
@R1
M=D
@R2
D=M
@R0
D=D+M
@R2
M=D

@LOOP
0;JMP
(END)
@END
0;JMP