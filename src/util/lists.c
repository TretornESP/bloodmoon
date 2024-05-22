#include "lists.h"
#include "printf.h"
#include "string.h"
#include "bool.h"
#include "../memory/heap.h"

struct linked_list * linked_list_create() {
    struct linked_list * list = (struct linked_list *) kmalloc(sizeof(struct linked_list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void linked_list_destroy(struct linked_list * list) {
    linked_list_remove_all(list);
    kfree(list);
}

void linked_list_add(struct linked_list * list, void * data) {
    struct node * node = (struct node *) kmalloc(sizeof(struct node));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    if (list->size == 0) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->size++;
}

void linked_list_remove(struct linked_list * list, void * data) {
    struct node * node = list->head;
    while (node != NULL) {
        if (node->data == data) {
            if (node->prev != NULL) {
                node->prev->next = node->next;
            } else {
                list->head = node->next;
            }
            if (node->next != NULL) {
                node->next->prev = node->prev;
            } else {
                list->tail = node->prev;
            }
            kfree(node);
            list->size--;
            return;
        }
        node = node->next;
    }
}

void linked_list_remove_at(struct linked_list * list, int index) {
    struct node * node = list->head;
    int i = 0;
    while (node != NULL) {
        if (i == index) {
            if (node->prev != NULL) {
                node->prev->next = node->next;
            } else {
                list->head = node->next;
            }
            if (node->next != NULL) {
                node->next->prev = node->prev;
            } else {
                list->tail = node->prev;
            }
            kfree(node);
            list->size--;
            return;
        }
        node = node->next;
        i++;
    }
}

void linked_list_remove_all(struct linked_list * list) {
    struct node * node = list->head;
    while (node != NULL) {
        struct node * next = node->next;
        kfree(node);
        node = next;
    }
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void * linked_list_get(struct linked_list * list, int index) {
    struct node * node = list->head;
    int i = 0;
    while (node != NULL) {
        if (i == index) {
            return node->data;
        }
        node = node->next;
        i++;
    }
    return NULL;
}

void * linked_list_get_first(struct linked_list * list) {
    return list->head->data;
}

void * linked_list_get_last(struct linked_list * list) {
    return list->tail->data;
}

int linked_list_size(struct linked_list * list) {
    return list->size;
}

int linked_list_index_of(struct linked_list * list, void * data) {
    struct node * node = list->head;
    int i = 0;
    while (node != NULL) {
        if (node->data == data) {
            return i;
        }
        node = node->next;
        i++;
    }
    return -1;
}

void linked_list_print(struct linked_list * list) {
    struct node * node = list->head;
    while (node != NULL) {
        printf("%s\n", (char *) node->data);
        node = node->next;
    }
}

//MIT License
//
//Copyright (c) 2018 Roen
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

struct s_circularBuffer{
    
    int size; //capacity bytes size
    int dataSize; //occupied data size
    int tailOffset; //head offset, the oldest byte position offset
    int headOffset; //tail offset, the lastest byte position offset
    void *buffer;

};

extern CircularBuffer CircularBufferCreate(int size)
{
    int totalSize = sizeof(struct s_circularBuffer) + size;
    void *p = kmalloc(totalSize);
    CircularBuffer buffer = (CircularBuffer)p;
    buffer->buffer = p + sizeof(struct s_circularBuffer);
    buffer->size = size;
    CircularBufferReset(buffer);
    return buffer;
}

void CircularBufferkfree(CircularBuffer cBuf)
{
    CircularBufferReset(cBuf);
    cBuf->size = 0;
    cBuf->dataSize = 0;
    cBuf->buffer = NULL;
    kfree(cBuf);
}

void CircularBufferReset(CircularBuffer cBuf)
{
    cBuf->headOffset = -1;
    cBuf->tailOffset = -1;
    cBuf->dataSize = 0;
}

int CircularBufferGetCapacity(CircularBuffer cBuf) {
    
    return cBuf->size;
}

int CircularBufferGetSize(CircularBuffer cBuf)
{
    return cBuf->size;
}

int CircularBufferGetDataSize(CircularBuffer cBuf)
{
    return cBuf->dataSize;
}

void CircularBufferPush(CircularBuffer cBuf,void *src, int length)
{
    if(length == 0)
        return;

    int writableLen = length;
    void *pSrc = src;
    
    if(writableLen > cBuf->size)//in case of size overflow
    {
        int overFlowLen = writableLen - cBuf->size;
        writableLen = cBuf->size;
        pSrc = src + overFlowLen;
    }
    
    
    bool resetHead = false;
    //in case the circle buffer won't be full after adding the data
    if(cBuf->tailOffset+writableLen < cBuf->size)
    {
        memcpy(&((uint8_t*)cBuf->buffer)[cBuf->tailOffset + 1], pSrc, writableLen);
        
        if((cBuf->tailOffset < cBuf->headOffset) && (cBuf->tailOffset+writableLen >= cBuf->headOffset) )
            resetHead = true;
        
        cBuf->tailOffset += writableLen;
    }
    else//in case the circle buffer will be overflow after adding the data
    {
        int remainSize = cBuf->size - cBuf->tailOffset - 1; //the remain size
        memcpy(&((uint8_t*)cBuf->buffer)[cBuf->tailOffset+1], pSrc, remainSize);
        
        int coverSize = writableLen - remainSize; //size of data to be covered from the beginning
        memcpy(cBuf->buffer, pSrc+remainSize, coverSize);
        
        if(cBuf->tailOffset < cBuf->headOffset)
            resetHead = true;
        else
        {
            if(coverSize>cBuf->headOffset)
                resetHead = true;
        }
        
        cBuf->tailOffset = coverSize - 1;
    }
    
    if(cBuf->headOffset == -1)
        cBuf->headOffset = 0;
    
    if(resetHead)
    {
        if(cBuf->tailOffset+1 < cBuf->size)
            cBuf->headOffset = cBuf->tailOffset + 1;
        else
            cBuf->headOffset = 0;
        
        cBuf->dataSize = cBuf->size;
    }
    else
    {
        if(cBuf->tailOffset >= cBuf->headOffset)
            cBuf->dataSize = cBuf->tailOffset - cBuf->headOffset + 1;
        else
            cBuf->dataSize = cBuf->size - (cBuf->headOffset - cBuf->tailOffset - 1);
    }
}

int inter_circularBuffer_read(CircularBuffer cBuf, int length, void *dataOut, bool resetHead)
{
    if(cBuf->dataSize == 0 || length == 0)
        return 0;
    
    int rdLen = length;
    
    if(cBuf->dataSize < rdLen)
        rdLen = cBuf->dataSize;
    
    
    if(cBuf->headOffset <= cBuf->tailOffset)
    {
        if(dataOut)
            memcpy(dataOut, &((uint8_t*)cBuf->buffer)[cBuf->headOffset], rdLen);
        
        if(resetHead)
        {
            cBuf->headOffset += rdLen;
            if(cBuf->headOffset > cBuf->tailOffset)
            {
                cBuf->headOffset = -1;
                cBuf->tailOffset = -1;
            }
        }
    }
    else
    {
        if(cBuf->headOffset+rdLen <= cBuf->size)
        {
            if(dataOut)
                memcpy(dataOut, &((uint8_t*)cBuf->buffer)[cBuf->headOffset], rdLen);
            
            if(resetHead)
            {
                cBuf->headOffset += rdLen;
                if(cBuf->headOffset == cBuf->size)
                    cBuf->headOffset = 0;
            }
        }
        else
        {
            int frg1Len = cBuf->size - cBuf->headOffset;
            if(dataOut)
                memcpy(dataOut, &((uint8_t*)cBuf->buffer)[cBuf->headOffset], frg1Len);
            
            int frg2len = rdLen - frg1Len;
            if(dataOut)
                memcpy(dataOut+frg1Len, cBuf->buffer, frg2len);
            
            if(resetHead)
            {
                cBuf->headOffset = frg2len;
                if(cBuf->headOffset > cBuf->tailOffset)
                {
                    cBuf->headOffset = -1;
                    cBuf->tailOffset = -1;
                }
            }
        }
    }
    
    if(resetHead)
        cBuf->dataSize -= rdLen;
    
    return rdLen;
}


int CircularBufferPop(CircularBuffer cBuf, int length, void *dataOut)
{
    return inter_circularBuffer_read(cBuf,length,dataOut,true);
}

int CircularBufferRead(CircularBuffer cBuf, int length, void *dataOut)
{
    return inter_circularBuffer_read(cBuf,length,dataOut,false);
}


//print circular buffer's content into str,
void CircularBufferPrint(CircularBuffer cBuf, bool hex)
{
    char *b = cBuf->buffer;
    int cSize = CircularBufferGetSize(cBuf);
    char *str = kmalloc(2*cSize+1);
    
    char c;
    
    for(int i=0; i<cSize; i++)
    {
        if(CircularBufferGetDataSize(cBuf) == 0)
        {
            c = '_';
        }
        else if (cBuf->tailOffset < cBuf->headOffset)
        {
            if(i>cBuf->tailOffset && i<cBuf->headOffset)
                c = '_';
            else
              c = b[i];
        }
        else
        {
            if(i>cBuf->tailOffset || i<cBuf->headOffset)
                c = '_';
            else
                c = b[i];
        }
        if(hex)
            sprintf(str+i*2, "%02X|",c);
        else
            sprintf(str+i*2, "%c|",c);
    }
    
    printf("CircularBuffer: %s <size %zu dataSize:%zu>\n",str,CircularBufferGetSize(cBuf),CircularBufferGetDataSize(cBuf));
    
    kfree(str);
}