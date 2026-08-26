#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ESCORT_SHIPS 100

/* Structure for the Battleship */
struct Battleship {
    char type;
    double x;
    double y;
    double maxVelocity;
};

/* Structure for an Escort Ship */
struct EscortShip {
    int id;
    char type[3];
    double x;
    double y;
    double minAngle;
    double maxAngle;
    double minVelocity;
    double maxVelocity;
    double impactPower;
};

/* Global variables */
struct Battleship battleship;
struct EscortShip escortShips[MAX_ESCORT_SHIPS];
int numberOfEscortShips = 0;


/* Function to display the main menu */
void displayMainMenu()
{
    printf("\n");
    printf("====================================\n");
    printf(" ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("====================================\n");

    printf("1. Start Simulation\n");
    printf("2. View Instructions\n");
    printf("3. Simulation Statistics\n");
    printf("4. Exit\n");
}


/* Function to display Battleship information */
void displayBattleship()
{
    printf("\n--- Battleship Information ---\n");

    printf("Type: %c\n", battleship.type);

    printf("Position: (%.2f, %.2f)\n",
           battleship.x,
           battleship.y);

    printf("Maximum Velocity: %.2f\n",
           battleship.maxVelocity);
}


/* Setup the Battleship */
void setupBattleship()
{
    printf("\n========== BATTLESHIP SETUP ==========\n");

    printf("\nAvailable Battleships:\n");
    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");


    /* Get Battleship type */

    printf("\nEnter Battleship type (U/M/R/S): ");
    scanf(" %c", &battleship.type);


    /* Check whether the type is valid */

    while (battleship.type != 'U' &&
           battleship.type != 'M' &&
           battleship.type != 'R' &&
           battleship.type != 'S')
    {
        printf("Invalid Battleship type!\n");

        printf("Please enter U, M, R or S: ");

        scanf(" %c", &battleship.type);
    }


    /* Get X coordinate */

    printf("Enter Battleship X coordinate: ");
    scanf("%lf", &battleship.x);


    /* Get Y coordinate */

    printf("Enter Battleship Y coordinate: ");
    scanf("%lf", &battleship.y);


    /* Get maximum shell velocity */

    printf("Enter maximum shell velocity: ");
    scanf("%lf", &battleship.maxVelocity);


    printf("\nBattleship setup completed successfully!\n");
}


/*
 * Generate a random decimal number
 * between minimum and maximum.
 */
double randomDouble(double minimum, double maximum)
{
    return minimum +
           ((double)rand() / RAND_MAX) *
           (maximum - minimum);
}


/*
 * Generate a random Escort Ship type.
 *
 * Possible types:
 * EA
 * EB
 * EC
 * ED
 * EE
 */
void generateEscortType(struct EscortShip *ship)
{
    int type;

    type = rand() % 5;

    ship->type[0] = 'E';
    ship->type[1] = 'A' + type;
    ship->type[2] = '\0';
}


/*
 * Generate random Escort Ships
 */
void generateEscortShips()
{
    int i;

    printf("\n========== ESCORT SHIP SETUP ==========\n");

    printf("Enter number of Escort Ships (1-%d): ",
           MAX_ESCORT_SHIPS);

    scanf("%d", &numberOfEscortShips);


    /* Validate number of ships */

    while (numberOfEscortShips < 1 ||
           numberOfEscortShips > MAX_ESCORT_SHIPS)
    {
        printf("Invalid number of Escort Ships!\n");

        printf("Enter number of Escort Ships (1-%d): ",
               MAX_ESCORT_SHIPS);

        scanf("%d", &numberOfEscortShips);
    }


    /*
     * Generate each Escort Ship
     */
    for (i = 0; i < numberOfEscortShips; i++)
    {
        /* Ship ID */
        escortShips[i].id = i + 1;


        /* Random ship type */
        generateEscortType(&escortShips[i]);


        /* Random X coordinate */
        escortShips[i].x =
            randomDouble(0.0, 100.0);


        /* Random Y coordinate */
        escortShips[i].y =
            randomDouble(0.0, 100.0);


        /*
         * Velocity range
         */
        escortShips[i].minVelocity = 10.0;

        escortShips[i].maxVelocity = 100.0;


        /*
         * Angle range
         */
        escortShips[i].minAngle = 0.0;

        escortShips[i].maxAngle = 90.0;


        /*
         * Impact power
         */
        escortShips[i].impactPower =
            randomDouble(0.04, 0.08);
    }


    printf("\n%d Escort Ships generated successfully!\n",
           numberOfEscortShips);
}


/*
 * Display Escort Ship information
 */
void displayEscortShips()
{
    int i;

    printf("\n========== ESCORT SHIP INFORMATION ==========\n");


    for (i = 0; i < numberOfEscortShips; i++)
    {
        printf("\nEscort Ship %d\n",
               escortShips[i].id);

        printf("Type: %s\n",
               escortShips[i].type);

        printf("Position: (%.2f, %.2f)\n",
               escortShips[i].x,
               escortShips[i].y);

        printf("Velocity Range: %.2f - %.2f\n",
               escortShips[i].minVelocity,
               escortShips[i].maxVelocity);

        printf("Angle Range: %.2f - %.2f degrees\n",
               escortShips[i].minAngle,
               escortShips[i].maxAngle);

        printf("Impact Power: %.2f\n",
               escortShips[i].impactPower);
    }
}


int main()
{
    int choice;


    /*
     * Seed the random number generator.
     * This makes the Escort Ships different
     * each time the program runs.
     */
    srand((unsigned int)time(NULL));


    /* Initial Battleship values */

    battleship.type = '\0';
    battleship.x = 0;
    battleship.y = 0;
    battleship.maxVelocity = 0;


    printf("Welcome to the Advanced Naval Battle Simulator!\n");


    do
    {
        displayMainMenu();


        printf("\nEnter your choice: ");
        scanf("%d", &choice);


        switch (choice)
        {
            case 1:

                printf("\nStarting simulation...\n");

                /* Day 3 */
                setupBattleship();

                displayBattleship();

                /* Day 4 */
                generateEscortShips();

                displayEscortShips();

                break;


            case 2:

                printf("\n========== INSTRUCTIONS ==========\n");

                printf("This program simulates a naval battle.\n");

                printf("One Battleship fights against multiple Escort Ships.\n");

                printf("Ships will be placed on a 2D battlefield.\n");

                break;


            case 3:

                printf("\nNo previous simulation statistics available yet.\n");

                break;


            case 4:

                printf("\nExiting program...\n");

                break;


            default:

                printf("\nInvalid choice! Please try again.\n");
        }


    } while (choice != 4);


    return 0;
}
