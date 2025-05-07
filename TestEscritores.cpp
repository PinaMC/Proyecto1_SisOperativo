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
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

void Lector(int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribucion_tiempo(1, TIEMPO_LECTURA_MAX);

    while (true) {
        cout << "[Lector " << id << "] intentando acceder." << endl;

        unique_lock<mutex> lock(mutex_control);
        if (!escritor_esperando && lectores_en_sala < MAX_LECTURAS_CONSECUTIVAS && 
            lecturas_consecutivas < MAX_LECTURAS_CONSECUTIVAS) {
            
            lectores_en_sala++;
            lecturas_consecutivas++;
            cout << "[Lector " << id << "] leyendo (lectura " << lecturas_consecutivas 
                 << "/" << MAX_LECTURAS_CONSECUTIVAS << ")" << endl;
            lock.unlock();

            // Simular tiempo de lectura
            this_thread::sleep_for(chrono::seconds(distribucion_tiempo(gen)));

            lock.lock();
            lectores_en_sala--;
            if (lectores_en_sala == 0) {
                sem_post(&sem_escritor);
                lecturas_consecutivas = 0;
            }
            lock.unlock();

            cout << "[Lector " << id << "] terminó de leer." << endl;
            break;
        } else {
            cout << "[Lector " << id << "] esperando (escritor: " << escritor_esperando 
                 << ", lectores: " << lectores_en_sala << ", lecturas: " 
                 << lecturas_consecutivas << ")." << endl;
            lock.unlock();
            sem_wait(&sem_lector);
        }
    }
}

void Escritor(int id) {
    cout << "[Escritor " << id << "] intentando acceder." << endl;

    unique_lock<mutex> lock(mutex_control);
    escritor_esperando = true;
    cout << "[Escritor " << id << "] esperando turno tras " 
         << lecturas_consecutivas << " lecturas." << endl;
    lock.unlock();

    sem_wait(&sem_escritor);

    lock.lock();
    escritor_esperando = false;
    cout << "[Escritor " << id << "] escribiendo..." << endl;
    lock.unlock();

    // Simular tiempo de escritura
    this_thread::sleep_for(chrono::seconds(TIEMPO_ESCRITURA));

    lock.lock();
    cout << "[Escritor " << id << "] terminó de escribir." << endl;
    lecturas_consecutivas = 0;
    
    // Notificar a los lectores
    for (int i = 0; i < MAX_LECTURAS_CONSECUTIVAS; ++i) {
        sem_post(&sem_lector);
    }
    lock.unlock();
}

int main() {
    // Inicializar semáforos
    if (sem_init(&sem_lector, 0, MAX_LECTURAS_CONSECUTIVAS) == -1) {
        cerr << "Error al inicializar sem_lector" << endl;
        return 1;
    }
    if (sem_init(&sem_escritor, 0, 0) == -1) {
        cerr << "Error al inicializar sem_escritor" << endl;
        return 1;
    }

    // Crear hilos
    vector<thread> lectores;
    vector<thread> escritores;

    for (int i = 0; i < NUM_LECTORES; ++i) {
        lectores.emplace_back(Lector, i + 1);
    }

    for (int i = 0; i < NUM_ESCRITORES; ++i) {
        escritores.emplace_back(Escritor, i + 1);
    }

    // Esperar a que terminen
    for (auto& t : lectores) {
        t.join();
    }
    for (auto& t : escritores) {
        t.join();
    }

    // Liberar recursos
    sem_destroy(&sem_lector);
    sem_destroy(&sem_escritor);

    return 0;
}