base: base.c lib/lcgrand.c
	$(CC) base.c lib/lcgrand.c -o base -lm

3: Ejercicio1.3.c lib/lcgrand.c
	$(CC) Ejercicio1.3.c lib/lcgrand.c -o Ejercicio3 -lm

5: Ejercicio1.5.c lib/lcgrand.c
	$(CC) Ejercicio1.5.c lib/lcgrand.c -o Ejercicio5 -lm

7: Ejercicio1.7.c lib/lcgrand.c
	$(CC) Ejercicio1.7.c lib/lcgrand.c -o Ejercicio7 -lm

9: Ejercicio1.9.c lib/lcgrand.c
	$(CC) Ejercicio1.9.c lib/lcgrand.c -o Ejercicio9 -lm