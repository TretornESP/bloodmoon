#include "hashmap.h"
#include "../memory/heap.h"
#include "printf.h"
#include "string.h"
#include "md5.h"
#include "../drivers/keyboard/keyboard.h"

struct hashmap * create_hashmap(uint64_t items) {
    if (items > HASHMAP_MAX_ITEMS)
        return 0;

    struct hashmap * hmap = (struct hashmap*)malloc(sizeof(struct hashmap));
    hmap->items = items;
    uint64_t size = sizeof(struct hashmap_item) * items;
    hmap->pointer = (uint64_t)malloc(size);

    memset((void*)hmap->pointer, 0, size);
    if (hmap->pointer == 0 || (zerocheck((void*)hmap->pointer, size) >= 0)) {
        printf("HASHMAP: Failed to allocate memory for hashmap\n");
        return 0;
    }
    return hmap;
}

void delete_hashmap(struct hashmap * hmap) {
    for (uint64_t i = 0; i < hmap->items; i++) {
        struct hashmap_item * item = (struct hashmap_item*)(hmap->pointer + (i * sizeof(struct hashmap_item)));
        if (item->key) {
            free(item);
        }
    }
    free(hmap);
}

uint64_t calculate_hash(void * address, uint64_t size) {

    uint8_t * hash = (uint8_t*)malloc(16);
    MD5_Digest(hash, address, size);

    uint64_t hash_value = 0;
    for (int i = 0; i < 16; i++) {
        hash_value += hash[i];
    }

    free(hash);
    return hash_value;
}

uint64_t debug_get_hash(struct hashmap_item * data) {
    return calculate_hash((void*)data->pointer, data->size);
}

uint64_t insert(struct hashmap * hmap, struct hashmap_item * data) {
    if (data->key == 0 || data->pointer == 0 || data->size == 0) {
        printf("HASHMAP: Invalid data\n");
        printf("HASHMAP: Key: %llx, Pointer: %llx, Size: %llx\n", data->key, data->pointer, data->size);
        return 0;
    }

    uint64_t hash = calculate_hash((void*)data->pointer, data->size);

    uint64_t index = hash % hmap->items;
    uint64_t original_index = index;
    
    struct hashmap_item * item = (struct hashmap_item*)(hmap->pointer + (index * sizeof(struct hashmap_item)));
    while (item->key) {
        index++;
        if (index >= hmap->items) {
            index = 0;
        }
        if (index == original_index) {
            printf("HASHMAP: No free space in hashmap\n");
            return 0;
        }
        item = (struct hashmap_item*)(hmap->pointer + (index * sizeof(struct hashmap_item)));
    }

    item->key = data->key;
    item->pointer = (uint64_t)data->pointer;
    item->size = data->size;

    return hash;
}

int retrieve(struct hashmap * hmap, struct hashmap_item * data, uint64_t key) {
    uint64_t index = key % hmap->items;
    uint64_t original_index = index;
    
    struct hashmap_item * item = (struct hashmap_item*)(hmap->pointer + (index * sizeof(struct hashmap_item)));
    while (item->key != key) {
        index++;
        if (index >= hmap->items) {
            index = 0;
        }
        if (index == original_index) {
            printf("HASHMAP: No item with key %x\n", key);
            return 0;
        }
        item = (struct hashmap_item*)(hmap->pointer + (index * sizeof(struct hashmap_item)));
    }

    data->pointer = item->pointer;
    data->size = item->size;

    return 1;
}