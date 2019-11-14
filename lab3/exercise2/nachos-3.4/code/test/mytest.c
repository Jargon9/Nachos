/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

#define N 2000	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
		int i;
		int result[N];
		result[0]=0;
		result[1]=1;
		for(i=2; i<N; i++){
			result[i] = result[i-1] +result[i-2];
		}
        Halt();	/* and then we're done -- should be 0! */
}
