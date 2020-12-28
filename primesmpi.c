/*
Rayssa Rosa - 2020
Disciplina de Computação de Alto Desempenho

Descrição:
Crie um programa paralelo em MPI para contar o nu ́mero de primos menores ou iguais a n, onde n  ́e um inteiro dado. Utilize a fun ̧c ̃ao isprime que retorna 1 se o inteiro dado  ́e primo e 0, caso contr ́ario. Veja arquivo primes.c
*/
# include <math.h>
# include <mpi.h>
# include <stdio.h>
# include <stdlib.h>
# include <time.h>

//mpicc primesmpi.c
//mpirun --oversubscribe -np 4 a.out 7


int isprime(int n ) {
     int i;
     for (i = 2; i <= n / 2; ++i) {
          // condition for non-prime
          if (n % i == 0) {
               return 0;
          }
     }
     return 1;
}


int num_primo_rank(int numero, int rank, int size ){
     int total= 0;
     for (int i = 2 + rank; i <= numero; i = i + size ){
          total += isprime(i);
     }
     return total;
}

int main(int argc, char *argv[] ){
     int numero=atoi(argv[1]);
     int resultado, primos;

     MPI_Init(&argc, &argv);
     int rank,size;
     MPI_Comm_size(MPI_COMM_WORLD, &size);
     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

     MPI_Bcast(&numero, 1, MPI_INT, 0, MPI_COMM_WORLD );

     primos = num_primo_rank(numero, rank, size );

     MPI_Reduce(&primos, &resultado, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );

     if( rank == 0 ){
          printf("Primos <= %d  :  %d  \n", numero, resultado);
     }

     MPI_Finalize();

     return 0;
}
