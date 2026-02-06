#include "../include/particle.h"
#include "../include/defines.h"
#include "../include/spatial_partitioning.h"

void particle_spawn(SimulationState* simulation_state, Vector2 position)
{
    Particle* particles = simulation_state->particles;
    int particle_count = simulation_state->particle_count;
    int particle_capacity = simulation_state->particle_capacity;

    // -> agregar partícula
    if (particle_count >= particle_capacity)
    {
        // redimensionar array
        int new_capacity = particle_capacity * 2;
        particles = realloc(particles, new_capacity * sizeof(Particle));
        
        if (particles == NULL)
        {
            printf("Error redimensionando array de partículas\n");
            return;
        }

        printf("LOG: Redimensionado array de partículas\n");
        
        simulation_state->particles = particles;
        simulation_state->particle_capacity = new_capacity;
    }

    // NOTE: por ahora se agrega solo 'arena'
    Particle* new_particle = &particles[particle_count]; // se toma el slot libre al final del array

    new_particle->position = position;
    new_particle->previous_position = new_particle->position;
    new_particle->velocity = (Vector2) { 0.0f, 0.0f };
    new_particle->max_speed = 600.0f;
    new_particle->index = particle_count;
    new_particle->radius = SAND_RADIUS;
    new_particle->restitution_coef = SAND_RESTITUTION_COEF;
    new_particle->mu = SAND_FRICTION_COEF;
    new_particle->color = SAND_COLOR_START;
    new_particle->start_color = new_particle->color;
    new_particle->end_color = SAND_COLOR_END;

    simulation_state->particle_count++;
    
}

void particle_update(Particle* p, SimulationParams* params)
{
    // se actualizan parametros de particula en base a params de simulación
    float delta_time = params->time_step * params->time_scale;
    float gravity = params->gravity;
    p->radius = params->particle_radius;
    p->restitution_coef = params->particle_restitution_coef;
    p->mu = params->particle_friction_coef;
    
    // verlet := pos = 2pos - prev_pos + a * dt^2
    
    // NOTE: por ahora la única fuerza que interactúa es la gravedad, luego extender si necesario
    Vector2 acceleration = { 0.0f, gravity };
    Vector2 previous_position = p->position;
    
    // se actualiza posición usando verlet 
    p->position.x = 2 * p->position.x - p->previous_position.x + acceleration.x * delta_time * delta_time;
    p->position.y = 2 * p->position.y - p->previous_position.y + acceleration.y * delta_time * delta_time;
    
    // se actualiza previous_position para siguiente update
    p->previous_position = previous_position;
    
    // SIN VERLET := actualizando directamente con velocidad
    // p->position.x += p->velocity.x * delta_time;
    // p->position.y += p->velocity.y * delta_time;

    // color cambia con el tiempo (gradiente constante)
    float time = (float) GetTime();
    float t = (sinf(time) + 1.0f) * 0.5f;

    // interpolacion lineal de color
    p->color = (Color) {
        (unsigned char) Lerp(p->start_color.r, p->end_color.r, t),
        (unsigned char) Lerp(p->start_color.g, p->end_color.g, t),
        (unsigned char) Lerp(p->start_color.b, p->end_color.b, t),
        255
    };

}

void particle_render(Particle* p)
{
    // NOTE: las partículas se tratan internamente como círculos
    // pero se renderizan como cuadrados para rendimiento;
    // hacer esto mejoró MUCHO el rendimiento

    float k = 0.9f; // factor de escala
    int side = (int) (2 * p->radius * k);
    DrawRectangle(
        (int) (p->position.x - p->radius * k),
        (int) (p->position.y - p->radius * k),
        side,
        side,
        p->color
    );

}





