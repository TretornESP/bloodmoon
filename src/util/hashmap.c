#include "hashmap.h"
#include "../memory/heap.h"
#include "printf.h"
#include "string.h"
#include "md5.h"

struct hashmap * create_hashmap(uint64_t items) {
    if (items > HASHMAP_MAX_ITEMS)
        return 0;

    uint8_t failed = 0;

    struct hashmap * hmap = (struct hashmap*)malloc(sizeof(struct hashmap));
    hmap->items = items;
    hmap->map = (struct hmap_item*)malloc(sizeof(struct hmap_item) * items);
    for (uint64_t i = 0; i < sizeof(struct hmap_item) * items; i++) {
        uint8_t * ptr = (uint8_t*)(hmap->map+i);
        if (ptr != 0) {
            printf("Test ready\n");
            break;
        }
    }

    memset(hmap->map, 0, sizeof(struct hmap_item) * items);

    for (uint64_t i = 0; i < sizeof(struct hmap_item) * items; i++) {
        uint8_t * ptr = (uint8_t*)(hmap->map+i);
        if (ptr != 0)
            failed = 1;
    }

    if (failed) {
        printf("Test failed\n");
    } else {
        printf("Test passed\n");
    }
    while(1);
    
    return hmap;
}

void delete_hashmap(struct hashmap * hmap) {
    for (uint64_t i = 0; i < hmap->items; i++) {
        struct hmap_item * item = (struct hmap_item*)(hmap->map + (i * sizeof(struct hmap_item)));
        if (item->hash) {
            item->hash = 0;
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

uint64_t insert(struct hashmap * hmap, struct hashmap_data * data) {
    uint64_t hash = calculate_hash(data->pointer, data->size);

    uint64_t index = hash % hmap->items;
    uint64_t original_index = index;
    
    struct hmap_item * item = (struct hmap_item*)(hmap->map + (index * sizeof(struct hmap_item)));
    printf("Item: %p, Index: %d, Hash: %d\n", item, index, hash);
    while (item->hash != 0) {
        index++;
        if (index >= hmap->items)
            index = 0;
        if (index == original_index)
            return 0;
        item = (struct hmap_item*)(hmap->map + (index * sizeof(struct hmap_item)));
        printf("Item: %p, Index: %d, Hash: %d\n", item, index, hash);
    }

    item->data.pointer = data->pointer;
    item->data.size = data->size;
    item->hash = hash;
    printf("Inserted stuff\n");
    return hash;
}

int retrieve(struct hashmap * hmap, struct hashmap_data * data, uint64_t key) {
    uint64_t index = key % hmap->items;
    uint64_t original_index = index;
    
    struct hmap_item * item = (struct hmap_item*)(hmap->map + (index * sizeof(struct hmap_item)));
    while (item != 0) {
        if (item->hash == key) {
            data->pointer = item->data.pointer;
            data->size = item->data.size;
            return 1;
        }
        index++;
        if (index >= hmap->items)
            index = 0;
        if (index == original_index)
            return 0;
        item = (struct hmap_item*)(hmap->map + (index * sizeof(struct hmap_item)));
    }
    
    return 0;
}