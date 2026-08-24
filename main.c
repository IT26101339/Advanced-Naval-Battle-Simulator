#include <stdio.h>

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


/* Display the main menu */
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


/* Display Battleship information */
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


/* Main function */
int main()
{
    int choice;

    /* Initial values */
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
                setupBattleship();
                displayBattleship();
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
