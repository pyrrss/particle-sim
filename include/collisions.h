#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "raylib.h"
#include "states.h"

void collisions_handle(SimulationState *simulation_state);
void collisions_resolve_particle_particle(Particle *p1, Particle *p2);
void collisions_resolve_particle_wall_segment(Particle *p, WallSegment *segment);
void collisions_resolve_particle_bounds(Particle *p, Vector3 which_bound, float bound);
void collisions_resolve_particle_mouse(Particle *p, float distance_mouse, float radius_sum_mouse, Vector2 diff_mouse);



#endif


