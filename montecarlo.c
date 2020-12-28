/*
Rayssa Rosa - 2020
Disciplina de Computação de Alto Desempenho

Descrição:
Podemos estimar o valor deπatrav ́es do seguinte m ́etodo de Monte Carlo:1.  Gerar  aleatoriamente  pares(x,y)em  um  quadrado  de  lado2r.  Desta  forma  o  quadradopossui  ́areaAq= 4r22.  Contar o n ́umero de pontos que est ̃ao dentro do c ́ıculo de raior.  Este c ́ırculo encontra-secircunscrito no quadrado e possui  ́areaAc=πr2.3.  Temos a seguinte rela ̧c ̃aoP=AcAq=π4.4.  O  n ́umero  de  pontos  que  est ̃ao  dentro  do  c ́ırculo  divido  pelo  n ́umero  de  pontos  totais  ́e≈π4.Crie as seguintes fun ̧c ̃oes1.double randf();retorna um n ́umero double no intervalo(−1,1).2.int inside_circle(double x, double y);retorna  1 se  o  ponto  est ́a  no  c ́ırculo e  0caso contr ́ario.Fa ̧ca um programa em MPI que receba o valor deNe exiba uma aproxima ̧c ̃ao deπ.N

*/
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//mpicc montecarlo.c
//mpirun --oversubscribe -np 4 a.out 10000

double randf(void){ // retorna numero aleatorio entre -1 e 1
	double min=-1.0 , max=1.0;
	return min + (rand() / ( RAND_MAX / ( max - min) ) ) ; 

}

int inside_circle(double x, double y){

	double distancia=0;
	distancia=sqrt((x*x)+(y*y)); // distancia entre o centro do circulo e o ponto
	if (distancia > 1){ // compara a distancia com o raio
		return 0; // ponto esta fora do circulo
	}else{
		return 1; // ponto esta dentro do circulo
	}

}

int main( int argc, char** argv){	
	int N = atoi(argv[1]);
	int pontosNoCirculo=0;
    MPI_Init(&argc, &argv); 

    int rank,numero_processos,count=0,recv_data=0,send_data=0;
    double x,y,pi;
    MPI_Comm_size (MPI_COMM_WORLD,&numero_processos);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);


    for(int i=0 ; i<numero_processos ; i++){
    	if( rank == i){
			int tempo= time(0);	//semente do rand 
			srand(tempo);
    		for (int j = 0; j < N/numero_processos; ++j){
		    	x =randf(); 
		    	y =randf();
		    	send_data += inside_circle( x, y); //soma todos os pontos dentro do circulo de cada processo
    		}
    	}
    }


    MPI_Reduce( &send_data, &recv_data, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
    MPI_Barrier( MPI_COMM_WORLD );

    if( rank == 0 ) {
    	int somaPontos=recv_data;
        printf("result = %d\n", somaPontos );
        pi= ((float)somaPontos/(float)N)*4.0; 
        printf("pi = %lf\n", pi );

    }



    MPI_Finalize();
}