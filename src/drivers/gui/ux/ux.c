#include "ux.h"

#include "../gui.h"
#include "../../../memory/heap.h"
#include "../../../vfs/vfs_interface.h"
#include "../../../vfs/vfs.h"
#include "../../../util/printf.h"
#include "../../../util/string.h"
#include "../../../util/json.h"
#include "../../../util/json_parse.h"

uint8_t startx_started = 0;

struct ux_component windows[UX_MAX_WINDOWS] = {0};
uint8_t window_count = 0;

static void process_value(json_value* value, int depth);

static void process_object(json_value* value, int depth) {

}

static void process_array(json_value* value, int depth) {

}

void ux_write_extra_byte(struct ux_component * component, uint8_t data) {
    if (component == 0) {
        printf("Component is null\n");
        return;
    }

    if (component->extra_data == 0) {
        component->extra_data = UX_MAX_EXTRA_DATA;
        memset(component->extra_data, 0, UX_MAX_EXTRA_DATA);
    }

    if (component->extra_data_count >= UX_MAX_EXTRA_DATA) {
        printf("Max extra data reached\n");
        return;
    }

    component->extra_data[component->extra_data_count++] = data;

}

void ux_write_bytes(struct ux_component * component, uint8_t * data, uint64_t size) {
    if (component == 0) {
        printf("Component is null\n");
        return;
    }

    if (component->data == 0) {
        component->data = UX_MAX_DATA;
        memset(component->data, 0, UX_MAX_DATA);
    }

    if (component->data_count + size >= UX_MAX_DATA) {
        printf("Max data reached\n");
        return;
    }

    memcpy(component->data + component->data_count, data, size);
    component->data_count += size;
}

//FORMAT: [type][name_len][name][data_len][data][crc32]
//SIZES:  [   1][       2][   N][       2][   M][    4]
static void serialize_extra_data(const char* name, uint8_t type, void* data, struct ux_component * component) {
    if (ux_component == 0) {
        printf("Component is null\n");
        return;
    }

    if (strlen(name) > 0xFFFF) {
        printf("Name too long\n");
        return;
    }

    switch (type) {
        case UX_DATATYPE_STRING: {
            char* buffer = malloc(buflen);
            uint32_t buflen = 5 + strlen(name) + strlen((char*)data);
            char * str = (char*)data;

            uint64_t str_len = strlen(str);
            if (str_len > 0xFFFF) {
                printf("String too long\n");
                free(buffer);
                return;
            }

            str_len = strlen(name);
            memset(buffer, 0, buflen);
            buffer[0] = type;
            buffer[1] = str_len & 0xFF;
            buffer[2] = (str_len >> 8) & 0xFF;
            memcpy(buffer + 3, name, str_len);
            buffer[3 + str_len] = UX_SERIALIZE_SEPARETOR;
            str_len = strlen(str);
            buffer[4 + str_len] = str_len & 0xFF;
            buffer[5 + str_len] = (str_len >> 8) & 0xFF;
            memcpy(buffer + 6 + str_len, str, str_len);
            uint32_t crc = crc32_byte(buffer, buflen - 4);
            buffer[buflen - 4] = crc & 0xFF;
            buffer[buflen - 3] = (crc >> 8) & 0xFF;
            buffer[buflen - 2] = (crc >> 16) & 0xFF;
            buffer[buflen - 1] = (crc >> 24) & 0xFF;
            ux_write_bytes(component, buffer, buflen);
            free(buffer);
        }
    }
}

static void process_value(json_value* value, int depth, struct ux_component * component) {
    if (value == NULL) {
        return;
    }
    switch (value->type) {
        case json_none:
            break;
        case json_null:
            break;
        case json_object:
            process_object(value, depth+1);
            break;
        case json_array:
            process_array(value, depth+1);
            break;
        case json_integer:
            printf("int: %10ld\n", (long)value->u.integer);
            break;
        case json_double:
            printf("double: %f\n", value->u.dbl);
            break;
        case json_string:
            printf("string: %s\n", value->u.string.ptr);
            break;
        case json_boolean:
            printf("bool: %d\n", value->u.boolean);
            break;
    }
}

struct ux_component * create_window_component(char * json, uint64_t jsonsize) {
    if (window_count >= UX_MAX_WINDOWS) {
        printf("Max windows reached\n");
        return 0;
    }
    struct ux_component * window = &windows[window_count++];
    json_parse_cb(json, jsonsize);

    json_char * json_c = (json_char*)json;
    json_value * value = json_parse(json_c, jsonsize);

    if (value == 0) {
        printf("Failed to parse json\n");
        return 0;
    }

    //Process the json

    json_value_free(value);
    return window;
}

struct ux_component * create_window(char * path) {
    int fd = vfs_file_open(path, 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", path);
        return 0;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    char* buf = malloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    struct ux_component * window = create_window_component(buf, size);
    
    free(buf);

    free(path);

    return window;
}

uint8_t find_logo_cb(struct vfs_mount* mount, void * data) {
    if (!strcmp(mount->fst->name, "EXT2")) {
        memset(data, 0, 1024);
        strcpy((char*)data, mount->partition->name);
        strcat((char*)data, "/");
        return (vfs_file_search("logo.ppm", data));
    }
    return 0;
}

uint8_t find_resources_cb(struct vfs_mount* mount, void * data) {
    if (!strcmp(mount->fst->name, "EXT2")) {
        memset(data, 0, 1024);
        strcpy((char*)data, mount->partition->name);
        strcat((char*)data, "/");
        return (vfs_file_search(".resources.json", data));
    }
    return 0;
}

uint8_t find_terminal_cb(struct vfs_mount* mount, void * data) {
    if (!strcmp(mount->fst->name, "EXT2")) {
        memset(data, 0, 1024);
        strcpy((char*)data, mount->partition->name);
        strcat((char*)data, "/");
        return (vfs_file_search("terminal.json", data));
    }
    return 0;
}

void spawn_tty() {
    char * path = malloc(1024);
    memset(path, 0, 1024);
    if (!iterate_mounts(find_terminal_cb, (void*)path)) {
        printf("Failed to find resources\n");
        return;
    }

    printf("Found terminal.json at %s\n", path);
    struct ux_component * window = create_window(path);
    if (window == 0) {
        printf("Failed to create window\n");
        return;
    }

    //Display the window
}

void print_bg() {
    char * path = malloc(1024);
    memset(path, 0, 1024);
    if (iterate_mounts(find_logo_cb, (void*)path)) {
        printf("Found logo at %s\n", path);
        draw_image(path, 0, 0);
    } else {
        printf("Failed to find resources\n");
    }
    free(path);
}