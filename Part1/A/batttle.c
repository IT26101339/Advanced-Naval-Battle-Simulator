#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_SHIPS 100
#define GRAVITY 9.81
#define PI 3.141592653589793

struct Escort {
    int id;
    char type[3];
    double x, y;
    double minAngle, maxAngle;
    double minVelocity, maxVelocity;
    int alive;
};

double distance(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

double projectileRange(double v, double angle)
{
    double r = angle * PI / 180.0;
    return (v * v * sin(2 * r)) / GRAVITY;
}

/* Find a firing angle inside the allowed range */
double findAngle(double v, double d, double minA, double maxA)
{
    double a, r, value;

    for (a = minA; a <= maxA; a += 0.1) {
        r = a * PI / 180.0;
        value = (d * GRAVITY) / (v * v);

        if (value <= 1.0 && fabs(sin(2 * r) - value) < 0.01)
            return a;
    }

    return -1;
}

void generateShips(struct Escort ships[], int n, double bVelocity)
{
    int i, t;
    double range;

    for (i = 0; i < n; i++) {

        ships[i].id = i + 1;

        t = rand() % 5;
        ships[i].type[0] = 'E';
        ships[i].type[1] = 'A' + t;
        ships[i].type[2] = '\0';

        ships[i].x = (double)rand() / RAND_MAX * 100.0;
        ships[i].y = (double)rand() / RAND_MAX * 100.0;

        /* EA = 20, EB = 30, EC = 25, ED = 50, EE = 70 */
        if (t == 0) range = 20;
        else if (t == 1) range = 30;
        else if (t == 2) range = 25;
        else if (t == 3) range = 50;
        else range = 70;

        ships[i].minAngle = 1 + (rand() % (int)(90 - range));
        ships[i].maxAngle = ships[i].minAngle + range;

        ships[i].minVelocity = 1 + (rand() % (int)(bVelocity * 0.5));

        if (t == 0)
            ships[i].maxVelocity = bVelocity * 1.2;
        else
            ships[i].maxVelocity =
                ships[i].minVelocity +
                ((double)rand() / RAND_MAX) *
                (bVelocity - ships[i].minVelocity);

        ships[i].alive = 1;
    }
}

void runPart1A()
{
    char bType;
    double bx, by, bVelocity;
    int n, i;
    int destroyed = 0;
    int sinkingShip = -1;
    int battleshipDestroyed = 0;
    double battleEndTime = 0;

    struct Escort ships[MAX_SHIPS];

    FILE *f1, *f2, *f3;

    printf("\n========================================\n");
    printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("========================================\n");

    printf("\n========== BATTLESHIP SETUP ==========\n");

    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");

    printf("\nEnter Battleship type (U/M/R/S): ");
    scanf(" %c", &bType);

    printf("Enter Battleship X coordinate (0-100): ");
    scanf("%lf", &bx);

    printf("Enter Battleship Y coordinate (0-100): ");
    scanf("%lf", &by);

    printf("Enter maximum shell velocity: ");
    scanf("%lf", &bVelocity);

    printf("\n========== ESCORT SHIP SETUP ==========\n");

    printf("Enter number of Escort Ships (1-100): ");
    scanf("%d", &n);

    if (n < 1 || n > MAX_SHIPS) {
        printf("Invalid number of ships!\n");
        return;
    }

    generateShips(ships, n, bVelocity);

    printf("\n%d Escort Ships generated successfully!\n", n);

    /* Save initial conditions */
    f1 = fopen("initial_condition.txt", "w");

    if (f1 != NULL) {
        fprintf(f1, "BATTLESHIP\n");
        fprintf(f1, "Type: %c\n", bType);
        fprintf(f1, "Position: (%.2f, %.2f)\n", bx, by);
        fprintf(f1, "Maximum Velocity: %.2f\n", bVelocity);

        fprintf(f1, "\nESCORT SHIPS\n");

        for (i = 0; i < n; i++) {
            fprintf(f1,
                    "E%d (%s): Position=(%.2f, %.2f), "
                    "Velocity=%.2f-%.2f, Angle=%.2f-%.2f\n",
                    ships[i].id,
                    ships[i].type,
                    ships[i].x,
                    ships[i].y,
                    ships[i].minVelocity,
                    ships[i].maxVelocity,
                    ships[i].minAngle,
                    ships[i].maxAngle);
        }

        fclose(f1);
    }

    /* ================= BATTLESHIP ATTACK ================= */

    printf("\n========== BATTLESHIP ATTACK ==========\n");

    printf("Battleship Maximum Attack Range: %.2f m\n",
           projectileRange(bVelocity, 45));

    f2 = fopen("simulation_data.txt", "w");

    for (i = 0; i < n; i++) {

        double d = distance(bx, by, ships[i].x, ships[i].y);
        double angle;
        double timeHit;

        printf("\nE%d (%s)\n", ships[i].id, ships[i].type);
        printf("Distance: %.2f m\n", d);

        angle = findAngle(bVelocity, d, 0.1, 89.9);

        if (angle >= 0) {

            double r = angle * PI / 180.0;

            ships[i].alive = 0;
            destroyed++;

            timeHit = (2 * bVelocity * sin(r)) / GRAVITY;

            if (timeHit > battleEndTime)
                battleEndTime = timeHit;

            printf("Result: HIT\n");
            printf("Firing Angle: %.2f degrees\n", angle);
            printf("Time to Hit: %.2f seconds\n", timeHit);

            if (f2 != NULL)
                fprintf(f2,
                        "E%d (%s): HIT, Distance=%.2f, "
                        "Angle=%.2f, Time=%.2f\n",
                        ships[i].id, ships[i].type,
                        d, angle, timeHit);
        }
        else {
            printf("Result: OUTSIDE ATTACK RANGE\n");

            if (f2 != NULL)
                fprintf(f2,
                        "E%d (%s): SURVIVED, Distance=%.2f\n",
                        ships[i].id, ships[i].type, d);
        }
    }

    printf("\nTotal Escort Ships destroyed by B: %d\n", destroyed);

    /* ================= ESCORT ATTACK ================= */

    printf("\n========== ESCORT ATTACK ==========\n");

    for (i = 0; i < n; i++) {

        double d, maxRange, angle, timeHit;

        if (!ships[i].alive)
            continue;

        d = distance(ships[i].x, ships[i].y, bx, by);

        maxRange = 0;

        {
            double a;

            for (a = ships[i].minAngle;
                 a <= ships[i].maxAngle;
                 a += 0.1) {

                double r = projectileRange(
                    ships[i].maxVelocity, a);

                if (r > maxRange)
                    maxRange = r;
            }
        }

        printf("\nE%d (%s)\n", ships[i].id, ships[i].type);
        printf("Distance: %.2f m\n", d);
        printf("Maximum Attack Range: %.2f m\n", maxRange);

        angle = findAngle(
            ships[i].maxVelocity,
            d,
            ships[i].minAngle,
            ships[i].maxAngle);

        if (angle >= 0) {

            double r = angle * PI / 180.0;

            timeHit =
                (2 * ships[i].maxVelocity * sin(r))
                / GRAVITY;

            printf("Result: HIT - Battleship destroyed\n");
            printf("Firing Angle: %.2f degrees\n", angle);
            printf("Time to Hit: %.2f seconds\n", timeHit);

            battleshipDestroyed = 1;
            sinkingShip = ships[i].id;
            battleEndTime = timeHit;

            if (f2 != NULL)
                fprintf(f2,
                        "E%d (%s): HIT B, Angle=%.2f, "
                        "Time=%.2f\n",
                        ships[i].id, ships[i].type,
                        angle, timeHit);

            break;
        }
        else {
            printf("Result: Cannot hit Battleship\n");

            if (f2 != NULL)
                fprintf(f2,
                        "E%d (%s): Cannot hit B\n",
                        ships[i].id, ships[i].type);
        }
    }

    if (f2 != NULL)
        fclose(f2);

    /* ================= FINAL RESULT ================= */

    printf("\n========================================\n");

    if (battleshipDestroyed) {

        printf("       BATTLESHIP DESTROYED!\n");
        printf("========================================\n");
        printf("Escort Ship E%d sank the Battleship.\n",
               sinkingShip);
        printf("Battle End Time: %.2f seconds\n",
               battleEndTime);
    }
    else {

        printf("       BATTLESHIP SURVIVED!\n");
        printf("========================================\n");
        printf("Escort Ships destroyed by B: %d\n",
               destroyed);
        printf("Battle End Time: %.2f seconds\n",
               battleEndTime);
        printf("Battle ended successfully.\n");
    }

    /* Save final conditions */
    f3 = fopen("final_condition.txt", "w");

    if (f3 != NULL) {

        fprintf(f3, "BATTLE RESULT\n");

        if (battleshipDestroyed) {
            fprintf(f3, "Battleship Status: DESTROYED\n");
            fprintf(f3, "Sinking Escort Ship: E%d\n",
                    sinkingShip);
        }
        else {
            fprintf(f3, "Battleship Status: SURVIVED\n");
        }

        fprintf(f3, "Battle End Time: %.2f seconds\n",
                battleEndTime);

        fprintf(f3, "Escort Ships Destroyed: %d\n",
                destroyed);

        fprintf(f3, "\nESCORT STATUS\n");

        for (i = 0; i < n; i++) {
            fprintf(f3, "E%d (%s): %s\n",
                    ships[i].id,
                    ships[i].type,
                    ships[i].alive ?
                    "ALIVE" : "DESTROYED");
        }

        fclose(f3);
    }

    printf("\nResults saved to text files.\n");
}

/* ================= INSTRUCTIONS ================= */

void showInstructions()
{
    printf("\n========== INSTRUCTIONS ==========\n");
    printf("1. One Battleship fights multiple Escort Ships.\n");
    printf("2. Enter Battleship type and position.\n");
    printf("3. Enter maximum shell velocity.\n");
    printf("4. Enter number of Escort Ships.\n");
    printf("5. Battleship attacks first.\n");
    printf("6. Destroyed Escort Ships cannot attack.\n");
    printf("7. Each surviving Escort Ship fires once.\n");
    printf("8. One shell can destroy a ship.\n");
    printf("9. Results are saved to text files.\n");
}

/* ================= MAIN ================= */

int main()
{
    int choice;

    srand((unsigned int)time(NULL));

    printf("Welcome to the Advanced Naval Battle Simulator!\n");

    do {

        printf("\n========================================\n");
        printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
        printf("========================================\n");
        printf("1. Start Part 1-A Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Exit\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                runPart1A();
                break;

            case 2:
                showInstructions();
                break;

            case 3:
                printf("\nExiting program...\n");
                break;

            default:
                printf("\nInvalid choice!\n");
        }

    } while (choice != 3);

    return 0;
}
