#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include "../include/collisions.h"
#include "../include/particle.h"
#include "../include/wall.h"
#include "../include/spatial_partitioning.h"
#include "../include/states.h"
#include "../include/ui.h"
#include "../include/simulation.h"

void handle_input(GlobalState *global_state, Rectangle sim_rect)
{
    Vector2 mouse_position = GetMousePosition();
    int mouse_in_sim_rect = CheckCollisionPointRec(mouse_position, sim_rect);
    // agregar partícula en posición del mouse con click izquierdo
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && mouse_in_sim_rect) 
    {
        Vector2 mouse_position = GetMousePosition();
        // NOTE: se suma un radio aleatorio para evitar que muchas partículas
        // spawneen exactamente en la misma posición
        float random_radius = GetRandomValue(-20, 20);
        mouse_position = Vector2AddValue(mouse_position, random_radius);
        particle_spawn(&global_state->simulation_state, mouse_position);
    }

    // -- creación de muros --
    SimulationState *simulation_state = &global_state->simulation_state;
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && mouse_in_sim_rect)
    {
        // se agrega el primer punto del muro
        simulation_state->wall_drawing = TRUE;
        Wall new_wall;
        wall_init(&new_wall);
        wall_add_to_array(&new_wall, simulation_state);
    }
    
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && simulation_state->wall_drawing && mouse_in_sim_rect)
    {
        // se continúan agregando puntos al muro mientras se mantenga presionado
        Wall *current_wall = &simulation_state->walls[simulation_state->wall_count - 1];
        wall_add_point(current_wall, mouse_position, simulation_state);
    }

    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && simulation_state->wall_drawing && mouse_in_sim_rect)
    {   
        // se finaliza la creacion del muro
        simulation_state->wall_drawing = FALSE;
        // wall_add_to_array(current_wall, simulation_state);
    }
        
}

int main() 
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window haha");

    // -- texto --
    const char *welcome_msg = "Simulación de físicas 2D";
    int text_width = MeasureText(welcome_msg, SIM_FONT_SIZE);
    int welcome_msg_x = SCREEN_WIDTH / 2 - text_width / 2;
    int welcome_msg_y = SCREEN_HEIGHT / 6 - SIM_FONT_SIZE / 2;

    SetTargetFPS(60);

    // -- setup --
    SpatialPartitioning sp = {
        .cell_size = SPATIAL_CELL_SIZE,
        .grid_width = ceil((float) SCREEN_WIDTH / SPATIAL_CELL_SIZE),
        .grid_height = ceil((float) SCREEN_HEIGHT / SPATIAL_CELL_SIZE)
    };
    sp_init(&sp);

    // rect de panel y simulacion
    int panel_width = UI_PANEL_WIDTH_DEFAULT;
    Rectangle panel_rect = {0, 0, panel_width, SCREEN_HEIGHT };
    Rectangle sim_rect = {panel_width, 0, SCREEN_WIDTH - panel_width, SCREEN_HEIGHT };
  
    SimulationParams simulation_params =
    {
        .time_step = 1.0f / 60.0f,
        .time_scale = 1.0f,
        .gravity = GRAVITY,
        .particle_radius = SAND_RADIUS,
        .particle_restitution_coef = SAND_RESTITUTION_COEF,
        .particle_friction_coef = SAND_FRICTION_COEF,
    };

    GlobalState global_state = {
        .simulation_state =
        {
            .running = TRUE,
            .particle_count = 0,
            .particle_capacity = INITIAL_PARTICLE_CAPACITY,
            .particles = malloc(sizeof(Particle) * INITIAL_PARTICLE_CAPACITY), // -> espacio inicial
            .wall_count = 0,
            .wall_capacity = WALL_INITIAL_CAPACITY,
            .walls = malloc(sizeof(Wall) * WALL_INITIAL_CAPACITY), // espacio inicial para muros
            .wall_drawing = FALSE,
            .segment_count = 0,
            .segment_capacity = WALL_INITIAL_CAPACITY * 3, // espacio inicial para segmentos
            .segments = malloc(sizeof(WallSegment) * (WALL_INITIAL_CAPACITY * 3)),
            .sp = &sp,
            .sim_rect = sim_rect,
            .simulation_params = simulation_params,
        },
        .ui_actions =
        {
            .clear_particles = FALSE,
            .clear_walls = FALSE,
            .pause = FALSE,
        }
    };
        
    // -- game loop --
    while (!WindowShouldClose()) 
    {
        // input
        handle_input(&global_state, sim_rect);
        
        global_state.simulation_state.sim_rect = sim_rect; // actualizar rect de simulación

        // update
        if (!global_state.ui_actions.pause)
        {
            sim_update(&global_state.simulation_state, &global_state.ui_actions);
        }

        // DEBUG: imprimir partículas de cada celda 
        // sp_print_particles(&sp);

        // NOTE: en cada frame se pueden hacer varias iteraciones para el manejo de
        // colisiones; en teoría hace que sea mas estable pero aumenta el costo (no
        // sé hasta qué punto compensa aumentar)
        
        if (!global_state.ui_actions.pause)
        {
            for (int i = 0; i < SOLVER_ITERS; i++) 
            {
                sim_handle_collisions(&global_state.simulation_state);
            }
        }
        
        // TODO: render
        BeginDrawing();
        {
            ClearBackground(GREEN);

            // panel de control
            BeginScissorMode(panel_rect.x, panel_rect.y, panel_rect.width, panel_rect.height);
            {
                DrawRectangleRec(panel_rect, UI_PANEL_BG_COLOR);
                DrawLine(panel_width, 0, panel_width, GetScreenHeight(), DARKGRAY);
                


                // NOTE: revisar orden de instrucciones; quiza esto deba ir antes
                ui_handle(&global_state.simulation_state.simulation_params, &global_state.ui_actions, &panel_rect);
            }
            EndScissorMode();

            // simulacion
            BeginScissorMode(sim_rect.x, sim_rect.y, sim_rect.width, sim_rect.height);
            {
                // NOTE: BeginScisscorMode hace que todo lo que se renderice esté acotado al rectangulo especificado,
                // si hay algo fuera de sus límites, no se renderiza

                DrawRectangleRec(sim_rect, SIM_BG_COLOR);

                const char *fps_text = TextFormat("FPS: %d", GetFPS());
                DrawText(fps_text, sim_rect.x + 10, 10, SIM_FONT_SIZE, BLACK);
                
                int num_particles = global_state.simulation_state.particle_count;
                const char *particle_count_text = TextFormat("Número de partículas: %d", num_particles);
                DrawText(particle_count_text,  sim_rect.width, 10, SIM_FONT_SIZE, BLACK);
                
                sim_render_particles(&global_state.simulation_state);
                sim_render_walls(&global_state.simulation_state);
            }
            EndScissorMode();

        }
        EndDrawing();
        
        // float frame_time = GetFrameTime();
        // printf("Frame time: %.4f \n", frame_time);
    }

    // -- cleanup de todo --
    free(global_state.simulation_state.particles);
    
    for (int i = 0; i < global_state.simulation_state.wall_count; i++)
    {
        wall_free(&global_state.simulation_state.walls[i]);
    }

    free(global_state.simulation_state.walls);
    sp_free(&sp);

    CloseWindow();

    return 0;
}
