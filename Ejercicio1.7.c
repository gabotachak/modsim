//SIMULADOR DE SISTEMA DE COLA
//CÓDIGO REALIZADO CON BASE EN EL LIBRO DE SIMULACIÓN DE LAW

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lib/lcgrand.h" /* Header file for the random-number-generator */

#define Q_LIMIT_1 100 /* Limit on the queue length */
#define Q_LIMIT_2 100 /* Limit on the queue length */
#define BUSY 1        /* Mnemonics for server's being busy */
#define IDLE 0        /* and idle. */

int simulation, next_event_type, num_events, num_custs_delayed_1, num_custs_delayed_2, total_delays_1, total_delays_2, num_in_q_1, num_in_q_2, server_status;
float mean_interarrival_1, mean_interarrival_2, mean_service_1, mean_service_2, mean_simulations_1, mean_simulations_2, sim_time, time_end, time_last_event;

float time_next_event[10],
    time_arrival_1[Q_LIMIT_1 + 1], mean_service1, total_delays1, area_num_q1, area_server_status,
    time_arrival_2[Q_LIMIT_2 + 1], mean_service2, total_delays2, area_num_q2;
FILE *infile, *outfile;

void initialize(void);
void timing(void);
void arrive(int);
void depart(int);
void report(void);
void update_time_avg_stats(void);
float expon(float mean);
//float poisson(float mean);

int main()
{
    /* Archivos de entrada y salida - CAMBIAR NOMBRES PARA CADA EJERCICIO*/
    infile = fopen("mm1.7.in", "r");
    outfile = fopen("mm1.7.out", "w");

    //fscanf(infile, "%f %f %f %f %f", &mean_interarrival_1, &mean_service1, &mean_interarrival_2, &mean_service2, );

    int i = 0;
    mean_simulations_1 = mean_simulations_2 = 0;
    for (i; i < 100; i++)
    {
        simulation = i;
        mean_interarrival_1 = 12;
        mean_interarrival_2 = 10;
        mean_service_1 = 6;
        mean_service_2 = 5;
        num_events = 5;

        fprintf(outfile, "\n %d Simulación", simulation);
        fprintf(outfile, "Media de tiempo entre llegadas de personas%16.3f minutos\n\n", mean_interarrival_1);
        fprintf(outfile, "Media de tiempo de servicio a personas%16.3f minutos\n\n", mean_service_1);
        fprintf(outfile, "Media de tiempo entre llegadas de llamadas%16.3f minutos\n\n", mean_interarrival_2);
        fprintf(outfile, "Media de tiempo de servicio de llamadas%11.3f minutos\n\n", mean_service_2);

        initialize();

        while (next_event_type != 5)
        {
            timing();
            update_time_avg_stats();
            switch (next_event_type)
            {
            case 1:
                arrive(1);
                break;
            case 2:
                depart(1);
                break;
            case 3:
                arrive(2);
                break;
            case 4:
                depart(2);
                break;
            }
        }

        mean_simulations_1 += total_delays_1 / num_custs_delayed_1;
        mean_simulations_2 += total_delays_2 / num_custs_delayed_2;
        report();
    }
    fprintf(outfile, "\n\n Average delay time in simulations-calls %11.3f minutes\n\n",
            mean_simulations_2 / 100.0);
    fprintf(outfile, "Average delay time in simulations-persons %11.3f minutes\n\n",
            mean_simulations_1 / 100.0);
    fclose(outfile);

    return 0;
}

void initialize(void)
{ /* Initialize function. */
    /* Initialize the simulation clock. */
    sim_time = 0.0;
    next_event_type = 0;
    /* Initialize the state variables. */
    server_status = IDLE;
    num_in_q_1 = 0;
    num_in_q_2 = 0;
    time_last_event = 0.0;

    /* Initialize the statistical counters. */
    num_custs_delayed_1 = 0;
    num_custs_delayed_2 = 0;
    total_delays_1 = 0.0;
    total_delays_2 = 0.0;
    area_num_q1 = 0.0;
    area_num_q2 = 0.0;
    area_server_status = 0.0;

    time_next_event[1] = 2;
    time_next_event[2] = 1.0e+30;
    time_next_event[3] = 3;
    time_next_event[4] = 1.0e+30;
    time_next_event[5] = 480.0;
}

void timing(void)
{ /* Timing function. */
    int i;
    float min_time_next_event = 1.0e+29;
    next_event_type = 0;
    for (i = 1; i <= num_events; i++)
    {
        if (time_next_event[i] < min_time_next_event)
        {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }
    }
    sim_time = min_time_next_event;
}

void arrive(int cust)
{ /* Arrival event function. */
    float delay, mean_interarrival;

    if (cust == 1)
    {
        mean_interarrival = mean_interarrival_1;
        time_next_event[1] = sim_time + expon(mean_interarrival);
    }
    else
    {
        mean_interarrival = mean_interarrival_2;
        time_next_event[3] = sim_time + expon(mean_interarrival);
    }
    if (server_status == BUSY && cust == 1)
    {
        ++num_in_q_1;
        time_arrival_1[num_in_q_1] = sim_time;
    }
    if (server_status == BUSY && cust == 2)
    {
        ++num_in_q_2;
        time_arrival_2[num_in_q_2] = sim_time;
    }
    else
    {
        if (cust == 1)
        {
            delay = 0.0;
            total_delays_1 += delay;
            ++num_custs_delayed_1;
            server_status = BUSY;
            time_next_event[2] = sim_time + expon(mean_service_1);
        }
        else
        {
            if (num_in_q_1 == 0)
            {
                delay = 0.0;
                total_delays_2 += delay;
                ++num_custs_delayed_2;
                server_status = BUSY;
                time_next_event[4] = sim_time + expon(mean_service_2);
            }
            else
            {
                ++num_in_q_2;
                time_arrival_2[num_in_q_2] = sim_time;
            }
        }
    }
}

void depart(int cust)
{ /*Departure event function. */
    int i;
    float delay;

    if (num_in_q_1 == 0 && num_in_q_2 == 0)
    {
        server_status = IDLE;
        time_next_event[2] = 1.0e+30;
        time_next_event[4] = 1.0e+30;
    }
    else if (num_in_q_1 != 0)
    {
        --num_in_q_1;
        delay = sim_time - time_arrival_1[1];
        total_delays_1 += delay;
        ++num_custs_delayed_1;
        time_next_event[2] = sim_time + expon(mean_service_1);
        for (i = 1; i <= num_in_q_1; i++)
        {
            time_arrival_1[i] = time_arrival_1[i + 1];
        }
    }
    else if (num_in_q_2 != 0)
    {
        --num_in_q_2;
        delay = sim_time - time_arrival_2[1];
        total_delays_2 += delay;
        ++num_custs_delayed_2;
        time_next_event[4] = sim_time + expon(mean_service_2);
        for (i = 1; i <= num_in_q_2; i++)
        {
            time_arrival_2[i] = time_arrival_2[i + 1];
        }
    }
}

/* void report(void)
{ /*Report generator function */
/* Compute and write estimates of desired measures of performance. */
/*fprintf(outfile, "_________________________________________________________\n");
	fprintf(outfile, "Tiempo de espera en la cola promedio de %11.3f minutos\n\n", total_service_time / num_entities_served);
	fprintf(outfile, "Número promedio de entidades en la cola %10.3f\n\n", area_num_in_q / sim_time);
	fprintf(outfile, "Utilización del sistema servidor%15.3f\n\n", area_server_status / sim_time);
	fprintf(outfile, "La simulación termina en %12.3f minutos", sim_time);
}*/

void update_time_avg_stats(void)
{ /* Update area accumulators for time-average statistics. */
    float time_since_last_event;

    /* Compute time since last event, and update last-event-time- marker. */
    time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;

    /* Update area under number-in-queue function */
    area_num_q1 += num_in_q_1 * time_since_last_event;
    area_num_q2 += num_in_q_2 * time_since_last_event;

    /* Update area under server-busy indicator function. */
    area_server_status += server_status * time_since_last_event;
}

float expon(float mean)
{ /* Exponential variate generation function. */
    /* Return an exponential random variate with mean "mean". */
    return -mean * log(lcgrand(1));
}

/*float poisson(float mean)  /* Exponential variate generation function. */
/*{
    int f = rand() % 20;
    return -mean * log((lcgrand(f)));
}*/
void report(void)
{ /*Report generator function */
    /* Compute and write estimates of desired measures of performance. */
    fprintf(outfile, "_________________________________________________________\n");
    fprintf(outfile, "Tiempo de espera en la cola de personas promedio de %11.3d minutos\n\n", total_delays_1 / num_custs_delayed_1);
    fprintf(outfile, "Tiempo de espera en la cola de llamadas promedio de %11.3d minutos\n\n", total_delays_2 / num_custs_delayed_2);
    fprintf(outfile, "Número promedio de entidades en la cola de personas %10.3f\n\n", area_num_q1 / sim_time);
    fprintf(outfile, "Número promedio de entidades en la cola de llamadas %10.3f\n\n", area_num_q2 / sim_time);
    fprintf(outfile, "Utilización del sistema servidor%15.3f\n\n", area_server_status / sim_time);
    fprintf(outfile, "La simulación termina en %12.3f minutos", sim_time);
}