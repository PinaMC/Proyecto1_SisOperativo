
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
}
*/


#include<stdio.h>
#include <thread>
#include <mutex>
#include <unistd.h>
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


using namespace std;
#define LECTURAS_CONSECUTIVAS 5     // Lectores que han leído consecutivamente
#define MAX_LECTURAS_CONSECUTIVAS 5 // Maximo de Lectores que han leído consecutivamente
#define ESCRITORES_ESPERANDO 5   // Maximum number of readings
#define MAX_ESPERA 5      // Tiempo maximo de los lectores?
#define TIEMPO_ESCRITURA 10 //el tiempo sera de 10 segundos para el escritor
#define TIEMPO_LECTURA_MAX 5 // Tiempo maximo de lectura para los lectores
#define NUM_LECTORES 5 // Numero de lectores
#define NUM_ESCRITORES 2 // Numero de escritores

//const char* lector_sem = lector_sem;
//const char* escritor_sem = escritor_sem;
// Variables compartidas
int lecturas_consecutivas = 0;
mutex mutex_control;
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

void Lector(int id){
    //generacion de aleatorios ovo
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribucion_tiempo(1, TIEMPO_LECTURA_MAX);

    cout << "[Lector " << id << "] intentando acceder." << endl;
    //ahora vendra un while en dodnde estara el bucle de los lectores 0w0nt
    while(true){
        unique_lock<mutex> lock(mutex_control);
        if(!escritor_esperando &&  lectores_en_sala < MAX_LECTURAS_CONSECUTIVAS){
            //AQUI VA LA LOGICA OWO
            lectores_en_sala++;
            lecturas_consecutivas++;
            cout<< "[Lector " << id << "] leyendo (lectura " << lecturas_consecutivas << "/" << MAX_LECTURAS_CONSECUTIVAS << ")" <<endl;
            lock.unlock();
            this_thread::sleep_for(chrono::seconds(distribucion_tiempo(gen)));
            lock.lock();
            lectores_en_sala--;
            //en el if que viene aca es cuando los lectores en sala sean 0 deberia de tocarle a un escritor
            if(lectores_en_sala == 0){
                sem_post(&sem_escritor);
                lecturas_consecutivas = 0;
            }

        }
    }


}

//---------------------escritores--------------------------------------------
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
        for (int i = 0; i < MAX_LECTURAS_CONSECUTIVAS; ++i) {
            sem_post(&sem_lector); // Desbloquear hasta el número máximo de lectores
        }
    }
    lock.unlock();
}












int main() {
    std::srand(std::time(nullptr));

    if (sem_init(&sem_lector, 0, MAX_LECTURAS_CONSECUTIVAS) == -1) {
        std::cerr << "Error al inicializar el semáforo de lector." << std::endl;
        return 1;
    }
    if (sem_init(&sem_escritor, 0, 0) == -1) {
        std::cerr << "Error al inicializar el semáforo de escritor." << std::endl;
        return 1;
    }

    std::vector<std::thread> lectores_hilos(NUM_LECTORES);
    for (int i = 0; i < NUM_LECTORES; ++i) {
        lectores_hilos[i] = std::thread(Lector, i + 1);
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
            cout << "Lector " << i + 1 << " intentando leer." << endl;
            sem_wait(lector_sem);
            cout << "Lector " << i + 1 << " leyendo." << endl;
            sleep(rand() % 5 + 1); // Simular lectura
            cout << "Lector " << i + 1 << " terminó de leer." << endl;
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

            cout << "Escritor comenzando a escribir." << endl;
            sleep(10); // Simular escritura
            cout << "Escritor terminó de escribir." << endl;

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
mutex mutex_control;
sem_t sem_lector;
sem_t sem_escritor;
bool escritor_esperando = false;
int lectores_en_sala = 0;

// Función para simular la lectura
void lector(int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribucion_tiempo(1, TIEMPO_LECTURA_MAX);

    cout << "[Lector " << id << "] intentando acceder." << endl;

    while (true) {
        unique_lock<mutex> lock(mutex_control);
        if (!escritor_esperando && lectores_en_sala < MAX_LECTORES_SIMULTANEOS && lecturas_consecutivas < MAX_LECTURAS_CONSECUTIVAS) {
            lectores_en_sala++;
            lecturas_consecutivas++;
            cout << "[Lector " << id << "] leyendo (lectura " << lecturas_consecutivas << "/" << MAX_LECTURAS_CONSECUTIVAS << ")" << endl;
            lock.unlock();
            this_thread::sleep_for(chrono::seconds(distribucion_tiempo(gen)));
            lock.lock();
            lectores_en_sala--;
            if (lectores_en_sala == 0) {
                sem_post(&sem_escritor); // Si fui el último lector, permito a un escritor
                lecturas_consecutivas = 0;
            }
            break;
        } else {
            cout << "[Lector " << id << "] esperando para leer (escritor esperando: " << escritor_esperando << ", lectores en sala: " << lectores_en_sala << ", lecturas consecutivas: " << lecturas_consecutivas << ")." << endl;
            lock.unlock();
            sem_wait(&sem_lector); // Espera a que un escritor termine y dé paso o haya espacio
        }
    }
    cout << "[Lector " << id << "] terminó de leer." << endl;
}

// Función para simular la escritura
void escritor(int id) {
    cout << "[Escritor " << id << "] intentando acceder para escribir." << endl;

    unique_lock<mutex> lock(mutex_control);
    escritor_esperando = true;
    cout << "[Escritor " << id << "] esperando turno tras " << lecturas_consecutivas << " lecturas." << endl;
    lock.unlock();

    sem_wait(&sem_escritor); // Espera a que los lectores terminen su bloque

    lock.lock();
    escritor_esperando = false;
    cout << "[Escritor " << id << "] escribiendo..." << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::seconds(TIEMPO_ESCRITURA));

    lock.lock();
    cout << "[Escritor " << id << "] terminó de escribir." << endl;
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
    srand(time(nullptr));

    if (sem_init(&sem_lector, 0, 0) == -1) {
        cerr << "Error al inicializar el semáforo de lector." << endl;
        return 1;
    }
    if (sem_init(&sem_escritor, 0, 0) == -1) {
        cerr << "Error al inicializar el semáforo de escritor." << endl;
        return 1;
    }

    vector<thread> lectores_hilos(NUM_LECTORES);
    for (int i = 0; i < NUM_LECTORES; ++i) {
        lectores_hilos[i] = thread(lector, i + 1);
    }

    vector<thread> escritores_hilos(NUM_ESCRITORES);
    for (int i = 0; i < NUM_ESCRITORES; ++i) {
        escritores_hilos[i] = thread(escritor, i + 1);
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














//------Ejemplo de Piña----------------------
/*
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LECTURAS_CONSECUTIVAS 5
#define N_LECTORES 4
#define N_ESCRITORES 2

// Variables compartidas
int lectores_activos = 0;
int lectores_esperando = 0;
int escritores_activos = 0;
int escritores_esperando = 0;
int lecturas_consecutivas = 0;

// Sincronización
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_lectores = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_escritores = PTHREAD_COND_INITIALIZER;

// Funciones de lectura/escritura simuladas
void leer(int id) {
    printf("[Lector %d] leyendo (lectura %d/%d)\n", id, lecturas_consecutivas, MAX_LECTURAS_CONSECUTIVAS);
    sleep(1);
}

void escribir(int id) {
    printf("[Escritor %d] escribiendo...\n", id);
    sleep(2);
}

// Lector
void* lector(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

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



--------------------funcion principal------------------
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


*/