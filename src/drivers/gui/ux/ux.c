#include "ux.h"
#include "psf1.h"
#include "drawer.h"
#include "../gui.h"
#include "../../../memory/heap.h"
#include "../../../vfs/vfs_interface.h"
#include "../../../vfs/vfs.h"
#include "../../../util/printf.h"
#include "../../../util/string.h"
#include "../../../util/json.h"
#include "../../../util/json_parse.h"
#include "../../../net/crc.h"

uint8_t startx_started = 0;
struct ux_component windows[UX_MAX_WINDOWS] = {0};
uint8_t window_count = 0;
struct psf1_header * font_buffer = 0;
struct event_subscriptors * subscriptors = 0;

void draw_tree(struct ux_component * root, uint16_t depth) {
    if (root == 0) {
        return;
    }

    switch(root->type) {
        case 0:
            ux_draw_button(root);
            break;
        case 1:
            ux_draw_label(root);
            break;
        case 2:
            ux_draw_image(root);
            break;
        case 3:
            ux_draw_text(root);
            break;
        case 4:
            ux_draw_textarea(root);
            break;
        case 5:
            ux_draw_checkbox(root);
            break;
        case 6:
            ux_draw_radio(root);
            break;
        case 7:
            ux_draw_dropdown(root);
            break;
        case 8:
            ux_draw_slider(root);
            break;
        case 9:
            ux_draw_tab(root);

            for (int x = 0; x < root->children_count; x++) {
                draw_tree(root->children[x], depth+1);
            }

            break;
        case 10:
            ux_draw_panel(root);
            
            for (int x = 0; x < root->children_count; x++) {
                draw_tree(root->children[x], depth+1);
            }

            break;
        case 11:
            ux_draw_window(root);

            for (int x = 0; x < root->children_count; x++) {
                draw_tree(root->children[x], depth+1);
            }

            break;
    }
}

void print_tree(struct ux_component * root, uint16_t depth) {
    if (root == 0) {
        return;
    }
    for (int x = 0; x < depth; x++) {
        printf("  ");
    }
    switch(root->type) {
        case 0:
            printf("BTN");
            break;
        case 1:
            printf("LBL");
            break;
        case 2:
            printf("IMG");
            break;
        case 3:
            printf("TXT");
            break;
        case 4:
            printf("TAR");
            break;
        case 5:
            printf("CHK");
            break;
        case 6:
            printf("RAD");
            break;
        case 7:
            printf("DRP");
            break;
        case 8:
            printf("SLD");
            break;
        case 9:
            printf("TAB");
            break;
        case 10:
            printf("PAN");
            break;
        case 11:
            printf("WIN");
            break;
    }
    printf(" [NAME:%s TEXT:%s CHILDS:%d] [X:%d Y:%d W:%d H:%d COLOR: %x COORDTYPE:", root->name, root->text, root->children_count, root->x, root->y, root->width, root->height, root->coord_type, root->color);
    switch (root->coord_type) {
        case 0:
            printf("ABS]\n");
            break;
        case 1:
            printf("REL]\n");
            break;
        case 2:
            printf("PER]\n");
            break;
    }

    //Iterate linked list 

    struct ux_specific_data * head = root->specific_data;
    while (head->next != 0) {
        for (int x = 0; x < depth; x++) {
            printf("  ");
        }

        switch (head->type) {
            case UX_DATATYPE_INT:
                printf("%s: %d\n", head->name, head->int_data);
                break;
            case UX_DATATYPE_DOUBLE:
                printf("%s: %lf\n", head->name, head->double_data);
                break;
            case UX_DATATYPE_STRING:
                printf("%s: %s\n", head->name, head->string_data);
                break;
            case UX_DATATYPE_BOOL:
                printf("%s: %d\n", head->name, head->bool_data);
                break;
        }
        head = head->next;
    }

    if (root->children_count > 0) {
        for (int x = 0; x < depth; x++) {
            printf("  ");
        }
        printf("{\n");
        for (int x = 0; x < root->children_count; x++) {
            print_tree(root->children[x], depth+1);
        }
        for (int x = 0; x < depth; x++) {
            printf("  ");
        }
        printf("}\n");
    }
}

void dispatch_tree(struct ux_component * root, struct ux_event * event) {
    if (root == 0) {
        return;
    }

    if (root->has_focus) {
        if (root->event) {
            root->event(root, event);
        }
    }

    for (int x = 0; x < root->children_count; x++) {
        dispatch_tree(root->children[x], event);
    }
}

struct ux_component * create_component(struct ux_component * parent, const char * name) {
    struct ux_component * component = malloc(sizeof(struct ux_component));
    memset(component, 0, sizeof(struct ux_component));
    component->parent = parent;
    memcpy(component->name, name, strlen(name));
    return component;
}

static void process_value(json_value* value, int depth, const char * vname, struct ux_component * component);

uint32_t string_to_color(char * str) {
    if (str == 0) {
        return 0;
    }

    if (str[0] != '#') {
        return 0;
    }

    uint32_t color = 0;
    for (int i = 1; i < 7; i++) {
        color <<= 4;
        if (str[i] >= '0' && str[i] <= '9') {
            color |= str[i] - '0';
        } else if (str[i] >= 'A' && str[i] <= 'F') {
            color |= str[i] - 'A' + 10;
        } else if (str[i] >= 'a' && str[i] <= 'f') {
            color |= str[i] - 'a' + 10;
        } else {
            return 0;
        }
    }

    return color;
}

static void finish_object(struct ux_component * component) {
    if (component == NULL) {
        return;
    }

    //Iterate linked list

    struct ux_specific_data * data = component->specific_data;

    while (data != 0) {
        if (!strcmp(data->name, "x")) {
            component->x = data->int_data;
        } else if (!strcmp(data->name, "y")) {
            component->y = data->int_data;
        } else if (!strcmp(data->name, "width")) {
            component->width = data->int_data;
        } else if (!strcmp(data->name, "height")) {
            component->height = data->int_data;
        } else if (!strcmp(data->name, "coord_type")) {
            component->coord_type = data->int_data;
        } else if (!strcmp(data->name, "color")) {
            component->color = string_to_color(data->string_data); 
        } else if (!strcmp(data->name, "type")) {
            component->type = data->int_data;
        } else if (!strcmp(data->name, "text")) {
            memcpy(component->text, data->string_data, strlen(data->string_data));
        }
        data = data->next;
    }
    
    component->cursor.color = 0xFFFFFFFF;
    component->cursor.x = component->x;
    component->cursor.y = component->y;
    component->cursor.x_min = component->x;
    component->cursor.x_max = component->x + component->width;
    component->cursor.y_min = component->y;
    component->cursor.y_max = component->y + component->height;
    component->cursor.font = font_buffer;
    component->cursor.font_width = font_buffer->charsize;
    component->cursor.font_height = CURSOR_FONT_HEIGHT;
}

static void process_object(json_value* value, int depth, const char * vname, struct ux_component * component) {
    if (value == NULL) {
        return;
    }
    int length = value->u.object.length;
    for (int x = 0; x < length; x++) {

        process_value(value->u.object.values[x].value, depth+1, value->u.object.values[x].name, component);
    }
}

static void process_array(json_value* value, int depth, const char * vname, struct ux_component * component) {
    if (value == NULL) {
        return;
    }
    int length = value->u.array.length;
    for (int x = 0; x < length; x++) {
        process_value(value->u.array.values[x], depth, vname, component);
    }

}

void add_specific_data(struct ux_component * component, const char * vname, uint8_t type, void * buffer, uint64_t length) {
    struct ux_specific_data * data = malloc(sizeof(struct ux_specific_data));
    memset(data, 0, sizeof(struct ux_specific_data));

    if (strlen(vname) > UX_MAX_SPECIFIC_DATA_NAME) {
        printf("Specific data name too long\n");
        return;
    }

    memcpy(data->name, vname, strlen(vname));
    data->type = type;
    switch (type) {
        case UX_DATATYPE_INT:
            data->int_data = *((int*)buffer);
            break;
        case UX_DATATYPE_DOUBLE:
            data->double_data = *((double*)buffer);
            break;
        case UX_DATATYPE_STRING:
            memcpy(data->string_data, buffer, length);
            break;
        case UX_DATATYPE_BOOL:
            data->bool_data = *((uint8_t*)buffer);
            break;
    }

    if (component->specific_data == 0) {
        component->specific_data = data;
    } else {
        struct ux_specific_data * head = component->specific_data;
        while (head->next != 0) {
            head = head->next;
        }
        head->next = data;
    }
    component->specific_data_items++;
}

static void process_value(json_value* value, int depth, const char * vname, struct ux_component * component) {
    if (value == NULL) {
        return;
    }
    switch (value->type) {
        case json_none:
            break;
        case json_null:
            break;
        case json_object:
            struct ux_component * target;
            if (depth == 0) {
                target = component;
            } else {
                target = create_component(component, vname);
                component->children[component->children_count++] = target;
            }
            process_object(value, depth+1, vname, target);
            finish_object(target);
            break;
        case json_array:
            process_array(value, depth+1, vname, component);
            break;
        case json_integer:
            add_specific_data(component, vname, UX_DATATYPE_INT, &value->u.integer, sizeof(int));
            break;
        case json_double:
            add_specific_data(component, vname, UX_DATATYPE_DOUBLE, &value->u.dbl, sizeof(double));
            break;
        case json_string:
            add_specific_data(component, vname, UX_DATATYPE_STRING, value->u.string.ptr, value->u.string.length);
            break;
        case json_boolean:
            add_specific_data(component, vname, UX_DATATYPE_BOOL, &value->u.boolean, sizeof(uint8_t));
            break;
    }
}

uint8_t collides(struct ux_component * component, uint16_t x, uint16_t y) {
    if (x >= component->x && x <= component->x + component->width && y >= component->y && y <= component->y + component->height) {
        return 1;
    }
    return 0;
}

void set_component_focus(struct ux_component* ux_component, uint16_t depth, uint16_t x, uint16_t y) {
    if (ux_component == 0) {
        return;
    }
    if (collides(ux_component, x, y)) {
        ux_component->parent->has_focus = 0;
        ux_component->has_focus = 1;
        for (int i = 0; i < ux_component->children_count; i++) {
            set_component_focus(ux_component->children[i], depth+1, x, y);
        }
    } else {
        ux_component->has_focus = 0;
    }
}

void set_window_focus(uint16_t x, uint16_t y) {
    for (int i = 0; i < window_count; i++) {
        struct ux_component * window = &windows[i];
        if (collides(window, x, y)) {
            set_component_focus(window, 0, x, y);
            //If we are already in focus, do nothing
            if (i == 0) {
                return;
            }
            //Else we move the new window to the front
            for (int j = i; j > 0; j--) {
                windows[j] = windows[j-1];
            }
            windows[0] = *window;
            break;
        }
    }
}

void set_window_focus_explicit(struct ux_component* window) {
    window->has_focus = 1;

    for (int i = 0; i < window_count; i++) {
        if (&windows[i] == window) {
            if (i == 0) {
                return;
            }
            for (int j = i; j > 0; j--) {
                windows[j] = windows[j-1];
            }
            windows[0] = *window;
            break;
        }
    }

    for (int i = 1; i < window_count; i++) {
        windows[i].has_focus = 0;
    }
}

struct ux_component* get_window_focus() {
    return &windows[0];
}

struct ux_component * create_window_component(const char* name, char * json, uint64_t jsonsize) {
    if (window_count >= UX_MAX_WINDOWS) {
        printf("Max windows reached\n");
        return 0;
    }
    struct ux_component * window = &windows[window_count++];
    memset(window, 0, sizeof(struct ux_component));
    memcpy(window->name, name, strlen(name));

    json_char * json_c = (json_char*)json;
    json_settings *settings = malloc(sizeof(json_settings));
    memset(settings, 0, sizeof(json_settings));
    json_value * value = json_parse_ex(settings, json_c, jsonsize, 0);

    if (value == 0) {
        printf("Failed to parse json\n");
        return 0;
    }

    process_value(value, 0, "root", window);
    free(settings);
    json_value_free(value);
    return window;
}

struct ux_component * create_window(const char * name, char * path) {
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

    struct ux_component * window = create_window_component(name, buf, size);
    
    free(buf);
    free(path);

    return window;
}

struct ux_component* get_component_by_name(struct ux_component * root, const char* name) {
    if (root == 0) {
        return 0;
    }

    if (!strcmp(root->name, name)) {
        return root;
    }

    for (int i = 0; i < root->children_count; i++) {
        struct ux_component * component = get_component_by_name(root->children[i], name);
        if (component != 0) {
            return component;
        }
    }

    return 0;
}

void terminal_handler(struct ux_component * component, struct ux_event * event) {
    printf("[COM %s EVT: %x][X:%d|Y:%d|DAT:%x]\n", component->name, event->device, event->x, event->y, event->data);
}

void button_handler(struct ux_component * component, struct ux_event * event) {
    printf("[COM %s EVT: %x][X:%d|Y:%d|DAT:%x]\n", component->name, event->device, event->x, event->y, event->data);
}

uint8_t find_file_cb(struct vfs_mount* mount, void * data, void* searchstr) {
    if (!strcmp(mount->fst->name, "EXT2")) {
        memset(data, 0, 1024);
        strcpy((char*)data, mount->partition->name);
        strcat((char*)data, "/");
        return (vfs_file_search((const char*)searchstr, data));
    }
    return 0;
}

struct ux_component* inflate_layout(const char * title, const char * layout) {
    char * path = malloc(1024);
    memset(path, 0, 1024);
    if (!iterate_mounts(find_file_cb, (void*)path, (void*)layout)) {
        printf("Failed to find resources\n");
        return 0;
    }

    struct ux_component * window = create_window(title, path);
    if (window == 0) {
        printf("Failed to create window\n");
        return 0;
    }

    set_window_focus_explicit(window);
    return window;
}

void redraw_component(struct ux_component * window) {
    draw_tree(window, 0);
}

uint8_t find_font_cb(struct vfs_mount* mount, void * data) {
    if (!strcmp(mount->fst->name, "EXT2")) {
        memset(data, 0, 1024);
        strcpy((char*)data, mount->partition->name);
        strcat((char*)data, "/");
        return (vfs_file_search("zap-light16.psf", data));
    }
    return 0;
}

void hook_event(struct ux_component* component, void (*event)(struct ux_component* component, struct ux_event * event)) {
    component->event = event;
}

void unhook_event(struct ux_component* component) {
    component->event = 0;
}

void event_dispatch(struct ux_event * event) {
    //Check if the event is a left click, if so change focus
    if (event->device == UX_EVENT_DEVICE_MOUSE && event->data & UX_EVENT_MOUSE_LEFT_MASK) {
        printf("Changing focus\n");
        set_window_focus(event->x, event->y);
    }

    struct ux_component * focus = get_window_focus();
    if (focus != 0) {
        dispatch_tree(focus, event);
    }

    event->processed = 1;
}

void load_font() {
    printf("Searching for font\n");
    char * path = malloc(1024);
    memset(path, 0, 1024);
    if (!iterate_mounts(find_file_cb, (void*)path, "zap-light16.psf")) {
        printf("Failed to find resources\n");
        return;
    }

    printf("Found font at %s\n", path);

    int fd = vfs_file_open(path, 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", path);
        return 0;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    font_buffer = malloc(size);
    memset(font_buffer, 0, size);
    vfs_file_read(fd, font_buffer, size);
    vfs_file_close(fd);
    free(path);

    if (font_buffer == 0) {
        printf("Failed to load font\n");
        return;
    }
    if (font_buffer->magic[0] != 0x36 || font_buffer->magic[1] != 0x04) {
        printf("Invalid font magic %x %x\n", font_buffer->magic[0], font_buffer->magic[1]);
        free(font_buffer);
        font_buffer = 0;
        return;
    }
}

void print_bg() {
    char * path = malloc(1024);
    memset(path, 0, 1024);
    if (iterate_mounts(find_file_cb, (void*)path, "logo.ppm")) {
        printf("Found logo at %s\n", path);
        draw_image(path, 0, 0);
    } else {
        printf("Failed to find resources\n");
    }
    free(path);
}