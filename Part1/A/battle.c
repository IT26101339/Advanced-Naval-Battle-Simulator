#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GRAVITY 9.81

struct Escort {
    int id;
    double x;
    double y;
    int type;
    double V;
    double angle;
    int alive;
};

void runPart1A();
void showInstructions();

void runPart1A() {
    char battleshipType;
    double battleshipX, battleshipY, maxShellVelocity;
    int numberOfShips, i;
    int hitsByB = 0;
    double maxBattleTime = 0.0;
    struct Escort ships[100];
    FILE *fpInit, *fpSim, *fpFinal;

    srand((unsigned int)time(NULL));

    printf("\nWelcome to Part 1-A of the Advanced Naval Battle Simulator!\n\n");
    printf("========================================\n");
    printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("========================================\n");

    printf("\n============ BATTLESHIP SETUP ============\n\n");
    printf("Available Battleships:\n");
    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n\n");

    printf("Enter Battleship type (U/M/R/S): ");
    scanf(" %c", &battleshipType);
    printf("Enter Battleship X coordinate (0-100): ");
    scanf("%lf", &battleshipX);
    printf("Enter Battleship Y coordinate (0-100): ");
    scanf("%lf", &battleshipY);
    printf("Enter maximum shell velocity (>0): ");
    scanf("%lf", &maxShellVelocity);

    printf("\nBattleship setup completed successfully!\n");

    printf("\n============ ESCORT SHIP SETUP ============\n");
    printf("Enter number of Escort Ships (1-100): ");
    scanf("%d", &numberOfShips);

    for (i = 0; i < numberOfShips; i++) {
        ships[i].id = i + 1;
        ships[i].x = (double)(rand() % 101);
        ships[i].y = (double)(rand() % 101);
        ships[i].type = rand() % 5 + 1;
        ships[i].V = (double)(rand() % 10 + 1);
        ships[i].angle = (double)(rand() % 360);
        ships[i].alive = 1;
    }

    printf("\n%d Escort Ships generated successfully!\n", numberOfShips);

    // Save Initial Conditions
    fpInit = fopen("initial_condition.txt", "w");
    if (fpInit) {
        fprintf(fpInit, "Battleship Type: %c\n", battleshipType);
        fprintf(fpInit, "Battleship Pos: (%.2f, %.2f)\n", battleshipX, battleshipY);
        fprintf(fpInit, "Max Shell Velocity: %.2f m/s\n", maxShellVelocity);
        fprintf(fpInit, "Number of Escort Ships: %d\n", numberOfShips);
        for (i = 0; i < numberOfShips; i++) {
            fprintf(fpInit, "Escort %d: Pos=(%.2f, %.2f) Type=%d Speed=%.2f Angle=%.2f\n",
                    ships[i].id, ships[i].x, ships[i].y, ships[i].type, ships[i].V, ships[i].angle);
        }
        fclose(fpInit);
        printf("\nInitial conditions saved to initial_condition.txt\n");
    }

    // Battleship Attack Calculations
    double maxRange = (maxShellVelocity * maxShellVelocity) / GRAVITY;

    printf("\n========================================\n");
    printf("           BATTLESHIP ATTACK\n");
    printf("========================================\n\n");
    printf("Battleship Maximum Attack Range: %.2f m\n\n", maxRange);

    fpSim = fopen("simulation_data.txt", "w");

    for (i = 0; i < numberOfShips; i++) {
        double dx = ships[i].x - battleshipX;
        double dy = ships[i].y - battleshipY;
        double distance = sqrt(dx * dx + dy * dy);

        printf("Escort Ship %d (EE)\n", ships[i].id);
        printf("Distance from Battleship: %.2f m\n", distance);

        if (distance <= maxRange) {
            ships[i].alive = 0;
            hitsByB++;

            // Physics Calculations
            double sin2Theta = (distance * GRAVITY) / (maxShellVelocity * maxShellVelocity);
            if (sin2Theta > 1.0) sin2Theta = 1.0;
            double firingAngleRad = 0.5 * asin(sin2Theta);
            double firingAngleDeg = firingAngleRad * (180.0 / M_PI);

            double timeToHit = (2.0 * maxShellVelocity * sin(firingAngleRad)) / GRAVITY;

            if (timeToHit > maxBattleTime) {
                maxBattleTime = timeToHit;
            }

            printf("Result: HIT - Escort Ship destroyed\n");
            printf("Firing Angle: %.2f degrees\n", firingAngleDeg);
            printf("Time to hit: %.2f seconds\n\n", timeToHit);

            if (fpSim) {
                fprintf(fpSim, "Escort %d: Distance=%.2f m, FiringAngle=%.2f deg, TimeToHit=%.2f s, Status=Destroyed\n",
                        ships[i].id, distance, firingAngleDeg, timeToHit);
            }
        } else {
            printf("Result: MISS - Out of range\n\n");
            if (fpSim) {
                fprintf(fpSim, "Escort %d: Distance=%.2f m, Status=Survived\n", ships[i].id, distance);
            }
        }
    }

    printf("Total Escort Ships destroyed by B: %d\n\n", hitsByB);

    // Escort Ships Turn Status
    for (i = 0; i < numberOfShips; i++) {
        printf("Escort Ship %d (ED)\n", ships[i].id);
        if (ships[i].alive == 0) {
            printf("Status: Destroyed by Battleship\n");
            printf("Cannot attack.\n\n");
        } else {
            printf("Status: Active\n");
            printf("Attacking Battleship...\n\n");
        }
    }

    printf("========================================\n");
    printf("           BATTLESHIP SURVIVED!\n");
    printf("========================================\n");
    printf("Escort Ships destroyed by B: %d\n", hitsByB);
    printf("Battle End Time: %.2f seconds\n", maxBattleTime);
    printf("Battle ended successfully.\n\n");

    printf("Simulation data saved to simulation_data.txt\n");

    // Save Final Conditions
    fpFinal = fopen("final_condition.txt", "w");
    if (fpFinal) {
        fprintf(fpFinal, "Battle End Time: %.2f seconds\n", maxBattleTime);
        fprintf(fpFinal, "Total Escorts Destroyed: %d\n", hitsByB);
        fprintf(fpFinal, "Battleship Status: SURVIVED\n");
        for (i = 0; i < numberOfShips; i++) {
            fprintf(fpFinal, "Escort %d Status: %s\n", ships[i].id, ships[i].alive ? "Alive" : "Destroyed");
        }
        fclose(fpFinal);
        printf("Final conditions saved to final_condition.txt\n");
    }

    if (fpSim) fclose(fpSim);
}

void showInstructions() {
    printf("\n========================================\n");
    printf("             INSTRUCTIONS\n");
    printf("========================================\n");
    printf("1. Select Start Part 1-A Simulation from the menu.\n");
    printf("2. Select Battleship Type (U, M, R, S).\n");
    printf("3. Input coordinates (X, Y) and maximum shell velocity.\n");
    printf("4. Set number of Escort Ships to generate.\n");
    printf("5. View trajectory, firing angles, hit details, and saved results.\n");
}

int main() {
    int choice;

    do {
        printf("\n========================================\n");
        printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
        printf("========================================\n");
        printf("1. Start Part 1-A Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Exit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                runPart1A();
                break;
            case 2:
                showInstructions();
                break;
            case 3:
                printf("Exiting... Thank you!\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
                break;
        }
    } while (choice != 3);

    return 0;
}
