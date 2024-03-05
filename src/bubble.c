#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "sorting.h"

/* 
   bubble sort -- sequential, parallel -- 
*/

void sequential_bubble_sort (uint64_t *T, const uint64_t size)
{
    /* TODO: sequential implementation of bubble sort */
    
    return ;
}

void parallel_bubble_sort (uint64_t *T, const uint64_t size)
{
    /* TODO: parallel implementation of bubble sort */
    uint32_t max_thread_num = omp_get_max_threads();
    uint32_t chunk_num = max_thread_num > size ? size : max_thread_num;
    uint32_t chunk_size = size / chunk_num;

    while(!is_sorted(T, size)) {

        // Execute chunked bubble sort
        #pragma omp parallel for
        for(int c = 0; c < chunk_num; c++) {
            // Sequential bubble sort execution on chunk index c
            // Chunk starts at T + c * chunk_size
            // Every chunk but the last one have a size of chunk_size
            // The last chunk has a size of size % chunk_num
            sequential_bubble_sort(T+(c*chunk_size), (c==chunk_size-1 ? size % chunk_num : chunk_size));
        }

        // Swaps borders
        #pragma omp parallel for
        uint64_t tmp;
        for(int c = 0; c < chunk_num-1; c++) {
            if(T[(c+1) * chunk_size -1] > T[(c+1) * chunk_size]) {
                tmp = T[(c+1) * chunk_size -1];
                T[(c+1) * chunk_size -1] = T[(c+1) * chunk_size];
                T[(c+1) * chunk_size] = tmp;
            }
        }
    }
    return;
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
        fprintf (stderr, "Usage: bubble.run N \n") ;
        exit (-1) ;
    }

    uint64_t N = 1 << (atoi(argv[1])) ;
    /* the array to be sorted */
    uint64_t *X = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

    printf("--> Sorting an array of size %lu\n",N);
#ifdef RINIT
    printf("--> The array is initialized randomly\n");
#endif
    

    for (exp = 0 ; exp < NB_EXPERIMENTS; exp++){
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif
        
    cpu_stats_begin(stats);
        
        sequential_bubble_sort (X, N) ;

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

    println_cpu_stats_report("bubble serial", average_report(experiments, NB_EXPERIMENTS));
  
    for (exp = 0 ; exp < NB_EXPERIMENTS; exp++)
    {
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif
        
        cpu_stats_begin(stats);

        parallel_bubble_sort (X, N) ;

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
    
    println_cpu_stats_report("bubble parallel", average_report(experiments, NB_EXPERIMENTS));
    
    /* print_array (X, N) ; */

    /* before terminating, we run one extra test of the algorithm */
    uint64_t *Y = (uint64_t *) malloc (N * sizeof(uint64_t)) ;
    uint64_t *Z = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

#ifdef RINIT
    init_array_random (Y, N);
#else
    init_array_sequence (Y, N);
#endif

    memcpy(Z, Y, N * sizeof(uint64_t));

    sequential_bubble_sort (Y, N) ;
    parallel_bubble_sort (Z, N) ;

    if (! are_vector_equals (Y, Z, N)) {
        fprintf(stderr, "ERROR: sorting with the sequential and the parallel algorithm does not give the same result\n") ;
        exit (-1) ;
    }


    free(X);
    free(Y);
    free(Z);
    
}
