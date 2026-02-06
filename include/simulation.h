#ifndef SIMULATION_H
#define SIMULATION_H

#include "states.h"

void sim_render_particles(SimulationState *simulation_state);
void sim_render_walls(SimulationState *simulation_state);
void sim_update(SimulationState *simulation_state, UIActions *ui_actions);
void sim_handle_collisions(SimulationState *simulation_state);
void sim_clear_particles(SimulationState *simulation_state);
void sim_clear_walls(SimulationState *simulation_state);

#endif
