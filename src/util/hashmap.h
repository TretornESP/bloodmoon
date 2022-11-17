#ifndef _HASHMAP_H
#define _HASHMAP_H
#include <stdint.h>
#define HASHMAP_MAX_ITEMS 256
#define HASHMAP_ITEM_LEN  16
struct hashmap_data {
    void * pointer;
    uint64_t size;
};

struct hmap_item {
    struct hashmap_data data;
    uint64_t hash;
};

struct hashmap {
    struct hmap_item * map;
    uint64_t items;
} __attribute__((packed));

struct hashmap * create_hashmap(uint64_t);
void delete_hashmap(struct hashmap *);

uint64_t insert(struct hashmap *, struct hashmap_data *);
int retrieve(struct hashmap *, struct hashmap_data *, uint64_t);

#endif