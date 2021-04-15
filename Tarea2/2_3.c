#include "lib/simlib.h"

// Variables del sistema

#define EVENT_ARRIVAL 1	   //Llegada camioneta tipo 1
#define EVENT_DEPARTURE 2  //Salida camioneta tipo 1
#define STORAGE_QUEUE 1	   //Cola de la bodega
#define UNLOADING_QUEUE 2  //Cola para las descargas
#define TRAVELLING_QUEUE 3 //Cola para las descargas
#define SAMPST_DELAYS 1	   //Variable para datos y estadística de las camionetas atendidas
#define BUSY 1			   //Estado del muchacho
#define FREE 0			   //Estado del muchacho

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
	if (list_size[TRAVELLING_QUEUE] > 0 && sim_time - transfer[2] >= travel_time)
	{
		list_remove(FIRST, TRAVELLING_QUEUE);
	}
	if (list_size[UNLOADING_QUEUE] == 1 && list_size[TRAVELLING_QUEUE] <= num_vans)
	{
		transfer[1] = sim_time;
		list_file(LAST, STORAGE_QUEUE);
	}
	else
	{
		sampst(0.0, SAMPST_DELAYS);
		++num_custs_delayed;
		list_file(FIRST, UNLOADING_QUEUE);
		double service_time = expon(1 / (list_size[UNLOADING_QUEUE] + list_size[STORAGE_QUEUE]), 1);
		event_schedule(sim_time + service_time, EVENT_DEPARTURE);
	}
}

void depart(void)
{
	if (list_size[STORAGE_QUEUE] == 0 && list_size[TRAVELLING_QUEUE] <= num_vans)
	{
		list_remove(FIRST, UNLOADING_QUEUE);
		list_file(LAST, TRAVELLING_QUEUE);
	}
	else
	{
		list_remove(FIRST, STORAGE_QUEUE);
		sampst(sim_time - transfer[1], SAMPST_DELAYS);
		++num_custs_delayed;
		double service_time = expon(1 / list_size[UNLOADING_QUEUE], 1);
		event_schedule(sim_time + service_time, EVENT_DEPARTURE);
	}
}

void report(void)
{
	fprintf(outfile, "-------------------------------------------------------------\n");
	fprintf(outfile, "Resultados:\n");
	fprintf(outfile, "\nClientes atendidos:\n");
	out_sampst(outfile, SAMPST_DELAYS, SAMPST_DELAYS);
	fprintf(outfile, "\nOcupación de la cola:\n");
	out_filest(outfile, STORAGE_QUEUE, STORAGE_QUEUE);
	fprintf(outfile, "\nOcupación del muchacho:\n");
	out_filest(outfile, UNLOADING_QUEUE, UNLOADING_QUEUE);
	fprintf(outfile, "\nViajes de las camionetas:\n");
	out_filest(outfile, TRAVELLING_QUEUE, TRAVELLING_QUEUE);
	fprintf(outfile, "\nSimulación terminada en %12.3f horas\n", sim_time);
	//fprintf(outfile, STORAGE_QUEUE);
}
