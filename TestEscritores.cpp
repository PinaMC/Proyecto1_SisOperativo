#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <random>
#include <unistd.h>

using namespace std;

// Constantes de configuración
#define MAX_LECTURAS_CONSECUTIVAS 5
#define TIEMPO_ESCRITURA 2
#define TIEMPO_LECTURA_MAX 3
#define NUM_LECTORES 5
#define NUM_ESCRITORES 2

// Variables compartidas
int lecturas_consecutivas = 0;
mutex mutex_control;
mutex cout_mutex; 
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

//se creara la funcion protectora de los cout
void print_safe(const string& messege){
    lock_guard<mutex> lock(cout_mutex);
    cout<<messege<<endl;
}

void Lector(int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribucion_tiempo(1, TIEMPO_LECTURA_MAX);

    while (true) { // aqui va toda la sintaxis 0w0
    print_safe("[Lector " + to_string(id) + "] intentando acceder.");

        unique_lock<mutex> lock(mutex_control);
        if(!escritor_esperando && lectores_en_sala < MAX_LECTURAS_CONSECUTIVAS && lecturas_consecutivas < MAX_LECTURAS_CONSECUTIVAS) {
            
            lectores_en_sala++;
            lecturas_consecutivas++;
            print_safe("[Lector " + to_string(id) + "] leyendo (lectura " + to_string(lecturas_consecutivas) + 
                      "/" + to_string(MAX_LECTURAS_CONSECUTIVAS) + ")");
            lock.unlock();

            // Simular tiempo de lectura
            this_thread::sleep_for(chrono::seconds(distribucion_tiempo(gen)));

            lock.lock();
            lectores_en_sala--;
            if(lectores_en_sala == 0){
                sem_post(&sem_escritor);
                lecturas_consecutivas = 0;
            }
            lock.unlock();

            print_safe("[Lector " + to_string(id) + "] terminó de leer.");
            break;
        } else {
            print_safe("[Lector " + to_string(id) + "] esperando (escritor: " + to_string(escritor_esperando) + 
                      ", lectores: " + to_string(lectores_en_sala) + ", lecturas: " + 
                      to_string(lecturas_consecutivas) + ").");
            lock.unlock();
            sem_wait(&sem_lector);
        }
    }
}

void Escritor(int id) {
    print_safe("[Escritor " + to_string(id) + "] intentando acceder.");

    unique_lock<mutex> lock(mutex_control);
    escritor_esperando = true;
    print_safe("[Escritor " + to_string(id) + "] esperando turno tras " + 
               to_string(lecturas_consecutivas) + " lecturas.");
    lock.unlock();

    sem_wait(&sem_escritor);

    lock.lock();
    escritor_esperando = false;
    print_safe("[Escritor " + to_string(id) + "] escribiendo...");
    lock.unlock();

    // Simulacion del tiempo de escritura
    this_thread::sleep_for(chrono::seconds(TIEMPO_ESCRITURA));

    lock.lock();
    print_safe("[Escritor " + to_string(id) + "] terminó de escribir.");
    lecturas_consecutivas = 0;
    
    // Notificar a los lectores
    for (int i = 0; i < MAX_LECTURAS_CONSECUTIVAS; ++i) {
        sem_post(&sem_lector);
    }
    lock.unlock();
}

int main() {
    // creacion de los hilos
    vector<thread> lectores;
    vector<thread> escritores;
    // Inicializar semáforos
    //el se init tiene un puntero al semaforo, un pshared que indicaa hilos a compartir
    //y un valor inicial, por lo que primero lo señalamos a sem lector, indicamos
    //sus hilos a compartir al inicio y el tope max, que es la var gen de max lect
  
    if(sem_init(&sem_lector, 0, MAX_LECTURAS_CONSECUTIVAS) == -1){
        print_safe("Error al Inicializar el semaforo del lector");
        return 1;
    }
    if(sem_init(&sem_escritor, 0, 0) == -1){
        print_safe("Error al iniciar semaforo de escritores");
        return 1;
    }
    //----------------------------------------------------------
    //              creacion lectores y escritores
    //----------------------------------------------------------

    int i = 0;
    while(i < NUM_LECTORES){ 
        lectores.emplace_back(Lector, i + 1);
        ++i;
    }
    int ii= 0;
   /* while(i< NUM_ESCRITORES){
        escritores.emplace_back(Escritor, ii +1);
        ++ii;
    }*/
    int count = NUM_ESCRITORES;
    while(count > 0){
        escritores.emplace_back(Escritor, count);
        count --;
    }

    // Esperar a que terminen
    for(auto& t : lectores){
        t.join();
    }
    for(auto& t : escritores){
        t.join();
    }

    // Liberar recursos
    sem_destroy(&sem_lector);
    sem_destroy(&sem_escritor);

    return 0;
}


// */#include <stdio.h>
/*
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// --- Parámetros configurables ---
#define N_LECTORES 3
#define N_ESCRITORES 2
#define MAX_LECTURAS_CONSECUTIVAS 5
#define CICLOS_LECTOR 4
#define CICLOS_ESCRITOR 2

// --- Variables globales ---
int lectores_activos = 0;
int escritores_activos = 0;
int escritores_esperando = 0;
int lecturas_consecutivas = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_lector;
sem_t sem_escritor;

// --- Funciones auxiliares ---
void *lector(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < CICLOS_LECTOR; i++) {
        usleep(rand() % 500000); // Simula tiempo antes de intentar leer

        pthread_mutex_lock(&mutex);
        if (escritores_esperando > 0 && lecturas_consecutivas >= MAX_LECTURAS_CONSECUTIVAS) {
            pthread_mutex_unlock(&mutex);
            sem_wait(&sem_lector); // Se bloquea esperando turno
        } else {
            lectores_activos++;
            lecturas_consecutivas++;
            pthread_mutex_unlock(&mutex);
        }

        // Sección crítica de lectura
        printf("[Lector %d] leyendo (lectura %d/%d)\n", id, i + 1, CICLOS_LECTOR);
        usleep(200000); // Simula tiempo de lectura

        pthread_mutex_lock(&mutex);
        lectores_activos--;
        if (lectores_activos == 0 && escritores_esperando > 0 && lecturas_consecutivas >= MAX_LECTURAS_CONSECUTIVAS) {
            sem_post(&sem_escritor);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *escritor(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < CICLOS_ESCRITOR; i++) {
        usleep(rand() % 800000); // Simula tiempo antes de intentar escribir

        pthread_mutex_lock(&mutex);
        escritores_esperando++;
        while (lectores_activos > 0 || escritores_activos > 0) {
            pthread_mutex_unlock(&mutex);
            sem_wait(&sem_escritor);
            pthread_mutex_lock(&mutex);
        }
        escritores_esperando--;
        escritores_activos++;
        pthread_mutex_unlock(&mutex);

        // Sección crítica de escritura
        printf("[Escritor %d] escribiendo (escritura %d/%d)\n", id, i + 1, CICLOS_ESCRITOR);
        usleep(400000); // Simula tiempo de escritura

        pthread_mutex_lock(&mutex);
        escritores_activos--;
        lecturas_consecutivas = 0;

        if (escritores_esperando > 0) {
            sem_post(&sem_escritor);
        } else {
            for (int i = 0; i < N_LECTORES; i++) {
                sem_post(&sem_lector);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

// --- Main ---
int main() {
    pthread_t lectores[N_LECTORES], escritores[N_ESCRITORES];
    int ids_lectores[N_LECTORES], ids_escritores[N_ESCRITORES];

    sem_init(&sem_lector, 0, 0);
    sem_init(&sem_escritor, 0, 0);

    for (int i = 0; i < N_LECTORES; i++) {
        ids_lectores[i] = i + 1;
        pthread_create(&lectores[i], NULL, lector, &ids_lectores[i]);
    }

    for (int i = 0; i < N_ESCRITORES; i++) {
        ids_escritores[i] = i + 1;
        pthread_create(&escritores[i], NULL, escritor, &ids_escritores[i]);
    }

    for (int i = 0; i < N_LECTORES; i++) {
        pthread_join(lectores[i], NULL);
    }
    for (int i = 0; i < N_ESCRITORES; i++) {
        pthread_join(escritores[i], NULL);
    }

    sem_destroy(&sem_lector);
    sem_destroy(&sem_escritor);
    pthread_mutex_destroy(&mutex);

    printf("Todos los hilos han terminado.\n");
    return 0;
}/
*/