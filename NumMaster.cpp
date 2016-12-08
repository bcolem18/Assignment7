


/*
** This is an MPI program that computes
** a Monte Carlo approximation to an integral
** using multiple processors.
**
** This program evaluates the mean of a standard normal random variable.
** It uses the gsl library (www.gnu.org/software/gsl).
*/


#include <iostream>
#include <math.h>
#include "mpi.h"
#include <gsl/gsl_randist.h>


using namespace std;


const int TAG_DOPARTOFSUM=0, TAG_TERMINATE=1;
int numprocesses, procnum;


gsl_rng *the_generator;


const int partial_sum_size = 100000;



void MasterProcess()
{
  int i, num_complete;
  bool done;
  int flag, source;
  MPI_Status status;
  double tx, total=0.0;

  cout << "The master process is alive and sees " 
       << numprocesses-1 << " slaves." << endl << endl;

  // Now request part of the Monte Carlo sum from each slave
  for (i=1 ; i<numprocesses ; ++i)
    MPI_Send(NULL, 0, MPI_CHAR, i, TAG_DOPARTOFSUM, MPI_COMM_WORLD);

  // Get the results back from the slaves in no particular order
  num_complete = 0; 
  done = false;
  while (!done)
    {
      // see if any messages have been sent back
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      if (flag)
	{
	  source = status.MPI_SOURCE;
	  
	  MPI_Recv(&tx, 1, MPI_DOUBLE, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	  total += tx;

	  cout << "Received partial total " << tx << " from Process #" << source << endl;


	  ++num_complete;
	  if (num_complete == numprocesses-1)
	    done=true;
	}
    }


  // Report result
  cout << endl << "Total    = " << total << endl;
  cout <<         "Estimate = " << total/(partial_sum_size*(numprocesses-1)) << endl;


  // Finally, terminate the slaves before returning and terminating self.
  for (i=1 ; i<numprocesses ; ++i)
    MPI_Send(NULL, 0, MPI_CHAR, i, TAG_TERMINATE, MPI_COMM_WORLD);
}


double partial_sum()
{
  int i;
  double total=0;

  for (i=0 ; i<partial_sum_size ; ++i)   
    total += gsl_ran_gaussian(the_generator, 1.0);

  return total;
}


void SlaveProcess()
{
  bool done=false;
  int source=0;        // 0 means source is master process
  MPI_Status status;
  double tx;

  while (!done)
    {
      MPI_Recv(NULL, 0, MPI_CHAR, source, MPI_ANY_TAG,
	       MPI_COMM_WORLD, &status);

      switch (status.MPI_TAG)
	{
	case TAG_DOPARTOFSUM:
	  tx = partial_sum();
	  MPI_Send(&tx, 1, MPI_DOUBLE, 0, TAG_DOPARTOFSUM, MPI_COMM_WORLD);
	  break;

	case TAG_TERMINATE:    // tells slave to stop waiting for messages and terminate
	  done=true;
	  break;
	}
    }
}


void init_generator(long int sd)
{
  const gsl_rng_type *T;
  gsl_rng_env_setup();
  T = gsl_rng_default;  // get default type

  the_generator = gsl_rng_alloc(T);
  gsl_rng_set(the_generator, sd);
}



main(int nargs, char **argv)
{
  // MPI Initialization
  int result=MPI_Init(&nargs, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &numprocesses);         // get total number of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &procnum);              // get process number for this process

  init_generator(1000*procnum);      // a different seed for each process

  // Now decide what to do: process 0 is master, others are slaves.
  switch (procnum)
    {
    case 0:
      MasterProcess();
      break;
    default:
      SlaveProcess();
      break;
    }

  MPI_Finalize();
}
