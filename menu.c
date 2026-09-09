#include <stdio.h>
#include <stdlib.h>

void showMenu();
void startSimulationMenu();
void viewInstructions();
void viewStatisticsMenu();
void readTextFile(const char *filepath);

int main() {
    int choice;

    while (1) {
        showMenu();
        printf("Enter your choice (1-4): ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1:
                startSimulationMenu();
                break;
            case 2:
                viewInstructions();
                break;
            case 3:
                viewStatisticsMenu();
                break;
            case 4:
                printf("\nExiting Naval Battle Simulator. Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }
    return 0;
}

void showMenu() {
    printf("\n==================================================\n");
    printf("         ADVANCED NAVAL BATTLE SIMULATOR          \n");
    printf("==================================================\n");
    printf("  1. Start Simulation\n");
    printf("  2. View Instructions\n");
    printf("  3. Simulation Statistics\n");
    printf("  4. Exit\n");
    printf("==================================================\n");
}

void startSimulationMenu() {
    int partChoice;
    printf("\n--- SELECT SIMULATION PART ---\n");
    printf("1. Part 1-A (Basic Battle)\n");
    printf("2. Part 1-B (Moving Path & Jammed Gun)\n");
    printf("3. Part 1-C (Impact Power & Continuous HP)\n");
    printf("4. Part 2-A (Battleship Reload Time & Strategy)\n");
    printf("5. Part 2-B (Escort Continuous Firing)\n");
    printf("6. Part 2-C (Impact Power Degradation)\n");
    printf("7. Back to Main Menu\n");
    printf("Select Part: ");
    scanf("%d", &partChoice);

    // Running compiled binaries inside your exact folder structure
    switch (partChoice) {
        case 1: system("cd Part1-A && ./1A"); break;
        case 2: system("cd Part1-B && ./1B"); break;
        case 3: system("cd Part1-C && ./1C"); break;
        case 4: system("cd Part2-A && ./2A"); break;
        case 5: system("cd Part2-B && ./2B"); break;
        case 6: system("cd Part2-C && ./2C"); break;
        case 7: return;
        default: printf("Invalid option!\n");
    }
}

void viewInstructions() {
    printf("\n==================================================\n");
    printf("                   INSTRUCTIONS                   \n");
    printf("==================================================\n");
    printf("1. Select 'Start Simulation' to execute battle models.\n");
    printf("2. Enter coordinates as (X Y) separated by space.\n");
    printf("3. Enter -1 -1 or 0 for auto-generating values.\n");
    printf("4. Logs are automatically saved in respective folders.\n");
    printf("==================================================\n");
}

void viewStatisticsMenu() {
    int partChoice, statChoice;
    char filepath[100];
    char folder[20];
    char filename[30];

    printf("\n--- SELECT SIMULATION PART ---\n");
    printf("1. Part 1-A\n");
    printf("2. Part 1-B\n");
    printf("3. Part 1-C\n");
    printf("4. Part 2-A\n");
    printf("5. Part 2-B\n");
    printf("6. Part 2-C\n");
    printf("Select Part (1-6): ");
    scanf("%d", &partChoice);

    printf("\n--- SELECT LOG FILE ---\n");
    printf("1. Initial Conditions\n");
    printf("2. Simulation Statistics\n");
    printf("3. Final Conditions\n");
    printf("Select File (1-3): ");
    scanf("%d", &statChoice);

    // Mapped to your exact folder names
    switch (partChoice) {
        case 1: sprintf(folder, "Part1-A"); break;
        case 2: sprintf(folder, "Part1-B"); break;
        case 3: sprintf(folder, "Part1-C"); break;
        case 4: sprintf(folder, "Part2-A"); break;
        case 5: sprintf(folder, "Part2-B"); break;
        case 6: sprintf(folder, "Part2-C"); break;
        default:
            printf("\nInvalid Part choice!\n");
            return;
    }

    switch (statChoice) {
        case 1: sprintf(filename, "Initial_Conditions.txt"); break;
        case 2: sprintf(filename, "Simulation_Statistics.txt"); break;
        case 3: sprintf(filename, "Final_Conditions.txt"); break;
        default:
            printf("\nInvalid File choice!\n");
            return;
    }

    sprintf(filepath, "%s/%s", folder, filename);
    readTextFile(filepath);
}

void readTextFile(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("\n[ERROR] File not found: %s\n", filepath);
        return;
    }
    printf("\n--- FILE CONTENT: %s ---\n", filepath);
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    fclose(file);
    printf("\n----------------------------------------\n");
}
