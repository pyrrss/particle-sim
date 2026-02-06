#ifndef WALL_H
#define WALL_H

#include "raylib.h"
#include "raymath.h"

#include "states.h"
#include "defines.h"

typedef struct Wall
{
    Vector2 *points; // muros se representan como polilíneas (lista de puntos)
    int count; // número de puntos
    int capacity;
    float thickness; // grosor del muro

    Color color;
} Wall;

typedef struct WallSegment
{
    Vector2 start;
    Vector2 end;
    int segment_index; // índice de segmento
    int wall_index; // índice del muro al que pertence el segmento
    float thickness;
} WallSegment;

void wall_init(Wall *wall);
void wall_add_point(Wall *wall, Vector2 point, SimulationState *simulation_state);
void wall_add_to_array(Wall *wall, SimulationState *simulation_state);
void wall_render(Wall *wall);
void wall_free(Wall *wall);

#endif
