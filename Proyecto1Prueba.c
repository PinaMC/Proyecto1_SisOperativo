#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/** Funciones de hilos
 * pthread_create: crea un nuevo hilo.
 * pthread_join: espera a que un hilo termine su ejecución.
 * pthread_mutex_lock: bloquea un mutex.
 * pthread_mutex_unlock: desbloquea un mutex.
 * pthread_cond_wait: espera a que una condición se cumpla.
 * pthread_cond_signal: despierta un hilo que está esperando una condición.
 * pthread_cond_broadcast: despierta a todos los hilos que están esperando una condición.
 * pthread_mutex_init: inicializa un mutex.
 * pthread_mutex_destroy: destruye un mutex.
 * pthread_cond_init: inicializa una variable de condición.
 * pthread_cond_destroy: destruye una variable de condición.
 * pthread_exit: termina la ejecución de un hilo.
 * pthread_self: obtiene el ID del hilo que está ejecutando la función.

 * 
*/
//Falta agregar semaforos y mutexes

#define MAX_LECTURAS_CONSECUTIVAS 5
#define MAX_LECTURAS 4
#define N_LECTORES 4
#define N_ESCRITORES 2


// Variables compartidas
int lectores_activos = 0;
int lectores_esperando = 0;
int escritores_activos = 0;
int escritores_esperando = 0;
int lecturas_consecutivas = 0;
int ciclos_completados = 0;
bool terminar = false;


// Sincronización
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_lectores = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_escritores = PTHREAD_COND_INITIALIZER;

// Funciones de lectura/escritura simuladas
void leer(int id) { //sirve para simular y ver que el lector lee
    printf("[Lector %d] intentando acceder.\n", id);
    printf("[Lector %d] leyendo (lectura %d/%d)\n", id, lecturas_consecutivas, MAX_LECTURAS_CONSECUTIVAS);
    sleep(1);
}

void escribir(int id) { //sirve para simular y ver que el escritor escribe
    printf("[Escritor %d] intentando acceder para escribir.\n", id);
    printf("[Escritor %d] escribiendo...\n", id);
    
    sleep(2);
}

// Lector
void* lector(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);//bloquea el mutex

        lectores_esperando++;

        while (escritores_activos > 0 || 
              (lecturas_consecutivas >= MAX_LECTURAS_CONSECUTIVAS && escritores_esperando > 0)) {
            printf("[Lector %d] bloqueado temporalmente.\n", id);
            pthread_cond_wait(&cond_lectores, &mutex);
        }

        lectores_esperando--;
        lectores_activos++;
        lecturas_consecutivas++;

        pthread_mutex_unlock(&mutex);

        leer(id);

        pthread_mutex_lock(&mutex);
        lectores_activos--;

        // Si es el último lector y hay un escritor esperando y se llegó al límite
        if (lectores_activos == 0 && escritores_esperando > 0 &&
            lecturas_consecutivas >= MAX_LECTURAS_CONSECUTIVAS) {
            pthread_cond_signal(&cond_escritores);
        }

        pthread_mutex_unlock(&mutex);

        sleep(1);  // esperar antes de volver a leer
    }
    return NULL;
}

// Escritor
void* escritor(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        escritores_esperando++;

        while (lectores_activos > 0 || escritores_activos > 0) {
            printf("[Escritor %d] esperando turno tras %d lecturas.\n", id, lecturas_consecutivas);
            pthread_cond_wait(&cond_escritores, &mutex);
        }

        escritores_esperando--;
        escritores_activos = 1;
        pthread_mutex_unlock(&mutex);

        escribir(id);

        pthread_mutex_lock(&mutex);
        escritores_activos = 0;
        lecturas_consecutivas = 0;

        // Despertar a lectores si hay
        if (lectores_esperando > 0) {
            pthread_cond_broadcast(&cond_lectores);
        } else {
            // Si no hay lectores, despertar a otro escritor
            pthread_cond_signal(&cond_escritores);
        }

        pthread_mutex_unlock(&mutex);

        sleep(2);  // esperar antes de volver a escribir
    }
    return NULL;
}



//--------------------funcion principal------------------
int main() {
    pthread_t lectores[N_LECTORES], escritores[N_ESCRITORES];
    int id_lectores[N_LECTORES], id_escritores[N_ESCRITORES];

    for (int i = 0; i < N_LECTORES; i++) {
        id_lectores[i] = i + 1;
        pthread_create(&lectores[i], NULL, lector, &id_lectores[i]);
    }

    for (int i = 0; i < N_ESCRITORES; i++) {
        id_escritores[i] = i + 1;
        pthread_create(&escritores[i], NULL, escritor, &id_escritores[i]);
    }

    for (int i = 0; i < N_LECTORES; i++) {
        pthread_join(lectores[i], NULL);
    }

    for (int i = 0; i < N_ESCRITORES; i++) {
        pthread_join(escritores[i], NULL);
    }

    return 0;
}

