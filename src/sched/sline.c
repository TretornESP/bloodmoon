
#include <stdint.h>

#include "concurrency.h"
#include "sline.h"

#include "../util/printf.h"
#include "../memory/heap.h"
#include "../io/interrupts.h"

lock_t sleep_lock = 0;

struct sleep_queue {
    uint64_t started_at;
    uint64_t expires_in;
    void * sleep_reason;
    struct task * task;
    struct sleep_queue * prev;
    struct sleep_queue * next;
};

struct sleep_queue * sleep_time_queue_head = 0x0;
struct sleep_queue * sleep_reason_queue_head = 0x0;

void wakeup_handler() {
    //Remove all elements which have expired
    acquire_lock_ns(&sleep_lock);
    struct sleep_queue * sq = sleep_time_queue_head;
    uint32_t res = 0;
    while (sq != 0x0) {
        if (sq->task == 0x0) {
            sq = sq->next;
            continue;
        }
        //printf("Task %d has been sleeping for %d ticks out of %d\n", sq->task->pid, get_ticks_since_boot() - sq->started_at, sq->expires_in);
        if (sq->expires_in != 0 && get_ticks_since_boot() - sq->started_at >= sq->expires_in) {
            //resume task
            if (sq->task->state == TASK_UNINTERRUPTIBLE) {
                sq->task->state = TASK_READY;
            }

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
            kfree(tmp);

            res++;
        } else {
            sq = sq->next;
        }
    }
    release_lock_ns(&sleep_lock);
}

void init_sline() {
    set_wakeup_call(wakeup_handler, 100);
}

void _tsleep(uint64_t ticks) {
    acquire_lock_ns(&sleep_lock);

    struct task *ctask = get_current_task();
    if (ctask == 0x0) {
        release_lock_ns(&sleep_lock);
        return;
    }

    //printf("I want to sleep for %d ticks\n", ticks);
    struct sleep_queue * sq = (struct sleep_queue *)kmalloc(sizeof(struct sleep_queue));
    sq->started_at = get_ticks_since_boot();
    sq->expires_in = ticks;
    sq->task = ctask;
    sq->prev = 0x0;
    sq->next = 0x0;
    sq->sleep_reason = (void*)SLEEP_AWAITING_IO;

    //Insert into the sleep queue at the beginning
    if (sleep_time_queue_head != 0x0) {
        sleep_time_queue_head->prev = sq;
        sq->next = sleep_time_queue_head;
    }

    sleep_time_queue_head = sq;

    sq->task->state = TASK_UNINTERRUPTIBLE;
    release_lock_ns(&sleep_lock);
}

void _ssleep(uint64_t seconds) {
    _tsleep(ms_to_ticks(seconds * 1000));
}

void _msleep(uint64_t milliseconds) {
    //printf("I want to sleep for %d ms\n", milliseconds);
    _tsleep(ms_to_ticks(milliseconds));
}

void _ksleep(void* addr) {
    acquire_lock_ns(&sleep_lock);

    struct task *ctask = get_current_task();
    if (ctask == 0x0) {
        release_lock_ns(&sleep_lock);
        return;
    }

    struct sleep_queue * sq = (struct sleep_queue *)kmalloc(sizeof(struct sleep_queue));
    sq->started_at = get_ticks_since_boot();
    sq->expires_in = 0;
    sq->task = ctask;

    sq->prev = 0x0;
    sq->next = 0x0;
    sq->sleep_reason = addr;

    //Insert into the sleep queue at the beginning
    if (sleep_reason_queue_head != 0x0) {
        sleep_reason_queue_head->prev = sq;
        sq->next = sleep_reason_queue_head;
    }

    sleep_reason_queue_head = sq;

    sq->task->state = TASK_UNINTERRUPTIBLE;
    acquire_lock_ns(&sleep_lock);
}

void _kwakeup(void* addr) {
    acquire_lock_ns(&sleep_lock);
    //Remove all elements which match the address

    struct sleep_queue * sq = sleep_reason_queue_head;
    while (sq != 0x0) {
        if (sq->sleep_reason == addr) {
            //resume task
            if (sq->task == 0) {
                sq = sq->next;
                continue;
            }
            if (sq->task->state == TASK_UNINTERRUPTIBLE) {
                sq->task->state = TASK_READY;
            }

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
            kfree(tmp);
        } else {
            sq = sq->next;
        }
    }
    release_lock_ns(&sleep_lock);
}