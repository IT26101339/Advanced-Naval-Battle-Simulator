#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ESCORT_SHIPS 100
#define GRAVITY 9.81
#define PI 3.14159265358979323846
#define CANVAS_SIZE 100.0

/* =========================
   BATTLESHIP
   ========================= */

struct Battleship
{
    char type;
    double x;
    double y;
    double maxVelocity;
};

/* =========================
   ESCORT SHIP
   ========================= */

struct EscortShip
{
    int id;
    char type[3];

    double x;
    double y;

    double minAngle;
    double maxAngle;

    double minVelocity;
    double maxVelocity;

    double impactPower;

    int destroyed;
    int hitByBattleship;

    double timeToHit;
};

/* =========================
   GLOBAL VARIABLES
   ========================= */

struct Battleship battleship;
struct EscortShip escortShips[MAX_ESCORT_SHIPS];

int numberOfEscortShips = 0;

/* =========================
   RANDOM NUMBER
   ========================= */

double randomDouble(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

/* =========================
   MAIN MENU
   ========================= */

void displayMainMenu()
{
    printf("\n========================================\n");
    printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("========================================\n");
    printf("1. Start Part 1-A Simulation\n");
    printf("2. View Instructions\n");
    printf("3. Exit\n");
}

/* =========================
   IMPACT POWER
   ========================= */

double getImpactPower(char type)
{
    switch(type)
    {
        case 'A': return 0.08;
        case 'B': return 0.06;
        case 'C': return 0.07;
        case 'D': return 0.05;
        case 'E': return 0.04;
        default:  return 0.00;
    }
}

/* =========================
   ANGLE RANGE
   EA=20, EB=30,
   EC=25, ED=50, EE=70
   ========================= */

double getAngleRange(char type)
{
    switch(type)
    {
        case 'A': return 20.0;
        case 'B': return 30.0;
        case 'C': return 25.0;
        case 'D': return 50.0;
        case 'E': return 70.0;
        default:  return 0.0;
    }
}

/* =========================
   GENERATE E TYPE
   ========================= */

void generateEscortType(struct EscortShip *ship)
{
    int n = rand() % 5;

    ship->type[0] = 'E';
    ship->type[1] = 'A' + n;
    ship->type[2] = '\0';
}

/* =========================
   BATTLESHIP SETUP
   ========================= */

void setupBattleship()
{
    printf("\n========== BATTLESHIP SETUP ==========\n");

    printf("\nAvailable Battleships:\n");
    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");

    printf("\nEnter Battleship type (U/M/R/S): ");
    scanf(" %c", &battleship.type);

    while(battleship.type != 'U' &&
          battleship.type != 'M' &&
          battleship.type != 'R' &&
          battleship.type != 'S')
    {
        printf("Invalid type. Enter U, M, R or S: ");
        scanf(" %c", &battleship.type);
    }

    printf("Enter Battleship X coordinate (0-100): ");
    scanf("%lf", &battleship.x);

    while(battleship.x < 0 || battleship.x > CANVAS_SIZE)
    {
        printf("Invalid X. Enter 0-100: ");
        scanf("%lf", &battleship.x);
    }

    printf("Enter Battleship Y coordinate (0-100): ");
    scanf("%lf", &battleship.y);

    while(battleship.y < 0 || battleship.y > CANVAS_SIZE)
    {
        printf("Invalid Y. Enter 0-100: ");
        scanf("%lf", &battleship.y);
    }

    printf("Enter maximum shell velocity (>0): ");
    scanf("%lf", &battleship.maxVelocity);

    while(battleship.maxVelocity <= 0)
    {
        printf("Invalid velocity. Enter a value > 0: ");
        scanf("%lf", &battleship.maxVelocity);
    }

    printf("\nBattleship setup completed successfully!\n");
}

/* =========================
   ESCORT SHIP SETUP
   ========================= */

void generateEscortShips()
{
    int i;
    double angleRange;

    printf("\n========== ESCORT SHIP SETUP ==========\n");

    printf("Enter number of Escort Ships (1-%d): ",
           MAX_ESCORT_SHIPS);
    scanf("%d", &numberOfEscortShips);

    while(numberOfEscortShips < 1 ||
          numberOfEscortShips > MAX_ESCORT_SHIPS)
    {
        printf("Invalid number. Enter 1-%d: ",
               MAX_ESCORT_SHIPS);
        scanf("%d", &numberOfEscortShips);
    }

    for(i = 0; i < numberOfEscortShips; i++)
    {
        escortShips[i].id = i + 1;

        /* Random type EA - EE */
        generateEscortType(&escortShips[i]);

        /* Random position */
        escortShips[i].x =
            randomDouble(0.0, CANVAS_SIZE);

        escortShips[i].y =
            randomDouble(0.0, CANVAS_SIZE);

        /* Angle range */
        angleRange =
            getAngleRange(escortShips[i].type[1]);

        /*
           Generate minimum angle so that
           maximum angle does not exceed 90.
        */
        escortShips[i].minAngle =
            randomDouble(1.0, 90.0 - angleRange);

        escortShips[i].maxAngle =
            escortShips[i].minAngle + angleRange;

        /* Velocity */

        if(escortShips[i].type[1] == 'A')
        {
            /*
               EA maximum velocity can be
               1.2 * Battleship maximum velocity.
            */
            escortShips[i].minVelocity =
                randomDouble(1.0,
                             battleship.maxVelocity * 0.5);

            escortShips[i].maxVelocity =
                1.2 * battleship.maxVelocity;
        }
        else
        {
            /*
               Other E types have maximum
               velocity below B maximum velocity.
            */
            escortShips[i].minVelocity =
                randomDouble(1.0,
                             battleship.maxVelocity * 0.5);

            escortShips[i].maxVelocity =
                randomDouble(
                    escortShips[i].minVelocity + 0.1,
                    battleship.maxVelocity
                );
        }

        /* Impact power */
        escortShips[i].impactPower =
            getImpactPower(escortShips[i].type[1]);

        escortShips[i].destroyed = 0;
        escortShips[i].hitByBattleship = 0;
        escortShips[i].timeToHit = 0.0;
    }

    printf("\n%d Escort Ships generated successfully!\n",
           numberOfEscortShips);
}

/* =========================
   DISTANCE
   ========================= */

double calculateDistance(double x1, double y1,
                         double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}

/* =========================
   PROJECTILE RANGE
   ========================= */

double calculateProjectileRange(double velocity,
                                double angle)
{
    double radians = angle * PI / 180.0;

    return (velocity * velocity *
            sin(2.0 * radians)) / GRAVITY;
}

/* =========================
   ESCORT MAX ATTACK RANGE
   ========================= */

double calculateEscortAttackRange(
    struct EscortShip *ship)
{
    double angle;
    double range;
    double maximumRange = 0.0;

    for(angle = ship->minAngle;
        angle <= ship->maxAngle;
        angle += 0.1)
    {
        range =
            calculateProjectileRange(
                ship->maxVelocity,
                angle
            );

        if(range > maximumRange)
            maximumRange = range;
    }

    return maximumRange;
}

/* =========================
   FIND FIRING ANGLE
   ========================= */

double findFiringAngle(double velocity,
                       double distance,
                       double minAngle,
                       double maxAngle)
{
    double angle;
    double range;

    for(angle = minAngle;
        angle <= maxAngle;
        angle += 0.1)
    {
        range =
            calculateProjectileRange(
                velocity,
                angle
            );

        if(range >= distance)
            return angle;
    }

    return -1.0;
}

/* =========================
   FLIGHT TIME
   ========================= */

double calculateFlightTime(double velocity,
                           double angle)
{
    double radians = angle * PI / 180.0;

    return (2.0 * velocity *
            sin(radians)) / GRAVITY;
}

/* =========================
   SAVE INITIAL CONDITIONS
   ========================= */

void saveInitialConditions()
{
    FILE *file;
    int i;

    file = fopen("initial_condition.txt", "w");

    if(file == NULL)
    {
        printf("Error creating initial_condition.txt\n");
        return;
    }

    fprintf(file,
            "========================================\n");
    fprintf(file,
            "INITIAL BATTLEFIELD CONDITIONS\n");
    fprintf(file,
            "========================================\n\n");

    fprintf(file, "BATTLESHIP\n");
    fprintf(file, "Type: %c\n",
            battleship.type);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            battleship.x,
            battleship.y);

    fprintf(file,
            "Maximum Velocity: %.2f m/s\n\n",
            battleship.maxVelocity);

    fprintf(file,
            "NUMBER OF ESCORT SHIPS: %d\n\n",
            numberOfEscortShips);

    fprintf(file, "ESCORT SHIPS\n");

    for(i = 0; i < numberOfEscortShips; i++)
    {
        fprintf(file,
                "\nEscort Ship %d\n",
                escortShips[i].id);

        fprintf(file,
                "Type: %s\n",
                escortShips[i].type);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                escortShips[i].x,
                escortShips[i].y);

        fprintf(file,
                "Minimum Angle: %.2f degrees\n",
                escortShips[i].minAngle);

        fprintf(file,
                "Maximum Angle: %.2f degrees\n",
                escortShips[i].maxAngle);

        fprintf(file,
                "Minimum Velocity: %.2f m/s\n",
                escortShips[i].minVelocity);

        fprintf(file,
                "Maximum Velocity: %.2f m/s\n",
                escortShips[i].maxVelocity);

        fprintf(file,
                "Impact Power: %.2f\n",
                escortShips[i].impactPower);
    }

    fclose(file);

    printf("\nInitial conditions saved to ");
    printf("initial_condition.txt\n");
}

/* =========================
   BATTLESHIP ATTACK
   ========================= */

void battleshipAttack(double *battleEndTime)
{
    int i;
    int hitCount = 0;

    double range;
    double distance;
    double angle;
    double flightTime;

    printf("\n========================================\n");
    printf("          BATTLESHIP ATTACK\n");
    printf("========================================\n");

    /*
       Maximum B range occurs at 45 degrees.
    */
    range =
        calculateProjectileRange(
            battleship.maxVelocity,
            45.0
        );

    printf("\nBattleship Maximum Attack Range: %.2f m\n",
           range);

    for(i = 0; i < numberOfEscortShips; i++)
    {
        distance =
            calculateDistance(
                battleship.x,
                battleship.y,
                escortShips[i].x,
                escortShips[i].y
            );

        printf("\nEscort Ship %d (%s)\n",
               escortShips[i].id,
               escortShips[i].type);

        printf("Distance from Battleship: %.2f m\n",
               distance);

        if(distance <= range)
        {
            angle =
                findFiringAngle(
                    battleship.maxVelocity,
                    distance,
                    0.1,
                    89.9
                );

            if(angle >= 0)
            {
                flightTime =
                    calculateFlightTime(
                        battleship.maxVelocity,
                        angle
                    );

                escortShips[i].destroyed = 1;
                escortShips[i].hitByBattleship = 1;
                escortShips[i].timeToHit = flightTime;

                hitCount++;

                /*
                   Battle ends after the last
                   B shell reaches its target.
                */
                if(flightTime > *battleEndTime)
                    *battleEndTime = flightTime;

                printf("Result: HIT - Escort Ship destroyed\n");
                printf("Firing Angle: %.2f degrees\n",
                       angle);
                printf("Time to hit: %.2f seconds\n",
                       flightTime);
            }
            else
            {
                printf("Result: Cannot find firing angle\n");
            }
        }
        else
        {
            printf("Result: Outside Attack Range\n");
        }
    }

    printf("\nTotal Escort Ships destroyed by B: %d\n",
           hitCount);
}

/* =========================
   ESCORT ATTACK
   ========================= */

int escortAttack(int *sinkingShipIndex,
                 double *battleEndTime)
{
    int i;

    double distance;
    double range;
    double angle;
    double flightTime;

    for(i = 0; i < numberOfEscortShips; i++)
    {
        /*
           Destroyed E ships cannot attack.
        */
        if(escortShips[i].destroyed)
            continue;

        distance =
            calculateDistance(
                escortShips[i].x,
                escortShips[i].y,
                battleship.x,
                battleship.y
            );

        range =
            calculateEscortAttackRange(
                &escortShips[i]
            );

        printf("\nEscort Ship %d (%s)\n",
               escortShips[i].id,
               escortShips[i].type);

        printf("Distance from Battleship: %.2f m\n",
               distance);

        printf("Escort Maximum Attack Range: %.2f m\n",
               range);

        /*
           Each E can fire only once.
        */

        if(distance <= range)
        {
            angle =
                findFiringAngle(
                    escortShips[i].maxVelocity,
                    distance,
                    escortShips[i].minAngle,
                    escortShips[i].maxAngle
                );

            if(angle >= 0)
            {
                flightTime =
                    calculateFlightTime(
                        escortShips[i].maxVelocity,
                        angle
                    );

                printf("Result: HIT - Battleship destroyed\n");
                printf("Firing Angle: %.2f degrees\n",
                       angle);

                printf("Time to hit: %.2f seconds\n",
                       flightTime);

                /*
                   One E shell destroys B
                   in Part 1-A.
                */

                *sinkingShipIndex =
                    escortShips[i].id;

                *battleEndTime =
                    flightTime;

                return 1;
            }
        }

        printf("Result: Escort cannot hit Battleship\n");
    }

    return 0;
}

/* =========================
   SAVE SIMULATION DATA
   ========================= */

void saveSimulationData(int battleshipDestroyed,
                        int sinkingShipIndex,
                        double battleEndTime)
{
    FILE *file;
    int i;
    int hitCount = 0;

    file = fopen("simulation_data.txt", "w");

    if(file == NULL)
    {
        printf("Error creating simulation_data.txt\n");
        return;
    }

    fprintf(file,
            "========================================\n");
    fprintf(file,
            "PART 1-A SIMULATION RESULTS\n");
    fprintf(file,
            "========================================\n\n");

    if(battleshipDestroyed)
    {
        fprintf(file,
                "RESULT: BATTLESHIP DESTROYED\n");

        fprintf(file,
                "Escort Ship Index: %d\n",
                sinkingShipIndex);

        fprintf(file,
                "Battle End Time: %.2f seconds\n",
                battleEndTime);
    }
    else
    {
        fprintf(file,
                "RESULT: BATTLESHIP SURVIVED\n\n");

        fprintf(file,
                "ESCORT SHIPS HIT BY BATTLESHIP\n");

        for(i = 0; i < numberOfEscortShips; i++)
        {
            if(escortShips[i].hitByBattleship)
            {
                hitCount++;

                fprintf(file,
                        "\nEscort Ship Index: %d\n",
                        escortShips[i].id);

                fprintf(file,
                        "Type: %s\n",
                        escortShips[i].type);

                fprintf(file,
                        "Position: (%.2f, %.2f)\n",
                        escortShips[i].x,
                        escortShips[i].y);

                /*
                   IMPORTANT:
                   Part 1-A requires Time to Hit.
                */
                fprintf(file,
                        "Time to hit: %.2f seconds\n",
                        escortShips[i].timeToHit);
            }
        }

        fprintf(file,
                "\nTotal Escort Ships Hit: %d\n",
                hitCount);

        fprintf(file,
                "Battle End Time: %.2f seconds\n",
                battleEndTime);
    }

    fclose(file);

    printf("\nSimulation data saved to ");
    printf("simulation_data.txt\n");
}

/* =========================
   SAVE FINAL CONDITIONS
   ========================= */

void saveFinalConditions(int battleshipDestroyed,
                         int sinkingShipIndex)
{
    FILE *file;
    int i;

    file = fopen("final_condition.txt", "w");

    if(file == NULL)
    {
        printf("Error creating final_condition.txt\n");
        return;
    }

    fprintf(file,
            "========================================\n");
    fprintf(file,
            "FINAL BATTLEFIELD CONDITIONS\n");
    fprintf(file,
            "========================================\n\n");

    fprintf(file, "BATTLESHIP\n");

    fprintf(file,
            "Type: %c\n",
            battleship.type);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            battleship.x,
            battleship.y);

    fprintf(file,
            "Maximum Velocity: %.2f m/s\n",
            battleship.maxVelocity);

    if(battleshipDestroyed)
    {
        fprintf(file,
                "Status: DESTROYED\n");

        fprintf(file,
                "Sunk by Escort Ship: %d\n",
                sinkingShipIndex);
    }
    else
    {
        fprintf(file,
                "Status: SURVIVED\n");
    }

    fprintf(file, "\nESCORT SHIPS\n");

    for(i = 0; i < numberOfEscortShips; i++)
    {
        fprintf(file,
                "\nEscort Ship %d\n",
                escortShips[i].id);

        fprintf(file,
                "Type: %s\n",
                escortShips[i].type);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                escortShips[i].x,
                escortShips[i].y);

        fprintf(file,
                "Minimum Angle: %.2f degrees\n",
                escortShips[i].minAngle);

        fprintf(file,
                "Maximum Angle: %.2f degrees\n",
                escortShips[i].maxAngle);

        fprintf(file,
                "Minimum Velocity: %.2f m/s\n",
                escortShips[i].minVelocity);

        fprintf(file,
                "Maximum Velocity: %.2f m/s\n",
                escortShips[i].maxVelocity);

        fprintf(file,
                "Impact Power: %.2f\n",
                escortShips[i].impactPower);

        if(escortShips[i].destroyed)
        {
            fprintf(file,
                    "Status: DESTROYED BY BATTLESHIP\n");
        }
        else
        {
            fprintf(file,
                    "Status: ALIVE\n");
        }
    }

    fclose(file);

    printf("Final conditions saved to ");
    printf("final_condition.txt\n");
}

/* =========================
   RUN SIMULATION
   ========================= */

void runSimulation()
{
    int sinkingShipIndex = -1;
    int battleshipDestroyed;

    double battleEndTime = 0.0;

    setupBattleship();

    generateEscortShips();

    /*
       Save BEFORE battle.
    */
    saveInitialConditions();

    /*
       B attacks first.
    */
    battleshipAttack(&battleEndTime);

    /*
       Remaining E ships attack.
    */
    battleshipDestroyed =
        escortAttack(
            &sinkingShipIndex,
            &battleEndTime
        );

    printf("\n========================================\n");

    if(battleshipDestroyed)
    {
        printf("        BATTLESHIP DESTROYED!\n");
        printf("========================================\n");

        printf("Escort Ship %d sank the Battleship.\n",
               sinkingShipIndex);

        printf("Battle End Time: %.2f seconds\n",
               battleEndTime);
    }
    else
    {
        int hitCount = 0;
        int i;

        for(i = 0; i < numberOfEscortShips; i++)
        {
            if(escortShips[i].hitByBattleship)
                hitCount++;
        }

        printf("        BATTLESHIP SURVIVED!\n");
        printf("========================================\n");

        printf("Escort Ships destroyed by B: %d\n",
               hitCount);

        printf("Battle End Time: %.2f seconds\n",
               battleEndTime);

        printf("Battle ended successfully.\n");
    }

    /*
       Save both required files.
    */
    saveSimulationData(
        battleshipDestroyed,
        sinkingShipIndex,
        battleEndTime
    );

    saveFinalConditions(
        battleshipDestroyed,
        sinkingShipIndex
    );
}

/* =========================
   INSTRUCTIONS
   ========================= */

void displayInstructions()
{
    printf("\n========== PART 1-A INSTRUCTIONS ==========\n");

    printf("1. One Battleship fights multiple Escort Ships.\n");
    printf("2. Battleship has zero reload time.\n");
    printf("3. Each Escort Ship can fire only once.\n");
    printf("4. One shell can destroy the Battleship.\n");
    printf("5. One shell can destroy an Escort Ship.\n");
    printf("6. Battleship can hit E ships inside its range.\n");
    printf("7. E ships can hit B if B is inside their range.\n");
    printf("8. Initial conditions are saved to a text file.\n");
    printf("9. Final conditions are saved to a text file.\n");
    printf("10. Simulation results are saved to a text file.\n");
}

/* =========================
   MAIN
   ========================= */

int main()
{
    int choice;

    srand((unsigned int)time(NULL));

    printf("Welcome to Part 1-A of the ");
    printf("Advanced Naval Battle Simulator!\n");

    do
    {
        displayMainMenu();

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                runSimulation();
                break;

            case 2:
                displayInstructions();
                break;

            case 3:
                printf("\nExiting program...\n");
                break;

            default:
                printf("\nInvalid choice!\n");
        }

    } while(choice != 3);

    return 0;
}
