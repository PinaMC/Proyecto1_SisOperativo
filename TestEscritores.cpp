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
#define TIEMPO_LECTURA_MAX 5
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
    sem_post(&sem_escritor);
    lock.unlock();
}

int main() {
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

    vector<thread> lectores;
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
    vector<thread> escritores;
    int count = NUM_ESCRITORES;
    while(count > 0){
        escritores.emplace_back(Escritor, count);
        count --;
    }
    



    // Liberar todos los semáforos para despertar hilos bloqueados
    for (int i = 0; i < MAX_LECTURAS_CONSECUTIVAS; ++i) {
        sem_post(&sem_lector);
        sem_post(&sem_escritor);
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