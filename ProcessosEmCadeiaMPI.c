/*
Rayssa Rosa - 2020
Disciplina de Computação de Alto Desempenho

Descrição:
Crie um programa em MPI onde cada processo envia o seu pr ́oprio rank para o processo subsequente. Utilize send e recv n ̃ao bloqueante. Tamb ́em utilize a fun ̧c ̃ao MPI_Wait para aguardar o recebimento apo ́s todos os envios.
*/
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

//mpicc subsequente.c
//mpirun -np 4 a.out
//mpirun --oversubscribe -np 3 a.out 


int main( int argc, char** argv){	

    MPI_Init(&argc, &argv); 

    int rank,numero_processos,recebido,origem,destino;
    MPI_Request request_recv, request_send,request;

    MPI_Comm_size (MPI_COMM_WORLD,&numero_processos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);


    if (rank == numero_processos-1){
        destino= 0;
    }else{
        destino= rank+1;
    }

    if (rank == 0){
        origem= numero_processos-1;
    }else{
        origem= rank-1;
    }


    MPI_Isend( &rank,1,MPI_INT,destino,0,MPI_COMM_WORLD,&request );
        
    MPI_Irecv(&recebido,1,MPI_INT,origem,0,MPI_COMM_WORLD,&request );

    MPI_Wait( &request, NULL );


    printf("Rank = %d  - Origem = %d -  Destino = %d - Mensagem = %d\n",rank,origem,destino,recebido );


    MPI_Finalize();
}