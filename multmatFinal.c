/*
Rayssa Rosa - 2020
Disciplina de Computação de Alto Desempenho
*/
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//mpicc multmat.c
//mpirun --oversubscribe -np 4 a.out 10000


int main( int argc, char** argv){   

    MPI_Init(&argc, &argv); 

    int  np;
    int rank;

    MPI_Comm_size (MPI_COMM_WORLD,&np);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int n=2*np;

    //declara matriz
    float ** A = (float**) malloc ( n * sizeof (float*));
    float ** B = (float**) malloc ( n * sizeof (float*));
    float ** C = (float**) malloc ( n * sizeof (float*));

    for(int i=0 ; i<n; i++){
        A[i] = (float*) malloc(n*sizeof (float));
        B[i] = (float*) malloc(n*sizeof (float));
        C[i] = (float*) malloc(n*sizeof (float));
    }

    //inicializa matriz
    for(int i=0 ; i<n; i++){
        for(int j=0 ; j<n; j++){
            A[i][j]=1.0;
            B[i][j]=1.0;
            C[i][j]=0.0;
        }
    }


    int nl=n/np; // numero de linhas por processo.  dimensao da matriz/processos. =. 2
    int init = rank*nl; 
    int final = init +nl;


    //cada rank calcula a multiplicaçao
    for(int i=init ; i<final; i++){
        for(int j=0 ; j<n; j++){
            for(int k=0 ; k<n; k++){
                C[i][j]+=A[i][k]*B[k][j];
            }
        }
    }





    //Separaçao dos grupos
    int *ranks = (int *) malloc (sizeof(int)*(np/2)); // serão dois grupos com np/2 cada

    for (int i = 0; i < np/2; i++){
        ranks[i]=i;
    }


    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD,&world_group);

    MPI_Group grupo2;
    MPI_Group grupo1;
    MPI_Group_incl(world_group,np/2,ranks,&grupo1); //cria grupo
    MPI_Group_excl(world_group,np/2,ranks,&grupo2); //cria grupo

    MPI_Comm comm1;
    MPI_Comm comm2;
    MPI_Comm_create_group(MPI_COMM_WORLD,grupo1,0,&comm1); // cria comunicador
    MPI_Comm_create_group(MPI_COMM_WORLD,grupo2,0,&comm2); // cria comunicador

    int local_np,rank_local;
    int local_nl;
    int local_init;
    int local_final;
    if ( comm1 != MPI_COMM_NULL){
        MPI_Comm_size (comm1,&local_np);
        MPI_Comm_rank (comm1,&rank_local);
        
        local_nl=(n/2)/local_np; //     numero de linhas por processo.  dimensao da matriz/processos. =. 2
        local_init = rank*local_nl; 
        local_final = local_init +local_nl;   

        if (rank_local != 0){
            for(int i=local_init ; i<local_final; i++){ // envia as linhas de cada rank para o rank 0
                MPI_Request request;
                MPI_Isend(C[i],n, MPI_FLOAT, 0, i,comm1, &request); 
                //printf("send  from=%d/%d  linha=%d\n",rank,rank_local,i);  
            }
        }else{
            for (int rank_recv = 1; rank_recv<local_np ; rank_recv++){ // recebe os dados de todos os ranks menos dele mesmo (0)
                for (int i = rank_recv*local_nl; i < (rank_recv * local_nl)+ local_nl; i++){ //recebe as linhas enviadas de cada rank
                    MPI_Request request;
                    MPI_Irecv(C[i],n, MPI_FLOAT, rank_recv, i,comm1, &request);
                    //printf("recive  from=%d  linha=%d\n",rank_recv,i);
                    MPI_Wait(&request,NULL);
                }
            }

        }
    }else if ( comm2 != MPI_COMM_NULL){
        MPI_Comm_size (comm2,&local_np);
        MPI_Comm_rank (comm2,&rank_local);
        
        local_nl=(n/2)/local_np; //     numero de linhas por processo.  dimensao da matriz/processos. =. 2
        local_init = rank*local_nl; 
        local_final = local_init +local_nl;   


        if (rank_local != 0){
            for(int i=local_init ; i<local_final; i++){ // envia as linhas de cada rank para o rank 0
                MPI_Request request;
                MPI_Isend(C[i],n, MPI_FLOAT, 0, i,comm2, &request); 
                //printf("**send  from=%d/%d  linha=%d\n",rank,rank_local,(n/2)+i);  
            }
        }else{
            for (int rank_recv = 1; rank_recv<local_np ; rank_recv++){ // recebe os dados de todos os ranks menos dele mesmo (0)
                for (int i = (n/2)+rank_recv*local_nl; i < (n/2)+(rank_recv * local_nl)+ local_nl; i++){ //recebe as linhas enviadas de cada rank
                    MPI_Request request;
                    MPI_Irecv(C[i],n, MPI_FLOAT, rank_recv, i,comm2, &request);
                    //printf("**recive  from=%d  linha=%d\n",rank_recv,(n/2)+i);
                    MPI_Wait(&request,NULL);
                }
            }
            for (int rank_recv = 0; rank_recv<local_np ; rank_recv++){ // recebe os dados de todos os ranks menos dele mesmo (0)
                for (int i = (n/2)+(rank_recv*local_nl); i < (n/2)+(rank_recv * local_nl)+ local_nl; i++){ //recebe as linhas enviadas de cada rank
                    MPI_Request request;
                    MPI_Isend(C[i],n, MPI_FLOAT, 0, i,MPI_COMM_WORLD, &request); 
                    MPI_Wait(&request,NULL);
                    //printf("---send  i=%d-%d\n",i,(n/2)+i); 

                }
            }
        }
    }

    if ( comm1 != MPI_COMM_NULL){

        MPI_Comm_size (comm1,&local_np);
        MPI_Comm_rank (comm1,&rank_local);
        
        local_nl=(n/2)/local_np; //     numero de linhas por processo.  dimensao da matriz/processos. =. 2
        local_init = rank_local*local_nl; 
        local_final = local_init +local_nl;   

        if (rank_local == 0){
            for (int rank_recv = 0; rank_recv<local_np ; rank_recv++){ // recebe os dados de todos os ranks menos dele mesmo (0)
                for (int i = (n/2)+(rank_recv*local_nl); i <(n/2)+  (rank_recv * local_nl)+ local_nl; i++){ //recebe as linhas enviadas de cada rank
                    MPI_Request request;
                    MPI_Irecv(C[i],n, MPI_FLOAT, np/2, i,MPI_COMM_WORLD, &request);
                    //printf("---recive  i=%d.   indice=%d\n",i,(n/2)+i); 
                    MPI_Wait(&request,NULL);

                }
            }
        }
    }

///*
    //printa matriz final
    if(rank == 0){
        printf("-------------------\n");
        for(int i=0 ; i<n ; i++){ 
            for(int j=0 ; j<n ; j++){
                printf(" %f ", C[i][j] );

            }
            printf("\n");
        }
    }
    //*/

    MPI_Finalize();

}