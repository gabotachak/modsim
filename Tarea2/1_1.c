#include "lib/simlib.h"

// Variables del sistema

#define EVENT_PASS_ARRIVALA 1	//Llegada de pasajeros fila A
#define EVENT_PASS_ARRIVALB 2  	//Llegada de pasajeros fila B
#define EVENT_BUS_ARRIVALA 3   	//Llegada Bus fila A
#define EVENT_BUS_ARRIVALB 4 	//Llegada Bus fila B
#define LIST_PASS_A 1			//Lista pasajeros fila A
#define LIST_PASS_B 2	   		//Lista pasajeros fila B
#define LIST_PASS_BUS 3	   		//Lista pasajeros bus
#define SAMPST_DELAYS 1	   		//Variable para datos sobre ni�os
#define SAMPST_DELAYS2 2   		//Variable para datos sobre adultos

int num_custs_delayed, ending, un1, un2, num_custs_delayed2, ending2, un12, un22;
float interarrival, mean_extra, p1, extra, interarrival2, mean_extra2, p12, extra2;
FILE *infile, *outfile;

void init_model(void);
void arrive(void);	//Llegada ni�o
void arrive2(void); //Llegada adulto
void depart(void);	//Salida ni�o
void depart2(void); //Salida adulto
void report(void);	//Generaci�n del reporte

int main() //Programa principal
{
	infile = fopen("template.in", "r");
	outfile = fopen("template.out", "w");
	fscanf(infile, "%f %f %d %d %f %f %f %f %d %d %f %f %d", &interarrival, &mean_extra, &un1, &un2, &p1, &extra, &interarrival2, &mean_extra2, &un12, &un22, &p12, &extra2, &ending);
	fprintf(outfile, "Pediatr�a:\n");
	fprintf(outfile, "Interarrival time %.2f minutes\n", interarrival);
	fprintf(outfile, "Mean registry time %.2f minutes\n", mean_extra);
	fprintf(outfile, "Uniform between %d - %d minutes\n", un1 - un2, un1 + un2);
	fprintf(outfile, "Probability of extra time %.2f minutes\n", p1);
	fprintf(outfile, "Mean extra time %.2f minutes\n\n", extra);
	fprintf(outfile, "Medicina general:\n");
	fprintf(outfile, "Interarrival time %.2f minutes\n", interarrival2);
	fprintf(outfile, "Mean registry time %.2f minutes\n", mean_extra2);
	fprintf(outfile, "Uniform between %d - %d minutes\n", un12 - un22, un12 + un22);
	fprintf(outfile, "Probability of extra time %.2f minutes\n", p12);
	fprintf(outfile, "Mean extra time %.2f minutes\n\n", extra2);
	fprintf(outfile, "Ending time%14d\n\n\n", ending);
	init_simlib();
	maxatr = 4;
	init_model();
	while (sim_time < ending)
	{
		timing();
		switch (next_event_type)
		{
		case EVENT_ARRIVAL:
			arrive();
			break;
		case EVENT_DEPARTURE:
			depart();
			break;
		case EVENT_ARRIVAL2:
			arrive2();
			break;
		case EVENT_DEPARTURE2:
			depart2();
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
	num_custs_delayed = 0;
	event_schedule(sim_time + interarrival, EVENT_ARRIVAL);
	event_schedule(sim_time + interarrival2, EVENT_ARRIVAL2);
}

void arrive(void)
{
	event_schedule(sim_time + interarrival, EVENT_ARRIVAL);
	if (list_size[LIST_SERVER] == 1)
	{
		transfer[1] = sim_time;
		list_file(LAST, LIST_QUEUE);
	}
	else
	{
		sampst(0.0, SAMPST_DELAYS);
		++num_custs_delayed;
		list_file(FIRST, LIST_SERVER);
		double t = uniform(un1 - un2, un1 + un2, 1);
		double h = lcgrand(2);
		double m = 0;
		if (h > p1)
			m = expon(extra, 3);
		event_schedule(sim_time + t + h + m, EVENT_DEPARTURE);
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
