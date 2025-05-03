# Proyecto1_SisOperativo
 proyecto 1 de sistema operativo y redes realizado por Vicente Farías y Martin Vera


Lo que pide el proyecto:


Variante 3: Lector preferente con límite (Martín Vera - Vicente Farias)
Hilos necesarios: mínimo 3, ideal hasta 6.

Motivación: Evitar que los escritores sean bloqueados sin eliminar la eficiencia de la lectura simultánea.

Problema a resolver: Lectores que se suceden uno tras otro pueden bloquear a los escritores indefinidamente.

Estrategia de solución:
•	Se permite que N lectores lean consecutivamente.
•	Tras cierto umbral (MAX_LECTURAS_CONSECUTIVAS), se fuerza el paso a un escritor.

Estructuras necesarias:
•	int lecturas_consecutivas = 0;
•	mutex: protege las variables compartidas.
•	sem_lector, sem_escritor: bloquean o liberan paso.
•	Variable para saber si hay escritores esperando.

Ventajas:
•	Conserva la eficiencia de múltiples lectores.
•	Evita inanición de escritores sin sacrificar rendimiento.

Desventajas:
•	Más variables de control.
•	Difícil ajustar el umbral correctamente para todos los escenarios.

Ejemplo de salida esperada:
[Lector 1] leyendo (lectura 1/5)
[Lector 2] leyendo (lectura 2/5)
...
[Lector 5] leyendo (lectura 5/5)
[Escritor 1] esperando turno tras 5 lecturas.
[Lector 6] bloqueado temporalmente.
[Escritor 1] escribiendo...




--------------------------Avances--------------------------------
|    1- Objetivo es crear lectores maximos                       |
|    2- Tener contador de lectores ingresando                    |
|    3- Maximo de lectores para ingresar                         |
|    4- Maximo de lectores leyendo                               |
|    5- Umbral del "cambio" del semaforo                         |
--------------------------RESOLUCION-----------------------------

Los lectores tienen prioridad para leer y "actuar" en el codigo, por lo tanto mientras hayan lectores queriendo leer pueden entrar de forma simultánea.

Los escritores tienen que esperar que no hay ningún lector activo, el problema esta en que si hay una cantidad muy grande de lectores los escritores nunca pueden "escribir".

Bajo este enfoque podemos decir que necesitamos una forma de "forzar" el turno del escritor para que pase al estado de activo y pueda escribir. Para realizarlo se necesita que después de que X cantidad de lectores lean consecutivamente el programa "pause" los pausen y active al escritor



fin