#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <random>

#define LECTURAS_CONSECUTIVAS 1     // Lectores que han leído consecutivamente
#define MAX_LECTURAS_CONSECUTIVAS 5 // Maximo de Lectores que han leído consecutivamente
#define ESCRITORES_ESPERANDO 5   // Maximum number of readings
#define MAX_ESPERA 5      // Tiempo maximo de los lectores?
#define TIEMPO_ESCRITURA = 10 //el tiempo sera de 10 secondos para el escritor

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