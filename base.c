//SIMULADOR DE SISTEMA DE COLA
//CÓDIGO REALIZADO CON BASE EN EL LIBRO DE SIMULACIÓN DE LAW

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lib/lcgrand.h" /* Header file for the random-number-generator */

#define Q_LIMIT 100 /* Limit on the queue length */
#define BUSY 1		/* Mnemonics for server's being busy */
#define IDLE 0		/* and idle. */

int next_event_type, num_entities_served, num_total_entities, num_events, num_in_q, server_status;
float area_num_in_q, area_server_status, mean_interarrival, mean_service, sim_time, time_arrival[Q_LIMIT + 1], time_last_event, time_next_event[3], total_service_time;
FILE *infile, *outfile;

void initialize(void);
void timing(void);
void arrive(void);
void depart(void);
void report(void);
void update_time_avg_stats(void);
float expon(float mean);
int poisson(float mean);

int main(void)
{
	/* Archivos de entrada y salida - CAMBIAR NOMBRES PARA CADA EJERCICIO*/
	infile = fopen("mm1.in", "r");
	outfile = fopen("mm1.out", "w");

	/* Como son dos eventos, se establece en 2 */
	num_events = 2;

	/* Parámetros de entrada */
	fscanf(infile, "%f %f %d", &mean_interarrival, &mean_service, &num_total_entities);

	/* Write report heading and input parameters */
	fprintf(outfile, "Sistema de cola\n\n");
	fprintf(outfile, "Tiempo promedio entre llegadas de %16.3f minutos\n\n", mean_interarrival);
	fprintf(outfile, "Tiempo medio de servicio de %16.3f minutos\n\n", mean_service);
	fprintf(outfile, "Número de entidades %14d\n\n", num_total_entities);

	/*Initialize the simulation */
	initialize();

	/* Run the simulation while more delays are still needed */
	while (num_entities_served < num_total_entities)
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
	/* Initialize the simulation clock. */
	sim_time = 0;

	/* Initialize the state variables. */
	server_status = IDLE;
	num_in_q = 0;
	time_last_event = 0.0;

	/* Initialize the statistical counters. */
	num_entities_served = 0;
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

	/* Check to see whether server is busy. */
	if (server_status == BUSY)
	{
		/* Server is busy so increment the number of customers in the queue. */
		++num_in_q;

		/* Check to see whether an overflow condition exists. */
		if (num_in_q > Q_LIMIT)
		{
			/* The queue has overflowed, so stop the simulation. */
			fprintf(outfile, "\nOverflow of the array time_arrival at");
			fprintf(outfile, " time %f", sim_time);
			exit(2);
		}

		/* There is still room in the queue, so store the time of arrival of the arriving customer at the (new) end of time_arrival. */
		time_arrival[num_in_q] = sim_time;
	}

	else
	{
		/* Server is idle, so arriving customer has a delay of zero. */
		delay = 0.0;
		total_service_time += delay;

		/* Increment the number of customers delayed, and make server busy. */
		++num_entities_served;
		server_status = BUSY;

		/* Schedule a departure (service completion). */
		time_next_event[2] = sim_time + expon(mean_service);
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
		++num_entities_served;
		time_next_event[2] = sim_time + expon(mean_service);

		/* Move each customer in queue (if any) up one place. */
		for (i = 1; i <= num_in_q; ++i)
		{
			time_arrival[i] = time_arrival[i + 1];
		}
	}
}

void report(void)
{ /*Report generator function */
	/* Compute and write estimates of desired measures of performance. */
	fprintf(outfile, "_________________________________________________________\n");
	fprintf(outfile, "Tiempo de espera en la cola promedio de %11.3f minutos\n\n", total_service_time / num_entities_served);
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

int poisson(float mean)
{
	int poi_value; // Computed Poisson value to be returned
	double t_sum;  // Time sum value

	// Loop to generate Poisson values using exponential distribution
	poi_value = 0;
	t_sum = 0.0;
	while (1)
	{
		t_sum = t_sum + expon(mean);
		if (t_sum >= 1.0)
			break;
		poi_value++;
	}
	return (poi_value);
}