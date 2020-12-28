/*
Rayssa Rosa - 2020
Disciplina de Computação de Alto Desempenho

Descrição:
Crie um programa em MPI para encontrar a soma do m ́aximo subvetor.  Sejavumvetor, esta soma  ́e dada pormax{j∑x=iv[x]}  ∀i≤jPor exemplo,v= [−2,1,−3,4,−1,2,1,−5,4]o subvetor cont ́ınuo com maior soma  ́e[4,−1,2,1]cujo a soma  ́e6.

*/
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//mpicc montecarlo.c
//mpirun --oversubscribe -np 4 a.out 10000


int main( int argc, char** argv){	

    MPI_Init(&argc, &argv); 

    int  size=9;
    int v[] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int rank,numero_processos,count=0,recv_data=0,send_data=0, max=-999999, subarea=0, tamanho_vetor_max, inicio_vetor_max;
    double x,y,pi;
    int dados_para_cada_processo;

    MPI_Comm_size (MPI_COMM_WORLD,&numero_processos);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    dados_para_cada_processo=size/numero_processos;


    count=0;
    for(int i=0 ; i<numero_processos ; i++){
        if( rank == i){
            for (int j = (rank*dados_para_cada_processo); j <(rank*dados_para_cada_processo)+dados_para_cada_processo; j++){
                count=0;
                for (int k = j; k <size; k++){
                    count+=v[k];
                    if (count >max){
                        max=count;
                        tamanho_vetor_max=(k-j)+1; 
                        inicio_vetor_max=j;
                    }
                }
            }
        }
    }
    printf("max = %d   tam= %d    inicio=%d \n", max,tamanho_vetor_max,inicio_vetor_max );


    MPI_Reduce( &max, &recv_data, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD );
    MPI_Barrier( MPI_COMM_WORLD );

    if( rank == 0 ) {
        printf("Soma do máximo subvetor= %d\n", recv_data );

    }



    MPI_Finalize();
}