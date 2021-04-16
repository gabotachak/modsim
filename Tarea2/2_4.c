#include "lib/simlib.h"

// Variables del sistema

#define EVENT_PASS_AER 1   //Llegada pasajero aeropuerto
#define EVENT_PASS_H1 2    //Llegada pasajero Hotel 1
#define EVENT_PASS_H1 3    //Llegada pasajero Hotel 2
#define EVENT_BUS1_AER 4    //Llegada bus 1 aeropuerto
#define EVENT_BUS1_H1 5     //Llegada bus 1 Hotel 1
#define EVENT_BUS1_H2 6     //Llegada bus 1 Hotel 2
#define EVENT_BUS2_AER 7    //Llegada bus 2 aeropuerto
#define EVENT_BUS2_H1 8     //Llegada bus 2 Hotel 1
#define EVENT_BUS2_H2 9     //Llegada bus 2 Hotel 2
#define LIST_QUEUE_AER 1	//Cola aeropuerto
#define LIST_QUEUE_H1 2		//Cola hotel 1
#define LIST_QUEUE_H2 3		//Cola hotel 2
#define LIST_QUEUE_B1_H 4		//Cola bus 1
#define LIST_QUEUE_B2_H 5		//Cola bus 2
#define LIST_QUEUE_B1_AER 6		//Cola bus 1 aeropuerto
#define LIST_QUEUE_B2_AER 7		//Cola bus 2

#define BUS_IN_AER 1
#define BUS_IN_H1 2
#define BUS_IN_H2 3

#define SAMPST_DELAYS 1		//Cola bus 1
#define STREAM_INTERARRIVAL 1
// #define STREAM_SERVICE 2

int estado_b1, estado_b2, asientos_B1, asientos_B2, ending;
float mean_interarrival_par, mean_interarrival_aer, mean_interarrival_bustop, desviacion_par;
FILE *infile, *outfile;

void init_model(void);
void arrive_pass_h1(void);
void arrive_pass_h2(void);
void arrive_pass_aer(void);
void arrive_bus1_h1(void);
void arrive_bus1_h2(void);
void arrive_bus1_aer(void);
void arrive_bus2_h1(void);
void arrive_bus2_h2(void);
void arrive_bus2_aer(void);

float normal(float mean, float std);
void report(void); //Generación del reporte

int main() //Programa principal
{
	infile = fopen("2_4.in", "r");
	outfile = fopen("2_4.out", "w");
	fscanf(infile, "%f %f %f %f %d %d %d", &mean_interarrival_par, &mean_interarrival_bustop, &mean_interarrival_aer, &desviacion_par, &asientos_B1, &asientos_B2, &ending);
	fprintf(outfile, "Media de tiempo para llegar a cada parada %.2f minutes\n", mean_interarrival_par);
	fprintf(outfile, "Desviación estandar de las llegadas a cada parada %.2f minutes\n", desviacion_par);
	fprintf(outfile, "Media de tiempo de llegada de los vuelos %.2f minutes\n", mean_interarrival_aer);
	fprintf(outfile, "Media entre llegadas a las paradas de autobus en cada hotel %.2f minutes\n\n", mean_interarrival_bustop);
	fprintf(outfile, "Ending time%14d\n\n\n", ending);
	init_simlib();
	maxatr = 4;
	init_model();
	while (sim_time < ending)
	{
		timing();
		switch (next_event_type)
		{
		case EVENT_PASS_AER:
			arrive_pass_aer();
			break;

		case EVENT_PASS_H1:
			arrive_pass_h1();
			break;

		case EVENT_PASS_H2:
			arrive_pass_h2();
			break;

		case EVENT_BUS1_AER:
			arrive_bus1_aer();
			break;

		case EVENT_BUS1_H1:
			arrive_bus1_h1();
			break;

		case EVENT_BUS1_H2:
			arrive_bus1_h2();
			break;

		case EVENT_BUS2_AER:
			arrive_bus2_aer();
			break;

		case EVENT_BUS2_H1:
			arrive_bus2_h1();
			break;

		case EVENT_BUS2_H2:
			arrive_bus2_h2();
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
	//par_b1 = 2;
	float init_b1 = normal(mean_interarrival_par, desviacion_par);
	event_schedule(sim_time + init_b1, EVENT_ARRIVAL_B1);
	event_schedule(sim_time + init_b1 + 30, EVENT_ARRIVAL_B2);
	event_schedule(sim_time + expon(mean_interarrival_aer),EVENT_ARRIVAL_AER);
	event_schedule(sim_time + expon(mean_interarrival_bustop,STREAM_INTERARRIVAL),EVENT_ARRIVAL_H1);
	event_schedule(sim_time + expon(mean_interarrival_bustop,STREAM_INTERARRIVAL),EVENT_ARRIVAL_H2);
}

void arrive_bus1_aer(void){
	if (estado_b1 != BUS_IN_AER)
	{
		estado_b1 = BUS_IN_AER;
		float random_normal = normal(mean_interarrival_par, desviacion_par);
		printf("First normal: %.2f\n", random_normal);
		event_schedule(sim_time + random_normal, EVENT_BUS1_AER);
		printf("A-----------\n");
		//printf("En el bus al llegar: %d\n", list_size[LIST_PASS_BUS]);
		
		while (list_size[LIST_QUEUE_B1_AER] > 0)
		{
			list_remove(FIRST, LIST_QUEUE_B1_AER);
		}
		//printf("En el bus después de descargar: %d\n", list_size[LIST_PASS_BUS]);

		//printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		while ((list_size[LIST_QUEUE_B1_AER] + list_size(LIST_QUEUE_B1_H)) < asientos_B1 && list_size[LIST_QUEUE_AER] > 0)
		{
			//printf("%d\n",list_size[LIST_PASS_BUS]);
			list_remove(FIRST, LIST_QUEUE_AER);
			list_file(FIRST, LIST_QUEUE_B1_H);
		}
		// printf("En el bus después de subir a la gente de A: %d\n", list_size[LIST_PASS_BUS]);
		// printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		// printf("Normal: %.2f\n", normal(bus_mean, bus_std));

		float next_normal;
		do
		{
			next_normal = normal(mean_interarrival_aer, desviacion_par);
			printf("Next normal: %.2f\n", next_normal);
		} while (next_normal <= random_normal);

		event_schedule(sim_time + next_normal, EVENT_BUS1_H1);
	}
}

void arrive_bus1_h1(void){
	if (estado_b1 != BUS_IN_H1)
	{
		estado_b1 = BUS_IN_H1;
		float random_normal = normal(mean_interarrival_par, desviacion_par);
		printf("First normal: %.2f\n", random_normal);
		event_schedule(sim_time + random_normal, EVENT_BUS1_H1);
		printf("A-----------\n");
		//printf("En el bus al llegar: %d\n", list_size[LIST_PASS_BUS]);
		int bajados = list_size[LIST_QUEUE_B1_H]/2;
		while (list_size[LIST_QUEUE_B1_H] > bajados)
		{
			list_remove(FIRST, LIST_QUEUE_B1_H);
		}
		//printf("En el bus después de descargar: %d\n", list_size[LIST_PASS_BUS]);

		//printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		while ((list_size[LIST_QUEUE_B1_AER] + list_size(LIST_QUEUE_B1_H)) < asientos_B1 && list_size[LIST_QUEUE_H1] > 0)
		{
			//printf("%d\n",list_size[LIST_PASS_BUS]);
			list_remove(FIRST, LIST_QUEUE_H1);
			list_file(FIRST, LIST_QUEUE_B1_AER);
		}
		// printf("En el bus después de subir a la gente de A: %d\n", list_size[LIST_PASS_BUS]);
		// printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		// printf("Normal: %.2f\n", normal(bus_mean, bus_std));

		float next_normal;
		do
		{
			next_normal = normal(mean_interarrival_aer, desviacion_par);
			printf("Next normal: %.2f\n", next_normal);
		} while (next_normal <= random_normal);

		event_schedule(sim_time + next_normal, EVENT_BUS1_H2);
	}
}

void arrive_bus1_h2(void){
	if (estado_b1 != BUS_IN_H2)
	{
		estado_b1 = BUS_IN_H2;
		float random_normal = normal(mean_interarrival_par, desviacion_par);
		printf("First normal: %.2f\n", random_normal);
		event_schedule(sim_time + random_normal, EVENT_BUS1_H2);
		printf("A-----------\n");
		//printf("En el bus al llegar: %d\n", list_size[LIST_PASS_BUS]);
		
		while (list_size[LIST_QUEUE_B1_H] > 0)
		{
			list_remove(FIRST, LIST_QUEUE_B1_H);
		}
		//printf("En el bus después de descargar: %d\n", list_size[LIST_PASS_BUS]);

		//printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		while ((list_size[LIST_QUEUE_B1_AER] + list_size(LIST_QUEUE_B1_H)) < asientos_B1 && list_size[LIST_QUEUE_H2] > 0)
		{
			//printf("%d\n",list_size[LIST_PASS_BUS]);
			list_remove(FIRST, LIST_QUEUE_H2);
			list_file(FIRST, LIST_QUEUE_B1_AER);
		}
		// printf("En el bus después de subir a la gente de A: %d\n", list_size[LIST_PASS_BUS]);
		// printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		// printf("Normal: %.2f\n", normal(bus_mean, bus_std));

		float next_normal;
		do
		{
			next_normal = normal(mean_interarrival_aer, desviacion_par);
			printf("Next normal: %.2f\n", next_normal);
		} while (next_normal <= random_normal);

		event_schedule(sim_time + next_normal, EVENT_BUS1_H2);
	}
}

void arrive_bus2_aer(void){
	if (estado_b2 != BUS_IN_AER)
	{
		estado_b2 = BUS_IN_AER;
		float random_normal = normal(mean_interarrival_par, desviacion_par);
		printf("First normal: %.2f\n", random_normal);
		event_schedule(sim_time + random_normal, EVENT_BUS2_AER);
		printf("A-----------\n");
		//printf("En el bus al llegar: %d\n", list_size[LIST_PASS_BUS]);
		
		while (list_size[LIST_QUEUE_B2_AER] > 0)
		{
			list_remove(FIRST, LIST_QUEUE_B2_AER);
		}
		//printf("En el bus después de descargar: %d\n", list_size[LIST_PASS_BUS]);

		//printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		while ((list_size[LIST_QUEUE_B2_AER] + list_size(LIST_QUEUE_B2_H)) < asientos_B2 && list_size[LIST_QUEUE_AER] > 0)
		{
			//printf("%d\n",list_size[LIST_PASS_BUS]);
			list_remove(FIRST, LIST_QUEUE_AER);
			list_file(FIRST, LIST_QUEUE_B2_H);
		}
		// printf("En el bus después de subir a la gente de A: %d\n", list_size[LIST_PASS_BUS]);
		// printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		// printf("Normal: %.2f\n", normal(bus_mean, bus_std));

		float next_normal;
		do
		{
			next_normal = normal(mean_interarrival_aer, desviacion_par);
			printf("Next normal: %.2f\n", next_normal);
		} while (next_normal <= random_normal);

		event_schedule(sim_time + next_normal, EVENT_BUS2_H1);
	}
}

void arrive_bus2_h1(void){
	if (estado_b2 != BUS_IN_H1)
	{
		estado_b2 = BUS_IN_H1;
		float random_normal = normal(mean_interarrival_par, desviacion_par);
		printf("First normal: %.2f\n", random_normal);
		event_schedule(sim_time + random_normal, EVENT_BUS2_H1);
		printf("A-----------\n");
		//printf("En el bus al llegar: %d\n", list_size[LIST_PASS_BUS]);
		int bajados = list_size[LIST_QUEUE_B2_H]/2;
		while (list_size[LIST_QUEUE_B2_H] > bajados)
		{
			list_remove(FIRST, LIST_QUEUE_B2_H);
		}
		//printf("En el bus después de descargar: %d\n", list_size[LIST_PASS_BUS]);

		//printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		while ((list_size[LIST_QUEUE_B2_AER] + list_size(LIST_QUEUE_B2_H)) < asientos_B2 && list_size[LIST_QUEUE_H1] > 0)
		{
			//printf("%d\n",list_size[LIST_PASS_BUS]);
			list_remove(FIRST, LIST_QUEUE_H1);
			list_file(FIRST, LIST_QUEUE_B2_AER);
		}
		// printf("En el bus después de subir a la gente de A: %d\n", list_size[LIST_PASS_BUS]);
		// printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		// printf("Normal: %.2f\n", normal(bus_mean, bus_std));

		float next_normal;
		do
		{
			next_normal = normal(mean_interarrival_aer, desviacion_par);
			printf("Next normal: %.2f\n", next_normal);
		} while (next_normal <= random_normal);

		event_schedule(sim_time + next_normal, EVENT_BUS2_H2);
	}
}

void arrive_bus2_h2(void){
	if (estado_b2 != BUS_IN_H2)
	{
		estado_b2 = BUS_IN_H2;
		float random_normal = normal(mean_interarrival_par, desviacion_par);
		printf("First normal: %.2f\n", random_normal);
		event_schedule(sim_time + random_normal, EVENT_BUS2_H2);
		printf("A-----------\n");
		//printf("En el bus al llegar: %d\n", list_size[LIST_PASS_BUS]);
		
		while (list_size[LIST_QUEUE_B2_H] > 0)
		{
			list_remove(FIRST, LIST_QUEUE_B2_H);
		}
		//printf("En el bus después de descargar: %d\n", list_size[LIST_PASS_BUS]);

		//printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		while ((list_size[LIST_QUEUE_B2_AER] + list_size(LIST_QUEUE_B2_H)) < asientos_B2 && list_size[LIST_QUEUE_H2] > 0)
		{
			//printf("%d\n",list_size[LIST_PASS_BUS]);
			list_remove(FIRST, LIST_QUEUE_H2);
			list_file(FIRST, LIST_QUEUE_B2_AER);
		}
		// printf("En el bus después de subir a la gente de A: %d\n", list_size[LIST_PASS_BUS]);
		// printf("En la fila A: %d\n", list_size[LIST_PASS_A]);
		// printf("Normal: %.2f\n", normal(bus_mean, bus_std));

		float next_normal;
		do
		{
			next_normal = normal(mean_interarrival_aer, desviacion_par);
			printf("Next normal: %.2f\n", next_normal);
		} while (next_normal <= random_normal);

		event_schedule(sim_time + next_normal, EVENT_BUS2_H2);
	}
}

void arrive_pass_aer(void)
{
	event_schedule(sim_time + expon(mean_interarrival_aer, STREAM_INTERARRIVAL), EVENT_PASS_AER);
	list_file(LAST, LIST_QUEUE_AER);
}

void arrive_pass_h1(void)
{
	event_schedule(sim_time + expon(mean_interarrival_bustop, STREAM_INTERARRIVAL), EVENT_PASS_H1);
	list_file(LAST, LIST_QUEUE_H1);
}

void arrive_pass_h2(void)
{
	event_schedule(sim_time + expon(mean_interarrival_bustop, STREAM_INTERARRIVAL), EVENT_PASS_H2);
	list_file(LAST, LIST_QUEUE_H2);
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



void report(void)
{
	fprintf(outfile, "-------------- RESULTADOS---------------\n");
	fprintf(outfile, "\nDelays in queue, in minutes:\n");
	out_sampst(outfile, SAMPST_DELAYS, SAMPST_DELAYS);
	// out_filest(outfile, LIST_QUEUE, LIST_SERVER);
	// fprintf(outfile, "Medicina general:\n");
	// fprintf(outfile, "\nDelays in queue, in minutes:\n");
	// out_sampst(outfile, SAMPST_DELAYS2, SAMPST_DELAYS2);
	// fprintf(outfile, "\nQueue length (1) and server utilization (2):\n");
	// out_filest(outfile, LIST_QUEUE2, LIST_SERVER2);
	// fprintf(outfile, "\nTime simulation ended:%12.3f minutes\n", sim_time);
}
