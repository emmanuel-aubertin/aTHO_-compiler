# Gettin

In this part we will see how to compile a program in pascal like.

> :warning: This documentation is for the moment only made for Linux users :warning:

**Compiling without the makefile :**

To do this you will have two steps to follow (make sure you have build before).
The first thing to do is to compile the pascal like in ASM x86.
```
flex++ -d -otokeniser.cpp tokeniser.l && g++ -c tokeniser.cpp
g++ -ggdb -o aTHOlang compilateur.cpp tokeniser.o
./aTHOlang <input.aTHO>test.s
```
The ASM file `test.s` will then be created (or rewritten), it now needs to be compiled to a binary file, to do this you need to use gcc :
```
gcc -ggdb test.s -o test.bin
```
> If you get a "relocation R_X86_64_32S" error, try these options :
> ```gcc -ggdb -no-pie -fno-pie test.s -o test.bin```

After this, all you have to do is run `./test.bin`

**Compiling with the Makefile :**

To compile with the makefile you just have to do `make` in your work directory.
> By default the makefile will compile the `input.aTHO` file, you can customise the compilation in the `aTHOlangStep` part of the makefile (see [MakeFile](https://gitlab.com/aTHO_/ue_compilateur/-/blob/master/makefile)).

The makefile runs the resulting program, however you can run it manually with `./test.bin`

[:arrow_backward: HOME](home)  |  [PASCAL LIKE :arrow_forward:](Pascal-Like)

Updated 25 May 2021 by [Aubertin Emmanuel](https://www.linkedin.com/in/emmanuel-aubertin/)