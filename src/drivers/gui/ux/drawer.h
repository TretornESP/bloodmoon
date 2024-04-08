#ifndef _DRAWER_H
#define _DRAWER_H
#include "ux.h"
void ux_draw_button(struct ux_component *component);
void ux_draw_label(struct ux_component *component);
void ux_draw_image(struct ux_component *component);
void ux_draw_text(struct ux_component *component);
void ux_draw_textarea(struct ux_component *component);
void ux_draw_checkbox(struct ux_component *component);
void ux_draw_radio(struct ux_component *component);
void ux_draw_dropdown(struct ux_component *component);
void ux_draw_slider(struct ux_component *component);
void ux_draw_tab(struct ux_component *component);
void ux_draw_panel(struct ux_component *component);
void ux_draw_window(struct ux_component *component);
#endif