/*
Rayssa Rosa
Disciplina de Computação de Alto Desempenho
*/
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//mpicc atividade6mpi.c
//mpirun --oversubscribe -np 4 a.out 


int main( int argc, char** argv){	
	int x[] = {1, 2, 3, 4, 5, 6, 7, 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};
	int y[] = {41, 42, 43, 44, 45, 46, 47, 48,49,50,51,52,53,54,55,56,57,58,59,50,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,70};
	int n=40;
	float var_y=0,var_x=0,sum_x=0,sum_y=0,p=0,b=0, numerador=0,denominador=0;
	float p_denominador_x=0,p_denominador_y=0,p_denominador=0;
	int i;


	int size,rank;

    MPI_Init(&argc, &argv); 

    MPI_Comm_size (MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int tamanho_vetor_por_processo=n/size;


	// y
	for( i=rank*tamanho_vetor_por_processo ; i<(rank*tamanho_vetor_por_processo) + tamanho_vetor_por_processo ; i++){
		sum_y+= y[i]/n;
	}

	// x
	for( i=rank*tamanho_vetor_por_processo ; i<(rank*tamanho_vetor_por_processo) + tamanho_vetor_por_processo ; i++){
		sum_x+= x[i]/n;
	}

    MPI_Reduce( &sum_x, &var_x, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD );
    MPI_Reduce( &sum_y, &var_y, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD );

	MPI_Bcast( &var_x, 1, MPI_FLOAT,0, MPI_COMM_WORLD );
	MPI_Bcast( &var_y, 1, MPI_FLOAT,0, MPI_COMM_WORLD );


	//p
	
	for( i=rank*tamanho_vetor_por_processo ; i<(rank*tamanho_vetor_por_processo) + tamanho_vetor_por_processo ; i++){
		numerador+= (x[i] - var_x)*(y[i] - var_y);
		p_denominador_x+= (x[i] - var_x)*(x[i] - var_x);
		p_denominador_y+= (y[i] - var_y)*(y[i] - var_y);
	}

	float sum_numerador, sum_p_denominador_x, sum_p_denominador_y;

    MPI_Reduce( &numerador, &sum_numerador, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD );
    MPI_Reduce( &p_denominador_x, &sum_p_denominador_x, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD );
    MPI_Reduce( &p_denominador_y, &sum_p_denominador_y, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD );

	
	if (rank == 0){
		//p
		p_denominador=sqrt(sum_p_denominador_x)*sqrt(sum_p_denominador_y);
		p=sum_numerador/p_denominador;

		//b
		b=sum_numerador/sum_p_denominador_x;

		float alfa = var_y - (b*var_x);

		printf("x=%f   y=%f   p=%f   b=%f   alfa=%f\n",var_x, var_y, p, b, 	alfa);

	}

    MPI_Finalize();

}