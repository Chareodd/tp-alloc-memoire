#include "mem.h"
#include "common.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

// constante définie dans gcc seulement
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define ALIGNMENT 16
#endif

#define ALIGN(adr)	((((intptr_t)(adr))+((ALIGNMENT)-1)) & ~((ALIGNMENT)-1))

struct fb {
	size_t size;
	struct fb* next;
};

struct ab {
	size_t size;
};

static struct fb* first_fb;	//pointeur vers le premier bloc libre

void mem_init(void* mem, size_t taille)
{
	assert(mem == get_memory_adr());
	assert(taille == get_memory_size());
	//Initialisation de la mémoire
	first_fb = (struct fb*) mem;
	first_fb->size = taille;
	first_fb->next = NULL;
	mem_fit(&mem_fit_first);
	return;
}

void mem_show(void (*print)(void *zone, size_t size, int free)) {
	void* mem_adr = get_memory_adr();	//adresse de début de zone
	void* end_mem_adr = mem_adr + get_memory_size();	//adresse de fin de zone
	void* adr = mem_adr;	//parcours de la mémoire
	struct fb* fb = first_fb;	//pointeur vers la prochaine zone libre(suivant adr)
	while (adr < end_mem_adr) {
		if(adr == fb) {	//si adr pointe sur une zone libre
			struct fb* current_adr = adr;
			print(adr, current_adr->size, 1);
			fb = fb->next;	//on avance à la prochaine zone libre
			adr += current_adr->size;	//on avance à la prochaine zone
		} else {	//si adr pointe sur une zone occupée
			struct ab* current_adr = adr;
			print(adr, current_adr->size, 0);
			adr += current_adr->size;
		}

	}
}

static mem_fit_function_t *mem_fit_fn;
void mem_fit(mem_fit_function_t *f) {
	mem_fit_fn=f;
}

void *mem_alloc(size_t taille) {
	taille += sizeof(struct ab);	//on ajoute la place nécessaire pour l'entête
	taille = ALIGN(taille);	//on ajoute du padding pour avoir une taille multiple de ALIGNMENT
	__attribute__((unused)) /* juste pour que gcc compile ce squelette avec -Werror */
	struct fb *fb = mem_fit_fn(first_fb, taille);
	if (fb == NULL) return NULL;	//si il n'y pas de mémoire disponible, inutile de continuer
	struct fb *current_fb = first_fb;
	if (fb == first_fb) first_fb = first_fb->next;
	while (current_fb != NULL) {

	}
	return NULL;
}


void mem_free(void* mem) {
}


struct fb* mem_fit_first(struct fb *list, size_t size) {
	while (list != NULL) {
		if (list->size >= size) {
			return list;
		}
		list = list->next;
	}
	return NULL;
}

/* Fonction à faire dans un second temps
 * - utilisée par realloc() dans malloc_stub.c
 * - nécessaire pour remplacer l'allocateur de la libc
 * - donc nécessaire pour 'make test_ls'
 * Lire malloc_stub.c pour comprendre son utilisation
 * (ou en discuter avec l'enseignant)
 */
size_t mem_get_size(void *zone) {
	/* zone est une adresse qui a été retournée par mem_alloc() */

	/* la valeur retournée doit être la taille maximale que
	 * l'utilisateur peut utiliser dans cette zone */
	struct ab* adr = zone - sizeof(struct ab);
	return adr->size;
}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb* mem_fit_best(struct fb *list, size_t size) {
	return NULL;
}

struct fb* mem_fit_worst(struct fb *list, size_t size) {
	return NULL;
}
