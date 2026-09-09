#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define M_PI 3.14159265358979323846
#define GRAVITY 9.81

typedef struct {
    char notation[3];
    char type_name[30];
    char gun_name[30];
    double impact_power;
    double angle_range;
    double min_angle;
    double min_vel;
    double max_vel;
    double reload_time; // T_E^p
} EscortType;

EscortType ESCORT_TYPES[5] = {
    {"EA", "1936A-class Destroyer", "SK C/34 naval gun", 0.08, 20.0, 0, 0, 0, 0},
    {"EB", "Gabbiano-class Corvette", "L/47 dual-purpose gun", 0.06, 30.0, 0, 0, 0, 0},
    {"EC", "Matsu-class Destroyer", "Type 89 dual-purpose gun", 0.07, 25.0, 0, 0, 0, 0},
    {"ED", "F-class Escort Ships", "SK C/32 naval gun", 0.05, 50.0, 0, 0, 0, 0},
    {"EE", "Japanese Kaibokan", "(4.7 inch) naval guns", 0.04, 70.0, 0, 0, 0, 0}
};

typedef struct {
    int id;
    int type_idx;
    double x, y;
    double min_angle, max_angle;
    double min_vel, max_vel;
    double min_range, max_range;
    double reload_time;
    double next_fire_time;
    int is_destroyed;
} EscortShip;

typedef struct {
    char notation;
    char name[30];
    char gun_name[30];
    double x, y;
    double vmax;
    double reload_time; // T_B
    double max_range;
    double hp_percent;
} Battleship;

double get_random_double(double min_val, double max_val) {
    return min_val + ((double)rand() / RAND_MAX) * (max_val - min_val);
}

double calc_max_projectile_range(double velocity) {
    return (velocity * velocity) / GRAVITY;
}

double calc_min_projectile_range(double velocity, double min_angle_deg) {
    double rad = min_angle_deg * M_PI / 180.0;
    return (velocity * velocity * sin(2.0 * rad)) / GRAVITY;
}

double calc_distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int main() {
    int seed;
    double D;
    int N;
    char b_type;
    Battleship B;

    printf("**************************************************\n");
    printf("*      ADVANCED NAVAL BATTLE SIMULATOR           *\n");
    printf("*      SE1012 - Part 2-B (Continuous Firing)     *\n");
    printf("*      World War 2 Naval Combat Simulation       *\n");
    printf("**************************************************\n\n");

    printf("[SETUP] Enter random seed (any number): ");
    if (scanf("%d", &seed) != 1) return 0;
    srand(seed);

    printf("[SETUP] Enter canvas size D (battlefield is D x D): ");
    scanf("%lf", &D);

    printf("[SETUP] Enter number of escort ships N (1-100): ");
    scanf("%d", &N);

    printf("\nAvailable battleship types:\n");
    printf("  U - USS Iowa (BB-61)      | 50-caliber Mark 7 gun\n");
    printf("  M - MS King George V      | (356 mm) Mark VII gun\n");
    printf("  R - Richelieu             | (15 inch) Mle 1935 gun\n");
    printf("  S - Sovetsky Soyuz-class  | (16 inch) B-37 gun\n");
    
    printf("[SETUP] Choose battleship type (U/M/R/S): ");
    scanf(" %c", &b_type);

    B.notation = b_type;
    if (b_type == 'U' || b_type == 'u') {
        strcpy(B.name, "USS Iowa (BB-61)");
        strcpy(B.gun_name, "50-caliber Mark 7 gun");
    } else if (b_type == 'M' || b_type == 'm') {
        strcpy(B.name, "MS King George V");
        strcpy(B.gun_name, "(356 mm) Mark VII gun");
    } else if (b_type == 'R' || b_type == 'r') {
        strcpy(B.name, "Richelieu");
        strcpy(B.gun_name, "(15 inch) Mle 1935 gun");
    } else {
        strcpy(B.name, "Sovetsky Soyuz-class");
        strcpy(B.gun_name, "(16 inch) B-37 gun");
    }

    printf("[SETUP] Enter Vmax for battleship shell\n");
    printf("        (enter 0 to randomly generate, max 200 m/s): ");
    scanf("%lf", &B.vmax);
    if (B.vmax <= 0) {
        B.vmax = get_random_double(100.0, 200.0);
        printf("--> Randomly generated Vmax_B = %.2f m/s\n", B.vmax);
    }

    printf("[SETUP] Enter Battleship reload time T_B in seconds (0 to auto-generate): ");
    scanf("%lf", &B.reload_time);
    if (B.reload_time <= 0) {
        B.reload_time = get_random_double(3.0, 6.0);
        printf("--> Generated Battleship T_B = %.2f s\n", B.reload_time);
    }

    printf("[SETUP] Enter battleship starting position as: x y\n");
    printf("        (enter -1 -1 to randomly generate): ");
    scanf("%lf %lf", &B.x, &B.y);
    if (B.x < 0 || B.y < 0) {
        B.x = get_random_double(0, D);
        B.y = get_random_double(0, D);
        printf("--> Randomly generated position = (%.2f, %.2f)\n", B.x, B.y);
    }

    B.max_range = calc_max_projectile_range(B.vmax);
    B.hp_percent = 100.0;

    printf("\n[SETUP] Firing intervals T_E^p for Escort Ship Categories:\n");
    for (int i = 0; i < 5; i++) {
        printf("Enter reload time T_E for %s (0 to auto-generate): ", ESCORT_TYPES[i].notation);
        scanf("%lf", &ESCORT_TYPES[i].reload_time);
        if (ESCORT_TYPES[i].reload_time <= 0) {
            ESCORT_TYPES[i].reload_time = get_random_double(3.0, 6.0);
            printf("--> Auto-generated T_E^%s = %.2f s\n", ESCORT_TYPES[i].notation, ESCORT_TYPES[i].reload_time);
        }
    }

    printf("\n[GENERATING] Placing %d escort ships on battlefield...\n", N);
    EscortShip *escorts = (EscortShip *)malloc(N * sizeof(EscortShip));

    for (int i = 0; i < N; i++) {
        escorts[i].id = i;
        escorts[i].type_idx = rand() % 5;
        escorts[i].x = get_random_double(0, D);
        escorts[i].y = get_random_double(0, D);
        
        EscortType ref = ESCORT_TYPES[escorts[i].type_idx];
        escorts[i].min_angle = get_random_double(10.0, 30.0);
        escorts[i].max_angle = escorts[i].min_angle + ref.angle_range;
        
        escorts[i].min_vel = get_random_double(20.0, 50.0);
        if (strcmp(ref.notation, "EA") == 0) {
            escorts[i].max_vel = 1.2 * B.vmax;
        } else {
            escorts[i].max_vel = get_random_double(escorts[i].min_vel + 10.0, B.vmax - 5.0);
        }
        
        escorts[i].min_range = calc_min_projectile_range(escorts[i].min_vel, escorts[i].min_angle);
        escorts[i].max_range = calc_max_projectile_range(escorts[i].max_vel);
        escorts[i].reload_time = ref.reload_time;
        escorts[i].next_fire_time = escorts[i].reload_time;
        escorts[i].is_destroyed = 0;
    }
    printf("[DONE] Escort ships placed.\n\n");

    FILE *f_init = fopen("Initial_Conditions.txt", "w");
    if (f_init) {
        fprintf(f_init, "Battlefield Size: %.2f x %.2f\n", D, D);
        fprintf(f_init, "Battleship: %s (%c), Pos: (%.2f, %.2f), Vmax: %.2f, T_B: %.2f\n", 
                B.name, B.notation, B.x, B.y, B.vmax, B.reload_time);
        fprintf(f_init, "Escort Ships:\n");
        for (int i = 0; i < N; i++) {
            fprintf(f_init, "E[%d] Type: %s, Pos: (%.2f, %.2f), MinA: %.2f, MaxA: %.2f, MinV: %.2f, MaxV: %.2f, T_E: %.2f\n",
                    i, ESCORT_TYPES[escorts[i].type_idx].notation, escorts[i].x, escorts[i].y,
                    escorts[i].min_angle, escorts[i].max_angle, escorts[i].min_vel, escorts[i].max_vel, escorts[i].reload_time);
        }
        fclose(f_init);
        printf("[SAVED] Initial_Conditions.txt\n\n");
    }

    printf("============ SIMULATION RUNNING (PART 2-B) ============\n");
    FILE *f_stat = fopen("Simulation_Statistics.txt", "w");

    double current_time = 0.0;
    double next_b_fire_time = 0.0;
    int escorts_destroyed = 0;
    int b_sunk = 0;

    while (escorts_destroyed < N && !b_sunk) {
        if (next_b_fire_time <= current_time) {
            int target_idx = -1;
            double min_dist = 1e9;

            for (int i = 0; i < N; i++) {
                if (!escorts[i].is_destroyed) {
                    double dist = calc_distance(B.x, B.y, escorts[i].x, escorts[i].y);
                    if (dist <= B.max_range && dist < min_dist) {
                        min_dist = dist;
                        target_idx = i;
                    }
                }
            }

            if (target_idx != -1) {
                escorts[target_idx].is_destroyed = 1;
                escorts_destroyed++;
                
                char log_buf[150];
                sprintf(log_buf, "[%.2fs] B fired at E[%d] (%s) | Dist: %.1fm | Destroyed!",
                        current_time, target_idx, ESCORT_TYPES[escorts[target_idx].type_idx].notation, min_dist);
                
                printf("%s\n", log_buf);
                if (f_stat) fprintf(f_stat, "%s\n", log_buf);
                
                next_b_fire_time = current_time + B.reload_time;
            }
        }

        for (int i = 0; i < N; i++) {
            if (!escorts[i].is_destroyed && escorts[i].next_fire_time <= current_time) {
                double dist = calc_distance(B.x, B.y, escorts[i].x, escorts[i].y);
                if (dist >= escorts[i].min_range && dist <= escorts[i].max_range) {
                    double impact = ESCORT_TYPES[escorts[i].type_idx].impact_power * 100.0;
                    B.hp_percent -= impact;
                    
                    char log_buf[150];
                    sprintf(log_buf, "[%.2fs] E[%d] (%s) hit Battleship! Damage: %.2f%% | B HP Left: %.2f%%",
                            current_time, i, ESCORT_TYPES[escorts[i].type_idx].notation, impact, B.hp_percent);
                    
                    printf("%s\n", log_buf);
                    if (f_stat) fprintf(f_stat, "%s\n", log_buf);

                    if (B.hp_percent <= 0) {
                        B.hp_percent = 0;
                        b_sunk = 1;
                        break;
                    }
                }
                escorts[i].next_fire_time = current_time + escorts[i].reload_time;
            }
        }

        current_time += 0.1;
    }

    if (f_stat) fclose(f_stat);
    printf("\n[SAVED] Simulation_Statistics.txt\n\n");

    printf("======= RESULT =======\n");
    if (b_sunk) {
        printf("Battleship was SUNK!\n");
    } else {
        printf("Battleship SURVIVED! Remaining HP: %.2f%%\n", B.hp_percent);
    }
    printf("Escort ships destroyed : %d / %d\n", escorts_destroyed, N);
    printf("Battle duration        : %.3f seconds\n", current_time);

    FILE *f_final = fopen("Final_Conditions.txt", "w");
    if (f_final) {
        fprintf(f_final, "Result: %s\n", b_sunk ? "Battleship SUNK" : "Battleship SURVIVED");
        fprintf(f_final, "Battleship HP Remaining: %.2f%%\n", B.hp_percent);
        fprintf(f_final, "Escort Ships Destroyed: %d / %d\n", escorts_destroyed, N);
        fprintf(f_final, "Total Time Elapsed: %.3f seconds\n", current_time);
        fclose(f_final);
        printf("[SAVED] Final_Conditions.txt\n\n");
    }

    free(escorts);

    printf("Press ENTER to continue...");
    getchar();
    getchar();

    return 0;
}
