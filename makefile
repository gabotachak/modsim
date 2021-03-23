base: base.c lib/lcgrand.c
	$(CC) base.c lib/lcgrand.c -o base -lm

9: Ejercicio1.9.c lib/lcgrand.c
	$(CC) Ejercicio1.9.c lib/lcgrand.c -o Ejercicio9 -lm

3: Ejercicio1.3.c lib/lcgrand.c
	$(CC) Ejercicio1.3.c lib/lcgrand.c -o Ejercicio3 -lm