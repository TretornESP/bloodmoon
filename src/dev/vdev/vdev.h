#ifndef _VDEV_H_
#define _VDEV_H_

#define VDEV_MAX_SIZE 1024

struct vdev {
    char indev[VDEV_MAX_SIZE];
    char outdev[VDEV_MAX_SIZE];
};

struct vdev* create_vdev(const char * indev, const char* outdev);
void destroy_vdev(struct vdev* vdev);
void read_vdev(struct vdev* vdev, char* data, int len);
void write_vdev(struct vdev* vdev, const char* data, int len);
#endif