base: base.c lib/lcgrand.c
	$(CC) base.c lib/lcgrand.c -o base -lm

9: Ejercicio1.9.c lib/lcgrand.c
	$(CC) Ejercicio1.9.c lib/lcgrand.c -o Ejercicio9 -lm