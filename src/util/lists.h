#ifndef _LISTS_H
#define _LISTS_H

struct node {
    void * data;
    struct node * next;
    struct node * prev;
};

struct linked_list {
    int size;
    struct node * head;
    struct node * tail;
};

struct linked_list * linked_list_create();
void linked_list_destroy(struct linked_list * list);

void linked_list_add(struct linked_list * list, void * data);
void linked_list_remove(struct linked_list * list, void * data);
void linked_list_remove_at(struct linked_list * list, int index);
void linked_list_remove_all(struct linked_list * list);

void * linked_list_get(struct linked_list * list, int index);
void * linked_list_get_first(struct linked_list * list);
void * linked_list_get_last(struct linked_list * list);

int linked_list_size(struct linked_list * list);
int linked_list_index_of(struct linked_list * list, void * data);

void linked_list_print(struct linked_list * list);

// Circular list for producer-consumer

typedef struct s_circularBuffer* CircularBuffer;

// Construct CircularBuffer with ‘size' in byte. You must call CircularBufferkfree() in balance for destruction.
extern CircularBuffer CircularBufferCreate(int size);

// Destruct CircularBuffer 
extern void CircularBufferkfree(CircularBuffer cBuf);

// Reset the CircularBuffer
extern void CircularBufferReset(CircularBuffer cBuf);

//get the capacity of CircularBuffer
extern int CircularBufferGetCapacity(CircularBuffer cBuf);

//same as CircularBufferGetCapacity, Just for compatibility with older versions
extern int CircularBufferGetSize(CircularBuffer cBuf);

//get occupied data size of CircularBuffer
extern int CircularBufferGetDataSize(CircularBuffer cBuf);

// Push data to the tail of a circular buffer from 'src' with 'length' size in byte.
extern void CircularBufferPush(CircularBuffer cBuf,void *src, int length);

// Pop data from a circular buffer to 'dataOut'  with wished 'length' size in byte,return the actual data size in byte popped out,which is less or equal to the input 'length parameter.
extern int CircularBufferPop(CircularBuffer cBuf, int length, void *dataOut);

// Read data from a circular buffer to 'dataOut'  with wished 'length' size in byte,return the actual data size in byte popped out,which is less or equal to the input 'length parameter.
extern int CircularBufferRead(CircularBuffer cBuf, int length, void *dataOut);

#endif