//CÓDIGO REALIZADO CON BASE EN EL LIBRO DE SIMULACIÓN DE LAW

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lib/lcgrand.h" /* Header file for the random-number-generator */
#include <time.h>    // time()

int bus_status, next_event_type, num_min, num_N, num_in_q_A, num_in_q_B,
total_pasajeros_a, total_pasajeros_b, bus_station, numeventos;

float reloj, time_next_event[6], promedio_esperado_a, 
promedio_esperado_b, suma_tiempo_esperado_a, suma_tiempo_esperado_b, 
num_p_esperando_A[51], num_p_esperando_B[51];

float mean_travel  = 31.0; /*Media de 31 minutos entre las 2 fabricas*/
float std_deviation = 5.0; /*Desviación estándar de 5 minutos entre las 2 fabricas*/

float mean_arrival_A = 6.7;
float mean_arrival_B = 12;

FILE *outfile;

void initialize(void);
void timing(void);
void passenger_arrival_A(void);
void passenger_arrival_B(void);
void bus_travel(void);
void bus_arrival(void);
void report(void);
float poisson(float);
float normal(void);

int main(){
    srand(time(0));

    outfile = fopen("mm1.5.out", "w");
    numeventos=5;
    for(int i=1; i<=10; i++){
        num_N=i;
        initialize();
        do{
            timing();
            switch(next_event_type){
            case 1:
                passenger_arrival_A();
                break;
            case 2:
                passenger_arrival_B();
                break;
            case 3:
                bus_travel();
                break;
            case 4:
                bus_arrival();
                break;
            case 5:
                report();
                break;
            }

        }while(next_event_type!=5);
    }
}

void initialize(){
    reloj=0;
    num_in_q_A=0;
    num_in_q_B=0;
    bus_station=1;

    num_min=480;
    bus_status=0;

    suma_tiempo_esperado_a=0.0;
    suma_tiempo_esperado_b=0.0;
    promedio_esperado_a=0.0;
    promedio_esperado_b=0.0;
    total_pasajeros_a=0;
    total_pasajeros_b=0;

    time_next_event[1]=reloj+poisson(mean_arrival_A);   /*llegada de pasajero a A*/
    time_next_event[2]=reloj+poisson(mean_arrival_B);   /*llegada de pasajero B*/
    time_next_event[3]=1.0e+30;           /* inicio y recorrido del bus*/
    time_next_event[4]=1.0e+30;            /*llegada de bus a terminal */
    time_next_event[5]=480.0;         /*finaliza la simulacion*/
}

void timing(){
    int i;
    float minTimeNextEvent = 1.0e+29;
    next_event_type=0;
    for(i = 1; i<=numeventos; i++)
        if(time_next_event[i] < minTimeNextEvent){
           minTimeNextEvent = time_next_event[i];
           next_event_type = i;
        }

    reloj = minTimeNextEvent;
    //sprintf("reloj va en %f \n",reloj);
}

void passenger_arrival_A(){
    //printf("llega pasajero en a %f \n", reloj);
    num_in_q_A++;
    num_p_esperando_A[num_in_q_A] = reloj;
    //printf("cola %i status %i station %i \n",num_in_q_A,bus_status,bus_station);
    if(num_in_q_A>=num_N && bus_status==0 && bus_station==1){
        //printf("se lleno \n");
        bus_travel();
    }
    time_next_event[1]=reloj+poisson(mean_arrival_A);
}

void passenger_arrival_B(){
    //printf("llega pasajero en b %f \n",reloj);
    num_in_q_B++;
    //printf("pasajeros en b %i \n",num_in_q_B);
    num_p_esperando_B[num_in_q_B] = reloj;
    if(num_in_q_B>=num_N && bus_status==0 && bus_station==2){
        //printf("se lleno \n");
        bus_travel();
    }
    time_next_event[2]=reloj+poisson(mean_arrival_B);
}

void bus_travel(){
    bus_status=1;
    if(bus_station==1){
       //printf("sale bus en %f con pasajeros %i \n",reloj,num_in_q_A);
        total_pasajeros_a+=num_in_q_A;
        for(int i=1; i<=num_in_q_A; i++){
            suma_tiempo_esperado_a=suma_tiempo_esperado_a+(reloj-num_p_esperando_A[i]);
            num_p_esperando_A[i]=-1;
        }
        num_in_q_A=0;
    }else{
        //printf("sale bus %f con pasajeros %i \n",reloj,num_in_q_B);
        total_pasajeros_b+=num_in_q_B;
        for(int i=1; i<=num_in_q_B; i++){
            suma_tiempo_esperado_b=suma_tiempo_esperado_b+(reloj-num_p_esperando_B[i]);
            //printf("suma %f \n",suma_tiempo_esperado);
            num_p_esperando_B[i]=-1;
        }
        num_in_q_B=0;
    }
    time_next_event[4]=reloj+normal();
    //printf("time next %f \n",time_next_event[4]);
}

void bus_arrival(){
    //printf("llego bus en %f \n",reloj);
    bus_status=0;
    time_next_event[4]=1.0e+30;
    if(bus_station==1){
        bus_station=2;
        if(num_in_q_B>=num_N){
            bus_travel();
        }
    }else{
        bus_station=1;
        if(num_in_q_A>=num_N){
            bus_travel();
        }
    }
}

void report(){
    promedio_esperado_a = suma_tiempo_esperado_a/total_pasajeros_a;
    promedio_esperado_b = suma_tiempo_esperado_b/total_pasajeros_b;
    //printf("suma %f y pasajeros %i \n",suma_tiempo_esperado,total_pasajeros);
    printf("\n numero N:%i    promedio esperado fila a:%f    promedio esperado fila b:%f     promedio total:%f \n\n",num_N,promedio_esperado_a,promedio_esperado_b,(promedio_esperado_a+promedio_esperado_b)/2);
    fprintf(outfile,"\n numero N:%i    promedio esperado fila a:%f    promedio esperado fila b:%f     promedio total:%f \n\n",num_N,promedio_esperado_a,promedio_esperado_b,(promedio_esperado_a+promedio_esperado_b)/2);
}


float poisson(float mean)  /* Exponential variate generation function. */
{
    int f = rand() % 20;
    return -mean * log((lcgrand(f)));
}

float normal(){
    float aux=0, cK=1000;
    for (int k=1;k<=cK;k++){
        aux=aux+(float)rand()/RAND_MAX;
    }
    return std_deviation*sqrt((float)12/cK)*(aux-(float)cK/2)+mean_travel;
}