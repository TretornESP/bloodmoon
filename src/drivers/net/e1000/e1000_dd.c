#include "e1000c.h"
#include "e1000_dd.h"
#include "../../../util/printf.h"
#include "../../../util/string.h"
#include "../../../devices/devices.h"

uint64_t dd_send(void* nic, uint8_t* buffer, uint16_t len) {
    return sendPacket(nic, buffer, len);
}

uint64_t dd_recv(void* nic, uint8_t* buffer, uint16_t len) {
    (void)nic;
    (void)buffer;
    (void)len;
    printf("READING FROM NIC DOESNT MAKE SENSE\n");
    return 0;
}

uint64_t dd_nic_ioctl (void* nic, uint32_t op , void* data) {
    //printf("IOCTL: %d\n", op);
    switch (op) {
        case IOCTL_E1000_INJECT_CB:
            e1000_set_injection_callback(nic, (void (*)(uint8_t*, uint8_t*, uint16_t))data);
            return 0;
        case IOCTL_E1000_GET_MAC:
            uint8_t* buffer = getMacAddress(nic);
            memcpy(data, buffer, 6);
            return 0;
        case IOCTL_E1000_GET_STATUS:
            return e1000_get_status(nic);
        case IOCTL_E1000_INJECT_SC:
            e1000_inject_status_change_callback(nic, (void (*)(uint8_t*, uint8_t))data);
            return 0;
    }
    return 0;
}

struct network_operations nops = {
    .send = dd_send,
    .recv = dd_recv,
    .ioctl = dd_nic_ioctl
};

void init_e1000_dd(void) {
    register_network(0x11, E1000_DD_NAME, &nops);
}

void exit_d1000_dd(void) {}