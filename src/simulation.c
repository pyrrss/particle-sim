#include "../include/simulation.h"
#include "../include/particle.h"
#include "../include/wall.h"
#include "../include/spatial_partitioning.h"

void sim_render_particles(SimulationState *simulation_state)
{
    for (int i = 0; i < simulation_state->particle_count; i++) 
    {
        Particle *p = &simulation_state->particles[i];
        particle_render(p);
    }
}

void sim_render_walls(SimulationState *simulation_state)
{
    for (int i = 0; i < simulation_state->wall_count; i++) 
    {
        Wall *w = &simulation_state->walls[i];
        wall_render(w);
    }

}

void sim_update(SimulationState *simulation_state, UIActions *ui_actions)
{
    // se manejan acciones de UI
    if (ui_actions->clear_particles)
    {
        sim_clear_particles(simulation_state);
        ui_actions->clear_particles = FALSE;
    }

    if (ui_actions->clear_walls)
    {
        sim_clear_walls(simulation_state);
        ui_actions->clear_walls = FALSE;
    }

    for (int i = 0; i < simulation_state->particle_count; i++) 
    {
        Particle *p = &simulation_state->particles[i];

        particle_update(p, &simulation_state->simulation_params);
    }
    
    sp_update(simulation_state->sp, simulation_state);
}

void sim_handle_collisions(SimulationState *simulation_state)
{
    collisions_handle(simulation_state);
}

void sim_clear_particles(SimulationState *simulation_state)
{
    // NOTE: basta con esto porque se van sobreescribiendo las partículas en el array
    // al setear particle count a 0
    simulation_state->particle_count = 0;
}

void sim_clear_walls(SimulationState *simulation_state)
{
    for (int i = 0; i < simulation_state->wall_count; i++)
    {
        Wall *w = &simulation_state->walls[i];
        wall_free(w);
    }
    
    simulation_state->wall_count = 0;
    simulation_state->segment_count = 0;
    int cell_count = simulation_state->sp->grid_width * simulation_state->sp->grid_height;
    for (int i = 0; i < cell_count; i++)
    {
        simulation_state->sp->segment_head[i] = -1; // se vacían celdas de segmentos
    }
    simulation_state->sp->reference_count = 0;
}
