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
	if (taille < sizeof (struct fb)) taille = sizeof (struct fb) ; //pour éviter d'avoir des problèmes pour les petite allocations
	taille += sizeof (struct ab); //pour stocker l'entête
	taille = ALIGN(taille);	//on ajoute du padding pour avoir une taille multiple de ALIGNMENT
	__attribute__((unused)) /* juste pour que gcc compile ce squelette avec -Werror */
	struct fb *fb = mem_fit_fn(first_fb, taille);
	if (fb == NULL) return NULL;	//si il n'y pas de mémoire disponible, inutile de continuer
	int taille_ini = fb->size ;
	struct fb *prev_fb = first_fb;
	while ((prev_fb != NULL) && (prev_fb->next != fb)) {
		prev_fb = prev_fb->next ;
	}
	struct ab *new_ab = (struct ab*) fb ;
	if (taille_ini - taille < sizeof (struct fb)) {
		taille = taille_ini; //on ne peut pas séparer la zone libre en 2 zones
		if (fb == first_fb) {
			first_fb = fb + sizeof (struct ab) + taille;	//si la zone libre alloué est la première, on met a jour first_fb
		} else {
			prev_fb->next = fb->next ;
		}
	} else {	//on sépare la zone libre en 2, la zone occupé à renvoyer et la zone libre restante
		struct fb* new_fb = fb + sizeof (struct ab) + taille ;
		new_fb->size = taille_ini - taille - sizeof(struct fb) ;
		if (fb == first_fb) {
			first_fb = fb + sizeof (struct ab) + taille;	//si la zone libre alloué est la première, on met a jour first_fb
		} else {
			prev_fb->next = new_fb;	//on raccorde la zone libre juste avant la zone à allouer avec celle juste après
		}
	}
	new_ab->size = taille ;


	return (void *) (new_ab + sizeof(struct ab));
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
