#include <iostream>
#include <pthread.h>
#include <cstdlib>

using namespace std;

long nthreads = 4; // numero de hebras
long partes; // numero de partes del intervalo
double ancho; // ancho de cada parte
double *sumas; // sumas parciales de cada hebra

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

   // almacenar la suma parcial en el vector
   sumas[nhebra] = sumap;

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

   sumas = new double[nthreads];

   // redondeo del numero de puntos hacia arriba
   partes = ((partes+nthreads-1)/nthreads)*nthreads;
   // ancho de las partes
   ancho = (intervalo[1]-intervalo[0])/partes;

   // lanzar las hebras
   pthread_t hebras[nthreads];
   for (long i = 0; i < nthreads; i++) {
      pthread_create(hebras+i,NULL,suma_parcial,(void*)i);
   }

   // juntar todas las sumas
   double suma = 0;
   for (long i = 0; i < nthreads; i++) {
      pthread_join(hebras[i],NULL);
      suma += sumas[i];
   }

   // imprimir resultado
   cout.precision(20);
   cout << "La suma es " << suma << endl;
}
