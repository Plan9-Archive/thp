#include <u.h>
#include <libc.h>

#include <sexp.h>

O *eval(O *o, O *env);

O *
evala(O *o, O *env)
{
	O *v;
	char *a;

	if(o == nil)
		return nil;
	if(o->type != Atom)
		return nil;

	a = atomstr(o);

	for(v = env; v != nil; v = cdr(v)){
		if(strcmp(atomstr(car(car(v))), a) == 0)
			return car(cdr(car(v)));
	}
	return o;
}

O *
evalargs(O *o, O *env)
{
	if(o == nil)
		return nil;
	if(o->type != List)
		return o;

	return cons(eval(car(o), env), evalargs(cdr(o), env));
}

/* XXX how to do partial application? */
O *
args2env(O *proto, O *args, O *env)
{
	if(proto == nil)
		return env;

	return cons(cons(car(proto), cons(car(args), nil)), args2env(cdr(proto), cdr(args), env));
}

/* XXX expensive */
O *
splice(O *head, O *tail)
{
	if(head == nil)
		return tail;
	return cons(car(head), splice(cdr(head), tail));
}

O *
lambda(O *o, O *env)
{
	O *f, *args;

	if(o == nil)
		return nil;

	f = car(o);
	if(f == nil)
		return nil;
	if(f->type != List)
		return nil;

	if(strcmp(atomstr(car(f)), "CLOSURE") == 0){
		env =  splice(car(cdr(f)), env); /* XXX expensive! especially on every function call! */
		args = evalargs(cdr(o), env);
		env = args2env(car(cdr(cdr(f))), args, env);
		return eval(car(cdr(cdr(cdr(f)))), env);
	}

	if(strcmp(atomstr(car(f)), "LAMBDA") == 0){
		args = evalargs(cdr(o), env);
		//print("lambda\n");
		//print("args: "); prin1(args); print("\n");
		env = args2env(car(cdr(f)), args, env);
		//print("env: "); prin1(env); print("\n");
		//print("body: "); prin1(cdr(cdr(f))); print("\n");
		//print("evaling body: "); prin1(eval(cdr(cdr(f)), env)); print("\n");
		return eval(car(cdr(cdr(f))), env);
	}

	print("reached bottom of lambda\n");
	return nil;
}

O *
eval(O *o, O *env)
{
	char *a;
	O *vars;

	if(o == nil)
		return nil;
	if(o->type != List)
		return evala(o, env);

	o = cons(eval(car(o), env), cdr(o));

	if(car(o) != nil && car(o)->type == List)
		return lambda(o, env);

	a = atomstr(car(o));

	if(strcmp(a, "CLOSURE") == 0){
		return o;
	}
	if(strcmp(a, "LAMBDA") == 0){
		return cons(mka("CLOSURE"), cons(env, cdr(o)));
	}
	o = cdr(o);

	if(strcmp(a, "CONS") == 0){
		o = evalargs(o, env);
		return cons(car(o), car(cdr(o)));
	}
	if(strcmp(a, "CAR") == 0)
		return car(eval(car(o), env));
	if(strcmp(a, "CDR") == 0)
		return cdr(eval(car(o), env));
	if(strcmp(a, "IF") == 0){
		/* (IF (cond expr) expr) */
		if(eval(car(car(o)), env) != nil)
			return eval(car(cdr(car(o))), env);
		else
			return eval(car(cdr(o)), env);
	}
	if(strcmp(a, "LET") == 0){
		/* (let ((x 6) (n 3)) (cond etc...)) */
		for(vars = car(o); vars != nil; vars = cdr(vars))
			env = cons(cons(car(car(vars)), cons(eval(car(cdr(car(vars))), env), nil)), env);
		//prin1(env);
		//prin1(car(cdr(o)));
		return eval(car(cdr(o)), env);
	}
	if(strcmp(a, "QUOTE") == 0)
		return car(o);

	print("eval reached bottom oops\n");
	return nil;
}
	
void
main(void)
{
	O *o;

	initsexp();
	for(;;){
		o = eval(r(), nil);
		prin1(o);
		print("\n");
	}
	exits("");

}
