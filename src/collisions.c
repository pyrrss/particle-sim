#include <math.h>
#include "raylib.h"
#include "raymath.h"

#include "../include/collisions.h"
#include "../include/particle.h"
#include "../include/states.h"
#include "../include/spatial_partitioning.h"

void collisions_resolve_particle_particle(Particle *p1, Particle *p2)
{
    // -- detección de colisión --

    Vector2 diff = Vector2Subtract(p1->position, p2->position);
    float distance_sqr = Vector2LengthSqr(diff);
    float radius_sum_sqr = (p1->radius + p2->radius) * (p1->radius + p2->radius);;

    if (distance_sqr > radius_sum_sqr) { return; } // no colisión
    
    // -- resolucion de colisión --
    
    float distance = sqrt(distance_sqr);
    float radius_sum = p1->radius + p2->radius;
    float overlap = radius_sum - distance;
    Vector2 collision_normal;

    if (distance < EPSILON){ collision_normal = (Vector2) {1.0f, 0.0f}; } // normal arbitrario
    else { collision_normal = Vector2Normalize(diff); }; // vec perpendicular a la superficie de colision
                                                          // permite saber en qué dirección mover las partículas 
    
    // se calculan velocidades implícitas antes de rectificar posiciones
    Vector2 velocity_p1 = Vector2Subtract(p1->position, p1->previous_position);
    Vector2 velocity_p2 = Vector2Subtract(p2->position, p2->previous_position);
    
    // rectificación de las posiciones
    // NOTE: parece que al aplicar el mismo valor de rectificación a las posiciones 
    // previas no se altera la velocidad y se evitan 'disparos' que ocurrían al rectificar
    // solo las posiciones actuales
    p1->position.x += collision_normal.x * (overlap / 2);
    p1->position.y += collision_normal.y * (overlap / 2);
    p1->previous_position.x += collision_normal.x * (overlap / 2);
    p1->previous_position.y += collision_normal.y * (overlap / 2);
    
    p2->position.x -= collision_normal.x * (overlap / 2);
    p2->position.y -= collision_normal.y * (overlap / 2);
    p2->previous_position.x -= collision_normal.x * (overlap / 2);
    p2->previous_position.y -= collision_normal.y * (overlap / 2);
    
    Vector2 relative_velocity = Vector2Subtract(velocity_p1, velocity_p2);
    float rel_vel_normal = Vector2DotProduct(relative_velocity, collision_normal);
    
    if (rel_vel_normal >= 0.0f)
    {
        return; // partículas no se están acercando, basta con recitificar posiciones
    }

    // se separa la velocidad en componentes normal y tangencial
    float v1_n = Vector2DotProduct(velocity_p1, collision_normal);
    Vector2 v1_normal_vec = Vector2Scale(collision_normal, v1_n);
    Vector2 v1_tangential_vec = Vector2Subtract(velocity_p1, v1_normal_vec);  

    float v2_n = Vector2DotProduct(velocity_p2, collision_normal);
    Vector2 v2_normal_vec = Vector2Scale(collision_normal, v2_n);
    Vector2 v2_tangential_vec = Vector2Subtract(velocity_p2, v2_normal_vec);  

    // se aplica regla de rebote a componente normal según coeficiente de restitución
    Vector2 v1_normal_after = Vector2Scale(v1_normal_vec, -p1->restitution_coef);
    Vector2 v2_normal_after = Vector2Scale(v2_normal_vec, -p2->restitution_coef);

    // se aplica fricción a componente tangencial con coeficiente mu
    Vector2 v1_tangential_after = Vector2Scale(v1_tangential_vec, (1.0f - p1->mu));
    Vector2 v2_tangential_after = Vector2Scale(v2_tangential_vec, (1.0f - p2->mu));

    // se actualizan velocidades implícitas
    Vector2 new_velocity_p1 = Vector2Add(v1_normal_after, v1_tangential_after);
    Vector2 new_velocity_p2 = Vector2Add(v2_normal_after, v2_tangential_after);

    // se actualizan posiciones previas para reflejar nuevas velocidades
    p1->previous_position = Vector2Subtract(p1->position, new_velocity_p1);
    p2->previous_position = Vector2Subtract(p2->position, new_velocity_p2);
}

void collisions_resolve_particle_wall_segment(Particle *p, WallSegment *segment)
{
    // NOTE: si hay problemas puedo intentar resolver colisiones con mas de 1 iteracion
    // para que sea más estable/robusto
    
    // -- detección --
    // 1: detectar punto más cercano a la partícula que pertenezca al segmento
    // 2: calcular distancia entre centro de partícula y punto más cercano
    // 3: distancia < radio -> colision
    
    Vector2 tangent = Vector2Subtract(segment->end, segment->start);
    Vector2 diff_start = Vector2Subtract(p->position, segment->start);
    Vector2 closest_point;
    
    float t = Vector2DotProduct(diff_start, tangent) / Vector2DotProduct(tangent, tangent);
    t = Clamp(t, 0.0f, 1.0f); // 0 <= t <= 1; para que esté entre start y end
    closest_point = Vector2Add(segment->start, Vector2Scale(tangent, t)); // s(t) = start + t * (end - start)

    Vector2 diff = Vector2Subtract(p->position, closest_point);
    float distance_sqr = Vector2LengthSqr(diff);
    float radius_sqr = (p->radius + segment->thickness / 2.0f) * (p->radius + segment->thickness / 2.0f);
    
    if (distance_sqr > radius_sqr)
    {
        return; // no colisión
    }

    // -- colisión --
    float distance = sqrt(distance_sqr);
    float radius_total = p->radius + segment->thickness / 2.0f;
    float overlap = radius_total - distance;
    Vector2 collision_normal = Vector2Normalize(diff);

    // recificación de posición
    p->position.x += collision_normal.x * overlap;
    p->position.y += collision_normal.y * overlap;
    p->previous_position.x += collision_normal.x * overlap;
    p->previous_position.y += collision_normal.y * overlap;

    Vector2 velocity = Vector2Subtract(p->position, p->previous_position);
    float v_n = Vector2DotProduct(velocity, collision_normal);
    if (v_n >= 0.0f)
    {
        return; // partícula no va hacia segmento, solo rectificar posicion
    }

    Vector2 v_normal_vec = Vector2Scale(collision_normal, v_n);
    Vector2 v_tangential_vec = Vector2Subtract(velocity, v_normal_vec);
    
    // restitucion
    Vector2 v_normal_after = Vector2Scale(v_normal_vec, -p->restitution_coef);

    // fricción
    Vector2 v_tangential_after = Vector2Scale(v_tangential_vec, (1.0f - p->mu));

    Vector2 new_velocity = Vector2Add(v_normal_after, v_tangential_after);

    p->previous_position = Vector2Subtract(p->position, new_velocity);
}

void collisions_resolve_particle_bounds(Particle *p, Vector3 which_bound, float bound)
{
    // which_bound: { 1, 0, 0 } -> borde izquierdo
    // which_bound: { 0, 1, 0 } -> borde inferior
    // which_bound: { 0, 0, 1 } -> borde derecho

    // NOTE: habrá algún problema con las esquinas?

    Vector2 collision_normal;

    if (which_bound.x == 1) // borde izquierdo
    { 
        p->position.x = bound + p->radius;
        collision_normal = (Vector2) {1.0f, 0.0f}; 
    }

    if (which_bound.y == 1) // borde inferior
    {
        p->position.y = bound - p->radius;
        collision_normal = (Vector2) {0.0f, -1.0f}; 
    }

    if (which_bound.z == 1) // borde derecho 
    {
        p->position.x = bound - p->radius;
        collision_normal = (Vector2) {-1.0f, 0.0f}; 
    }

    Vector2 velocity = Vector2Subtract(p->position, p->previous_position);

    float v_n = Vector2DotProduct(velocity, collision_normal);
    
    if (v_n >= 0.0f)
    {
        return; // partícula no está yendo hacia el borde, solo rectificar posición
    }
    
    Vector2 v_normal_vec = Vector2Scale(collision_normal, v_n);
    Vector2 v_tangential_vec = Vector2Subtract(velocity, v_normal_vec);

    // restitucion
    Vector2 v_normal_after = Vector2Scale(v_normal_vec, -p->restitution_coef);

    // fricción
    Vector2 v_tangential_after = Vector2Scale(v_tangential_vec, (1.0f - p->mu));

    Vector2 new_velocity = Vector2Add(v_normal_after, v_tangential_after);

    p->previous_position = Vector2Subtract(p->position, new_velocity);
}

void collisions_resolve_particle_mouse(Particle *p, float distance_mouse, float radius_sum_mouse, Vector2 diff_mouse)
{
    float overlap = radius_sum_mouse - distance_mouse;
    Vector2 collision_normal = Vector2Normalize(diff_mouse); // vec perpendicular a la superficie de colision 
    // permite saber en qué dirección mover las partículas 

    // rectificación de las posiciones
    p->position.x += collision_normal.x * overlap;
    p->position.y += collision_normal.y * overlap;

    // se obtiene la velocidad implícita
    Vector2 velocity_p = Vector2Subtract(p->position, p->previous_position);

    // se separa la velocidad en componentes normal y tangencial
    float v_n = Vector2DotProduct(velocity_p, collision_normal);
    Vector2 v_normal_vec = Vector2Scale(collision_normal, v_n);
    Vector2 v_tangential_vec = Vector2Subtract(velocity_p, v_normal_vec);  

    // se aplica regla de rebote a componente normal según coeficiente de restitución
    Vector2 v_normal_after = Vector2Scale(v_normal_vec, -p->restitution_coef);

    // se aplica fricción a componente tangencial con coeficiente mu
    Vector2 v_tangential_after = Vector2Scale(v_tangential_vec, (1.0f - p->mu));

    // se actualizan velocidades implícitas
    Vector2 new_velocity_p = Vector2Add(v_normal_after, v_tangential_after);

    // se actualizan posiciones previas para reflejar nuevas velocidades
    p->previous_position = Vector2Subtract(p->position, new_velocity_p);
}

void collisions_handle(SimulationState *simulation_state)
{
    int num_particles = simulation_state->particle_count;
    Rectangle sim_rect = simulation_state->sim_rect;
    float left = sim_rect.x;
    float right = sim_rect.x + sim_rect.width;
    float bottom = sim_rect.y + sim_rect.height;
    for (int i = 0; i < num_particles; i++)
    {
        Particle *p1 = &simulation_state->particles[i];

        // colisiones con bordes de pantalla
        if (p1->position.x - p1->radius < left) // borde izquierdo
        {
            collisions_resolve_particle_bounds(p1, (Vector3) {1, 0, 0}, left);
        }

        if (p1->position.y + p1->radius > bottom) // borde inferior
        {
            collisions_resolve_particle_bounds(p1, (Vector3) {0, 1, 0}, bottom);
        }

        if (p1->position.x + p1->radius > right) // borde derecho
        {
            collisions_resolve_particle_bounds(p1, (Vector3) {0, 0, 1}, right);
        }

        // colisiones con el cursor
        // NOTE: por ahora colisiones con mouse no aportan nada
        // Vector2 mouse_pos = GetMousePosition();
        // Vector2 diff_mouse = Vector2Subtract(p1->position, mouse_pos);
        // float distance_mouse = Vector2Length(diff_mouse);
        // float radius_sum_mouse = p1->radius + 10.0f; // cursor 10 px radio aprox.
        //
        // if (distance_mouse < radius_sum_mouse)
        // {
        //     // colision con cursor
        //     collisions_resolve_particle_mouse(p1, distance_mouse, radius_sum_mouse, diff_mouse);
        // }

        // colisiones partícula-partícula
        sp_check_particle_neighbors(simulation_state->sp, p1, simulation_state);
        
        // colisiones partícula-muro (segmentos)
        sp_check_wall_segment_neighbors(simulation_state->sp, p1, simulation_state);

        // NOTE: versión n^2; la dejo para comparar rendimiento
        // for (int j = i + 1; j < num_particles; j++) 
        // {
        //     Particle *p2 = &simulation_state->particles[j];
        //
        //     // colision cuando la distancia entre centros es menor a suma de los
        //     // radios
        //     Vector2 diff = Vector2Subtract(p1->position, p2->position);
        //     float distance = Vector2Length(diff);
        //     float radius_sum = p1->radius + p2->radius;
        //
        //     if (distance > radius_sum) 
        //     {
        //         continue; // no hay colisión
        //     }
        //
        //     // colision
        //     collisions_resolve_particle_particle(p1, p2, distance, radius_sum, diff);
        //
        // }
    }
}
