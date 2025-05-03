#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define LECTURAS_CONSECUTIVAS 1     // Lectores que han leído consecutivamente
#define MAX_LECTURAS_CONSECUTIVAS 5 // Maximo de Lectores que han leído consecutivamente
#define ESCRITORES_ESPERANDO 1   // Maximum number of readings
#define MAX_ESPERA 5      // tiempo maximo de espera en lectores cuando se llegue al max
#define MAX_ESPERAESCRITOR = 10 //tiempo maximo de espera de los escritores

using namespace std;
int main() {
    
    printf("Hello world!\n");
    
    return 0;
}