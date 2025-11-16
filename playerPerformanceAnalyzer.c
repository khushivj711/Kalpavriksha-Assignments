#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define TotalTeams 10
#define TotalPlayers 200

typedef struct MyPlayer {
    int id;
    const char *name;
    const char *team;
    const char *role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct MyPlayer* next;
} MyPlayer;

typedef struct {
    int id;
    const char* name;
    int totalPlayers;
    float averageBattingStrikeRate;
    MyPlayer* playerHead;
} MyTeams;

typedef struct {
    MyPlayer* player;
    int teamIndex;
} HeapNode;

MyTeams team[TotalTeams];

void initializeData();
void runPerformanceAnalyzer();
void updateTeamAverageStrikeRate(MyTeams* team);
int searchTeamById(int id);

void addPlayerToTeam();
void displayPlayersOfSpecificTeam();
void displayTeamsByAverageBattingStrikeRate();
void displayTopKPlayersOfASpecificTeamByRole();
void displayAllPlayersOfAllTeams();
void exitPerformanceAnalyzer();

int partition(MyPlayer* players[], int low, int high);
void quickSort(MyPlayer* players[], int low, int high, int numberOfPlayers);

void swapHeap(HeapNode* first, HeapNode* second);
void heapify(HeapNode heap[], int size, int index);


int main() {
    initializeData();
    runPerformanceAnalyzer();
    return 0;
}

void initializeData() {
    for(int index = 0; index < TotalTeams; index++) {
        team[index].id = index + 1;
        team[index].name = teams[index];
        team[index].totalPlayers = 0;
        team[index].averageBattingStrikeRate = 0.0;
        team[index].playerHead = NULL;
    }

    for(int index = 0; index < TotalPlayers; index++) {
        Player playerData = players[index];

        MyPlayer* p = (MyPlayer*)malloc(sizeof(MyPlayer));
        p->id = playerData.id;
        p->name = playerData.name;
        p->team = playerData.team;
        p->role = playerData.role;
        p->totalRuns = playerData.totalRuns;
        p->battingAverage = playerData.battingAverage;
        p->strikeRate = playerData.strikeRate;
        p->wickets = playerData.wickets;
        p->economyRate = playerData.economyRate;
        p->next = NULL;

        if(strcmp(p->role, "Batsman") == 0) {
            p->performanceIndex = (p->battingAverage * p->strikeRate) / 100;
        } else if(strcmp(p->role, "Bowler") == 0) {
            p->performanceIndex = (p->wickets * 2) + (100 - p->economyRate);
        } else {
            p->performanceIndex = ((p->battingAverage * p->strikeRate) / 100) + (p->wickets * 2);
        }

        for(int iterator = 0; iterator < TotalTeams; iterator++) {
            if(strcmp(team[iterator].name, p->team) == 0) {
                p->next = team[iterator].playerHead;
                team[iterator].playerHead = p;
                team[iterator].totalPlayers++;
                break;
            }
        }
    }
}

void runPerformanceAnalyzer()
{
    while(1) {
        printf("=======================================================================================\n");
        printf("ICC ODI Player Performance Analyzer \n");
        printf("=======================================================================================\n");
        int choice;
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of Specific Role Across All Teams by Performance Index\n");
        printf("6. Exit\n");

        printf("=======================================================================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                addPlayerToTeam();
                break;
            case 2:
                displayPlayersOfSpecificTeam();
                break;
            case 3:
                displayTeamsByAverageBattingStrikeRate();
                break;
            case 4:
                displayTopKPlayersOfASpecificTeamByRole();
                break;
            case 5:
                displayAllPlayersOfAllTeams();
                break;
            case 6:
                exitPerformanceAnalyzer();
                break;
            default:
                printf("Invalid choice\n");
        }
    }
}

void updateTeamAverageStrikeRate(MyTeams* team) {
    float totalStrikeRate = 0.0;
    int count = 0; 

    MyPlayer* temp = team->playerHead;

    while(temp != NULL) {
        if(strcmp(temp->role, "Batsman") == 0 || strcmp(temp->role, "All-Rounder") == 0) {
            totalStrikeRate += temp->strikeRate;
            count++;
        }
        temp = temp->next;
    }

    team->averageBattingStrikeRate = (count > 0) ? totalStrikeRate / count : 0.0;
}

int searchTeamById(int id) {
    int low = 0;
    int high = TotalTeams - 1;

    while(low <= high) {
        int mid = low + (high - low) / 2;
        if(team[mid].id == id) {
            return mid;
        } else if(team[mid].id > id) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return -1;
}

void addPlayerToTeam() {
    int teamId;
    printf("Enter Team ID To Add Player: ");
    scanf("%d", &teamId);

    if(teamId < 1 || teamId > TotalTeams) {
        printf("Invalid Team ID\n");
        return;
    }

    int index = searchTeamById(teamId);

    if(index == -1) {
        printf("Team not found\n");
        return;
    }

    MyTeams* selectedTeam = &team[index];

    MyPlayer *newPlayer = (MyPlayer *)malloc(sizeof(MyPlayer));
    if(!newPlayer) {
        printf("Memory allocation failed\n");
        return;
    }

    printf("Enter Player Details: \n");

    printf("Player ID: ");
    scanf("%d", &newPlayer->id);
    getchar();

    char nameBuffer[50];
    printf("Name: ");
    fgets(nameBuffer, sizeof(nameBuffer), stdin);
    nameBuffer[strcspn(nameBuffer, "\n")] = '\0';
    newPlayer->name = strdup(nameBuffer);

    newPlayer->team = selectedTeam->name;

    int roleChoice;
    printf("Role (1: Batsman 2: Bowler 3: All-Rounder): ");
    scanf("%d", &roleChoice);

    char* role;

    if(roleChoice == 1) {
        role = "Batsman";
    } else if(roleChoice == 2) {
        role = "Bowler";
    } else if(roleChoice == 3) {
        role = "All-Rounder";
    } else {
        printf("Invalid role choice\n");
        free(newPlayer);
        return;
    }

    newPlayer->role = role;

    printf("Total Runs: ");
    scanf("%d", &newPlayer->totalRuns);

    printf("Batting Average: ");
    scanf("%f", &newPlayer->battingAverage);

    printf("Strike Rate: ");
    scanf("%f", &newPlayer->strikeRate);

    printf("Wickets: ");
    scanf("%d", &newPlayer->wickets);

    printf("Economy Rate: ");
    scanf("%f", &newPlayer->economyRate);

    if (strcmp(newPlayer->role, "Batsman") == 0) {
        newPlayer->performanceIndex = (newPlayer->battingAverage * newPlayer->strikeRate) / 100;
    } else if (strcmp(newPlayer->role, "Bowler") == 0) {
        newPlayer->performanceIndex = (newPlayer->wickets * 2) + (100 - newPlayer->economyRate);
    } else {
        newPlayer->performanceIndex = ((newPlayer->battingAverage * newPlayer->strikeRate) / 100) + (newPlayer->wickets * 2);
    }

    newPlayer->next = selectedTeam->playerHead;
    selectedTeam->playerHead = newPlayer;
    selectedTeam->totalPlayers++;

    printf("Player added successfully to Team %s!\n", selectedTeam->name);
}

void displayPlayersOfSpecificTeam() {
    int teamId;
    printf("Enter Team ID: ");
    scanf("%d", &teamId);

    if(teamId < 1 || teamId > TotalTeams) {
        printf("Invalid Team ID\n");
        return;
    }

    int index = searchTeamById(teamId);

    if (index == -1) {
        printf("Team not found\n");
        return;
    }

    MyTeams selectedTeam = team[index];
    MyPlayer* current = selectedTeam.playerHead;

    printf("Players of Team %s:\n", selectedTeam.name);

    printf("=======================================================================================================\n");
    printf("%-5s %-20s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("=======================================================================================================\n");

    if(current == NULL) {
        printf("No players found for this team\n");
        return;
    }

    float totalStrikeRate = 0.0;
    int countForAvg = 0;

    while(current != NULL) {
        printf("%-5d %-20s %-15s %-8d %-8.2f %-8.2f %-8d %-8.2f %-12.2f\n", current->id, current->name, current->role, current->totalRuns, current->battingAverage, current->strikeRate, current->wickets, current->economyRate, current->performanceIndex);

        current = current->next;
    }

    updateTeamAverageStrikeRate(&selectedTeam);

    printf("Total Players: %d\n", selectedTeam.totalPlayers);
    printf("Average Batting Strike Rate: %f\n", selectedTeam.averageBattingStrikeRate);
}

void displayTeamsByAverageBattingStrikeRate() {
    for(int index = 0; index < TotalTeams; index++) {
        updateTeamAverageStrikeRate(&team[index]);
    }

    MyTeams sortedTeam[TotalTeams];

    for(int index = 0; index < TotalTeams; index++) {
        sortedTeam[index] = team[index];
    }

    for(int index = 0; index < TotalTeams - 1; index++) {
        for(int iterator = index + 1; iterator < TotalTeams; iterator++) {
            if(sortedTeam[index].averageBattingStrikeRate < sortedTeam[iterator].averageBattingStrikeRate) {
                MyTeams temp = sortedTeam[index];
                sortedTeam[index] = sortedTeam[iterator];
                sortedTeam[iterator] = temp;
            }
        }
    }

    printf("Team Sorted By Average Batting Strike Rate\n");
    printf("===============================================================\n");
    printf("%-5s %-25s %-20s %-5s\n", "ID", "Name", "Avg Bat SR", "Total Players");
    printf("===============================================================\n");

    for(int index = 0; index < TotalTeams; index++) {
        printf("%-5d %-25s %-20.2f %-5d\n", sortedTeam[index].id, sortedTeam[index].name, sortedTeam[index].averageBattingStrikeRate, sortedTeam[index].totalPlayers);
    }

    printf("===============================================================\n");
}

int partition(MyPlayer* players[], int low, int high) {
    float pivot = players[high]->performanceIndex;
    int index = low - 1;

    for(int iterator = low; iterator < high; iterator++) {
        if(players[iterator]->performanceIndex > pivot) {
            index++;
            MyPlayer* temp = players[index];
            players[index] = players[iterator];
            players[iterator] = temp;
        }
    }

    MyPlayer* temp = players[index + 1];
    players[index + 1] = players[high];
    players[high] = temp;

    return index + 1;
}

void quickSort(MyPlayer* players[], int low, int high, int numberOfPlayers) {
    if(low < high) {
        int pivotIndex = partition(players, low, high);
        int leftCount = pivotIndex - low + 1;

        if(leftCount == numberOfPlayers) {
            return;
        } else if(leftCount > numberOfPlayers) {
            quickSort(players, low, pivotIndex - 1, numberOfPlayers);
        } else {
            quickSort(players, pivotIndex + 1, high, numberOfPlayers - leftCount);
        }
    }
}

void displayTopKPlayersOfASpecificTeamByRole() {
    int teamId, roleId, numberOfPlayers;
    printf("Enter Team ID: ");
    scanf("%d", &teamId);

    if (teamId < 1 || teamId > TotalTeams) {
        printf("Invalid Team ID\n");
        return;
    }

    printf("Enter Role (1: Batsman 2: Bowler 3: All-Rounder): ");
    scanf("%d", &roleId);

    char* role;
    if(roleId == 1) {
        role = "Batsman";
    } else if(roleId == 2) {
        role = "Bowler";
    } else if(roleId == 3) {
        role = "All-Rounder";
    } else {
        printf("Invalid rold Id entered\n");
    }

    printf("Enter number of players: ");
    scanf("%d", &numberOfPlayers);

    MyTeams selectedTeam = team[teamId - 1];
    MyPlayer *current = selectedTeam.playerHead;

    MyPlayer* rolePlayers[200];
    int count = 0;

    while(current != NULL) {
        if(strcmp(current->role, role) == 0) {
            rolePlayers[count++] = current;
        }
        current = current->next;
    }

    if(count == 0) {
        printf("No players found of %s role in Team %s\n", role, selectedTeam.name);
        return;
    }

    if(numberOfPlayers > count) {
        numberOfPlayers = count;
    }

    quickSort(rolePlayers, 0, count - 1, numberOfPlayers);

    printf("Top %d %s of Team %s\n", numberOfPlayers, role, selectedTeam.name);

    printf("=======================================================================================================\n");
    printf("%-5s %-20s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("=======================================================================================================\n");

    for(int index = 0; index < count && index < numberOfPlayers; index++) {
        MyPlayer* player = rolePlayers[index];
        printf("%-5d %-20s %-15s %-8d %-8.2f %-8.2f %-8d %-8.2f %-12.2f\n", player->id, player->name, player->role, player->totalRuns, player->battingAverage, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);
    }

    printf("=======================================================================================================\n");
}

void swapHeap(HeapNode* first, HeapNode* second) {
    HeapNode temp = *first;
    *first = *second;
    *second = temp;
}

void heapify(HeapNode heap[], int size, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if(left < size && heap[left].player->performanceIndex > heap[largest].player->performanceIndex) {
        largest = left;
    }

    if(right < size && heap[right].player->performanceIndex > heap[largest].player->performanceIndex) {
        largest = right;
    }

    if(largest != index) {
        swapHeap(&heap[index], &heap[largest]);
        heapify(heap, size, largest);
    }
}

void displayAllPlayersOfAllTeams() {
    int roleId;
    printf("Enter Role (1: Batsman 2: Bowler 3: All-Rounder): ");
    scanf("%d", &roleId);

    char* role;
    if(roleId == 1) {
        role = "Batsman";
    } else if(roleId == 2) {
        role = "Bowler";
    } else if(roleId == 3) {
        role = "All-Rounder";
    } else {
        printf("Invalid choice for role.\n");
        return;
    }

    MyPlayer* rolePlayers[TotalTeams];
    
    for(int index = 0; index < TotalTeams; index++) {
        MyPlayer* current = team[index].playerHead;
        MyPlayer* filtered = NULL;

        while(current != NULL) {
            if(strcmp(current->role, role) == 0) {
                MyPlayer* node = current;
                node->next = filtered;
                filtered = node;
            }
            current = current->next;
        }

        MyPlayer* sorted = NULL;
        MyPlayer* p = filtered;

        while(p) {
            MyPlayer* next = p->next;

            if(!sorted || p->performanceIndex > sorted->performanceIndex) {
                p->next = sorted;
                sorted = p;
            } else {
                MyPlayer* temp = sorted;

                while(temp->next && temp->next->performanceIndex > p->performanceIndex) {
                    temp = temp->next;
                }
                p->next = temp->next;
                temp->next = p;
            }
            p = next;
        }
        rolePlayers[index] = sorted;
    }

    HeapNode heap[TotalTeams];
    int heapSize = 0;

    for(int index = 0; index < TotalTeams; index++) {
        if(rolePlayers[index]) {
            heap[heapSize].player = rolePlayers[index];
            heap[heapSize].teamIndex = index;
            heapSize++;
        }
    }

    for(int index = heapSize / 2 - 1; index >= 0; index--) {
        heapify(heap, heapSize, index);
    }

    printf("%s of all teams\n", role);
    printf("=======================================================================================================\n");
    printf("%-5s %-25s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("=======================================================================================================\n");

    while(heapSize > 0) {
        HeapNode top = heap[0];

        MyPlayer* player = top.player;

        printf("%-5d %-25s %-15s %-8d %-8.2f %-8.2f %-8d %-8.2f %-12.2f\n", player->id, player->name, player->role, player->totalRuns, player->battingAverage, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);

        rolePlayers[top.teamIndex] = rolePlayers[top.teamIndex]->next;

        if(rolePlayers[top.teamIndex]) {
            heap[0].player = rolePlayers[top.teamIndex];
            heap[0].teamIndex = top.teamIndex;
        } else {
            heap[0] = heap[heapSize - 1];
            heapSize--;
        }

        heapify(heap, heapSize, 0);
    }

    printf("=======================================================================================================\n");
}

void exitPerformanceAnalyzer() {
    for(int index = 0; index < TotalTeams; index++) {
        MyPlayer* temp = team[index].playerHead;

        while(temp != NULL) {
            MyPlayer* next = temp->next;
            free(temp);
            temp = next;
        }
        team[index].playerHead = NULL;
    }

    printf("Exiting the program\n");
    exit(0);
}