## Race Condition: 
Identifique exactamente cuáles líneas del código de Fase 1
constituyen una race condition. ¿Por qué la instrucción en_cruce = 1 sola no es
suficiente para garantizar exclusión mutua?
## Invariante del semáforo: 
¿Qué invariante garantiza el semáforo binario en Fase 2?
Explíquelo con sus propias palabras. ¿Por qué se inicializa en 1 y no en 0?
## Overhead: 
¿Por qué la Fase 2 suele tomar más tiempo que la Fase 1? ¿Este
overhead es aceptable? ¿En qué tipo de sistema NO sería aceptable?
## Experimento: Presente los resultados del experimento de escala (N_VEHICULOS =
5, 20, 50). ¿Cómo cambia la tasa de accidentes al aumentar el número de vehículos?
¿Tiene sentido ese comportamiento?
## Extensión (opcional, bonus +0.5): 
¿Cómo modificaría el sistema para permitir que
más de un vehículo cruce simultáneamente (carril de doble vía)? ¿Qué tipo de
semáforo usaría?