#include "stdlib.h"
#include "stdio.h"
#include <time.h>

#define MAX (128*128)
#define REP 100
#define B  (32*1024)
#define NSPEEDS 9


typedef struct
{
  double speeds[NSPEEDS];
} t_speed;


int main() { 
    int i,j,r;
    char array[MAX];
    t_speed cells[MAX];





    clock_t t0 = clock();

    for (i = 0; i < MAX; i++) {  // warmup to make things equal if array happens to fit in your L3
       cells[i].speeds[0] = 0;
    }

    clock_t t1 = clock();

    // Tiled loop
    for (i = 0; i < MAX; i += B) {
        for (r = 0; r < REP; r++) {
             for (j = i; j < (i + B); j+=64) {
                 cells[j].speeds[0] = r;
             }
        }
    }
    clock_t t2 = clock();

    // un-tiled loop
    for (r = 0; r < REP; r++) {
        for (i = 0; i < MAX; i+=64) {
             cells[i].speeds[0] = r;
        }
    }

    clock_t t3 = clock();

    for (i = 0; i < MAX; i += B) {
        for (r = 0; r < REP; r++) {
             for (j = i; j < (i + B); j+=64) {
                 cells[j].speeds[0] = r;
                 if (r == REP - 2)                         // SW prefetching
                     __builtin_prefetch(&cells[j+B].speeds[0], 0);    
             }
        }
    }

    clock_t t4 = clock();


    printf ("Nothing: %f sec\n", (double)(t1 - t0) / CLOCKS_PER_SEC);

    printf ("Tiled: %f sec\n", (double)(t2 - t1) / CLOCKS_PER_SEC);
    printf ("Untiled: %f sec\n", (double)(t3 - t2) / CLOCKS_PER_SEC);
    printf ("Tiled and Optomised: %f sec\n", (double)(t4 - t3) / CLOCKS_PER_SEC);

    printf ("array[0] = %d\n", array[0]);    // to prevent optimizing out all the writes
}
