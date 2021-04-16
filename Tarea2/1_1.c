#include "lib/simlib.h"

// Variables del sistema

#define EVENT_PASS_ARRIVAL_A 1	//Llegada de pasajeros fila A
#define EVENT_PASS_ARRIVAL_B 2  	//Llegada de pasajeros fila B
#define EVENT_BUS_ARRIVAL_A 3   	//Llegada Bus fila A
#define EVENT_BUS_ARRIVAL_B 4 	//Llegada Bus fila B

#define LIST_PASS_A 1			//Lista pasajeros fila A
#define LIST_PASS_B 2	   		//Lista pasajeros fila B
#define LIST_PASS_BUS 3	   		//Lista pasajeros bus
#define LIST_PASS_SERVED 4		//Lista pasajeros servidos

#define BUS_IN_A 1				//Bus en A
#define BUS_IN_B 2				//Bus en B

#define SAMPST 1	   			//

int N, bus_state;
float init_time, end_time, bus_mean, bus_std, a_mean, b_mean, total_time, wait_time;


FILE *infile, *outfile;

float normal(float mean, float std);
float poisson(float mean);

void init_model(void);
void pass_arrival_a(void);
void pass_arrival_b(void);
void bus_arrival_a(void);
void bus_arrival_b(void);
void report(void);

int main()
{
	infile = fopen("1_1.in", "r");
	outfile = fopen("1_1.out", "w");
	fscanf(infile, "%f %f %d %f %f %f %f", &init_time, &end_time, &N, &bus_mean, &bus_std, &a_mean, &b_mean);
	fprintf(outfile, "Simulación de buses\n");
	fprintf(outfile, "Tiempo de inicio a las %.2f horas\n", init_time);
	fprintf(outfile, "Tiempo de fin a las %.2f horas\n", end_time);
	fprintf(outfile, "Tiempo total de simulación: %.2f horas\n", end_time-init_time);
	fprintf(outfile, "Capacidad total de pasajeros en el bus (N): %d\n", N);
	fprintf(outfile, "Promedio de llegada a A en distribución de Poisson: %.2f\n", a_mean);
	fprintf(outfile, "Promedio de llegada a B en distribución de Poisson: %.2f\n", b_mean);
	init_simlib();
	maxatr = 4;
	init_model();
	while (sim_time < total_time)
	{
		timing();
		switch (next_event_type)
		{
		case EVENT_PASS_ARRIVAL_A:
			pass_arrival_a();
			break;
		case EVENT_PASS_ARRIVAL_B:
			pass_arrival_b();
			break;
		case EVENT_BUS_ARRIVAL_A:
			bus_arrival_a();
			break;
		case EVENT_BUS_ARRIVAL_B:
			bus_arrival_b();
			break;
		}
	}
	report();
	fclose(infile);
	fclose(outfile);
	return 0;
}

void init_model(void)
{
	total_time = (end_time - init_time) * 60;
	a_mean = a_mean / 60;
	b_mean = b_mean / 60;
	wait_time = 0;
	event_schedule(sim_time + normal(bus_mean, bus_std), EVENT_BUS_ARRIVAL_A);
	event_schedule(sim_time + poisson(a_mean), EVENT_PASS_ARRIVAL_A);
	event_schedule(sim_time + poisson(a_mean), EVENT_PASS_ARRIVAL_B);
}

void pass_arrival_a(void)
{
	event_schedule(sim_time + poisson(a_mean), EVENT_PASS_ARRIVAL_A);
	list_file(LAST, LIST_PASS_A);
}

void pass_arrival_b(void)
{
	event_schedule(sim_time + poisson(b_mean), EVENT_PASS_ARRIVAL_B);
	list_file(LAST, LIST_PASS_B);
}

void bus_arrival_a(void)
{
	bus_state = BUS_IN_A;
	event_schedule(sim_time + normal(bus_mean, bus_std), EVENT_BUS_ARRIVAL_A);
	
	while (list_size[LIST_PASS_BUS] != 0)
	{
		list_remove(FIRST, LIST_PASS_BUS);
		list_file(LAST, LIST_PASS_SERVED);
		printf("%d\n",list_size[LIST_PASS_BUS]);
	}

	while (list_size[LIST_PASS_BUS] < N)
	{
		//printf("%d\n",list_size[LIST_PASS_BUS]);
		if (list_size[LIST_PASS_A] != 0)
		{
			list_remove(FIRST, LIST_PASS_A);
			list_file(FIRST, LIST_PASS_BUS);
		}
	}

	event_schedule(sim_time + normal(bus_mean, bus_std), EVENT_BUS_ARRIVAL_B);
}

void bus_arrival_b(void)
{
	bus_state = BUS_IN_B;
	event_schedule(sim_time + normal(bus_mean, bus_std), EVENT_BUS_ARRIVAL_B);
	
	while (list_size[LIST_PASS_BUS] != 0)
	{
		list_remove(FIRST, LIST_PASS_BUS);
		list_file(LAST, LIST_PASS_SERVED);
	}

	while (list_size[LIST_PASS_BUS] < N)
	{
		if (list_size[LIST_PASS_B] != 0)
		{
			list_remove(FIRST, LIST_PASS_B);
			list_file(FIRST, LIST_PASS_BUS);
		}
	}

	event_schedule(sim_time + normal(bus_mean, bus_std), EVENT_BUS_ARRIVAL_A);
}

void report(void)
{
	fprintf(outfile, "Resultados:\n");
	fprintf(outfile, "\nPasajeros\n");
	// out_sampst(outfile, SAMPST_DELAYS, SAMPST_DELAYS);
	// fprintf(outfile, "\nQueue length (1) and server utilization (2):\n");
	// out_filest(outfile, LIST_QUEUE, LIST_SERVER);
	// fprintf(outfile, "Medicina general:\n");
}

float normal(float mean, float std)
{
    float aux = 0, cK = 1000;
    for (int k = 1; k <= cK; k++)
    {
        aux = aux + (float)rand() / RAND_MAX;
    }
    return std * sqrt((float)12 / cK) * (aux - (float)cK / 2) + mean;
}

float poisson(float mean) /* Exponential variate generation function. */
{
	int f = rand() % 20;
	return -mean * log((lcgrand(f)));
}
