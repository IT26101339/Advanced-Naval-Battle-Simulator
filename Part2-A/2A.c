#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G 9.81

// Escort type definition
typedef struct {
    char notation[3];
    char name[30];
    char gun_name[30];
    double impact_power;
    double reload_time;
    double max_velocity;
} EscortType;

// Individual Escort ship instance
typedef struct {
    int id;
    int type_idx;
    double x, y;
    double dist_to_b;
    int destroyed;
} EscortShip;

// Battleship definition
typedef struct {
    char type;
    double max_velocity;
    double reload_time;
    double x, y;
    double health;
} Battleship;

// Helper function to calculate distance
double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Calculate projectile flight time
double calculate_flight_time(double dist, double v_max) {
    double sin_2theta = (dist * G) / (v_max * v_max);
    if (sin_2theta > 1.0) sin_2theta = 1.0;
    double theta = 0.5 * asin(sin_2theta);
    double u_x = v_max * cos(theta);
    return dist / u_x;
}

// Comparison function for qsort to order targets by distance (closest first)
int compare_distance(const void *a, const void *b) {
    EscortShip *shipA = (EscortShip *)a;
    EscortShip *shipB = (EscortShip *)b;
    if (shipA->dist_to_b < shipB->dist_to_b) return -1;
    if (shipA->dist_to_b > shipB->dist_to_b) return 1;
    return 0;
}

int main() {
    int seed, canvas_size, N, k;
    char b_type;
    double vmax_b_input, tb_input;

    // Default Escort Types
    EscortType escort_types[5] = {
        {"EA", "1936A-class Destroyer", "SK C/34 naval gun", 0.08, 0, 0},
        {"EB", "Gabbiano-class Corvette", "L/47 dual-purpose gun", 0.06, 0, 0},
        {"EC", "Matsu-class Destroyer", "Type 89 dual-purpose gun", 0.07, 0, 0},
        {"ED", "F-class Escort Ships", "SK C/32 naval gun", 0.05, 0, 0},
        {"EE", "Japanese Kaibokan", "(4.7 inch) naval guns", 0.04, 0, 0}
    };

    printf("***************************************************\n");
    printf("*                                                 *\n");
    printf("*     ADVANCED NAVAL BATTLE SIMULATOR             *\n");
    printf("*     SE1012 - Part 2-A                           *\n");
    printf("*     Reload Time + Attack Order Strategy         *\n");
    printf("*                                                 *\n");
    printf("***************************************************\n\n");

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
    printf("[SETUP] Choose battleship type (U/M/R/S): ");
    scanf(" %c", &b_type);

    printf("[SETUP] Enter Vmax_B (0=random): ");
    scanf("%lf", &vmax_b_input);

    Battleship B;
    B.type = b_type;
    B.health = 100.0;
    B.max_velocity = (vmax_b_input == 0) ? (50.0 + ((double)rand() / RAND_MAX) * 50.0) : vmax_b_input;
    printf("--> Vmax_B = %.2f m/s\n", B.max_velocity);

    printf("[SETUP] Enter B reload time T_B in seconds (0=random): ");
    scanf("%lf", &tb_input);
    B.reload_time = (tb_input == 0) ? (3.0 + ((double)rand() / RAND_MAX) * 2.0) : tb_input;
    printf("--> T_B = %.2f s\n\n", B.reload_time);

    printf("[SETUP] Setting reload times for escort types...\n");
    for (int i = 0; i < 5; i++) {
        escort_types[i].reload_time = 4.0 + ((double)rand() / RAND_MAX) * 7.0;
        escort_types[i].max_velocity = ((double)rand() / RAND_MAX) * B.max_velocity;
        printf("%s reload time: %.2f s\n", escort_types[i].notation, escort_types[i].reload_time);
    }

    // Populate Escorts
    EscortShip escorts[100];
    for (int i = 0; i < N; i++) {
        escorts[i].id = i;
        escorts[i].type_idx = rand() % 5;
        escorts[i].x = ((double)rand() / RAND_MAX) * canvas_size;
        escorts[i].y = ((double)rand() / RAND_MAX) * canvas_size;
        escorts[i].destroyed = 0;
    }

    // Path points
    double path_x[20], path_y[20];
    for (int i = 0; i < k; i++) {
        path_x[i] = ((double)rand() / RAND_MAX) * canvas_size;
        path_y[i] = ((double)rand() / RAND_MAX) * canvas_size;
    }

    printf("\n\n========== SIMULATION RUNNING ==========\n");
    printf("Strategy: B attacks closest enemy first\n");
    printf("B reload time: %.2f s\n\n", B.reload_time);

    for (int step = 0; step < k; step++) {
        B.x = path_x[step];
        B.y = path_y[step];

        printf("\n=== Step %d | B at (%.2f,%.2f) | Health=%.1f%% ===\n", step + 1, B.x, B.y, B.health);

        // Collect remaining active escorts
        EscortShip active_escorts[100];
        int active_count = 0;
        for (int i = 0; i < N; i++) {
            if (!escorts[i].destroyed) {
                escorts[i].dist_to_b = calculate_distance(B.x, B.y, escorts[i].x, escorts[i].y);
                active_escorts[active_count++] = escorts[i];
            }
        }

        if (active_count > 0) {
            // Sort by distance (closest first strategy)
            qsort(active_escorts, active_count, sizeof(EscortShip), compare_distance);

            printf("Attack order (closest first):");
            for (int i = 0; i < active_count; i++) {
                printf(" E[%d]", active_escorts[i].id);
            }
            printf("\n");

            // Execute attack sequence with reload delays
            double current_time = 0.0;
            for (int i = 0; i < active_count; i++) {
                current_time += B.reload_time;
                int original_idx = active_escorts[i].id;
                int t_idx = escorts[original_idx].type_idx;
                double dist = active_escorts[i].dist_to_b;
                double flight = calculate_flight_time(dist, B.max_velocity);

                escorts[original_idx].destroyed = 1;

                printf("t=%.2fs: B fires at E[%d] (%s) dist=%.1f flight=%.3fs\n",
                       current_time, original_idx, escort_types[t_idx].notation, dist, flight);
            }
        }
    }

    printf("\n\nRESULT: B SURVIVED with %.1f%% health!\n\n", B.health);

    // Save simulation results to text files

FILE *f1 = fopen("Initial_Conditions.txt", "w");
if (f1) {
    fprintf(f1, "=== INITIAL CONDITIONS ===\n");
    fprintf(f1, "Random Seed: %d\n", seed);
    fprintf(f1, "Canvas Size: %d\n", canvas_size);
    fprintf(f1, "Number of Escorts: %d\n", N);
    fprintf(f1, "Path Points: %d\n", k);
    fprintf(f1, "Battleship Type: %c\n", B.type);
    fprintf(f1, "Battleship Vmax: %.2f m/s\n", B.max_velocity);
    fprintf(f1, "Battleship Reload Time: %.2f s\n", B.reload_time);

    fprintf(f1, "\n=== ESCORT SHIPS ===\n");
    for (int i = 0; i < N; i++) {
        int t = escorts[i].type_idx;

        fprintf(f1, "E[%d] | Type: %s | Position: (%.2f, %.2f)\n",
                escorts[i].id,
                escort_types[t].notation,
                escorts[i].x,
                escorts[i].y);
    }

    fclose(f1);
}

FILE *f2 = fopen("Simulation_Statistics.txt", "w");
if (f2) {
    fprintf(f2, "=== SIMULATION STATISTICS ===\n");
    fprintf(f2, "Battleship Type: %c\n", B.type);
    fprintf(f2, "Battleship Vmax: %.2f m/s\n", B.max_velocity);
    fprintf(f2, "Battleship Reload Time: %.2f s\n", B.reload_time);
    fprintf(f2, "Number of Escorts: %d\n", N);
    fprintf(f2, "Number of Path Points: %d\n", k);

    fprintf(f2, "\n=== ESCORT RELOAD TIMES ===\n");
    for (int i = 0; i < 5; i++) {
        fprintf(f2, "%s: %.2f s\n",
                escort_types[i].notation,
                escort_types[i].reload_time);
    }

    fclose(f2);
}

FILE *f3 = fopen("Final_Conditions.txt", "w");
if (f3) {
    fprintf(f3, "=== FINAL CONDITIONS ===\n");
    fprintf(f3, "Battleship Health: %.1f%%\n", B.health);
    fprintf(f3, "Battleship Final Position: (%.2f, %.2f)\n", B.x, B.y);

    fprintf(f3, "\n=== ESCORT STATUS ===\n");
    for (int i = 0; i < N; i++) {
        fprintf(f3, "E[%d] - %s - %s\n",
                escorts[i].id,
                escort_types[escorts[i].type_idx].notation,
                escorts[i].destroyed ? "DESTROYED" : "ALIVE");
    }

    fclose(f3);
}

printf("[SAVED] Initial_Conditions.txt\n");
printf("[SAVED] Simulation_Statistics.txt\n");
printf("[SAVED] Final_Conditions.txt\n\n");

    printf("Press ENTER to exit...");
    getchar();
    getchar();

    return 0;
}
