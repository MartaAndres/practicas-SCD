#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <cstdlib>

using namespace std;

// numero de hebras
long nthreads = 4;
// numero de partes del intervalo
long partes;
// suma de todas las integrales
double suma;
// semaforo de exclusion mutua
sem_t mutex;

// la funcion a integrar
double f (double x) {
   return 4.0/(1+x*x);
}

// la funcion para las sumas parciales
void* suma_parcial(void* indice) {
   long nhebra = (long) indice;

   // comienzo y fin del bucle
   long ini = nhebra * partes/nthreads;
   long fin = ini + partes/nthreads;

   // sumar cada parte
   double sumap = 0;
   for (long i = ini; i < fin; i++) {
      sumap += f(i*1.0/partes);
   }

   sem_wait(&mutex);
   suma += sumap/partes;
   sem_post(&mutex);

   return NULL;
}

int main(int argc, char* argv[]) {
   // leer argumentos de entrada (numero de partes y hebras)
   switch (argc) {
      case 3:
         nthreads = atoi(argv[2]);
      case 2:
         partes = atol(argv[1]);
         break;
      default:
         partes = 1;
   }

   // redondeo del numero de puntos hacia arriba
   partes = ((partes+nthreads-1)/nthreads)*nthreads;

   // crear el semaforo de exclusion mutua
   sem_init(&mutex, 0, 1);

   // lanzar las hebras
   pthread_t hebras[nthreads];
   for (long i = 0; i < nthreads; i++) {
      pthread_create(hebras+i, NULL, suma_parcial, (void*) i);
   }

   // esperar a que finalizen todas las hebras
   for (long i = 0; i < nthreads; i++) {
      pthread_join(hebras[i], NULL);
   }

   sem_destroy(&mutex);

   // modificar la precision al imprimir el float
   cout.precision(20);
   // imprimir resultado
   cout << "La suma es " << suma << endl;
}
