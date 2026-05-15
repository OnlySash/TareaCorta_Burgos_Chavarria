




int main (){
    /* === VARIABLES GLOBALES (compartidas entre hilos) === */
    int vehiculos_cruzados = 0; /* total de vehiculos que han cruzado */
    int accidentes = 0; /* cruces simultaneos detectados */
    int en_cruce = 0; /* 1 si hay vehiculo en cruce, 0 si no */
    /* Una entrada por carril: Norte=0, Sur=1, Este=2, Oeste=3 */
    int cola[4]; /* vehiculos esperando por carril */
    int cruzados_por_carril[4]; /* vehiculos que cruzaron por carril */
    sem_t semaforo_cruce; /* controla acceso al cruce central */
    sem_t mutex_contadores; /* protege vehiculos_cruzados y accidentes */


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
}