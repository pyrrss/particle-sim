#ifndef SPATIAL_PARTITIONING_H
#define SPATIAL_PARTITIONING_H

#include "defines.h"
#include "particle.h"
#include "states.h"
#include "collisions.h"
#include "wall.h"

typedef struct SpatialPartitioning
{
    int cell_size; // tamaño de cada celda
    int grid_width; // cantidad de celdas en x
    int grid_height; // cantidad de celdas en y
    
    int *head; // array donde cada valor es el indice de la partícula cabeza de cada celda
    int *next; // array donde cada valor es el indice de la siguiente partícula en misma celda
    int next_capacity; // capacidad actual de next (para redimensionar)
    
    int *segment_head; // array donde cada valor es el índice de una referencia (aparición de un segmento en una celda)
    int *reference_next; // lista enlazada de referencias
    int *reference_segment; // array que mapea cada referencia a un segmento (qué segment_id representa cada ref)
    int reference_count; // cantidad actual de referencias
    int reference_capacity;

    int max_distance; // distancia máxima para considerar partículas vecinas 

    

} SpatialPartitioning;

void sp_init(SpatialPartitioning* sp);
void sp_insert_particle(SpatialPartitioning* sp, Particle* particle);
void sp_insert_wall_segment(SpatialPartitioning* sp, WallSegment* segment);
void sp_update(SpatialPartitioning* sp, SimulationState* simulation_state);
void sp_check_particle_neighbors(SpatialPartitioning *sp, Particle *particle, SimulationState *simulation_state);
void sp_check_wall_segment_neighbors(SpatialPartitioning *sp, Particle *particle, SimulationState *simulation_state);
void sp_print_particles(SpatialPartitioning* sp);
void sp_free(SpatialPartitioning* sp);

#endif
