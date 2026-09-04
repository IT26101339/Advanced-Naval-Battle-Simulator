#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define G 9.81
#define PI 3.14159265
#define MAX 100

typedef struct {
    int id;
    char type, name[40], gun[40];
    double x, y, vmin, vmax, tmin, tmax, hitTime;
    int alive;
    double impact;
} Escort;

typedef struct {
    char type, name[40], gun[40];
    double x, y, vmax;
} Battleship;

/* Random number */
double rnd(double a, double b)
{
    return a + (double)rand() / RAND_MAX * (b - a);
}

/* Distance */
double dist(double x1, double y1, double x2, double y2)
{
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

/* Projectile range */
double range(double v, double angle)
{
    return v*v*sin(2*angle*PI/180.0)/G;
}

/* Find if an E can hit B and calculate fastest hit */
int canHit(Escort *e, double d, double *time)
{
    double a, v, best = 999999;

    for(a = e->tmin; a <= e->tmax; a += 0.1) {
        double s = sin(2*a*PI/180.0);

        if(s <= 0) continue;

        v = sqrt(d * G / s);

        if(v >= e->vmin && v <= e->vmax) {
            double rad = a * PI / 180.0;
            double t = d / (v * cos(rad));

            if(t < best)
                best = t;
        }
    }

    if(best < 999999) {
        *time = best;
        return 1;
    }

    return 0;
}

/* Save initial conditions */
void saveInitial(Escort e[], int n, Battleship b, double D)
{
    FILE *f = fopen("Initial_Conditions.txt", "w");
    int i;

    fprintf(f, "PART 1-A INITIAL CONDITIONS\n");
    fprintf(f, "Battlefield: (0,0) to (%.2f,%.2f)\n", D, D);

    fprintf(f, "\nBATTLESHIP\n");
    fprintf(f, "Type: %c\nName: %s\nGun: %s\n", b.type,b.name,b.gun);
    fprintf(f, "Position: %.2f %.2f\nVmax: %.2f\n",
            b.x,b.y,b.vmax);

    fprintf(f, "\nESCORT SHIPS\n");

    for(i=0;i<n;i++)
        fprintf(f,
        "E%d [%c %s] Gun:%s Pos:(%.2f,%.2f) "
        "Vmin:%.2f Vmax:%.2f Angle:%.2f-%.2f Impact:%.2f\n",
        e[i].id,e[i].type,e[i].name,e[i].gun,
        e[i].x,e[i].y,e[i].vmin,e[i].vmax,
        e[i].tmin,e[i].tmax,e[i].impact);

    fclose(f);
}

/* Save hit information */
void saveHits(Escort e[], int n)
{
    FILE *f = fopen("simulation_data.txt", "w");
    int i;

    fprintf(f, "PART 1-A BATTLE RESULTS\n\n");

    for(i=0;i<n;i++)
        if(!e[i].alive)
            fprintf(f,"E%d  Time to hit: %.3f seconds\n",
                    e[i].id,e[i].hitTime);

    fclose(f);
}

/* Save final conditions */
void saveFinal(Escort e[], int n, int sunk, int killer)
{
    FILE *f = fopen("Final_Conditions.txt", "w");
    int i;

    fprintf(f,"PART 1-A FINAL CONDITIONS\n\n");

    if(sunk)
        fprintf(f,"Battleship: DESTROYED\nSunk by: E%d\n",killer);
    else
        fprintf(f,"Battleship: SURVIVED\n");

    fprintf(f,"\nESCORT SHIPS\n");

    for(i=0;i<n;i++)
        fprintf(f,"E%d: %s\n",
                e[i].id,e[i].alive?"Active":"Destroyed");

    fclose(f);
}

/* Instructions */
void instructions()
{
    printf("\n------------- Instructions -------------\n");
    printf("1. Start Part 1-A simulation.\n");
    printf("2. Select a Battleship type.\n");
    printf("3. Enter battlefield size and B position.\n");
    printf("4. Enter B maximum shell velocity.\n");
    printf("5. Enter number of escort ships.\n");
    printf("6. E ships are randomly generated.\n");
    printf("7. B and E attack according to projectile range.\n");
    printf("8. Results are saved in text files.\n");
    printf("----------------------------------------\n");
}

/* Read previous statistics */
void statistics()
{
    FILE *f = fopen("simulation_data.txt", "r");
    char line[150];

    printf("\n========== SIMULATION STATISTICS ==========\n");

    if(f == NULL) {
        printf("No previous simulation found.\n");
        return;
    }

    while(fgets(line,sizeof(line),f))
        printf("%s",line);

    fclose(f);
    printf("===========================================\n");
}

int main()
{
    Battleship b;
    Escort e[MAX];

    int choice,n,i;
    int destroyed=0, sunk=0, killer=-1;

    double D, d, bRange, eTime, battleTime=0;

    srand((unsigned)time(NULL));

    while(1) {

        printf("\n==========================================\n");
        printf("          NAVAL BATTLE SIMULATOR\n");
        printf("==========================================\n");
        printf("1. Start part 1-A simulation\n");
        printf("2. View instructions\n");
        printf("3. View simulation Statistics\n");
        printf("4. Exit\n");
        printf("enter your choice: ");
        scanf("%d",&choice);

        if(choice==2) {
            instructions();
            continue;
        }

        if(choice==3) {
            statistics();
            continue;
        }

        if(choice==4)
            break;

        if(choice!=1) {
            printf("Invalid choice!\n");
            continue;
        }

        /* Battleship */
        printf("\n============ START SIMULATION ============\n");
        printf("Available Battleships:\n");
        printf("U - USS Iowa (BB-61)\n");
        printf("M - MS King George V\n");
        printf("R - Richelieu\n");
        printf("S - Sovetsky Soyuz-class\n");

        printf("\nEnter Battleship type (U/M/R/S): ");
        scanf(" %c",&b.type);

        switch(b.type) {
            case 'U':
                sprintf(b.name,"USS Iowa (BB-61)");
                sprintf(b.gun,"50-caliber Mark 7 gun");
                break;

            case 'M':
                sprintf(b.name,"MS King George V");
                sprintf(b.gun,"356 mm Mark VII gun");
                break;

            case 'R':
                sprintf(b.name,"Richelieu");
                sprintf(b.gun,"15 inch Mle 1935 gun");
                break;

            case 'S':
                sprintf(b.name,"Sovetsky Soyuz-class");
                sprintf(b.gun,"16 inch B-37 gun");
                break;

            default:
                printf("Invalid type!\n");
                continue;
        }

        printf("Enter battlefield size: ");
        scanf("%lf",&D);

        printf("Enter Battleship X Y Velocity: ");
        scanf("%lf %lf %lf",&b.x,&b.y,&b.vmax);

        printf("\nBattleship setup completed successfully!\n");

        printf("Enter number of Escort Ships: ");
        scanf("%d",&n);

        if(n<1 || n>MAX || D<=0 || b.vmax<=0) {
            printf("Invalid input!\n");
            continue;
        }

        /* Generate E ships */
        for(i=0;i<n;i++) {

            e[i].id=i+1;
            e[i].type='A'+rand()%5;
            e[i].x=rnd(0,D);
            e[i].y=rnd(0,D);
            e[i].alive=1;
            e[i].hitTime=0;

            switch(e[i].type) {

                case 'A':
                    sprintf(e[i].name,"1936A-class Destroyer");
                    sprintf(e[i].gun,"SK C/34 naval gun");
                    e[i].impact=.08;
                    e[i].tmax=20;
                    e[i].vmax=1.2*b.vmax;
                    break;

                case 'B':
                    sprintf(e[i].name,"Gabbiano-class Corvette");
                    sprintf(e[i].gun,"L/47 dual-purpose gun");
                    e[i].impact=.06;
                    e[i].tmax=30;
                    e[i].vmax=rnd(b.vmax*.6,b.vmax*.95);
                    break;

                case 'C':
                    sprintf(e[i].name,"Matsu-class Destroyer");
                    sprintf(e[i].gun,"Type 89 dual-purpose gun");
                    e[i].impact=.07;
                    e[i].tmax=25;
                    e[i].vmax=rnd(b.vmax*.6,b.vmax*.95);
                    break;

                case 'D':
                    sprintf(e[i].name,"F-class Escort Ships");
                    sprintf(e[i].gun,"SK C/32 naval gun");
                    e[i].impact=.05;
                    e[i].tmax=50;
                    e[i].vmax=rnd(b.vmax*.6,b.vmax*.95);
                    break;

                case 'E':
                    sprintf(e[i].name,"Japanese Kaibokan");
                    sprintf(e[i].gun,"(4.7 inch) naval guns");
                    e[i].impact=.04;
                    e[i].tmax=70;
                    e[i].vmax=rnd(b.vmax*.6,b.vmax*.95);
                    break;
            }

            /* Random minimum angle */
            e[i].tmin=rnd(5,20);

            /* Make sure upper angle is <= 90 */
            if(e[i].tmin+e[i].tmax>90)
                e[i].tmin=90-e[i].tmax;

            /* Random minimum velocity */
            e[i].vmin=rnd(b.vmax*.2,b.vmax*.5);

            if(e[i].vmax<=e[i].vmin)
                e[i].vmin=e[i].vmax*.5;
        }

        printf("\n%d Escort Ships generated successfully!\n",n);

        saveInitial(e,n,b,D);
        printf("Initial_Conditions.txt saved!\n");

        /* Battleship attacks */
        printf("\n================ BATTLESHIP ATTACK ================\n");

        bRange=range(b.vmax,45);

        printf("Battleship Maximum Attack Range: %.2f m\n",bRange);

        for(i=0;i<n;i++) {

            d=dist(b.x,b.y,e[i].x,e[i].y);

            printf("\nEscort Ship %d [E%c (%s)]\n",
                   e[i].id,e[i].type,e[i].name);

            printf("Distance from Battleship: %.2f m\n",d);

            if(d<=bRange) {

                double rad=45*PI/180.0;

                e[i].alive=0;
                e[i].hitTime=d/(b.vmax*cos(rad));

                destroyed++;

                if(e[i].hitTime>battleTime)
                    battleTime=e[i].hitTime;

                printf("Result: HIT - Escort Ship destroyed\n");
                printf("Firing Angle: 45.00 degrees\n");
                printf("Time to hit: %.2f seconds\n",
                       e[i].hitTime);
            }
            else
                printf("Result: MISSED - Out of range\n");
        }

        printf("\nTotal Escort Ships destroyed by Battleship: %d\n",
               destroyed);

        /* Escort counter attack */
        printf("\n================ ESCORT SHIPS COUNTER-ATTACK ================\n");

        for(i=0;i<n;i++) {

            if(!e[i].alive) {
                printf("\nEscort Ship %d [E%c (%s)]\n",
                       e[i].id,e[i].type,e[i].name);
                printf("Status: Destroyed by Battleship\n");
                printf("Cannot attack.\n");
                continue;
            }

            d=dist(e[i].x,e[i].y,b.x,b.y);

            printf("\nEscort Ship %d [E%c (%s)]\n",
                   e[i].id,e[i].type,e[i].name);

            printf("Status: Active\n");
            printf("Distance from Battleship: %.2f m\n",d);
            printf("Attacking Battleship...\n");

            if(canHit(&e[i],d,&eTime)) {

                printf("Result: HIT - Battleship destroyed\n");
                printf("Time to hit: %.2f seconds\n",eTime);

                if(eTime>battleTime)
                    battleTime=eTime;

                sunk=1;
                killer=e[i].id;
            }
            else
                printf("Result: MISSED - Battleship out of reach\n");
        }

        saveHits(e,n);
        printf("\nSimulation data saved to simulation_data.txt\n");

        if(sunk) {
            printf("\n========== BATTLESHIP DESTROYED! ==========\n");
            printf("Battleship was sunk by Escort Ship E%d\n",killer);
        }
        else {
            printf("\n========== BATTLESHIP SURVIVED! ==========\n");
            printf("Escort Ships destroyed by B: %d\n",destroyed);
            printf("Battle End Time: %.2f seconds\n",battleTime);
            printf("Battle ended successfully.\n");
        }

        saveFinal(e,n,sunk,killer);
        printf("\nFinal_Conditions.txt saved!\n");

        printf("\n------------- Simulation complete ------------\n");

        /* Reset counters for next simulation */
        destroyed=0;
        sunk=0;
        killer=-1;
        battleTime=0;
    }

    return 0;
}
