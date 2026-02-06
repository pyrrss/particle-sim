#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#include "states.h"
#include "defines.h"

typedef struct Particle
{
    Vector2 position;
    Vector2 previous_position; // para verlet integration
    Vector2 velocity;
    float radius;
    float max_speed; // para limitar vel. max. y gradiente de colores
    int index; // índice de partícula en el array global

    float restitution_coef; // coeficiente de restitución
    float mu; // coeficiente de fricción

    Color color; // por si no quiero gradiente
    
    Color start_color; // color de inicio para gradiente
    Color end_color; // color de fin para gradiente

} Particle;

void particle_spawn(SimulationState* simulation_state, Vector2 position);
void particle_update(Particle* p, SimulationParams* params);
void particle_render(Particle* p);


#endif
