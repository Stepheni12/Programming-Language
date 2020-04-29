# Programming Language
Created a small programming language as part of my undergraduate computer science degree.
Most of the code was written by myself, however, as part of the course we were given starter code created by our professor
to then adjust according to the implementation I decided to go with. 

In general, the language is very simple, it supports the creation of variables that are integers or strings. It can then perform basic operations on these variables dependent on their type. For example numbers and expressions that contain numbers follow an order of operations that allows for various types of computation. Strings have some basic functionality in that they can be sliced as well as multiplied by a number which will then output that same string repeated that many times. Feel free to email me if you have any additional questions.

The Programming language follows these grammar rules:

Prog := Slist<br>
Slist := Stmt SC { Slist }<br>
Stmt := VarStmt | SetStmt | PrintStmt | RepeatStmt<br>
VarStmt := VAR IDENT Expr<br>
SetStmt := SET IDENT Expr<br>
PrintStmt := PRINT Expr<br>
RepeatStmt := REPEAT Expr Stmt<br>
Expr := Term { (+|-) Term }<br>
Term := Factor { * Factor }<br>
Factor := Primary { [ Expr : Expr ] }<br>
Primary := IDENT | ICONST | SCONST | LPAREN Expr RPAREN<br>

Example code:

var x "hello";<br>
print x;<br>
