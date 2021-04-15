#include "lib/simlib.h"

// Variables del sistema

#define EVENT_ARRIVAL 1	  //Llegada camioneta tipo 1
#define EVENT_DEPARTURE 2 //Salida camioneta tipo 1
#define STORAGE_QUEUE 1	  //Cola de la bodega
#define UNLOADING_QUEUE 2 //Cola para las descargas
#define SAMPST_DELAYS 1	  //Variable para datos y estadística de las camionetas atendidas
#define BUSY 1			  //Estado del muchacho
#define FREE 0			  //Estado del muchacho

int num_custs_delayed, sim_hours, muchacho;
float travel_time, num_vans;
FILE *infile, *outfile;

void init_model(void);
void arrive(void); //Llegada de la camioneta
void depart(void); //Salida de la camioneta
void report(void); //Generación del reporte

int main() //Programa principal
{
	infile = fopen("2_3.in", "r");
	outfile = fopen("2_3.out", "w");
	fscanf(infile, "%f %f %d", &num_vans, &travel_time, &sim_hours);
	fprintf(outfile, "Sistema de una pequeña bodega:\n");
	fprintf(outfile, "Con una flotilla de %.2f camionetas\n", num_vans);
	fprintf(outfile, "Tiempo medio de viaje con distribución exponencial de %.2f horas\n", travel_time);
	fprintf(outfile, "Tiempo medio de descarga con distribución exponencial entre 1/%.2f y 1/%.2f horas\n", 1.0, num_vans);
	init_simlib();
	maxatr = 4;
	init_model();
	while (sim_time < sim_hours)
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
	muchacho = FREE;
	event_schedule(sim_time + expon(travel_time, 1), EVENT_ARRIVAL);
}

void arrive(void)
{
	event_schedule(sim_time + expon(travel_time, 1), EVENT_ARRIVAL);
	if (muchacho = BUSY)
	{
		transfer[1] = sim_time;
		list_file(LAST, STORAGE_QUEUE);
	}
	else
	{
		sampst(0.0, SAMPST_DELAYS);
		++num_custs_delayed;
		list_file(FIRST, UNLOADING_QUEUE);
		double service_time = expon(1 / list_size[UNLOADING_QUEUE], 1);
		event_schedule(sim_time + service_time, EVENT_DEPARTURE);
	}
}

void depart(void)
{
	printf("Depart");
	// if (list_size[STORAGE_QUEUE] == 0)
	// {
	// 	list_remove(FIRST, LIST_SERVER);
	// }
	// else
	// {
	// 	list_remove(FIRST, STORAGE_QUEUE);
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
	//fprintf(outfile, STORAGE_QUEUE);
}
