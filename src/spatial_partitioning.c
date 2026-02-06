#include <stdio.h>
#include <stdlib.h>

#include "../include/spatial_partitioning.h"
#include "../include/defines.h"

void sp_init(SpatialPartitioning *sp)
{
    int cell_count = sp->grid_width * sp->grid_height;
    sp->head = malloc(sizeof(int) * cell_count);
    
    sp->next_capacity = INITIAL_PARTICLE_CAPACITY;
    sp->next = malloc(sizeof(int) * sp->next_capacity);
    
    sp->max_distance = SPATIAL_MAX_NEIGHBOR_DISTANCE;
    
    sp->segment_head = malloc(sizeof(int) * cell_count);
    sp->reference_capacity = SPATIAL_INITIAL_REFS_CAPACITY;
    sp->reference_next = malloc(sizeof(int) * sp->reference_capacity);
    sp->reference_segment = malloc(sizeof(int) * sp->reference_capacity);
    sp->reference_count = 0;

    // se inicializan las celdas como vacías
    for (int i = 0; i < cell_count; i++)
    {
        sp->head[i] = -1; // -1 indica no partículas en la celda
        sp->segment_head[i] = -1; // -1 indica no segmentos en la celda
    }

}

void sp_insert_particle(SpatialPartitioning *sp, Particle *particle)
{   
    // ejemplo de insercion para grilla 10x10 celdas de 20px de particula i en pos (80, 80)
    // se calcula indices de celda: cell_x = 80/20 = 4, cell_y = 80/20 = 4
    // particula cae en celda (4, 4) -> indice de celda en el array := 4 + 4*10 = 44
    // finalmente indice de particula se inserta en indice 44 de head

    // se calculan los índices de la celda en donde cae la partícula
    int cell_x = (int) (particle->position.x / sp->cell_size);
    int cell_y = (int) (particle->position.y / sp->cell_size);
    
    cell_x = Clamp(cell_x, 0, sp->grid_width - 1);
    cell_y = Clamp(cell_y, 0, sp->grid_height - 1);

    // se calcula indice de la celda en head
    int cell_index = cell_x + cell_y * sp->grid_width;
    
    // se inserta el índice de la partícula
    int particle_index = particle->index;
    
    if (particle_index >= sp->next_capacity)
    {
        // se redimensiona next
        int new_capacity = sp->next_capacity * 2;
        int *new_next = realloc(sp->next, sizeof(int) * new_capacity);
        
        if (new_next == NULL)
        {
            printf("Error redimensionando array next en spatial_partitioning\n");
            return;
        }

        printf("LOG: Redimensionado array next en spatial_partitioning\n");
        sp->next = new_next;
        sp->next_capacity = new_capacity;

    }

    sp->next[particle_index] = sp->head[cell_index];
    sp->head[cell_index] = particle_index;
}

void sp_insert_wall_segment(SpatialPartitioning *sp, WallSegment *segment)
{
    // se calcula 'bounding box' del segmento
    float radius = segment->thickness / 2.0f;
    float min_x = fminf(segment->start.x, segment->end.x) - radius;
    float max_x = fmaxf(segment->start.x, segment->end.x) + radius;
    float min_y = fminf(segment->start.y, segment->end.y) - radius;
    float max_y = fmaxf(segment->start.y, segment->end.y) + radius;

    // NOTE: los anteriores son rangos; es decir, se insertan en todas las celdas
    // intermedias entre min y max
    
    int cell_x_start = (int) (min_x / sp->cell_size);
    int cell_x_end = (int) (max_x / sp->cell_size);
    int cell_y_start = (int) (min_y / sp->cell_size);
    int cell_y_end = (int) (max_y / sp->cell_size);
    
    // clamp
    cell_x_start = Clamp(cell_x_start, 0, sp->grid_width - 1);
    cell_x_end = Clamp(cell_x_end, 0, sp->grid_width - 1);
    cell_y_start = Clamp(cell_y_start, 0, sp->grid_height - 1);
    cell_y_end = Clamp(cell_y_end, 0, sp->grid_height - 1);
    
    // se recorren celdas intermedias y se va insertando segmento en cada una
    for (int cell_x = cell_x_start; cell_x <= cell_x_end; cell_x++)
    {
        for (int cell_y = cell_y_start; cell_y <= cell_y_end; cell_y++)
        {
            int cell_index = cell_x + cell_y * sp->grid_width;
            int reference_index = sp->reference_count++;
            
            if (reference_index >= sp->reference_capacity)
            {
                int new_capacity = sp->reference_capacity * 2;
                int *new_reference_next = realloc(sp->reference_next, sizeof(int) * new_capacity);
                int *new_reference_segment = realloc(sp->reference_segment, sizeof(int) * new_capacity);
                
                if (new_reference_next == NULL || new_reference_segment == NULL)
                {
                    printf("Error redimensionando arrays de referencias en spatial_partitioning\n");
                    return;
                }
                
                printf("LOG: Redimensionados arrays de referencias en spatial_partitioning\n");
                sp->reference_next = new_reference_next;
                sp->reference_segment = new_reference_segment;
                sp->reference_capacity = new_capacity;
            }

            sp->reference_segment[reference_index] = segment->segment_index;
            sp->reference_next[reference_index] = sp->segment_head[cell_index];
            sp->segment_head[cell_index] = reference_index;
        }
        
    }

}

void sp_update(SpatialPartitioning *sp, SimulationState *simulation_state)
{
    // en cada frame se vacían las celdas y se reinsertan los índices de las
    // partículas

    int cell_count = sp->grid_width * sp->grid_height;
    for (int i = 0; i < cell_count; i++)
    {
        sp->head[i] = -1; // se vacían las celdas
    }

    // NOTE: no es necesario updatear next porque se sobreescriben en insert
    for (int i = 0; i < simulation_state->particle_count; i++)
    {
        Particle *particle = &simulation_state->particles[i];
        sp_insert_particle(sp, particle);
    }

}

void sp_check_particle_neighbors(SpatialPartitioning *sp, Particle *particle, SimulationState *simulation_state)
{
    // se checkean partículas en celdas vecinas 3x3 (posibles candidatos a colisión)
    int cell_x = (int) (particle->position.x / sp->cell_size);
    int cell_y = (int) (particle->position.y / sp->cell_size);
    
    // clamp
    cell_x = Clamp(cell_x, 0, sp->grid_width - 1);
    cell_y = Clamp(cell_y, 0, sp->grid_height - 1);
    
    // se recorre la región de 3x3 de celdas vecinas
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int neighbor_cell_x = cell_x + i;
            int neighbor_cell_y = cell_y + j;
            
            // se verifica validez de celda
            if (neighbor_cell_x < 0 || neighbor_cell_x >= sp->grid_width ||
                neighbor_cell_y < 0 || neighbor_cell_y >= sp->grid_height)
            {
                continue; // celda vecina fuera de límites
            }

            int neighbor_cell_index = neighbor_cell_x + neighbor_cell_y * sp->grid_width;
            int neighbor_particle_index = sp->head[neighbor_cell_index];
            
            // idea es ir recorriendo las partículas candidatas de la celda vecina y
            // hacer checkeo específico de colisión
            while (neighbor_particle_index != -1)
            {
                // skip si mismo índice o menor (para evitar hacer checkeos ya hechos previamente)
                if (neighbor_particle_index <= particle->index) { 
                    neighbor_particle_index = sp->next[neighbor_particle_index];
                    continue;
                }
                
                Particle *neighbor_particle = &simulation_state->particles[neighbor_particle_index];
                
                // NOTE: por ahora esto es redundante. seria util si el tamaño de las celdas fuera
                // bastante mas grande que el diametro maximo de particula
                // Vector2 diff = Vector2Subtract(particle->position, neighbor_particle->position);
                // float distance_sqr = Vector2LengthSqr(diff);
                // if (distance_sqr > (sp->max_distance * sp->max_distance))
                // {   
                //     // no están lo suficientemente cerca para colisionar, avanzar
                //     neighbor_particle_index = sp->next[neighbor_particle_index];
                //     continue;
                // }

                
                collisions_resolve_particle_particle(particle, neighbor_particle);

                // step
                neighbor_particle_index = sp->next[neighbor_particle_index];

            }

        }
    }


};

void sp_check_wall_segment_neighbors(SpatialPartitioning *sp, Particle *particle, SimulationState *simulation_state)
{
    int cell_x = (int) (particle->position.x / sp->cell_size);
    int cell_y = (int) (particle->position.y / sp->cell_size);

    // clamp
    cell_x = Clamp(cell_x, 0, sp->grid_width - 1);
    cell_y = Clamp(cell_y, 0, sp->grid_height - 1);
 
    // checkeo en región 3x3 circundate
    // NOTE: quiza no baste con revisar región 3x3 si muros muy gruesos,
    // si hay problemas con deteccion prob. error esté aquí
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int neighbor_cell_x = cell_x + i;
            int neighbor_cell_y = cell_y + j;

            // se verifica validez de celda
            if (neighbor_cell_x < 0 || neighbor_cell_x >= sp->grid_width ||
                neighbor_cell_y < 0 || neighbor_cell_y >= sp->grid_height)
            {
                continue; // celda vecina fuera de límites
            }

            int neighbor_cell_index = neighbor_cell_x + neighbor_cell_y * sp->grid_width;
            int reference_index = sp->segment_head[neighbor_cell_index];

            while (reference_index != -1)
            {
                int segment_index = sp->reference_segment[reference_index];
                WallSegment *segment = &simulation_state->segments[segment_index];
                
                // checkeo/resolución de colisión partícula-segmento
                collisions_resolve_particle_wall_segment(particle, segment);
                reference_index = sp->reference_next[reference_index];
            }
        }
    }
}


void sp_print_particles(SpatialPartitioning *sp)
{
    // NOTE: si hay demasiadas celdas (80k+) los printf matan el rendimiento

    int cell_count = sp->grid_width * sp->grid_height;

    for (int i = 0; i < cell_count; i++)
    {
        printf("-- Celda %d --\n", i);
        int particle_index = sp->head[i];
        while (particle_index != -1)
        {
            printf("%d -> ", particle_index);
            particle_index = sp->next[particle_index];
        }
        printf("\n");
    }
}

void sp_free(SpatialPartitioning *sp)
{
    free(sp->head);
    free(sp->next);
    free(sp->segment_head);
    free(sp->reference_next);
    free(sp->reference_segment);
}


