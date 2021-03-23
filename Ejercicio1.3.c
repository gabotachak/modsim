//SIMULADOR DE SISTEMA DE COLA
//CÓDIGO REALIZADO CON BASE EN EL LIBRO DE SIMULACIÓN DE LAW

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lib/lcgrand.h" /* Header file for the random-number-generator */
#include <time.h>
#include <stdbool.h>

#define Q_LIMIT 100 /* Limit on the queue length */
#define BUSY 1		/* Mnemonics for server's being busy */
#define IDLE 0		/* and idle. */

int next_event_type, num_clients_served, num_total_clients, num_events;
float area_num_in_q, area_server_status, mean_interarrival, sim_time, time_last_event, time_next_event[3], total_service_time;
FILE *infile, *outfile;

int serverA1_status, serverA2_status, serverB_status, num_in_q1, num_in_q2;
float probtype1, mean_servicetype1, min_servicetype2, max_servicetype2, time_arrival_type1[Q_LIMIT + 1], time_arrival_type2[Q_LIMIT + 1];

void initialize(void);
void timing(void);
void arrive(void);
void depart(void);
bool is_type1(void);
void report(void);
void update_time_avg_stats(void);
float expon(float mean);
float poisson(float mean);
float uniform(float a, float b); 

int main(void)
{
	/* Archivos de entrada y salida - CAMBIAR NOMBRES PARA CADA EJERCICIO*/
	infile = fopen("mm1.3.in", "r");
	outfile = fopen("mm1.3.out", "w");

	/* Como son dos eventos, se establece en 2 */
	num_events = 2;

	/* Parámetros de entrada */
	fscanf(infile, "%f %f %f %f %f", &mean_interarrival, &probtype1, &mean_servicetype1, &min_servicetype2, &max_servicetype2);

	/* Write report heading and input parameters */
	fprintf(outfile, "Sistema de servidores\n\n");
	fprintf(outfile, "Tiempo promedio entre llegadas de clientes de %16.3f minutos\n\n", mean_interarrival);
	fprintf(outfile, "Tiempo medio de servicio para clientes de tipo 1 de %16.3f minutos\n\n", mean_servicetype1);
	fprintf(outfile, "Tiempo medio de servicio para clientes de tipo 2 entre %16.3f y %16.3f minutos\n\n", min_servicetype2, max_servicetype2);

	/*Initialize the simulation */
	initialize();

	/* Run the simulation while more delays are still needed */
	while (sim_time < 1000)
	{
		/*Determine the next event */
		timing();

		/* Update time-average statistical accumulators. */
		update_time_avg_stats();

		/* Invoke the appropriate event function */
		switch (next_event_type)
		{
		case 1:
			arrive();
			break;
		case 2:
			depart();
			break;
		}
	}

	/* Invoke the report generator and end the simulation */
	report();

	fclose(infile);
	fclose(outfile);

	return 0;
}

void initialize(void)
{ /* Initialize function. */

	/* Random seed for client types */
	srand(time(0));

	/* Initialize the simulation clock. */
	sim_time = 0;

	/* Initialize the state variables. */
	serverA1_status = IDLE;
	serverA2_status = IDLE;
	serverB_status = IDLE;
	num_in_q1 = 0;
	num_in_q2 = 0;
	time_last_event = 0.0;

	/* Initialize the statistical counters. */
	num_clients_served = 0;
	total_service_time = 0.0;
	area_num_in_q = 0.0;
	area_server_status = 0.0;

	/* Initialize the event list. Since no customers are present, the departure (service completion) event is eliminated from consideration. */
	time_next_event[1] = sim_time + expon(mean_interarrival);
	time_next_event[2] = 1.0e+30;
}

void timing(void)
{ /* Timing function. */
	int i;
	float min_time_next_event = 1.0e+29;

	next_event_type = 0;
	/* Determine the event type for the next event to occur. */
	for (i = 1; i <= num_events; ++i)
	{
		if (time_next_event[i] < min_time_next_event)
		{
			min_time_next_event = time_next_event[i];
			next_event_type = i;
		}
	}

	/* Check to see whether the event list is empty. */
	if (next_event_type == 0)
	{
		/* The event list is empty, so stop the simulation. */
		fprintf(outfile, "\nEvent list is empty at time %f", sim_time);
		exit(1);
	}

	/* The event list is not empty, so advance the simulation clock. */
	sim_time = min_time_next_event;
}

void arrive(void)
{ /* Arrival event function. */
	float delay;
	/* Schedule next arrival. */
	time_next_event[1] = sim_time + expon(mean_interarrival);

	/* Look if client is type 1 or 2 */
	if (is_type1())
	{
		/* Check to see whether server is busy. */
		if (serverA1_status == IDLE || serverA2_status == IDLE || serverB_status == IDLE)
		{
			/* Server is idle, so arriving customer has a delay of zero. */
			delay = 0.0;
			total_service_time += delay;

			/* Increment the number of customers delayed, and make server busy. */
			++num_clients_served;
			if (serverA1_status == IDLE)
			{
				serverA1_status = BUSY;
			}
			else if (serverA2_status == IDLE)
			{
				serverA2_status = BUSY;
			}
			else
			{
				serverB_status = BUSY;
			}

			/* Schedule a departure (service completion). */
			time_next_event[2] = sim_time + expon(mean_servicetype1);
		}
		else
		{
			/* Server is busy so increment the number of customers in the queue. */
			++num_in_q1;

			/* Check to see whether an overflow condition exists. */
			if (num_in_q1 > Q_LIMIT)
			{
				/* The queue has overflowed, so stop the simulation. */
				fprintf(outfile, "\nOverflow of the array time_arrival at");
				fprintf(outfile, " time %f", sim_time);
				exit(2);
			}

			/* There is still room in the queue, so store the time of arrival of the arriving customer at the (new) end of time_arrival. */
			time_arrival_type1[num_in_q1] = sim_time;
		}
	}
	else
	{
		if ((serverA1_status == IDLE || serverA2_status == IDLE) && serverB_status == IDLE)
		{
			/* Server is idle, so arriving customer has a delay of zero. */
			delay = 0.0;
			total_service_time += delay;

			/* Increment the number of customers delayed, and make server busy. */
			++num_clients_served;
			if (serverA1_status == IDLE)
			{
				serverA1_status = BUSY;
			}
			else if (serverA2_status == IDLE)
			{
				serverA2_status = BUSY;
			}
			serverB_status = BUSY;

			/* Schedule a departure (service completion). */
			time_next_event[2] = sim_time + uniform(min_servicetype2,max_servicetype2);
		}
		else
		{
			/* Server is busy so increment the number of customers in the queue. */
			++num_in_q2;

			/* Check to see whether an overflow condition exists. */
			if (num_in_q2 > Q_LIMIT)
			{
				/* The queue has overflowed, so stop the simulation. */
				fprintf(outfile, "\nOverflow of the array time_arrival at");
				fprintf(outfile, " time %f", sim_time);
				exit(2);
			}

			/* There is still room in the queue, so store the time of arrival of the arriving customer at the (new) end of time_arrival. */
			time_arrival_type2[num_in_q2] = sim_time;
		}
	}
}

void depart(void)
{ /*Departure event function. */
	int i;
	float delay;

	/* Check to see whether the queue is empty. */
	if (num_in_q == 0)
	{
		/* The queue is empty so make the server idle and eliminate the
		 * departure (service completion) event from consideration. */
		server_status = IDLE;
		time_next_event[2] = 1.0e+30;
	}
	else
	{
		/* The queue is nonempty, so decrement the number of customers in queue. */
		--num_in_q;

		/* Compute the delay of the customer who is beginning service
		 * and update the total delay of accumulator. */
		delay = sim_time - time_arrival[1];
		total_service_time += delay;

		/* Increment the number of customers delayed, and schedule departure. */
		++num_clients_served;
		time_next_event[2] = sim_time + expon(mean_service);

		/* Move each customer in queue (if any) up one place. */
		for (i = 1; i <= num_in_q; ++i)
		{
			time_arrival[i] = time_arrival[i + 1];
		}
	}
}

bool is_type1(void)
{ /* Returns true if type 1 or false if type 2 with a truncated chance of being true */
	if ((rand() % 100) < (probtype1 * 100))
	{
		return true;
	}
	return false;
}

void report(void)
{ /*Report generator function */
	/* Compute and write estimates of desired measures of performance. */
	fprintf(outfile, "_________________________________________________________\n");
	fprintf(outfile, "Tiempo de espera en la cola promedio de %11.3f minutos\n\n", total_service_time / num_clients_served);
	fprintf(outfile, "Número promedio de entidades en la cola %10.3f\n\n", area_num_in_q / sim_time);
	fprintf(outfile, "Utilización del sistema servidor%15.3f\n\n", area_server_status / sim_time);
	fprintf(outfile, "La simulación termina en %12.3f minutos", sim_time);
}

void update_time_avg_stats(void)
{ /* Update area accumulators for time-average statistics. */
	float time_since_last_event;

	/* Compute time since last event, and update last-event-time- marker. */
	time_since_last_event = sim_time - time_last_event;
	time_last_event = sim_time;

	/* Update area under number-in-queue function */
	area_num_in_q += num_in_q * time_since_last_event;

	/* Update area under server-busy indicator function. */
	area_server_status += server_status * time_since_last_event;
}

float expon(float mean)
{ /* Exponential variate generation function. */
	/* Return an exponential random variate with mean "mean". */
	return -mean * log(lcgrand(1));
}

float poisson(float mean) /* Exponential variate generation function. */
{
	int f = rand() % 20;
	return -mean * log((lcgrand(f)));
}

float uniform(float a, float b) /* Any float between a and b. */
{
	float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return a + scale * ( b - a );      /* [a, b] */
}