/*Simulador de Interseccion de Trafico Concurrente
  CI-0117 Programacion Paralela y Concurrente - I Ciclo 2026
 
  Compilar: gcc -Wall -Wextra -std=c11 interseccion.c -o interseccion -lpthread 
 */

#define _XOPEN_SOURCE 600 // El #define le dice al compilador antes de leer cualquier header: "activame las extensiones POSIX"
// y ahí ya reconoce algunas fucniones usadas en el código como usleep() y rand_r() que no son parte del estándar C11 pero si de POSIX
// esto porque el profe pide el std 11 porque creo que si se usa un std mas nuevo el compilador no se queja con los warnings

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N_CARRILES 4
#define N_VEHICULOS 50

const char *nombres[N_CARRILES] = {"Norte", "Sur", "Este", "Oeste"};

int vehiculos_cruzados = 0;
int vehiculos_cruzados_sem = 0;
int accidentes = 0;
int accidentes_sem = 0;
int en_cruce = 0;
int en_cruce_sem = 0;
double tiempo = 0;
double tiempo_sem = 0;
int cola[N_CARRILES];
int cruzados[N_CARRILES];

sem_t semaforo_cruce;
sem_t mutex_contadores;

double tiempo_actual(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    
    double segundos     = ts.tv_sec;
    double nanosegundos = ts.tv_nsec / 1000000000.0;
    
    return segundos + nanosegundos;
}

void *hilo_carril(void *dato_del_carril) {
  int numero_de_carril = *(int *)dato_del_carril;
  unsigned int semilla = numero_de_carril + 1;

  for (int i = 1; i <= N_VEHICULOS; i++) {
    char nombre[20];
    sprintf(nombre, "%s-%d", nombres[numero_de_carril], i);

    cola[numero_de_carril]++;
    //printf("[%s] cruzando\n", nombre);

    // aqui es donde se da la Race condition en Fase 1, y donde se debe usar semaforos en Fase 2
    if (en_cruce == 1) {
      accidentes++;
      printf("ACCIDENTE detectado con %s\n", nombre);
    }
    
    int time = 2500 + rand_r(&semilla) % 3000;
    en_cruce = 1; //marcar que hay un vehiculo en el cruce (sin proteccion)
    usleep(time);
    en_cruce = 0;

    //printf("[%s] cruzo\n", nombre);

    // aqui se actualizan los contadores sin proteccion (tambien es race condition)
    cola[numero_de_carril]--;
    vehiculos_cruzados++;
    cruzados[numero_de_carril]++;
  }
  return NULL;
}

void no_sincronizacion(){
  pthread_t hilos[N_CARRILES];
  int ids[N_CARRILES];

  for (int i = 0; i < N_CARRILES; i++) {
    cola[i] = 0;
    cruzados[i] = 0;
    ids[i] = i;
  }
  
  printf("--- FASE 1: Sin sincronizacion ---\n");
  double inicio = tiempo_actual();

  for (int i = 0; i < N_CARRILES; i++) {
    pthread_create(&hilos[i], NULL, hilo_carril, &ids[i]);
  }
  for (int i = 0; i < N_CARRILES; i++) {
    pthread_join(hilos[i], NULL);
  }

  double fin = tiempo_actual();
  tiempo = fin - inicio;

  printf("======== REPORTE ======== \n FASE 1 (sin sincronizacion):\n");
  printf("Total vehiculos: %d\n", vehiculos_cruzados);
  printf("Accidentes: %d\n", accidentes);
  printf("Vehiculos/carril Norte: %d | Sur: %d | Este: %d | Oeste: %d\n", cruzados[0],
         cruzados[1], cruzados[2], cruzados[3]);
  printf("Tiempo simulacion: %.3f segundos\n", tiempo);
}

void *hilo_carril_sem(void *dato_del_carril) {
  int numero_de_carril = *(int *)dato_del_carril;
  unsigned int semilla = numero_de_carril + 1;

  for (int i = 1; i <= N_VEHICULOS; i++) {
    char nombre[20];
    sprintf(nombre, "%s-%d", nombres[numero_de_carril], i);

    cola[numero_de_carril]++;

    // aqui es donde se da la Race condition en Fase 1
    sem_wait(&semaforo_cruce);
    printf("[%s] cruzando\n", nombre);

    //La variable en_cruce_sem ya no tiene forma de ser 1
    if (en_cruce_sem == 1) {
      accidentes_sem++;
      printf("ACCIDENTE detectado con %s\n", nombre);
    }

    int time = 2500 + rand_r(&semilla) % 3000;
    en_cruce_sem = 1; //marcar que hay un vehiculo en el cruce (sin proteccion)
    usleep(time);
    en_cruce_sem = 0;
    printf("[%s] cruzo\n", nombre);
    sem_post(&semaforo_cruce);

    sem_wait(&mutex_contadores);
    cola[numero_de_carril]--;
    vehiculos_cruzados_sem++;
    cruzados[numero_de_carril]++;
    sem_post(&mutex_contadores);
  }

  return NULL;
}

void semaforo(){
  sem_init(&semaforo_cruce, 0, 1);
  sem_init(&mutex_contadores, 0, 1);

  pthread_t t_sem [N_CARRILES];
  int ids_s [N_CARRILES];

  for (int i = 0; i < N_CARRILES; i++) {
    cola[i] = 0;
    cruzados[i] = 0;
    ids_s [i] = i;
  }
  
  printf("\n--- FASE 2: Con semaforos ---\n");
  double inicio = tiempo_actual();

  for (int i = 0; i < N_CARRILES; i++) {
    pthread_create(&t_sem[i], NULL, hilo_carril_sem, &ids_s[i]);
  }
  for (int i = 0; i < N_CARRILES; i++) {
    pthread_join(t_sem[i], NULL);
  }

  double fin = tiempo_actual();
  tiempo_sem = fin - inicio;
  printf("======== REPORTE ======== \n FASE 2 (con semaforos):\n");
  printf("Total vehiculos: %d\n", vehiculos_cruzados_sem);
  printf("Accidentes: %d\n", accidentes_sem);
  printf("Vehiculos/carril Norte: %d | Sur: %d | Este: %d | Oeste: %d\n", cruzados[0],
         cruzados[1], cruzados[2], cruzados[3]);
  printf("Tiempo simulacion: %.3f segundos\n", tiempo_sem);
  
  sem_destroy(&semaforo_cruce);
  sem_destroy(&mutex_contadores);
}

int main(void) {
  printf("========================================================\n");
  printf("SIMULADOR DE INTERSECCION DE TRAFICO - CI-0117\n");
  printf("========================================================\n");
  printf("Carriles: %d | Vehiculos por carril: %d\n",N_CARRILES, N_VEHICULOS);
  printf("========================================================\n");
  
  no_sincronizacion();
  semaforo();

  double overhead = tiempo_sem - tiempo;
  double porcent = (overhead / tiempo) * 100;
  printf("ANALISIS: \n Overhead de sincronizacion: %.3f  seg (%.2f%%)\n", overhead, porcent);
  printf("========================================================\n");

  return 0;
}