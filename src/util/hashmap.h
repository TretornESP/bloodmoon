#ifndef _HASHMAP_H
#define _HASHMAP_H
#include <stdint.h>
#define HASHMAP_MAX_ITEMS 65536
#define HASHMAP_ITEM_LEN  16

struct hashmap_item {
    uint64_t pointer;
    uint64_t size;
    uint64_t key;
};

struct hashmap {
    uint64_t pointer;
    uint64_t items;
} __attribute__((packed));

struct hashmap * create_hashmap(uint64_t);
void delete_hashmap(struct hashmap *);
uint64_t debug_get_hash(struct hashmap_item *);
uint64_t insert(struct hashmap *, struct hashmap_item *);
int retrieve(struct hashmap *, struct hashmap_item *, uint64_t);

#endif