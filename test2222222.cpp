#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <random>
#include <unistd.h>
#include <atomic>

using namespace std;

// Constantes de configuración
#define MAX_LECTURAS_CONSECUTIVAS 5
#define TIEMPO_ESCRITURA 2
#define TIEMPO_LECTURA_MAX 3
#define NUM_LECTORES 5
#define NUM_ESCRITORES 2

// Variables compartidas
atomic<bool> terminar_programa{false};
int lecturas_consecutivas = 0;
mutex mutex_control;
mutex cout_mutex;
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

// Función protegida para imprimir
void print_safe(const string& message) {
    lock_guard<mutex> lock(cout_mutex);
    cout << message << endl;
}

void Lector(int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribucion_tiempo(1, TIEMPO_LECTURA_MAX);

    while (!terminar_programa) {
        print_safe("[Lector " + to_string(id) + "] intentando acceder.");

        unique_lock<mutex> lock(mutex_control);
        if(!escritor_esperando && lectores_en_sala < MAX_LECTURAS_CONSECUTIVAS && 
           lecturas_consecutivas < MAX_LECTURAS_CONSECUTIVAS && !terminar_programa) {
            
            lectores_en_sala++;
            lecturas_consecutivas++;
            print_safe("[Lector " + to_string(id) + "] leyendo (lectura " + 
                      to_string(lecturas_consecutivas) + "/" + 
                      to_string(MAX_LECTURAS_CONSECUTIVAS) + ")");
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
            if (terminar_programa) break;
            print_safe("[Lector " + to_string(id) + "] esperando (escritor: " + 
                      to_string(escritor_esperando) + ", lectores: " + 
                      to_string(lectores_en_sala) + ", lecturas: " + 
                      to_string(lecturas_consecutivas) + ").");
            lock.unlock();
            
            // Espera con timeout para evitar bloqueos indefinidos
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1; // Timeout de 1 segundo
            while (sem_timedwait(&sem_lector, &ts) == -1 && errno == EINTR && !terminar_programa) {
                if (errno == ETIMEDOUT) break;
                continue;
            }
            if (terminar_programa) break;
        }
    }
    print_safe("[Lector " + to_string(id) + "] finalizando.");
}

void Escritor(int id) {
    while (!terminar_programa) {
        print_safe("[Escritor " + to_string(id) + "] intentando acceder.");

        unique_lock<mutex> lock(mutex_control);
        escritor_esperando = true;
        print_safe("[Escritor " + to_string(id) + "] esperando turno tras " + 
                  to_string(lecturas_consecutivas) + " lecturas.");
        lock.unlock();

        // Espera con timeout
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1; // Timeout de 1 segundo
        while (sem_timedwait(&sem_escritor, &ts) == -1 && errno == EINTR && !terminar_programa) {
            if (errno == ETIMEDOUT) break;
            continue;
        }
        if (terminar_programa) break;

        lock.lock();
        escritor_esperando = false;
        print_safe("[Escritor " + to_string(id) + "] escribiendo...");
        lock.unlock();

        // Simulación del tiempo de escritura
        this_thread::sleep_for(chrono::seconds(TIEMPO_ESCRITURA));

        lock.lock();
        print_safe("[Escritor " + to_string(id) + "] terminó de escribir.");
        lecturas_consecutivas = 0;
        
        // Notificar a los lectores
        for (int i = 0; i < MAX_LECTURAS_CONSECUTIVAS; ++i) {
            sem_post(&sem_lector);
        }
        lock.unlock();
        
        break; // Este escritor completa su trabajo
    }
    print_safe("[Escritor " + to_string(id) + "] finalizando.");
}

int main() {
    // Inicializar semáforos
    if(sem_init(&sem_lector, 0, MAX_LECTURAS_CONSECUTIVAS) == -1) {
        cerr << "Error al inicializar sem_lector" << endl;
        return 1;
    }
    if(sem_init(&sem_escritor, 0, 0) == -1) {
        cerr << "Error al inicializar sem_escritor" << endl;
        return 1;
    }

    // Crear hilos
    vector<thread> lectores;
    for (int i = 0; i < NUM_LECTORES; ++i) {
        lectores.emplace_back(Lector, i + 1);
    }

    vector<thread> escritores;
    for (int i = 0; i < NUM_ESCRITORES; ++i) {
        escritores.emplace_back(Escritor, i + 1);
    }

    // Esperar un tiempo razonable para la ejecución
    this_thread::sleep_for(chrono::seconds(10));

    // Señalizar terminación
    terminar_programa = true;

    // Liberar todos los semáforos para despertar hilos bloqueados
    for (int i = 0; i < MAX_LECTURAS_CONSECUTIVAS; ++i) {
        sem_post(&sem_lector);
        sem_post(&sem_escritor);
    }

    // Esperar a que terminen los hilos
    for (auto& t : lectores) {
        if (t.joinable()) t.join();
    }
    for (auto& t : escritores) {
        if (t.joinable()) t.join();
    }

    // Liberar recursos
    sem_destroy(&sem_lector);
    sem_destroy(&sem_escritor);

    print_safe("Programa terminado correctamente");
    return 0;
}