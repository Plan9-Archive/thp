</$objtype/mkfile

thp: thp.c
	$CC thp.c
	$LD -o thp thp.$O
