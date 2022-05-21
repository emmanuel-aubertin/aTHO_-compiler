# aTHOlang

Welcome, this is the documentation for aTHOlang. As you will see aTHOlang is a Pascal-like langage.

Before you start with aTHOlang, it is important to note that the aTHO_ compiler is a subject of study for me. As it is under development, it has some syntax not yet available (variable typing, function, class ...).

> :warning: Like in Pascal your program must end with a `.` :warning:
## 1. **Variables :**

In aTHOlang, the variables must be protyped at the beginning of the program between brackets. For the assignment you are to do it when you want.
```pascal
[a, x]
x := 1;

(* Some code *)

a := x + 1;
```
You can also increment variables like this:
```pascal
a := a + 1
```
## 2. **IF statements :**

```pascal
[x]
x := 1;

IF (x > 0) THEN
x := 5;
ELSE
x := 0.
```
## 3. **FOR statements :**
```pascal
[i]

i := 0;

FOR 0 To 4 DO
    DISPLAY "IN FOR\n";
    FOR 0 To 2 DO
        i := 1 + i
        DISPLAY "\ta = ";
        DISPLAY i;
        DISPLAY "\tIN FOR\n";
    END;
END;

DISPLAY "Goodbye\n".
```
## 3. **WHILE statements :**

```pascal
[x]

x := 0;

WHILE x < 4 DO
x := x + 2;
END.
```
## 4. **BLOCK statements :**

```pascal
[x]

x := 0;

BEGIN
x := x + 2;
END.
```

# 5. **DISPLAY statement :**

DISPLAY is an equivalent to print in python. At the moment you can DISPLAY an entire expression or a string. DISPLAY don't do a break-line bye default.



**Print an integer :** 
```pascal
[a]
a := 4;
DISPLAY a + 2.
```
Output : 6

**Print a string :**
```pascal
DISPLAY "Hello World !\n".
```

Output : Hello World !


DISPLAY Now support color :smile: !! Some ANSI features may not work on some terminals.

```
DISPLAY "\033[1m[OK]\033[0m\n"
```

Output : [OK] (But in green)

The color works exactly like in bash ([See here](https://misc.flogisoft.com/bash/tip_colors_and_formatting)).

# 6. **PROCEDURE :**

The procedure works like a function but without argument, you declare it and you can use it by calling it.

```
PROCEDURE PrintFine() ;
BEGIN
    DISPLAY "This is fine :)\n"
END ;

PrintFine;
DISPLAY "-----\n"
PrintFine;
.
```
Output :
```
This is fine :)
-----
This is fine :)
```

# 6. **VAR statement :**

With the VAR statement you can declare variable with a type. There is for the moment only 4 type (INT, BOOL, CHAR, DOUBLE).

```
VAR b,x: INT;
VAR test, yepchar: CHAR;
VAR pi, exp: DOUBLE;
```



----
[:homes:  GO HOME :homes:](home)

Updated 28 May 2021 by [Aubertin Emmanuel](https://www.linkedin.com/in/emmanuel-aubertin/)