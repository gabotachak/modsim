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

int N, bus_state, people_in_bus;
float init_time, end_time, bus_mean, bus_std, a_mean, b_mean, total_time, wait_time;


FILE *infile, *outfile;

float normal();
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
	event_schedule(sim_time + normal(bus_mean, bus_std)), EVENT_BUS_ARRIVAL_A);
	event_schedule(sim_time + poisson(a_mean), EVENT_PASS_ARRIVAL_A);
	event_schedule(sim_time + poisson(a_mean), EVENT_PASS_ARRIVAL_B);
}

void pass_arrival_a(void)
{
	event_schedule(sim_time + poisson(a_mean), EVENT_PASS_ARRIVAL_A);
	list_file(LAST, LIST_PASS_A);
	if (bus_state == BUS_IN_A && list_size[LIST_PASS_BUS] < N)
	{
		list_remove(FIRST, LIST_PASS_A);
		list_file(FIRST, LIST_PASS_BUS);
		wait_time += sim_time;
	}
}

void pass_arrival_b(void)
{
	event_schedule(sim_time + poisson(b_mean), EVENT_PASS_ARRIVAL_B);
	list_file(LAST, LIST_PASS_B);
	if (bus_state == BUS_IN_B && list_size[LIST_PASS_BUS] < N)
	{
		list_remove(FIRST, LIST_PASS_A);
		list_file(FIRST, LIST_PASS_BUS);
		wait_time += sim_time;
	}
}

void bus_arrival_a(void)
{
	bus_state = BUS_IN_A
	event_schedule(sim_time + normal(bus_mean, bus_std), EVENT_BUS_ARRIVAL_A);
	while (list_size[LIST_PASS_BUS] != 0)
	{
		list_remove(FIRST, LIST_PASS_BUS);
		list_file(LAST, LIST_PASS_SERVED);
	}
}

void depart(void)
{
	if (list_size[LIST_QUEUE] == 0)
	{
		list_remove(FIRST, LIST_SERVER);
	}
	else
	{
		list_remove(FIRST, LIST_QUEUE);
		sampst(sim_time - transfer[1], SAMPST_DELAYS);
		++num_custs_delayed;
		double t = uniform(un1 - un2, un1 + un2, 1);
		double h = lcgrand(2);
		double m = 0;
		if (h > p1)
			m = expon(extra, 3);
		event_schedule(sim_time + t + h + m, EVENT_DEPARTURE);
	}
}

void arrive2(void)
{
	event_schedule(sim_time + interarrival2, EVENT_ARRIVAL2);
	if (list_size[LIST_SERVER2] == 2)
	{
		transfer[1] = sim_time;
		list_file(LAST, LIST_QUEUE2);
	}
	else
	{
		sampst(0.0, SAMPST_DELAYS2);
		++num_custs_delayed2;
		list_file(FIRST, LIST_SERVER2);
		double t = uniform(un12 - un22, un12 + un22, 4);
		double h = lcgrand(5);
		double m = 0;
		if (h > p12)
			m = expon(extra2, 6);
		event_schedule(sim_time + t + h + m, EVENT_DEPARTURE2);
	}
}

void depart2(void)
{
	if (list_size[LIST_QUEUE2] == 0)
		list_remove(FIRST, LIST_SERVER2);
	else
	{
		list_remove(FIRST, LIST_QUEUE2);
		sampst(sim_time - transfer[1], SAMPST_DELAYS2);
		++num_custs_delayed2;
		double t = uniform(un12 - un22, un12 + un22, 4);
		double h = lcgrand(5);
		double m = 0;
		if (h > p12)
			m = expon(extra, 6);
		event_schedule(sim_time + t + h + m, EVENT_DEPARTURE2);
	}
}

void report(void)
{
	fprintf(outfile, "Pediatr�a:\n");
	fprintf(outfile, "\nDelays in queue, in minutes:\n");
	out_sampst(outfile, SAMPST_DELAYS, SAMPST_DELAYS);
	fprintf(outfile, "\nQueue length (1) and server utilization (2):\n");
	out_filest(outfile, LIST_QUEUE, LIST_SERVER);
	fprintf(outfile, "Medicina general:\n");
	fprintf(outfile, "\nDelays in queue, in minutes:\n");
	out_sampst(outfile, SAMPST_DELAYS2, SAMPST_DELAYS2);
	fprintf(outfile, "\nQueue length (1) and server utilization (2):\n");
	out_filest(outfile, LIST_QUEUE2, LIST_SERVER2);
	fprintf(outfile, "\nTime simulation ended:%12.3f minutes\n", sim_time);
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
