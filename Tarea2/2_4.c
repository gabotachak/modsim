#include "lib/simlib.h"

// Variables del sistema

#define EVENT_ARRIVAL_AER 1	//Llegada pasajero aeropuerto
#define EVENT_ARRIVAL_B1 2  //Llegada del bus 1
#define EVENT_ARRIVAL_H1 3  //Llegada pasajero hotel 1
#define EVENT_ARRIVAL_B2 4  //Llegada del bus 2
#define EVENT_ARRIVAL_H2 5   //Llegada pasajero hotel 2
#define LIST_QUEUE_AER 1	//Cola aeropuerto
#define LIST_QUEUE_H1 2	    //Cola hotel 1
#define LIST_QUEUE_H2 3	    //Cola hotel 2
#define SAMPST_DELAYS 1
#define STREAM_INTERARRIVAL 1
#define STREAM_SERVICE 2
	      

int par_b1, par_b2, personas_B1, personas_B2, personas_A_B1, personas_A_B2, asientos_B1, asientos_B2, ending;
float mean_interarrival_par, mean_interarrival_aer, mean_interarrival_bustop, desviacion_par;
FILE *infile, *outfile;

void init_model(void);
void arrive_B1(int par);	//Evento llegada
void arrive_B2(int par);	//Evento llegada
void arrive_H1(void);
void arrive_H2(void);
void arrive_Aer(void);
void report(void);	//Generación del reporte

int main() //Programa principal
{
	infile = fopen("2_4.in", "r");
	outfile = fopen("2_4.out", "w");
	fscanf(infile, "%f %f %d %d %f %f %f %f %d %d %f %f %d", &mean_interarrival_par, &mean_interarrival_bustop, &mean_interarrival_aer, &desviacion_par,&ending);
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
		case EVENT_ARRIVAL_AER:
			arrive_Aer();
			break;
		case EVENT_ARRIVAL_H1:
			arrive_H1();
			break;
		case EVENT_ARRIVAL_H2:
			arrive_H2();
			break;
		case EVENT_ARRIVAL_B1:
			arrive_B1();
			break;
        case EVENT_ARRIVAL_B2:
			arrive_B2();
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
	personas_B1 = 0;
	personas_B2 = 0;
	personas_A_B1 = 0;
	personas_A_B2 = 0;
	asientos_B1=0;
	asientos_B2;
	par_b1=2;
	event_schedule(uniform(mean_interarrival_par,desviacion_par,STREAM_INTERARRIVAL),EVENT_ARRIVAL_B1);
	arrive_B1(2);
}

//llegada del bus 1
void arrive_B1(int par)
{
	par_b1 = par;
	event_schedule(sim_time + mean_interarrival_par, EVENT_ARRIVAL_B1);
	//está en el hotel 1
	if (par_b1 == 0)
	{
		//apunta a la siguiente parada (hotel 2)
		++par_b1;
		int aux;

		//int size_queue_h1 = list_size(LIST_QUEUE_H1);

		//las personas en el bus 1 ahora son la mitad de las que venían del aeropuerto
		personas_B1 -= (personas_A_B1/2);

		//las personas que venían del aeropuerto ahora son la mitad
		personas_A_B1 /=2; 

		//for para remover todas las personas que están en la cola del paradero de bus del hotel 1
		for (aux = 0; aux < list_size(LIST_QUEUE_H1); aux++)
		{
			list_remove(FIRST,LIST_QUEUE_H1);
		}

		// se agregan las paersonas que estaban en la cola dell paradero de bus del hotel 1 al tota, de personas en el bus 1
		personas_B1 += aux;

		// se calculan las demoras (creo jejeje)
		sampst(sim_time,SAMPST_DELAYS);

		// está en la parada hotel 2
	}else if (par_b1 == 1)
	{
		//apunta a la parada del aerpuerto
		++par_b1;
		int aux;

		//las personas que venían del aeropuerto se bajan en el hotel 2
		personas_B1 -= personas_A_B1;
		//ya no hay personas que venían del aeropuerto
		personas_A_B1 = 0;

		//for para remover todas las personas que están en la cola del paradero de bus del hotel 2
		for (aux = 0; aux < list_size(LIST_QUEUE_H2); aux++)
		{
			list_remove(FIRST,LIST_QUEUE_H2);
		}

		//se agregan al total de personas en el bus 1 las que se acaban de subir en el hotel 2
		personas_B1 += aux;


		//está en el aeropuerto
	}else if (par_b1 == 2)
	{
		//apunta a la parada hotel 1
		par_b1 = 0;
		int aux;
		//for para remover todas las personas que están en la cola del paradero de bus del aeropuerto
		for (aux = 0; aux < list_size(LIST_QUEUE_AER); aux++)
		{
			list_remove(FIRST,LIST_QUEUE_AER);		
		}
		
		//las personas en el bus son las que se acaban de subir al aeropuerto
		personas_B1 = aux;
		//las personas que vienen del aeropuerto son las que se acaban de subir
		personas_A_B1 = aux;

	}
		
	//event_schedule(sim_time + interarrival, EVENT_ARRIVAL);
}


//HACE LO MISMO QUE LA arrive_B1 pero para el bus 2
void arrive_B2(int par){
	par_b2 = par;
	event_schedule(sim_time + mean_interarrival_par, EVENT_ARRIVAL_B2);
	if (par_b2 == 0)
	{
		++par_b2;
		int aux;

		//int size_queue_h1 = list_size(LIST_QUEUE_H1);

		personas_B2 -= (personas_A_B2/2);
		personas_A_B2 /=2; 
		for (aux = 0; aux < list_size(LIST_QUEUE_H1); aux)
		{
			list_remove(FIRST,LIST_QUEUE_H1);
		}
		personas_B2 += aux;
		sampst(sim_time,SAMPST_DELAYS);
	}else if (par_b2 == 1)
	{
		++par_b2;
		int aux;

		personas_B2 -= personas_A_B2;
		personas_A_B2 = 0;

		for (aux = 0; aux < list_size(LIST_QUEUE_H2); aux)
		{
			list_remove(FIRST,LIST_QUEUE_H2);
		}

	}else if (par_b2 == 2)
	{
		par_b2 = 0;
		int aux;

		for (aux = 0; aux < list_size(LIST_QUEUE_AER); aux++)
		{
			list_remove(FIRST,LIST_QUEUE_AER);		
		}
		
		personas_B2 = aux;
		personas_A_B2 = aux;

	}
		
	//event_schedule(sim_time + interarrival, EVENT_ARRIVAL);
}

void arrive_H1(void)
{
	list_file(LAST,LIST_QUEUE_H1);
	event_schedule(sim_time+expon(mean_interarrival_bustop,STREAM_INTERARRIVAL),EVENT_ARRIVAL_H1);
}

void arrive_H2(void)
{
	list_file(LAST,LIST_QUEUE_H2);
	event_schedule(sim_time+expon(mean_interarrival_bustop,STREAM_INTERARRIVAL),EVENT_ARRIVAL_H2);
}

void arrive_Aer(void)
{
	list_file(LAST,LIST_QUEUE_AER);
	event_schedule(sim_time+expon(mean_interarrival_aer,STREAM_INTERARRIVAL),EVENT_ARRIVAL_H1);
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
