# thp - a tiny Lisp interpretor

*thp* is a Tiny Hypertext Processo-`^w^w^w`“Weekend Lisp” interpretor.

Perhaps I should have named it *awl* -- A Weekend Lisp, or *yawl* -- You Figure It Out.

For that matter, it could also be said to be a “weakened” Lisp!

It requires *libsexp*:
https://bitbucket.org/dhoskin/libsexp

I have been running it on Plan 9, but it should work on Unix with *plan9port*,
or natively with a bit of `s/print/printf/g`.

### Examples

	(LET (
		(f (LAMBDA (x) (CONS goats x)))
		(g (LAMBDA (x) (CDR x))))
	
		(f (g (QUOTE (1 2 3)))))
	
	→ (goats 2 3)

#### Recursion!

	(LET ((f (LAMBDA (x) (IF (x (f (CDR x))) recursion!))))
		(f (QUOTE (a b c d e))))
	
	→ args: ((a b c d e))
	→ args: ((b c d e))
	→ args: ((c d e))
	→ args: ((d e))
	→ args: ((e))
	→ args: (nil)
	→ recursion!

#### Functions as arguments

	(LET (
		(f (LAMBDA (x) (CAR (CDR x))))
		(g (LAMBDA (f x) (f (CDR x)))))
	
		(g f (QUOTE (a b c))))
	→ c

### Notes

* There is no garbage collection.
* There is no tail-call optimization.
* It is dynamically scoped. (Lexical scope coming soon!)

		(LET ((f (LAMBDA (x) (CONS x y))))
			(LET ((y world))
			(f hello)))
	
		→ lambda
		→ args: (hello)
		→ (hello . world)

	* Closures are implemented, but full lexical scope is still not here.

			(LET ((f (LAMBDA (x) (LAMBDA (y) (CONS x y))))
				(helloer (f hello)))
				(helloer world))
			
			→ (hello . world)

* Built-in functions and special forms:
`CONS`, `CAR`, `CDR`, `IF`, `LAMBDA`, `CLOSURE`, `LET`, `QUOTE`.
* *thp* is a Lisp-1.
* There is no partial application.
* Builtins are baked into `eval`, which behaves magically on special atom names.
It would perhaps be saner to break them out into something like `(BUILTIN "CDR")`,
and then prepopulate the environment: `(LET (car (LAMBDA (l) (BUILTIN (CAR l)))))`.
Nonetheless, it is already possible to override builtins,
because they have a lower "precedence" than variables:

		(LET ((CAR CDR)) (CAR (QUOTE (a b c))))
		→ (b c)

* I think it might actually be pretty easy to allow user-defined special forms
by providing a `(SPECIAL-FORM ...)` wrapper that disables automatic evaluation of arguments,
along with an explicit `(EVAL ...)`.
* I sort of want to add Church numerals.

* 
	% wc -l thp.c ../libsexp/sexp.[ch]
		193 thp.c
		216 ../libsexp/sexp.c
		 30 ../libsexp/sexp.h
		439 total

* Goats!

### Acknowledgements

I would like to thank the first couple of chapters of SICP
for the discussion on special forms;
the Lisp 1.5 book for the outline of how eval evals,
and for reminding me that if they could do it in assembly language
on a 4 kiloword machine I should darn well be able
to do it in C with a gigabyte of RAM;
and lastly Wikipedia.
