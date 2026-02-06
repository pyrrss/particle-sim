#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/wall.h"
#include "../include/spatial_partitioning.h"

void wall_init(Wall *wall)
{
    wall->count = 0;
    wall->capacity = WALL_INITIAL_CAPACITY;
    wall->thickness = WALL_THICKNESS;
    wall->color = WALL_COLOR;

    wall->points = malloc(sizeof(Vector2) * wall->capacity);
}

void wall_add_point(Wall *wall, Vector2 point, SimulationState *simulation_state)
{
    // -- walls --
    if (wall->count >= wall->capacity)
    {
        // redimensionar array de puntos
        int new_capacity = wall->capacity * 2;
        Vector2 *new_points = realloc(wall->points, new_capacity * sizeof(Vector2));
        if (new_points == NULL)
        {
            printf("Error redimensionando array de puntos de muro\n");
            return;
        }
        
        wall->points = new_points;
        wall->capacity = new_capacity;
    }
    
    if (wall->count > 0)
    {
        Vector2 last_point = wall->points[wall->count - 1];
        float distance = Vector2Distance(last_point, point);
        if (distance < WALL_POINTS_SPACING)
        {
            return; // puntos muy cercanos, no agregar
        }
    }
    
    wall->points[wall->count] = point;
    wall->count++;
    
    // -- segments --
    // se agrega segmento a sp
    if (wall->count > 1)
    {
        SpatialPartitioning *sp = simulation_state->sp;
        WallSegment segment;
        segment.start = wall->points[wall->count - 2];
        segment.end = wall->points[wall->count - 1];
        segment.wall_index = simulation_state->wall_count - 1; // el muro ya fue agregado al array
        segment.segment_index = simulation_state->segment_count;
        segment.thickness = wall->thickness;
        
        // se agrega segmento a array de segmentos
        if (simulation_state->segment_count >= simulation_state->segment_capacity)
        {
            // redimensionar array de segmentos
            int new_capacity = simulation_state->segment_capacity * 2;
            WallSegment *new_segments = realloc(simulation_state->segments, new_capacity * sizeof(WallSegment));
            if (new_segments == NULL)
            {
                printf("Error redimensionando array de segmentos de muro\n");
                return;
            }

            simulation_state->segments = new_segments;
            simulation_state->segment_capacity = new_capacity;
        }
        
        simulation_state->segments[simulation_state->segment_count] = segment;
        simulation_state->segment_count++;

        // se agrega segmento a sp
        sp_insert_wall_segment(sp, &segment);
    
    }
    
}

void wall_add_to_array(Wall *wall, SimulationState *simulation_state)
{
    int wall_count = simulation_state->wall_count;
    int capacity = simulation_state->wall_capacity;

    if (wall_count >= capacity)
    {
        int new_capacity = capacity * 2;
        Wall *new_walls = realloc(simulation_state->walls, sizeof(Wall) * new_capacity);
        if (new_walls == NULL)
        {
            printf("Error redimensionando array de muros\n");
            return;
        }

        simulation_state->walls = new_walls;
        simulation_state->wall_capacity = new_capacity;
    }

    simulation_state->walls[wall_count] = *wall;
    simulation_state->wall_count++;
}

void wall_render(Wall* wall)
{
    Vector2 *points = wall->points;
    int count = wall->count;

    for (int i = 0; i < count; i++)
    {
        if (i == count - 1) { break; }

        Vector2 start_point = points[i];
        Vector2 end_point = points[(i + 1) % count];
        DrawLineEx(start_point, end_point, wall->thickness,wall->color);
    }

}

void wall_free(Wall *wall)
{
    free(wall->points);
}
