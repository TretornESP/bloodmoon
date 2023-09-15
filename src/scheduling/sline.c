
#include <stdint.h>

#include "sline.h"

#include "../util/printf.h"
#include "../memory/heap.h"

#include "../io/interrupts.h"

struct sleep_queue {
    uint64_t started_at;
    uint64_t expires_in;
    uint8_t sleep_reason;
    struct task * task;
    struct sleep_queue * prev;
    struct sleep_queue * next;
};

struct sleep_queue * sleep_time_queue_head = 0x0;
struct sleep_queue * sleep_reason_queue_head = 0x0;

void wakeup_handler() {
    //Remove all elements which have expired
    lock_scheduler();
    struct sleep_queue * sq = sleep_time_queue_head;
    uint32_t res = 0;
    while (sq != 0x0) {
        //printf("Task %d has been sleeping for %d ticks out of %d\n", sq->task->pid, get_ticks_since_boot() - sq->started_at, sq->expires_in);
        if (sq->expires_in != 0 && get_ticks_since_boot() - sq->started_at >= sq->expires_in) {
            //resume task
            resume_task(sq->task);

            if (sq->prev != 0x0) {
                sq->prev->next = sq->next;
            } else {
                sleep_time_queue_head = sq->next;
            }

            if (sq->next != 0x0) {
                sq->next->prev = sq->prev;
            }

            struct sleep_queue * tmp = sq;
            sq = sq->next;
            free(tmp);

            res++;
        } else {
            sq = sq->next;
        }
    }
    unlock_scheduler();
    if (res) yield();
}

void init_sline() {
    set_wakeup_call(wakeup_handler, 100);
}

void _tsleep(uint64_t ticks) {
    //printf("I want to sleep for %d ticks\n", ticks);
    struct sleep_queue * sq = (struct sleep_queue *)malloc(sizeof(struct sleep_queue));
    sq->started_at = get_ticks_since_boot();
    sq->expires_in = ticks;
    sq->task = get_current_task();
    sq->prev = 0x0;
    sq->next = 0x0;
    sq->sleep_reason = SLEEP_AWAITING_IO;

    //Insert into the sleep queue at the beginning
    if (sleep_time_queue_head != 0x0) {
        sleep_time_queue_head->prev = sq;
        sq->next = sleep_time_queue_head;
    }

    sleep_time_queue_head = sq;

    pause_task(sq->task);
}

void _ssleep(uint64_t seconds) {
    _tsleep(ms_to_ticks(seconds * 1000));
}

void _msleep(uint64_t milliseconds) {
    //printf("I want to sleep for %d ms\n", milliseconds);
    _tsleep(ms_to_ticks(milliseconds));
}

void _ksleep(uint8_t addr) {
    struct sleep_queue * sq = (struct sleep_queue *)malloc(sizeof(struct sleep_queue));
    sq->started_at = get_ticks_since_boot();
    sq->expires_in = 0;
    sq->task = get_current_task();
    sq->prev = 0x0;
    sq->next = 0x0;
    sq->sleep_reason = addr;

    //Insert into the sleep queue at the beginning
    if (sleep_reason_queue_head != 0x0) {
        sleep_reason_queue_head->prev = sq;
        sq->next = sleep_reason_queue_head;
    }

    sleep_reason_queue_head = sq;

    pause_task(sq->task);
}

void _kwakeup(uint8_t addr) {

    //Remove all elements which match the address

    struct sleep_queue * sq = sleep_reason_queue_head;
    while (sq != 0x0) {
        if (sq->sleep_reason == addr) {
            //resume task
            resume_task(sq->task);

            if (sq->prev != 0x0) {
                sq->prev->next = sq->next;
            } else {
                sleep_reason_queue_head = sq->next;
            }

            if (sq->next != 0x0) {
                sq->next->prev = sq->prev;
            }

            struct sleep_queue * tmp = sq;
            sq = sq->next;
            free(tmp);
        } else {
            sq = sq->next;
        }
    }
}