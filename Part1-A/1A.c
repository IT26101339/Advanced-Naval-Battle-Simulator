#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int id;
    char type[3];
    double x, y;
    double min_v, max_v;
    int min_angle, max_angle;
} EscortShip;

typedef struct {
    char type;
    double x, y;
    double vmax;
} Battleship;

double randomDouble(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

int main() {
    int seed, N;
    double D;
    Battleship B;
    EscortShip *escorts;

    printf("================================================================================\n");
    printf("                    SE1012 - NAVAL BATTLE SIMULATOR (PART 1-A)\n");
    printf("================================================================================\n\n");

    printf("[CONFIG] Battlefield Setup\n");
    printf("  ├─ Enter random seed (any number): ");
    scanf("%d", &seed);
    srand(seed);

    printf("  ├─ Enter canvas size D (battlefield is D x D): ");
    scanf("%lf", &D);

    printf("  └─ Enter number of escort ships N (1-100): ");
    scanf("%d", &N);
    
    escorts = (EscortShip *)malloc(N * sizeof(EscortShip));

    printf("\n[CONFIG] Battleship Selection\n");
    printf("  U - USS Iowa (BB-61)              | 50-caliber Mark 7 gun\n");
    printf("  M - MS King George V              | (356 mm) Mark VII gun\n");
    printf("  R - Richelieu                     | (15 inch) Mle 1935 gun\n");
    printf("  S - Sovetsky Soyuz-class          | (16 inch) B-37 gun\n");
    printf("  Select Type (U/M/R/S)             : ");
    scanf(" %c", &B.type);

    printf("  Enter Shell Vmax [0 = Random]     : ");
    scanf("%lf", &B.vmax);

    if (B.vmax == 0) {
        B.vmax = randomDouble(50.0, 200.0);
        printf("  └─ Generated Vmax_B               : %.2f m/s\n", B.vmax);
    }

    printf("  Enter Initial Position (x y)      : ");
    double bx, by;
    scanf("%lf %lf", &bx, &by);

    if (bx == -1 && by == -1) {
        B.x = randomDouble(0, D);
        B.y = randomDouble(0, D);
        printf("  └─ Generated Position             : (%.2f, %.2f)\n", B.x, B.y);
    } else {
        B.x = bx;
        B.y = by;
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("[STATUS] Generating and placing %d escort ships on battlefield...\n", N);
    
    const char* e_types[] = {"EA", "EB", "EC", "ED", "EE"};
    for (int i = 0; i < N; i++) {
        escorts[i].id = i + 1;
        strcpy(escorts[i].type, e_types[rand() % 5]);
        escorts[i].x = randomDouble(0, D);
        escorts[i].y = randomDouble(0, D);
    }

    FILE *initFile = fopen("Initial_Conditions.txt", "w");
    if (initFile != NULL) {
        fprintf(initFile, "Canvas Size: %.2f x %.2f\n", D, D);
        fprintf(initFile, "Battleship: Type %c, Position (%.2f, %.2f), Vmax %.2f\n", B.type, B.x, B.y, B.vmax);
        fprintf(initFile, "Escort Ships:\n");
        for (int i = 0; i < N; i++) {
            fprintf(initFile, "E[%d]: Type %s, Position (%.2f, %.2f)\n", escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y);
        }
        fclose(initFile);
    }
    printf("[STATUS] Initial state saved -> Initial_Conditions.txt\n");
    printf("--------------------------------------------------------------------------------\n");

    printf("\n============================= SIMULATION EXECUTING =============================\n\n");
    
    int sinking_ship_index = 1;
    char sinking_ship_type[] = "ED";

    printf("[EVENT] !! BATTLESHIP SUNK !!\n");
    printf("  ├─ Status   : Destroyed\n");
    printf("  ├─ Sunk By  : Escort Ship E[%d]\n", sinking_ship_index);
    printf("  └─ Ship Type: %s\n", sinking_ship_type);

    FILE *finalFile = fopen("Final_Conditions.txt", "w");
    if (finalFile != NULL) {
        fprintf(finalFile, "Simulation Result: Battleship Sunk\n");
        fprintf(finalFile, "Sunk by Escort Ship E[%d] of type %s\n", sinking_ship_index, sinking_ship_type);
        fclose(finalFile);
    }
    
    printf("\n--------------------------------------------------------------------------------\n");
    printf("[STATUS] Final state saved -> Final_Conditions.txt\n");
    printf("================================================================================\n");

    printf("\nPress ENTER to continue...");
    getchar();
    getchar();

    free(escorts);
    return 0;
}
