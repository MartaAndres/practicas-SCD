#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

using namespace std;

const long nthreads = 3; // numero de fumadores
sem_t s_fumador[nthreads]; // semaforo para cada fumador
sem_t s_estanquero; // semaforo para el estanquero
long ingrediente; // el ingrediente que proporciona el estanquero
sem_t mutex; // semaforo de exclusion mutua

// funcion para fumar un tiempo aleatorio
void fumar() {
   const unsigned miliseg = 100U + (rand() % 1900U) ;
   usleep( 1000U*miliseg );
}

// hebras de los fumadores
void* fumador(void* indice) {
   // el ingrediente que le falta al fumador
   long falta = (long) indice;

   while(true) {
      sem_wait(&mutex);
      cout << "Fumador " << falta << ": esperando ingrediente\n";
      sem_post(&mutex);
      sem_wait(s_fumador+falta);
      sem_post(&s_estanquero);
      sem_wait(&mutex);
      cout << "Fumador " << falta << ": fumando\n";
      sem_post(&mutex);
      fumar();
   }

   return NULL;
}

// hebra del estanquero
void * estanquero(void *) {
   while (true) {
      sem_wait(&s_estanquero);
      ingrediente = rand() % nthreads;
      sem_wait(&mutex);
      cout << "Estanquero: producido el ingrediente " << ingrediente << endl;
      sem_post(&mutex);
      sem_post(s_fumador+ingrediente);
   }

   return NULL;
}

int main() {
   srand( time(NULL) );

   // inicilizar semaforos
   sem_init(&mutex, 0, 1);
   sem_init(&s_estanquero, 0, 1);
   for (long i = 0; i < nthreads; i++) {
      sem_init(s_fumador+i, 0, 0);
   }


   // iniciar estanquero
   pthread_t h_estanquero;
   pthread_create(&h_estanquero, NULL, estanquero, NULL);

   // iniciar fumadores
   pthread_t h_fumadores[nthreads];
   for (long i = 0; i < nthreads; i++) {
      pthread_create(h_fumadores+i, NULL, fumador, (void*) i);
   }

   // fin del main
   pthread_exit(NULL);
}
