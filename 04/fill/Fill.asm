// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
@16384
D=A
@2
M=D

@24576
D=A
@3
M=D

(check)
@24576
D=M
@changeToFill
D;JGT
@0
M=0;
@start
0;JMP
(changeToFill)
@0
M=-1
(start)
@2
D=M
@3
D=D-M
@fill
D;JLT
@16384
D=A
@2
M=D
(fill)
@0
D=M
@2
A=M
M=D
@2
M=M+1
@check
0;JMP