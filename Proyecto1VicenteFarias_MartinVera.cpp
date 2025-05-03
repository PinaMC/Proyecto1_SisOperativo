#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h> // Use the C++ standard library semaphore header
#include <chrono>
#include <random>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>



#define LECTURAS_CONSECUTIVAS 1     // Lectores que han leído consecutivamente
#define MAX_LECTURAS_CONSECUTIVAS 5 // Maximo de Lectores que han leído consecutivamente
#define ESCRITORES_ESPERANDO 5   // Maximum number of readings
#define MAX_ESPERA 5      // Tiempo maximo de los lectores?
#define TIEMPO_ESCRITURA = 10 //el tiempo sera de 10 secondos para el escritor

const char* lector_sem = lector_sem;
const char* escritor_sem = escritor_sem;
// Variables compartidas
int lecturas_consecutivas = 0;
mutex mutex_control;
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

using namespace std;
int main() {
    //esta potente un codigo vacio os maldigo progra de disp moviles
    printf("Hello world!\n");
    
    return 0;
}
/*
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

// Definir nombres para los semáforos
const char* lector_sem_name = "/lector_sem";
const char* escritor_sem_name = "/escritor_sem";

int main() {
    sem_t *lector_sem, *escritor_sem;
    pid_t pid;
    int num_lectores = 5;

    // Crear y/o abrir los semáforos
    lector_sem = sem_open(lector_sem_name, O_CREAT | O_EXCL, 0666, num_lectores);
    if (lector_sem == SEM_FAILED) {
        perror("sem_open(lector_sem)");
        exit(EXIT_FAILURE);
    }

    escritor_sem = sem_open(escritor_sem_name, O_CREAT | O_EXCL, 0666, 0); // Inicialmente bloqueado
    if (escritor_sem == SEM_FAILED) {
        perror("sem_open(escritor_sem)");
        sem_unlink(lector_sem_name);
        exit(EXIT_FAILURE);
    }

    // Crear procesos lectores
    for (int i = 0; i < num_lectores; ++i) {
        pid = fork();
        if (pid == 0) { // Proceso hijo (lector)
            std::cout << "Lector " << i + 1 << " intentando leer." << std::endl;
            sem_wait(lector_sem);
            std::cout << "Lector " << i + 1 << " leyendo." << std::endl;
            sleep(rand() % 5 + 1); // Simular lectura
            std::cout << "Lector " << i + 1 << " terminó de leer." << std::endl;
            sem_post(lector_sem);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            perror("fork");
            // Manejar error
        }
    }

    // Crear proceso escritor
    pid = fork();
    if (pid == 0) { // Proceso hijo (escritor)
        while (true) {
            // Esperar a que no haya lectores (el semáforo de lectores llegue a 0)
            int lector_val;
            do {
                sem_getvalue(lector_sem, &lector_val);
                sleep(1); // Esperar un poco antes de verificar de nuevo
            } while (lector_val > 0);

            std::cout << "Escritor comenzando a escribir." << std::endl;
            sleep(10); // Simular escritura
            std::cout << "Escritor terminó de escribir." << std::endl;

            // Permitir que los lectores vuelvan a leer
            for (int i = 0; i < num_lectores; ++i) {
                sem_post(lector_sem);
            }
            sleep(5); // Esperar un poco antes de la siguiente ronda (opcional)
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("fork");
        // Manejar error
    }

    // Esperar a que terminen los procesos lectores
    for (int i = 0; i < num_lectores; ++i) {
        wait(NULL);
    }

    // No esperamos al escritor en este ejemplo para que siga escribiendo indefinidamente
    // En un escenario real, podrías necesitar una forma de terminar el escritor también.

    // Cerrar y eliminar los semáforos
    sem_close(lector_sem);
    sem_unlink(lector_sem_name);
    sem_close(escritor_sem);
    sem_unlink(escritor_sem_name);

    return 0;
}
*/
/*
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <random>

// Definiciones
const int MAX_LECTORES_SIMULTANEOS = 5;
const int MAX_LECTURAS_CONSECUTIVAS = 5;
const int NUM_LECTORES = 10;
const int NUM_ESCRITORES = 3;
const int TIEMPO_ESCRITURA = 2; // Tiempo de escritura reducido para pruebas
const int TIEMPO_LECTURA_MAX = 3;

// Variables compartidas
int lecturas_consecutivas = 0;
std::mutex mutex_control;
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

// Función para simular la lectura
void lector(int id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribucion_tiempo(1, TIEMPO_LECTURA_MAX);

    std::cout << "[Lector " << id << "] intentando acceder." << std::endl;

    while (true) {
        std::unique_lock<std::mutex> lock(mutex_control);
        if (!escritor_esperando && lectores_en_sala < MAX_LECTORES_SIMULTANEOS && lecturas_consecutivas < MAX_LECTURAS_CONSECUTIVAS) {
            lectores_en_sala++;
            lecturas_consecutivas++;
            std::cout << "[Lector " << id << "] leyendo (lectura " << lecturas_consecutivas << "/" << MAX_LECTURAS_CONSECUTIVAS << ")" << std::endl;
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(distribucion_tiempo(gen)));
            lock.lock();
            lectores_en_sala--;
            if (lectores_en_sala == 0) {
                sem_post(&sem_escritor); // Si fui el último lector, permito a un escritor
                lecturas_consecutivas = 0;
            }
            break;
        } else {
            std::cout << "[Lector " << id << "] esperando para leer (escritor esperando: " << escritor_esperando << ", lectores en sala: " << lectores_en_sala << ", lecturas consecutivas: " << lecturas_consecutivas << ")." << std::endl;
            lock.unlock();
            sem_wait(&sem_lector); // Espera a que un escritor termine y dé paso o haya espacio
        }
    }
    std::cout << "[Lector " << id << "] terminó de leer." << std::endl;
}

// Función para simular la escritura
void escritor(int id) {
    std::cout << "[Escritor " << id << "] intentando acceder para escribir." << std::endl;

    std::unique_lock<std::mutex> lock(mutex_control);
    escritor_esperando = true;
    std::cout << "[Escritor " << id << "] esperando turno tras " << lecturas_consecutivas << " lecturas." << std::endl;
    lock.unlock();

    sem_wait(&sem_escritor); // Espera a que los lectores terminen su bloque

    lock.lock();
    escritor_esperando = false;
    std::cout << "[Escritor " << id << "] escribiendo..." << std::endl;
    lock.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(TIEMPO_ESCRITURA));

    lock.lock();
    std::cout << "[Escritor " << id << "] terminó de escribir." << std::endl;
    lecturas_consecutivas = 0; // Reiniciar el contador de lecturas consecutivas
    // Dar preferencia a los lectores si no hay más escritores esperando inmediatamente
    if (!escritor_esperando) {
        for (int i = 0; i < MAX_LECTORES_SIMULTANEOS; ++i) {
            sem_post(&sem_lector); // Desbloquear hasta el número máximo de lectores
        }
    }
    lock.unlock();
}

int main() {
    std::srand(std::time(nullptr));

    if (sem_init(&sem_lector, 0, 0) == -1) {
        std::cerr << "Error al inicializar el semáforo de lector." << std::endl;
        return 1;
    }
    if (sem_init(&sem_escritor, 0, 0) == -1) {
        std::cerr << "Error al inicializar el semáforo de escritor." << std::endl;
        return 1;
    }

    std::vector<std::thread> lectores_hilos(NUM_LECTORES);
    for (int i = 0; i < NUM_LECTORES; ++i) {
        lectores_hilos[i] = std::thread(lector, i + 1);
    }

    std::vector<std::thread> escritores_hilos(NUM_ESCRITORES);
    for (int i = 0; i < NUM_ESCRITORES; ++i) {
        escritores_hilos[i] = std::thread(escritor, i + 1);
    }

    for (auto& hilo : lectores_hilos) {
        hilo.join();
    }

    for (auto& hilo : escritores_hilos) {
        hilo.join();
    }

    sem_destroy(&sem_lector);
    sem_destroy(&sem_escritor);

    return 0;
}
*/