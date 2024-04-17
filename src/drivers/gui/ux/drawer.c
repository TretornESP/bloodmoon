#include "drawer.h"
#include "../gui.h"
#include <stdint.h>

void ux_draw_button(struct ux_component *component) {
    draw_rect(component->x, component->y, component->width, component->height, component->color);
    draw_text(component->x+(component->width/2), component->y+10, component->text, &(component->cursor));
}
void ux_draw_label(struct ux_component *component) {
    draw_text(component->x, component->y, component->text, &(component->cursor));
}
void ux_draw_image(struct ux_component *component) {
    (void)component;

}
void ux_draw_text(struct ux_component *component) {
    draw_rect(component->x, component->y, component->width, component->height, component->color);
    draw_text(component->x, component->y, component->text, &(component->cursor));
}
void ux_draw_textarea(struct ux_component *component) {
    (void)component;

}
void ux_draw_checkbox(struct ux_component *component) {
    (void)component;

}
void ux_draw_radio(struct ux_component *component) {
    (void)component;

}
void ux_draw_dropdown(struct ux_component *component) {
    (void)component;

}
void ux_draw_slider(struct ux_component *component) {
    (void)component;

}
void ux_draw_tab(struct ux_component *component) {
    (void)component;

}
void ux_draw_panel(struct ux_component *component) {
    (void)component;
}
void ux_draw_window(struct ux_component *component) {
    draw_rect_rounded_corners(component->x, component->y, component->width, component->height, component->color, 10);
    draw_rect(component->x, component->y, component->width, 2, 0x000000);
    draw_text(component->x+(component->width/2), component->y+10, component->text, &(component->cursor));
}