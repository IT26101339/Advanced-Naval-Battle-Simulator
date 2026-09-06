#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define M_PI 3.14159265358979323846
#define GRAVITY 9.81

typedef struct {
    int id;
    char type[3];
    double x, y;
    double min_v, max_v;
    int min_angle, max_angle;
    int is_destroyed;
} EscortShip;

typedef struct {
    char type;
    double x, y;
    double vmax;
} Battleship;

typedef struct {
    double x, y;
} Point;

double randomDouble(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

// Calculates min and max range for a ship based on velocity & angle bounds
void computeRangeBounds(double v_min, double v_max, double theta_min_deg, double theta_max_deg, double *r_min, double *r_max) {
    double t_min = theta_min_deg * M_PI / 180.0;
    double t_max = theta_max_deg * M_PI / 180.0;

    double r1 = (v_min * v_min * sin(2 * t_min)) / GRAVITY;
    double r2 = (v_min * v_min * sin(2 * t_max)) / GRAVITY;
    double r3 = (v_max * v_max * sin(2 * t_min)) / GRAVITY;
    double r4 = (v_max * v_max * sin(2 * t_max)) / GRAVITY;

    *r_min = r1;
    if (r2 < *r_min) *r_min = r2;
    if (r3 < *r_min) *r_min = r3;
    if (r4 < *r_min) *r_min = r4;

    *r_max = r1;
    if (r2 > *r_max) *r_max = r2;
    if (r3 > *r_max) *r_max = r3;
    if (r4 > *r_max) *r_max = r4;
}

// Checks if target is within attack range ring of source ship
int isInRange(double src_x, double src_y, double target_x, double target_y, double r_min, double r_max) {
    double dx = target_x - src_x;
    double dy = target_y - src_y;
    double dist = sqrt(dx * dx + dy * dy);
    return (dist >= r_min && dist <= r_max);
}

int main() {
    int seed, N, k;
    double D;
    Battleship B;
    EscortShip *escorts;
    Point *path;

    printf("================================================================================\n");
    printf("                    SE1012 - NAVAL BATTLE SIMULATOR (PART 1-B)\n");
    printf("                          Simulation 1: Moving Path\n");
    printf("================================================================================\n\n");

    printf("[CONFIG] Battlefield Setup\n");
    printf("  Enter random seed (any number)         : ");
    scanf("%d", &seed);
    srand(seed);

    printf("  Enter canvas size D (battlefield D x D) : ");
    scanf("%lf", &D);

    printf("  Enter number of escort ships N (1-100) : ");
    scanf("%d", &N);

    printf("  Enter number of path points k (2-20)   : ");
    scanf("%d", &k);

    escorts = (EscortShip *)malloc(N * sizeof(EscortShip));
    path = (Point *)malloc(k * sizeof(Point));

    printf("\n[CONFIG] Battleship Selection\n");
    printf("  U - USS Iowa (BB-61)              | 50-caliber Mark 7 gun\n");
    printf("  M - HMS King George V             | (356 mm) Mark VII gun\n");
    printf("  R - Richelieu                     | (15 inch) Mle 1935 gun\n");
    printf("  S - Sovetsky Soyuz-class          | (16 inch) B-37 gun\n");
    printf("  Select Type (U/M/R/S)             : ");
    scanf(" %c", &B.type);

    printf("  Enter Shell Vmax [0 = Random]     : ");
    scanf("%lf", &B.vmax);

    if (B.vmax == 0) {
        B.vmax = randomDouble(50.0, 200.0);
        printf("  Generated Vmax_B                   : %.2f m/s\n", B.vmax);
    }

    // Place escort ships
    const char* e_types[] = {"EA", "EB", "EC", "ED", "EE"};
    for (int i = 0; i < N; i++) {
        escorts[i].id = i + 1;
        strcpy(escorts[i].type, e_types[rand() % 5]);
        escorts[i].x = randomDouble(0, D);
        escorts[i].y = randomDouble(0, D);
        escorts[i].is_destroyed = 0;

        // Set type parameters according to assignment specs
        if (strcmp(escorts[i].type, "EA") == 0) {
            escorts[i].min_angle = (int)randomDouble(0, 70);
            escorts[i].max_angle = escorts[i].min_angle + 20;
            escorts[i].min_v = randomDouble(10, 40);
            escorts[i].max_v = 1.2 * B.vmax;
        } else if (strcmp(escorts[i].type, "EB") == 0) {
            escorts[i].min_angle = (int)randomDouble(0, 60);
            escorts[i].max_angle = escorts[i].min_angle + 30;
            escorts[i].min_v = randomDouble(10, 30);
            escorts[i].max_v = randomDouble(escorts[i].min_v + 5, B.vmax - 1);
        } else if (strcmp(escorts[i].type, "EC") == 0) {
            escorts[i].min_angle = (int)randomDouble(0, 65);
            escorts[i].max_angle = escorts[i].min_angle + 25;
            escorts[i].min_v = randomDouble(10, 30);
            escorts[i].max_v = randomDouble(escorts[i].min_v + 5, B.vmax - 1);
        } else if (strcmp(escorts[i].type, "ED") == 0) {
            escorts[i].min_angle = (int)randomDouble(0, 40);
            escorts[i].max_angle = escorts[i].min_angle + 50;
            escorts[i].min_v = randomDouble(10, 30);
            escorts[i].max_v = randomDouble(escorts[i].min_v + 5, B.vmax - 1);
        } else { // EE
            escorts[i].min_angle = (int)randomDouble(0, 20);
            escorts[i].max_angle = escorts[i].min_angle + 70;
            escorts[i].min_v = randomDouble(10, 30);
            escorts[i].max_v = randomDouble(escorts[i].min_v + 5, B.vmax - 1);
        }
    }

    // Generate path points
    printf("\n--------------------------------------------------------------------------------\n");
    printf("[PATH] Generating %d Waypoints for Battleship:\n", k);
    for (int i = 0; i < k; i++) {
        path[i].x = randomDouble(0, D);
        path[i].y = randomDouble(0, D);
        printf("  Waypoint %d : (%.2f, %.2f)\n", i + 1, path[i].x, path[i].y);
    }
    printf("--------------------------------------------------------------------------------\n");

    // Save Initial Conditions
    FILE *initFile = fopen("Initial_Conditions.txt", "w");
    if (initFile != NULL) {
        fprintf(initFile, "Canvas Size: %.2f x %.2f\n", D, D);
        fprintf(initFile, "Battleship: Type %c, Vmax %.2f\n", B.type, B.vmax);
        fprintf(initFile, "Path Points (k=%d):\n", k);
        for (int i = 0; i < k; i++) {
            fprintf(initFile, "  P%d: (%.2f, %.2f)\n", i + 1, path[i].x, path[i].y);
        }
        fprintf(initFile, "Escort Ships:\n");
        for (int i = 0; i < N; i++) {
            fprintf(initFile, "  E[%d]: Type %s, Pos (%.2f, %.2f), V[%.1f-%.1f], Ang[%d-%d]\n",
                    escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y,
                    escorts[i].min_v, escorts[i].max_v, escorts[i].min_angle, escorts[i].max_angle);
        }
        fclose(initFile);
    }

    printf("\n============================= SIMULATION EXECUTING =============================\n\n");

    FILE *statFile = fopen("Simulation_Statistics.txt", "w");
    int battleship_sunk = 0;
    int sunk_step = -1;
    int sunk_by_id = -1;
    char sunk_by_type[3] = "";

    double max_b_range = (B.vmax * B.vmax) / GRAVITY; // Battleship max angle = 45 deg for max range

    for (int step = 0; step < k; step++) {
        B.x = path[step].x;
        B.y = path[step].y;

        printf("[STEP %d] Battleship at Waypoint %d (%.2f, %.2f)\n", step + 1, step + 1, B.x, B.y);
        if (statFile) fprintf(statFile, "--- Step %d | B at (%.2f, %.2f) ---\n", step + 1, B.x, B.y);

        // Check if any active escort ship can sink Battleship at this point
        for (int i = 0; i < N; i++) {
            if (escorts[i].is_destroyed) continue;

            double e_r_min, e_r_max;
            computeRangeBounds(escorts[i].min_v, escorts[i].max_v, escorts[i].min_angle, escorts[i].max_angle, &e_r_min, &e_r_max);

            if (isInRange(escorts[i].x, escorts[i].y, B.x, B.y, e_r_min, e_r_max)) {
                battleship_sunk = 1;
                sunk_step = step + 1;
                sunk_by_id = escorts[i].id;
                strcpy(sunk_by_type, escorts[i].type);
                break;
            }
        }

        if (battleship_sunk) {
            printf("  Status     : ENGAGING\n");
            printf("  Outcome    : CRITICAL HIT RECEIVED\n\n");
            if (statFile) fprintf(statFile, "  Outcome: Battleship Sunk by E[%d] (%s)\n", sunk_by_id, sunk_by_type);
            break;
        }

        // Battleship attacks remaining active escort ships in range
        int hits_this_step = 0;
        for (int i = 0; i < N; i++) {
            if (escorts[i].is_destroyed) continue;

            if (isInRange(B.x, B.y, escorts[i].x, escorts[i].y, 0.0, max_b_range)) {
                escorts[i].is_destroyed = 1;
                hits_this_step++;
                if (statFile) fprintf(statFile, "  Destroyed E[%d] (%s) at (%.2f, %.2f)\n", escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y);
            }
        }

        printf("  Status     : ENGAGING\n");
        printf("  Outcome    : CLEARED %d ENEMY SHIPS\n\n", hits_this_step);
    }

    if (statFile) fclose(statFile);

    // Save Final Conditions & Display Final Outcome
    FILE *finalFile = fopen("Final_Conditions.txt", "w");
    if (battleship_sunk) {
        printf("[EVENT] !! BATTLESHIP SUNK !!\n");
        printf("  Sunk At    : Step %d\n", sunk_step);
        printf("  Sunk By    : Escort Ship E[%d]\n", sunk_by_id);
        printf("  Ship Type  : %s\n", sunk_by_type);

        if (finalFile) {
            fprintf(finalFile, "Simulation Result: Battleship Sunk\n");
            fprintf(finalFile, "Sunk at Step: %d\n", sunk_step);
            fprintf(finalFile, "Sunk by Escort Ship E[%d] of type %s\n", sunk_by_id, sunk_by_type);
        }
    } else {
        printf("[EVENT] MISSION ACCOMPLISHED\n");
        printf("  Status     : Battleship Survived All %d Waypoints\n", k);
        int remaining = 0;
        for (int i = 0; i < N; i++) {
            if (!escorts[i].is_destroyed) remaining++;
        }
        printf("  Remaining  : %d/%d Escort Ships\n", remaining, N);

        if (finalFile) {
            fprintf(finalFile, "Simulation Result: Battleship Survived\n");
            fprintf(finalFile, "Completed all %d path points successfully.\n", k);
        }
    }
    if (finalFile) fclose(finalFile);

    printf("\n--------------------------------------------------------------------------------\n");
    printf("[FILE] Logs Saved Successfully:\n");
    printf("  Initial_Conditions.txt\n");
    printf("  Simulation_Statistics.txt\n");
    printf("  Final_Conditions.txt\n");
    printf("================================================================================\n");

    printf("\nPress ENTER to exit...");
    getchar();
    getchar();

    free(escorts);
    free(path);
    return 0;
}
