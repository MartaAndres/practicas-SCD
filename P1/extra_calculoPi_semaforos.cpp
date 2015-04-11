#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <semaphore.h>

using namespace std;

long nthreads = 4; // numero de hebras
long partes; // numero de partes del intervalo
double ancho; // ancho de cada parte
double suma; // suma global
sem_t mutex; // semaforo de exclusion mutua

// la funcion a integrar
double f (double x) {
   return 4.0/(1+x*x);
}

// intervalo a integrar
double intervalo[2] = {0,1};

// sumas parcial de cada hebra
void* suma_parcial(void* indice) {
   long nhebra = (long) indice;

   // asignacion contigua de las partes
   long ini = nhebra * partes/nthreads;
   long fin = ini + partes/nthreads;

   // sumar cada parte
   double sumap = 0;
   for (long i = ini; i < fin; i++) {
      sumap += f(intervalo[0] + i*ancho) * ancho;
   }

   // acumular las sumas parciales en suma
   sem_wait(&mutex);
   suma += sumap/partes;
   sem_post(&mutex);

   return NULL;
}

int main(int argc, char* argv[]) {
   // leer argumentos de entrada (numero de partes y hebras)
   switch (argc) {
      case 3:
         nthreads = atol(argv[2]);
      case 2:
         partes = atol(argv[1]);
         break;
      default:
         partes = 1;
   }

   // redondeo del numero de puntos hacia arriba
   partes = ((partes+nthreads-1)/nthreads)*nthreads;
   // ancho de las partes
   ancho = (intervalo[1]-intervalo[0])/partes;

   // crear el semaforo de exclusion mutua
   sem_init(&mutex, 0, 1);

   // lanzar las hebras
   pthread_t hebras[nthreads];
   for (long i = 0; i < nthreads; i++) {
      pthread_create(hebras+i,NULL,suma_parcial,(void*)i);
   }

   // esperar a que acaben las hebras
   for (long i = 0; i < nthreads; i++) {
      pthread_join(hebras[i],NULL);
   }

   // imprimir resultado
   cout.precision(20);
   cout << "La suma es " << suma << endl;
}
