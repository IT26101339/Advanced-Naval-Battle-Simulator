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

/* Function to display the main menu */
void displayMainMenu() {
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
void displayBattleship() {
    printf("\n--- Battleship Information ---\n");

    if (battleship.type == '\0') {
        printf("Battleship has not been configured yet.\n");
    } else {
        printf("Type: %c\n", battleship.type);
        printf("Position: (%.2f, %.2f)\n",
               battleship.x, battleship.y);
        printf("Maximum Velocity: %.2f\n",
               battleship.maxVelocity);
    }
}

int main() {

    int choice;

    /* Initial values */
    battleship.type = '\0';
    battleship.x = 0;
    battleship.y = 0;
    battleship.maxVelocity = 0;

    printf("Welcome to the Advanced Naval Battle Simulator!\n");

    do {
        displayMainMenu();

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                printf("\nStarting simulation...\n");
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
