#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 720

#define UI_PANEL_WIDTH_DEFAULT 400
#define UI_PANEL_BG_COLOR (Color){245, 242, 235, 255}
#define UI_FONT_SIZE 15

#define SIM_FONT_SIZE 20
#define SIM_BG_COLOR (Color){248, 250, 252, 255}

#define TRUE 1
#define FALSE 0
#define GRAVITY 512.0f
#define INITIAL_PARTICLE_CAPACITY 500

#define SOLVER_ITERS 1

#define SAND_RADIUS 10.0f
#define SAND_RESTITUTION_COEF 0.2f // normalmente para arena := 0.1f - 0.25f (o directamente 0)
#define SAND_FRICTION_COEF 0.2f   // normalmente para arena := 0.35f
#define SAND_COLOR_START (Color){241, 166, 90, 255}
#define SAND_COLOR_END (Color){46, 134, 171, 255}

#define SPATIAL_CELL_SIZE (SAND_RADIUS * 2.0f)
#define SPATIAL_MAX_NEIGHBOR_DISTANCE (SPATIAL_CELL_SIZE * 1.5f)
#define SPATIAL_INITIAL_REFS_CAPACITY 300

#define WALL_THICKNESS 10.0f // grosor de los muros; default 10.0f
#define WALL_INITIAL_CAPACITY 100 // capacidad inicial de puntos de un muro
#define WALL_POINTS_SPACING (WALL_THICKNESS * 0.3f) // distancia entre puntos al crear muros
#define WALL_COLOR (Color){44, 52, 64, 255}



