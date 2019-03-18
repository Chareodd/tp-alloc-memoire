#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NB_TESTS 1

int main(int argc, char *argv[]) {
	fprintf(stderr, "Test réalisant de multiples fois une initialisation suivie d'une alloc max.\n"
			"Définir DEBUG à la compilation pour avoir une sortie un peu plus verbeuse."
 		"\n");
	for (int i=0; i<NB_TESTS; i++) {
		debug("Initializing memory\n");
		mem_init(get_memory_adr(), get_memory_size());
		int * tab1 = mem_alloc (3 * sizeof(int)) ;
		tab1[0] = 12 ;
		tab1[1] = 1 ;
		tab1[2] = 23 ;
		int * tab2 = mem_alloc (5 * sizeof(int)) ;
		tab2[0] = 44 ;
		tab2[1] = 37 ;
		tab2[2] = 61 ;
		tab2[3] = 97 ;
		tab2[4] = 105 ;
		printf("%d   %d   %d\n", tab1[0], tab1[1], tab1[2]) ;	//attention, printf utilise des malloc
		printf("%d   %d   %d   %d   %d\n", tab2[0], tab2[1], tab2[2], tab2[3], tab2[4]) ;
		printf("%ld\n", mem_get_size(tab1)) ;
		printf("%ld\n", mem_get_size(tab2)) ;
	}

	// TEST OK
	return 0;
}
