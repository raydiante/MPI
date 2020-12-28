/*
Rayssa Rosa - 2020
Disciplina de Computação de Alto Desempenho

Descrição:
Crie um programa para executar a soma entre vetores seguindo os seguintes passos:
• O processo de rank igual 0 aloca dinamicamente dois vetores do tipo double com 1200
elementos.
• O processo de rank igual 0 envia partes iguais destes vetores para os outros processos. Considere que o nu ́mero de processos  ́e divis ́ıvel por 1200. Por exemplo, np = 4 ser ̃ao enviados 300 elementos, de cada vetor, para os processos de rank 1,2 e 3. O processo 0 ir ́a processar tamb ́em.
• Os processos somam e enviam o resultado para o processo 0.
• O processo de rank igual a 0 imprime o conteu ́do do resultado.
*/
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//mpicc subsequente.c
//mpirun -np 4 a.out
//mpirun --oversubscribe -np 3 a.out 

int main( int argc, char** argv){	


    MPI_Init(&argc, &argv); 

    int rank,numero_processos,count=0;
    MPI_Request request;

    MPI_Comm_size (MPI_COMM_WORLD,&numero_processos);

    if(rank == 0){
        double *v1,*v2,*v1_send,*v2_send, saida[1200]; 
        int elementos=1200/numero_processos;
        double double_sum[elementos]; 

        //aloca vetores
        v1 = malloc (1200 * sizeof (double));
        v2 = malloc (1200 * sizeof (double));

        //inicializa vetores
        for (int i = 0; i < 1200; ++i) {
            v1[i]=1;
            v2[i]=1;
        }
        //manda os elementos para os processos
        for(int i = 0; i < numero_processos; ++i) {
            v1_send = malloc (elementos * sizeof (double));
            v2_send = malloc (elementos * sizeof (double));

            for(int j = 0; j < elementos; ++j) {
                v1_send[j]=v1[(i*elementos)+j];
                v2_send[j]=v2[(i*elementos)+j];
            }
            if(i!=0){
                MPI_Isend( v1_send,elementos,MPI_DOUBLE,i,0,MPI_COMM_WORLD,&request );
                MPI_Isend( v2_send,elementos,MPI_DOUBLE,i,1,MPI_COMM_WORLD,&request );
            }else{        
                

                for(int k = 0; k < elementos; ++k) {
                    double_sum[k]=v1_send[k]+v2_send[k];
                }

            }
        }
        //coloca dados no vetor final
        for(int i = 0; i < elementos; ++i) {
            saida[i]=double_sum[i];
        }
        for (int i = 1; i < numero_processos; ++i) {
            MPI_Irecv(&double_sum,elementos,MPI_DOUBLE,i,2,MPI_COMM_WORLD,&request);
            for(int j = 0; j < elementos; ++j) {
                saida[(i*elementos)+j]=double_sum[j];
            }
        }
        // Printa Soma dos vetores
        for (int i = 1; i < 100; ++i) {
            printf("%f\n",saida[i]);
        }

    }else{
        int elementos;
        double *v1_send,*v2_send,double_sum[elementos]; 

        elementos=1200/numero_processos;

        //recebe vetores
        MPI_Irecv(&v1_send,elementos,MPI_DOUBLE,0,0,MPI_COMM_WORLD,&request );
        MPI_Irecv(&v2_send,elementos,MPI_DOUBLE,0,1,MPI_COMM_WORLD,&request );

        MPI_Wait( &request, NULL );

        //soma os vetores
        for(int i = 0; i < elementos; ++i) {
            double_sum[i]=v1_send[i]+v2_send[i];
        }

        //envia soma para 
        MPI_Isend(&double_sum,elementos,MPI_DOUBLE,0,2,MPI_COMM_WORLD,&request);

    }

    
    MPI_Finalize();
}