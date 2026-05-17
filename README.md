## Race Condition: 
#Identifique exactamente cuáles líneas del código de Fase 1 constituyen una race condition. ¿Por qué la instrucción en_cruce = 1 sola no es suficiente para garantizar exclusión mutua?
R/ Las siguientes partes constituyen race condition ya que son variables compartidas entre hilos y se modifican simultaneamente. La instruccion 
```c 
    en_cruce = 1
```
no garantiza exclusion mutua porque no es una operacion atomica ni esta protegida

```c
    if (en_cruce == 1) {
      accidentes++;
      printf("ACCIDENTE detectado con %s\n", nombre);
    }
    
    int time = 2500 + rand_r(&semilla) % 3000;
    en_cruce = 1; 
    usleep(time);
    en_cruce = 0;

    cola[numero_de_carril]--;
    vehiculos_cruzados++;
    cruzados[numero_de_carril]++;
```

## Invariante del semáforo: 
#¿Qué invariante garantiza el semáforo binario en Fase 2? Explíquelo con sus propias palabras. ¿Por qué se inicializa en 1 y no en 0?

R/ El uso de semaforos permite que un carro pueda cruzar correctamente. Mientras un hilo tenga el semáforo, los demás deben esperar.

```c
sem_wait(&semaforo_cruce);
sem_post(&semaforo_cruce);
```

Se inicializa en `1` porque el cruce está inicialmente libre y puede ser usado por el carro. Si se inicializara en `0`, todos los hilos quedarían bloqueados porque ningún hilo podría pasar por el semáforo para entrar al cruce.

## Overhead: 
#¿Por qué la Fase 2 suele tomar más tiempo que la Fase 1? ¿Este overhead es aceptable? ¿En qué tipo de sistema NO sería aceptable?
R/ Esto se debe a tiempos de espera dados por el semaforo y entre hilos, es aceptable ya que no es tanto tiempo en comparacion. No serìa aceptable en sistemas de tiempo real estricto.

## Experimento: Presente los resultados del experimento de escala (N_VEHICULOS = 5, 20, 50). 
#¿Cómo cambia la tasa de accidentes al aumentar el número de vehículos? ¿Tiene sentido ese comportamiento?
R/ Bajo la fase 1 la tasa de accidentes crece de manera proporcional a la cantidad de carros, en teorìa. Pero la practica muestra que con 50 carros por carril caen los accidentes, esto tiene sentido pensando en la cantidad de carriles. Con la fase 2 independientemente de los carros, no hay choques.
5= 3
20= 17
50= 4

## Extensión (opcional, bonus +0.5): 
#¿Cómo modificaría el sistema para permitir que más de un vehículo cruce simultáneamente (carril de doble vía)? ¿Qué tipo de semáforo usaría?

R/Para ello hay que aumentar los valores en el sem, lo que harìa una "doble via" y debe usarse "semaforo_cruce"

```c
sem_init(&semaforo_cruce, 0, 2);
```