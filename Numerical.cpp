#include <mpi.h> 
#include<stdio.h> 
#include <iostream>
#include <math.h>





double func_1(double x, int intensity)
{
	
	for(int i=0; i<intensity; i++)
	{
		x=x*x; 
	}
	return 1;
}





int main(int argc, char **argv)
{
   // if(argc==6)
  // {
    
    int rank; //processor number
    int Procs; //number of processors used in total
     
    double a, b; //points
    a= 1; 
    b=5; 
    int intensity=1; 
    int N=100; 
    double t1, t2; 
    
    t1 = MPI_Wtime(); 
    MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get process rank
     MPI_Comm_size(MPI_COMM_WORLD, &Procs); //find out how many processes are being used
     
     double sum=0; 
    double mySum=0;
    
     int begin = rank * N/Procs; 
     int end = (rank +1) * N/Procs; 
     
     for(int i=begin; i<end;++i)
     {
         const double h= (b-a)/N;
         mySum+= func_1(a + (i+.5) *h ,intensity)  * h;
     }
     
     
     if(rank !=0)
     {
         MPI_Send(&mySum,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD); //Sends to rank 0
     }
     else
     {
         for(int i=1; i<Procs;++i)
         {
             MPI_Recv(&mySum, 1, MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE); //recieve from other processes
             sum +=mySum;  
             
         }
         
         std::cout<<sum<<std::endl; 
         
     }
    
    MPI_Finalize();
    t2 = MPI_Wtime(); 
    printf( "Elapsed time is %f\n", t2 - t1 ); 
 
    return 0; 
    
  //  }
    
    
    
}