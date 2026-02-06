#ifndef UI_H
#define UI_H

#include "../include/states.h"

// TODO: prob. para el manejo de los params de simulacion deba tener algun struct como UIState
// o algo asi, y en el update actualizar esos params

void ui_handle(SimulationParams *simulation_params, UIActions *ui_actions, Rectangle *panel_rect);




#endif
