#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

size_t hash_string(const void* key) {
    const char* str = key;
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

int cmp_string(const void* k1, const void* k2) {
    return strcmp(k1, k2);
}

int main() {
    
    hash_map* map = new_hash_map(10, hash_string, cmp_string, free, free);
    
    int* val1 = malloc(sizeof(int)); *val1 = 100;
    int* val2 = malloc(sizeof(int)); *val2 = 200;

    hash_map_insert(map, strdup("usuario_id"), val1);
    hash_map_insert(map, strdup("score"), val2);

    int* res = (int*)hash_map_get(map, "score");
    if (res) printf("Score: %d\n", *res);

    hash_map_remove(map, "usuario_id");

    hash_map_free(map);

    return 0;
}