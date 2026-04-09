#include "hashmap.h"
#include <stdlib.h>
#include <stdbool.h>

#define RESIZE_FACTOR 0.75
#define SMALLEST_SIZE 31

typedef struct hash_node {
    void* key;
    size_t hash;
    void* value;
    struct hash_node* next;
} hash_node;

typedef struct hash_map {
    hash_node** table;
    size_t count;
    size_t size;
    size_t (*hash)(const void*);
    int (*cmp_key)(const void*, const void*);
    void (*free_key)(void*);
    void (*free_value)(void*);
} hash_map;

static bool is_prime(size_t n) {

    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (size_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }

    return true;
}

static size_t next_prime(size_t n) {

    while (!is_prime(n)) {
        n++;
    }

    return n;
}

hash_map* new_hash_map(size_t initial_size, size_t (*hash)(const void*), int (*cmp_key)(const void*, const void*), void (*free_key)(void*), void (*free_value)(void*)) {

    if (hash == NULL || cmp_key == NULL) {
        return NULL;
    }

    if (initial_size < SMALLEST_SIZE) {
        initial_size = SMALLEST_SIZE;
    }

    hash_map* hm = malloc(sizeof(*hm));

    if (hm == NULL) {
        return NULL;
    }

    size_t prime_size = next_prime(initial_size);
    hm->table = calloc(prime_size, sizeof(hash_node*));

    if (hm->table == NULL) {
        free(hm);
        return NULL;
    }

    hm->count = 0;
    hm->size = prime_size;
    hm->hash = hash;
    hm->cmp_key = cmp_key;
    hm->free_key = free_key;
    hm->free_value = free_value;

    return hm;
}

static hash_node* new_hash_node(void* key, size_t hash, void* value) {
    hash_node* n = malloc(sizeof(*n));

    if (n == NULL) {
        return NULL;
    }

    n->key = key;
    n->hash = hash;
    n->value = value;
    n->next = NULL;

    return n;
}

static status hash_map_resize(hash_map* hm) {
    size_t new_size = next_prime(hm->size * 2);
    hash_node** new_table = calloc(new_size, sizeof(*new_table));

    if (new_table == NULL) {
        return ALLOC_FAIL;
    }

    for (size_t i = 0; i < hm->size; i++) {
        hash_node* current = hm->table[i];

        while (current != NULL) {
            hash_node* next = current->next;
            size_t new_index = current->hash % new_size;
            current->next = new_table[new_index];
            new_table[new_index] = current;
            current = next;
        }
    }

    free(hm->table);
    hm->table = new_table;
    hm->size = new_size;

    return SUCCESS;
}

status hash_map_insert(hash_map* hm, void* key, void* value) {

    if (hm == NULL || key == NULL) {
        return NULL_POINTER;
    }

    size_t hash = hm->hash(key);
    size_t index = hash % hm->size;

    for (hash_node* current = hm->table[index]; current != NULL; current = current->next) {
        if (current->hash == hash && hm->cmp_key(current->key, key) == 0) {

            if (hm->free_value != NULL) {
                hm->free_value(current->value);
            }

            if (hm->free_key != NULL && current->key != key) {
                hm->free_key(current->key);
            }

            current->key = key;
            current->value = value;

            return SUCCESS;
        }
    }

    if ((double)hm->count / hm->size >= RESIZE_FACTOR) {
        if (hash_map_resize(hm) == ALLOC_FAIL) {
            return ALLOC_FAIL;
        }
    }

    index = hash % hm->size;
    hash_node* new_node = new_hash_node(key, hash, value);

    if (new_node == NULL) {
        return ALLOC_FAIL;
    }

    new_node->next = hm->table[index];
    hm->table[index] = new_node;
    hm->count++;

    return SUCCESS;
}

status hash_map_remove(hash_map* hm, const void* key) {

    if (hm == NULL || key == NULL) {
        return NULL_POINTER;
    }

    if (hm->count == 0) {
        return UNDERFLOW;
    }

    size_t hash = hm->hash(key);
    size_t index = hash % hm->size;

    hash_node* current = hm->table[index];
    hash_node* previous = NULL;

    while (current != NULL) {
        if (current->hash == hash && hm->cmp_key(current->key, key) == 0) {

            if (current == hm->table[index]) {
                hm->table[index] = current->next;
            } else {
                previous->next = current->next;
            }

            if (hm->free_key != NULL) {
                hm->free_key(current->key);
            }

            if (hm->free_value != NULL) {
                hm->free_value(current->value);
            }

            free(current);
            hm->count--;

            return SUCCESS;
        }

        previous = current;
        current = current->next;
    }

    return NOT_FOUND;
}

void* hash_map_get(const hash_map* hm, void* key) {

    if (hm == NULL || hm->count == 0 || key == NULL) {
        return NULL;
    }

    size_t hash = hm->hash(key);
    size_t index = hash % hm->size;

    for (hash_node* current = hm->table[index]; current != NULL; current = current->next) {
        if (current->hash == hash && hm->cmp_key(current->key, key) == 0) {
            return current->value;
        }
    }

    return NULL;
}

status hash_map_clear(hash_map* hm) {

    if (hm == NULL) return NULL_POINTER;
    
    for (size_t i = 0; i < hm->size; i++) {
        hash_node* current = hm->table[i];

        while (current != NULL) {
            hash_node* temp = current;
            current = current->next;
            if (hm->free_key) hm->free_key(temp->key);
            if (hm->free_value) hm->free_value(temp->value);
            free(temp);
        }

        hm->table[i] = NULL;
    }

    hm->count = 0;
    return SUCCESS;
}

void hash_map_free(hash_map* hm) {

    if (hm == NULL) return;
    hash_map_clear(hm);
    free(hm->table);
    free(hm);
}