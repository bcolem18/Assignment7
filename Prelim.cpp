#include <mpi.h> 
#include<stdio.h> 
#include <iostream>
#include<unistd.h>


int main(int argc, char **argv)
{
    int rank; 
    
    char Name[100]; 
   
   
    
    
    
    MPI_Init(&argc, &argv); 
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
     gethostname(Name, sizeof Name);
    
    std::cout<<rank<<std::endl; 
    
    printf("My hostname: %s\n", Name);
    
    MPI_Finalize(); 
    
    return 0; 
    
    
    
    
    
    
    
    
    
    
}