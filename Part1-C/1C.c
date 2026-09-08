#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define G 9.81

// Escort ship types structure
typedef struct {
    char notation[3];
    char name[30];
    char gun_name[30];
    double impact_power;
    double angle_range;
    double min_angle;
    double min_velocity;
    double max_velocity;
} EscortType;

// Escort ship instance on canvas
typedef struct {
    int id;
    int type_idx;
    double x, y;
    int destroyed;
} EscortShip;

// Battleship structure
typedef struct {
    char type;
    char name[30];
    char gun_name[30];
    double max_velocity;
    double x, y;
    double health;
} Battleship;

// Database of Escort Types (Table 1 in spec)
EscortType escort_types[5] = {
    {"EA", "1936A-class Destroyer", "SK C/34 naval gun", 0.08, 20.0, 0, 0, 0},
    {"EB", "Gabbiano-class Corvette", "L/47 dual-purpose gun", 0.06, 30.0, 0, 0, 0},
    {"EC", "Matsu-class Destroyer", "Type 89 dual-purpose gun", 0.07, 25.0, 0, 0, 0},
    {"ED", "F-class Escort Ships", "SK C/32 naval gun", 0.05, 50.0, 0, 0, 0},
    {"EE", "Japanese Kaibokan", "(4.7 inch) naval guns", 0.04, 70.0, 0, 0, 0}
};

// Helper function to calculate distance between two points
double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Check if target is within Battleship attack range (using projectile equation)
int is_in_range_B(double dist, double v_max) {
    double max_range = (v_max * v_max) / G; // max range occurs at 45 degrees
    return dist <= max_range;
}

// Calculate time of flight for Battleship shell
double calculate_time_of_flight(double dist, double v_max) {
    double sin_2theta = (dist * G) / (v_max * v_max);
    if (sin_2theta > 1.0) sin_2theta = 1.0;
    
    double theta = 0.5 * asin(sin_2theta); // choose lower angle
    double u_x = v_max * cos(theta);
    return dist / u_x;
}

int main() {
    int seed, canvas_size, N, k;
    char b_type;
    double vmax_b_input;

    // User Setups
    printf("[SETUP] Enter random seed: ");
    if (scanf("%d", &seed) != 1) return 1;
    srand(seed);

    printf("[SETUP] Enter canvas size D: ");
    scanf("%d", &canvas_size);

    printf("[SETUP] Enter number of escort ships N (1-100): ");
    scanf("%d", &N);

    printf("[SETUP] Enter number of path points k (1-20): ");
    scanf("%d", &k);

    printf("\nBattleship types:\n");
    printf("  U-USS Iowa  M-King George V  R-Richelieu  S-Sovetsky\n");
    printf("[SETUP] Choose type (U/M/R/S): ");
    scanf(" %c", &b_type);

    printf("[SETUP] Enter Vmax_B (0=random): ");
    scanf("%lf", &vmax_b_input);

    // Initialize Battleship
    Battleship B;
    B.type = b_type;
    B.health = 100.0;
    if (vmax_b_input == 0) {
        B.max_velocity = 100.0 + ((double)rand() / RAND_MAX) * 100.0; // Random Vmax
    } else {
        B.max_velocity = vmax_b_input;
    }
    printf("--> Vmax_B = %.2f m/s\n\n", B.max_velocity);

    // Set properties for Escort types based on Vmax_B
    for (int i = 0; i < 5; i++) {
        if (i == 0) { // EA
            escort_types[i].max_velocity = 1.2 * B.max_velocity;
        } else {
            escort_types[i].max_velocity = ((double)rand() / RAND_MAX) * B.max_velocity;
        }
        escort_types[i].min_velocity = ((double)rand() / RAND_MAX) * escort_types[i].max_velocity;
        escort_types[i].min_angle = ((double)rand() / RAND_MAX) * (90.0 - escort_types[i].angle_range);
    }

    // Populate canvas with Escort Ships
    EscortShip escorts[100];
    for (int i = 0; i < N; i++) {
        escorts[i].id = i;
        escorts[i].type_idx = rand() % 5;
        escorts[i].x = ((double)rand() / RAND_MAX) * canvas_size;
        escorts[i].y = ((double)rand() / RAND_MAX) * canvas_size;
        escorts[i].destroyed = 0;
    }

    // Generate k path points for Battleship
    double path_x[20], path_y[20];
    for (int i = 0; i < k; i++) {
        path_x[i] = ((double)rand() / RAND_MAX) * canvas_size;
        path_y[i] = ((double)rand() / RAND_MAX) * canvas_size;
    }

    // Save Initial Conditions to File
    FILE *f_init = fopen("Initial_Conditions.txt", "w");
    if (f_init) {
        fprintf(f_init, "Seed: %d\nCanvas: %dx%d\nBattleship Vmax: %.2f\n", seed, canvas_size, canvas_size, B.max_velocity);
        for (int i = 0; i < N; i++) {
            fprintf(f_init, "E[%d] Type: %s Pos: (%.2f, %.2f)\n", i, escort_types[escorts[i].type_idx].notation, escorts[i].x, escorts[i].y);
        }
        fclose(f_init);
    }

    printf("==================== SIMULATION RUNNING ====================\n\n");

    int destroyed_count = 0;
    int b_sunk = 0;

    // Run Simulation over k path points
    for (int step = 0; step < k; step++) {
        if (b_sunk) break;

        B.x = path_x[step];
        B.y = path_y[step];

        printf("=== Step %d | B at (%.2f,%.2f) | Health=%.1f%% ===\n", step + 1, B.x, B.y, B.health);

        // Escort ships attack B if they are active and in range
        for (int i = 0; i < N; i++) {
            if (!escorts[i].destroyed) {
                double dist = calculate_distance(B.x, B.y, escorts[i].x, escorts[i].y);
                
                // Simplified check: if within max reach
                double e_max_range = (pow(escort_types[escorts[i].type_idx].max_velocity, 2)) / G;
                if (dist <= e_max_range) {
                    double damage = escort_types[escorts[i].type_idx].impact_power * 100.0;
                    B.health -= damage;
                    if (B.health < 0) B.health = 0;

                    printf("E[%d] (%s) hit B! Damage=%.0f%% | B health=%.1f%% | Cumulative Damage=%.1f%%\n",
                           i, escort_types[escorts[i].type_idx].notation, damage, B.health, 100.0 - B.health);

                    if (B.health <= 0) {
                        b_sunk = 1;
                        break;
                    }
                }
            }
        }

        if (b_sunk) break;

        // Battleship attacks remaining active Escort ships in range
        for (int i = 0; i < N; i++) {
            if (!escorts[i].destroyed) {
                double dist = calculate_distance(B.x, B.y, escorts[i].x, escorts[i].y);
                if (is_in_range_B(dist, B.max_velocity)) {
                    double t_flight = calculate_time_of_flight(dist, B.max_velocity);
                    escorts[i].destroyed = 1;
                    destroyed_count++;
                    printf("B hit E[%d] (%s) dist=%.1f t=%.3fs\n", 
                           i, escort_types[escorts[i].type_idx].notation, dist, t_flight);
                }
            }
        }
        printf("\n");
    }

    // Output Final Results
    if (b_sunk) {
        printf("RESULT: B DESTROYED!\n");
    } else {
        printf("RESULT: B SURVIVED with %.1f%% health! (Cumulative Damage: %.1f%%)\n", B.health, 100.0 - B.health);
    }
    printf("E ships destroyed: %d / %d\n\n", destroyed_count, N);

    // Save Output Files
    FILE *f_stat = fopen("Simulation_Statistics.txt", "w");
    if (f_stat) {
        fprintf(f_stat, "Final B Health: %.2f%%\nTotal Destroyed E Ships: %d/%d\n", B.health, destroyed_count, N);
        fclose(f_stat);
    }

    FILE *f_final = fopen("Final_Conditions.txt", "w");
    if (f_final) {
        fprintf(f_final, "B Position: (%.2f, %.2f) Health: %.2f%%\n", B.x, B.y, B.health);
        for (int i = 0; i < N; i++) {
            fprintf(f_final, "E[%d] Destroyed: %s\n", i, escorts[i].destroyed ? "YES" : "NO");
        }
        fclose(f_final);
    }

    printf("[SAVED] Initial_Conditions.txt\n");
    printf("[SAVED] Simulation_Statistics.txt\n");
    printf("[SAVED] Final_Conditions.txt\n");

    return 0;
}
