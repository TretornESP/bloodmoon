#ifndef _HASHMAP_H
#define _HASHMAP_H
#define HASHMAP_MAX_ITEMS 256

struct hmap_item {
    void * pointer;
    uint64_t size;
}

struct hashmap {
    void ** map;
    uint64_t items;
} __attribute__((packed));

struct hashmap * create_hashmap(uint64_t);
void delete_hashmap(struct hashmap *);

int insert(struct hashmap *, void *, uint64_t);
int retrieve(struct hashmap *, void*, uint64_t);

#endif