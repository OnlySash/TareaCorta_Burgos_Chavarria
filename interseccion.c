/* FASE 2 ESPERADA DEL CRITICA - REFERENCIA
sem_wait(&semaforo_cruce); // P() - intentar entrar al cruce
en_cruce = 1;
// ... cruzar ...
en_cruce = 0;
sem_post(&semaforo_cruce); // V() - liberar el cruce
*/

/*========================================================
SIMULADOR DE INTERSECCION DE TRAFICO - CI-0117
Carriles: 4 | Vehiculos por carril: 10
========================================================*/
/*--- FASE 1: Sin sincronizacion ---*/
/*[Norte-001] entrando al cruce
[Sur-001] entrando al cruce <- ACCIDENTE con Norte-001
[Norte-001] cruce completado
...
--- FASE 2: Con semaforos ---
[Norte-001] sem_wait() -> cruzando
[Norte-001] sem_post() -> cruce libre
[Sur-001] sem_wait() -> cruzando
...
======== REPORTE FINAL ========
FASE 1 (sin sincronizacion):
Total vehiculos: 40
Accidentes: 7 <- debe ser > 0
Vehiculos/carril: Norte=10 Sur=10 Este=10 Oeste=10
Tiempo simulacion: 0.312 segundos
FASE 2 (con semaforos):
Total vehiculos: 40
Accidentes: 0 <- debe ser exactamente 0
Vehiculos/carril: Norte=10 Sur=10 Este=10 Oeste=10
Tiempo simulacion: 0.418 segundos
ANALISIS:
Overhead de sincronizacion: +0.106 seg (+34.0%)
=======================================================*/

/*
 * interseccion.c
 * Simulador de Interseccion de Trafico Concurrente
 * CI-0117 Programacion Paralela y Concurrente - I Ciclo 2026
 *
 * FASE 1: Sin sincronizacion (demuestra race condition)
 * FASE 2: Con semaforos POSIX (lo hace tu compañero)
 *
 * Compilar Fase 1: gcc -Wall -Wextra -std=c11 interseccion.c -o interseccion
 * -lpthread Compilar Fase 2: gcc -Wall -Wextra -std=c11 -DFASE2 interseccion.c
 * -o interseccion -lpthread
 */

/* Necesario para usleep(), rand_r() y clock_gettime() en C11 */

// 
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
#define N_VEHICULOS 10

const char *nombres[N_CARRILES] = {"Norte", "Sur", "Este", "Oeste"};

int vehiculos_cruzados = 0;
int accidentes = 0;
int en_cruce = 0;
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
    printf("[%s] cruzando\n", nombre);

    // aqui es donde se da la Race condition en Fase 1, y donde se debe usar semaforos en Fase 2
    if (en_cruce == 1) {
      accidentes++;
      printf("ACCIDENTE detectado con %s\n", nombre);
    }
    
    int tiempo = 2500 + rand_r(&semilla) % 3000;
    en_cruce = 1; //marcar que hay un vehiculo en el cruce (sin proteccion)
    usleep(tiempo);
    en_cruce = 0;

    printf("[%s] cruzo\n", nombre);

    // aqui se actualizan los contadores sin proteccion (tambien es race condition)
    cola[numero_de_carril]--;
    vehiculos_cruzados++;
    cruzados[numero_de_carril]++;
  }
  return NULL;
}

int main(void) {
  pthread_t hilos[N_CARRILES];
  int ids[N_CARRILES];

  for (int i = 0; i < N_CARRILES; i++) {
    cola[i] = 0;
    cruzados[i] = 0;
    ids[i] = i;
  }

  sem_init(&semaforo_cruce, 0, 1);
  sem_init(&mutex_contadores, 0, 1);

  printf("tarea de interseccion de vehiculos\n");
  printf("Vehiculos por carril: %d\n\n", N_VEHICULOS);

  double inicio = tiempo_actual();

  for (int i = 0; i < N_CARRILES; i++) {
    pthread_create(&hilos[i], NULL, hilo_carril, &ids[i]);
  }
  for (int i = 0; i < N_CARRILES; i++) {
    pthread_join(hilos[i], NULL);
  }

  double fin = tiempo_actual();

  printf("\nDatos de la simulacion:\n");
  printf("Vehiculos cruzados: %d\n", vehiculos_cruzados);
  printf("Accidentes: %d\n", accidentes);
  printf("Norte: %d | Sur: %d | Este: %d | Oeste: %d\n", cruzados[0],
         cruzados[1], cruzados[2], cruzados[3]);
  printf("Tiempo total: %.3f segundos\n", fin - inicio);

  sem_destroy(&semaforo_cruce);
  sem_destroy(&mutex_contadores);

  return 0;
}