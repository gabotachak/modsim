base: base.c lib/lcgrand.c
	$(CC) base.c lib/lcgrand.c -o base -lm