#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "sorting.h"


/*
   Merge two sorted chunks of array T!
   The two chunks are of size size
   First chunck starts at T[0], second chunck starts at T[size]
*/
void merge(uint64_t *T, const uint64_t size)
{
    uint64_t *buff = (uint64_t *) malloc(2 * size * sizeof(uint64_t));

    uint64_t i = 0;
    uint64_t j = size;
    uint64_t k = 0;

    while (i < size && j < 2 * size) {
        if (T[i] < T[j])
            buff[k] = T[i++];
        else
            buff[k] = T[j++];
        k++;
    }

    for (; i < size; i++, k++)
        buff[k] = T[i];
    for (; j < 2 * size; j++, k++)
        buff[k] = T[j];
    
    memcpy(T, buff, 2 * size * sizeof(uint64_t));

    free(buff);
    return;
}

void merge2(uint64_t *T, uint64_t *buff, const uint64_t size)
{
    memcpy(buff, T, 2 * size * sizeof(uint64_t));

    uint64_t i = 0;
    uint64_t j = size;
    uint64_t k = 0;

    while (i < size && j < 2 * size) {
        if (buff[i] < buff[j])
            T[k] = buff[i++];
        else
            T[k] = buff[j++];
        k++;
    }

    for (; i < size; i++, k++)
        T[k] = buff[i];
    for (; j < 2 * size; j++, k++)
        T[k] = buff[j];

    return;
}




/* 
   merge sort -- sequential, parallel -- 
*/

void sequential_merge_sort (uint64_t *T, const uint64_t size)
{
    /* DONE: sequential implementation of merge sort */ 
    if (size < 2)
	return;
    sequential_merge_sort(T, size / 2);
    sequential_merge_sort(T + size / 2, size / 2);
    merge(T, size / 2);
}

void parallel_merge_sort_rec(uint64_t *T, uint64_t *buff, const uint64_t size)
{
    /* DONE: parallel implementation of merge sort */
    if (size < 2)
        return;

    const uint64_t mid = size / 2;

    // The best value so far, after multiple tests
    #define TS 1024

    #pragma omp task firstprivate(T) if (size >= TS)
    parallel_merge_sort_rec(T, buff, mid);
    #pragma omp task firstprivate(T) if (size >= TS)
    parallel_merge_sort_rec(T + mid, buff + mid, mid);
    
    #pragma omp taskwait
    merge2(T, buff, mid);
}

void parallel_merge_sort(uint64_t *T, const uint64_t size)
{
    uint64_t *buff = (uint64_t *) malloc(size * sizeof(uint64_t));
    
    #pragma omp parallel
    #pragma omp single
    parallel_merge_sort_rec(T, buff, size);
    
    free(buff);
}


int main (int argc, char **argv)
{
    // Init cpu_stats to measure CPU cycles and elapsed time
    struct cpu_stats *stats = cpu_stats_init();

    unsigned int exp ;

    /* the program takes one parameter N which is the size of the array to
       be sorted. The array will have size 2^N */
    if (argc != 2)
    {
        fprintf (stderr, "Usage: merge.run N \n") ;
        exit (-1) ;
    }

    uint64_t N = 1 << (atoi(argv[1])) ;
    /* the array to be sorted */
    uint64_t *X = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

    printf("--> Sorting an array of size %lu\n",N);
#ifdef RINIT
    printf("--> The array is initialized randomly\n");
#elif defined(SINIT)
    printf("--> The array is initialized already sorted\n");
#endif
    

    for (exp = 0 ; exp < NB_EXPERIMENTS; exp++){
#ifdef RINIT
        init_array_random (X, N);
#elif defined(SINIT)
	init_array_sorted(X, N);
#else
        init_array_sequence (X, N);
#endif
        
        cpu_stats_begin(stats);      
        
        sequential_merge_sort (X, N) ;

        experiments[exp] = cpu_stats_end(stats);
        
        /* verifying that X is properly sorted */
#ifdef RINIT
        if (! is_sorted (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the sequential sorting of the array failed\n") ;
            exit (-1) ;
	}
#else
        if (! is_sorted_sequence (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the sequential sorting of the array failed\n") ;
            exit (-1) ;
	}
#endif
    }

  println_cpu_stats_report("mergesort serial", average_report(experiments, NB_EXPERIMENTS));

  
    for (exp = 0 ; exp < NB_EXPERIMENTS; exp++)
    {
#ifdef RINIT
        init_array_random (X, N);
#elif defined(SINIT)
	init_array_sorted(X, N);
#else
        init_array_sequence (X, N);
#endif

        cpu_stats_begin(stats);

        parallel_merge_sort (X, N) ;

        experiments[exp] = cpu_stats_end(stats);      

        /* verifying that X is properly sorted */
#ifdef RINIT
        if (! is_sorted (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the parallel sorting of the array failed\n") ;
            exit (-1) ;
	}
#else
        if (! is_sorted_sequence (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the parallel sorting of the array failed\n") ;
            exit (-1) ;
	}
#endif
                
        
    }
    
    println_cpu_stats_report("mergesort parallel", average_report(experiments, NB_EXPERIMENTS));
  
    /* print_array (X, N) ; */

    /* before terminating, we run one extra test of the algorithm */
    uint64_t *Y = (uint64_t *) malloc (N * sizeof(uint64_t)) ;
    uint64_t *Z = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

#ifdef RINIT
    init_array_random (Y, N);
#elif defined(SINIT)
    init_array_sorted(X, N);
#else
    init_array_sequence (Y, N);
#endif

    memcpy(Z, Y, N * sizeof(uint64_t));

    sequential_merge_sort (Y, N) ;
    parallel_merge_sort (Z, N);

    if (! are_vector_equals (Y, Z, N)) {
        fprintf(stderr, "ERROR: sorting with the sequential and the parallel algorithm does not give the same result\n") ;
        exit (-1) ;
    }


    free(X);
    free(Y);
    free(Z);
}
