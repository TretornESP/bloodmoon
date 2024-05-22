#include "vdev.h"
#include "../devices.h"
#include "../../memory/heap.h"
#include "../../util/string.h"

struct vdev* create_vdev(const char * indev, const char* outdev) {
    struct vdev* vdev = (struct vdev*)kmalloc(sizeof(struct vdev));
    strcpy(vdev->indev, indev);
    strcpy(vdev->outdev, outdev);
    return vdev;
}
void destroy_vdev(struct vdev* vdev) {
    kfree(vdev);
}
void read_vdev(struct vdev* vdev, char* data, int len) {
    device_read(vdev->indev, len, 0, (uint8_t*)data);

}
void write_vdev(struct vdev* vdev, const char* data, int len) {
    device_write(vdev->outdev, len, 0, (uint8_t*)data);
}