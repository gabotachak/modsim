#include "lib/simlib.h"

// Variables del sistema

#define EVENT_ARRIVAL 1	  //Llegada camioneta tipo 1
#define EVENT_DEPARTURE 2 //Salida camioneta tipo 1
#define LIST_QUEUE 1	  //Cola de la bodega
#define SAMPST_DELAYS 1	  //Variable para datos y estadística
#define BUSY true		  //Estado del muchacho
#define FREE false		  //Estado del muchacho

int num_custs_delayed, ending, un1, un2, num_custs_delayed2, ending2, un12, un22;
float travel_time, num_vans, mean_extra, p1, extra, travel_time2, mean_extra2, p12, extra2;
FILE *infile, *outfile;

void init_model(void);
void arrive(void); //Llegada de la camioneta
void depart(void); //Salida de la camioneta
void report(void); //Generación del reporte

int main() //Programa principal
{
	infile = fopen("2_3.in", "r");
	outfile = fopen("2_3.out", "w");
	fscanf(infile, "%f %f", &num_vans, &travel_time);
	fprintf(outfile, "Sistema de una pequeña bodega:\n");
	fprintf(outfile, "Con una flotilla de %.2f camionetas\n", num_vans);
	fprintf(outfile, "Tiempo medio de viaje con distribución exponencial de %.2f horas\n", travel_time);
	fprintf(outfile, "Tiempo medio de descarga con distribución exponencial entre 1/%.2f y 1/%.2f horas\n", 1.0, num_vans);
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
	event_schedule(sim_time + travel_time, EVENT_ARRIVAL);
}

void arrive(void)
{
	printf("Arrive");
	// event_schedule(sim_time + travel_time, EVENT_ARRIVAL);
	// if (list_size[LIST_SERVER] == 1)
	// {
	// 	transfer[1] = sim_time;
	// 	list_file(LAST, LIST_QUEUE);
	// }
	// else
	// {
	// 	sampst(0.0, SAMPST_DELAYS);
	// 	++num_custs_delayed;
	// 	list_file(FIRST, LIST_SERVER);
	// 	double t = uniform(un1 - un2, un1 + un2, 1);
	// 	double h = lcgrand(2);
	// 	double m = 0;
	// 	if (h > p1)
	// 		m = expon(extra, 3);
	// 	event_schedule(sim_time + t + h + m, EVENT_DEPARTURE);
	// }
}

void depart(void)
{
	printf("Depart");
	// if (list_size[LIST_QUEUE] == 0)
	// {
	// 	list_remove(FIRST, LIST_SERVER);
	// }
	// else
	// {
	// 	list_remove(FIRST, LIST_QUEUE);
	// 	sampst(sim_time - transfer[1], SAMPST_DELAYS);
	// 	++num_custs_delayed;
	// 	double t = uniform(un1 - un2, un1 + un2, 1);
	// 	double h = lcgrand(2);
	// 	double m = 0;
	// 	if (h > p1)
	// 		m = expon(extra, 3);
	// 	event_schedule(sim_time + t + h + m, EVENT_DEPARTURE);
	// }
}

void report(void)
{
	fprintf(outfile, "-------------------------------------------------------------\n");
	fprintf(outfile, "Resultados:\n");
	fprintf(outfile, "\nTiempo medio de espera en la cola:\n");
	fprintf(outfile, "\nProbabilidad de ocupación del muchacho:\n");
	fprintf(outfile, "\nProbabilidad de que todas las camionetas estén viajando:\n");
	fprintf(outfile, "\nNúmero promedio de camionetas viajando:\n");
	//fprintf(outfile, LIST_QUEUE);
}
