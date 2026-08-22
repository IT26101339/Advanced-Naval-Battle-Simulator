#include <stdio.h>

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

int main() {

    int choice;

    printf("Welcome to the Advanced Naval Battle Simulator!\n");

    do {
        displayMainMenu();

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                printf("\nStarting simulation...\n");
                break;

            case 2:
                printf("\nINSTRUCTIONS\n");
                printf("This program simulates a naval battle.\n");
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
