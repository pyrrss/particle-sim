#ifndef STATES_H
#define STATES_H

#include "raylib.h"

typedef struct Particle Particle;
typedef struct SpatialPartitioning SpatialPartitioning;
typedef struct Wall Wall;
typedef struct WallSegment WallSegment;

typedef struct SimulationParams
{
    bool paused;
    float time_step;
    float time_scale; // factor para time_step
    float gravity;
    float particle_radius;
    float particle_restitution_coef;
    float particle_friction_coef;
} SimulationParams;

typedef struct SimulationState
{
    int running; // TRUE o FALSE
    
    int particle_count; // -> contador de partículas activas
    int particle_capacity; // -> capacidad máxima del arreglo dinámico antes de redimensionar
    Particle *particles; // -> arr dinámico de partículas
    
    int wall_count; // -> contador de muros activos
    int wall_capacity; // -> capacidad máx. de arr dinamico antes de redimensionar
    int wall_drawing; // TRUE o FALSE si dibujando muro
    Wall *walls; // arr dinámico de muros

    // NOTE: un muro se compone de segmentos; los separo porque para manejo de colisiones y sp se opera
    // con los segmentos directamente; para la renderizacion es mas simple tener los muros completos
    int segment_count;
    int segment_capacity;
    WallSegment *segments; // arr dinámico de segmentos de muro

    SpatialPartitioning *sp; // -> estructura que maneja spatial part.

    Rectangle sim_rect; // rect de zona de simulación
    SimulationParams simulation_params;
} SimulationState;

typedef struct UIActions
{
    // se manejan algunas acciones de UI como limpiar/pausar
    // (modificación de params de sim. se hace directo desde ui)

    bool clear_particles;
    bool clear_walls;
    bool pause;

} UIActions;

typedef struct GlobalState
{
    SimulationState simulation_state;
    UIActions ui_actions;

} GlobalState;

#endif
