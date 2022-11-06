#include "hashmap.h"
#include "../memory/heap.h"
#include "string.h"
#include "md5.h"

struct hashmap * create_hashmap(uint64_t items) {
    struct hashmap * hmap = (struct hashmap*)malloc(sizeof(uint64_t) + (sizeof(void*) * items));
    hmap->items = items;
    hmap->map = calloc(0, sizeof(void*) * items);
    
    return hmap;
}

void delete_hashmap(struct hashmap * hmap) {
    free(hmap->map);
    free(hmap);
}

uint64_t calculate_hash(void * address, uint64_t size) {
    struct md5_ctx mdc;
    MD5_Init(&mdc);
    MD5_Update(&mdc, address, size);
    uint8_t * hash = (uint8_t*)malloc(16);
    MD5_Final(hash, &mdc);
    uint64_t hash_value = 0;
    for (int i = 0; i < 16; i++) {
        hash_value += hash[i];
    }
    free(hash);
    return hash_value;
}

int insert(struct hashmap * hmap, void * address, uint64_t size) {
    uint64_t hash = calculate_hash(address, size);
    uint64_t index = hash % hmap->items;
    if (hmap->map[index] == 0) {
        hmap->map[index] = address;
        return 0;
    }
    
    return -1;
}

int retrieve(struct hashmap * hmap, void*, uint64_t);