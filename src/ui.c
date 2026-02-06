#include <raylib.h>
#include <stdbool.h>

#include "../include/ui.h"
#include "../include/defines.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void ui_handle(SimulationParams *simulation_params, UIActions *ui_actions, Rectangle *panel_rect)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, UI_FONT_SIZE);
    GuiPanel(*panel_rect, "Panel de control");

    // radio partículas
    const char* radius_label_text = "Radio partículas:";
    float radius_label_width = MeasureText(radius_label_text, UI_FONT_SIZE);
    Rectangle radius_label_rect = {10, 40, radius_label_width, 30};
    Rectangle radius_slider_rect = {radius_label_width + 40, 40, panel_rect->width / 2 , 30};
    GuiLabel(radius_label_rect, radius_label_text);
    GuiSlider(radius_slider_rect, "1.0", "50.0", &simulation_params->particle_radius, 1.0f, 50.0f);
    
    // coeficiente restitución partículas
    const char* restitution_label_text = "Coef. restitución:";
    float restitution_label_width = MeasureText(restitution_label_text, UI_FONT_SIZE);
    Rectangle restitution_label_rect = {10, 80, restitution_label_width, 30};
    Rectangle restitution_slider_rect = {radius_label_width + 40, 80, panel_rect->width / 2, 30};
    GuiLabel(restitution_label_rect, restitution_label_text);
    GuiSlider(restitution_slider_rect, "0.0", "1.0", &simulation_params->particle_restitution_coef, 0.0f, 1.0f);

    // coeficiente fricción partículas
    const char* friction_label_text = "Coef. fricción:";
    float friction_label_width = MeasureText(friction_label_text, UI_FONT_SIZE);
    Rectangle friction_label_rect = {10, 120, friction_label_width, 30};
    Rectangle friction_slider_rect = {radius_label_width + 40, 120, panel_rect->width / 2, 30};
    GuiLabel(friction_label_rect, friction_label_text);
    GuiSlider(friction_slider_rect, "0.0", "1.0", &simulation_params->particle_friction_coef, 0.0f, 1.0f);

    // gravedad
    const char* gravity_label_text = "Gravedad:";
    float gravity_label_width = MeasureText(gravity_label_text, UI_FONT_SIZE);
    Rectangle gravity_label_rect = {10, 160, gravity_label_width, 30};
    Rectangle gravity_slider_rect = {radius_label_width + 40, 160, panel_rect->width / 2, 30};
    GuiLabel(gravity_label_rect, gravity_label_text);
    GuiSlider(gravity_slider_rect, "0.0", "2000.0", &simulation_params->gravity, 0.0f, 2000.0f);

    // time_scale (vel. simulación)
    const char* time_scale_label_text = "Vel. simulación:";
    float time_scale_label_width = MeasureText(time_scale_label_text, UI_FONT_SIZE);
    Rectangle time_scale_label_rect = {10, 200, time_scale_label_width, 30};
    Rectangle time_scale_slider_rect = {radius_label_width + 40, 200, panel_rect->width / 2, 30};
    GuiLabel(time_scale_label_rect, time_scale_label_text);
    GuiSlider(time_scale_slider_rect, "0.1", "2.0", &simulation_params->time_scale, 0.1f, 2.0f);

    // -- botones --
    float buttons_y = panel_rect->height - 40;
    float buttons_height = 30;

    // pausa
    const char* pause_button_text = "Pausar";
    float pause_button_width = MeasureText(pause_button_text, UI_FONT_SIZE);
    Rectangle pause_button_rect = {10, buttons_y, pause_button_width + 20,buttons_height};
    GuiToggle(pause_button_rect, pause_button_text, &ui_actions->pause);
    
    // limpiar partículas
    const char* clear_button_text = "Limpiar partículas";
    float clear_button_width = MeasureText(clear_button_text, UI_FONT_SIZE);
    Rectangle clear_button_rect = {pause_button_width + 40, buttons_y, clear_button_width + 20, buttons_height};
    if (GuiButton(clear_button_rect, clear_button_text))
    {
        ui_actions->clear_particles = TRUE;
    }

    // limpiar muros
    const char* clear_walls_button_text = "Limpiar muros";
    float clear_walls_button_width = MeasureText(clear_walls_button_text, UI_FONT_SIZE);
    Rectangle clear_walls_button_rect = {pause_button_width + clear_button_width + 70, buttons_y, clear_button_width + 20, buttons_height };
    if (GuiButton(clear_walls_button_rect, clear_walls_button_text))
    {
        ui_actions->clear_walls = TRUE;
    }

}


