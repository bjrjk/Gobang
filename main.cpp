#include <string>
#include <cstdio>
#include "jsoncpp/json.h"
#include "gobang.h"

ChessPiece grid[SIZE][SIZE];
char chessPieceChar[] = " XO";
char chessPieceCurrentChar[] = " *#";

Json::Value message;

void clearScreen() {
    printf("\033c");
}
void printSeparatorLine() {
    printf("   ");
    for (int i = 0; i < SIZE; i++) {
        printf("--");
    }
    printf("-\n");
}
void printGridLine(int line, ChessPiece arr[]) {
    printf("%2d ", line);
    for (int i = 0; i < SIZE; i++) {
        printf("|%c", chessPieceChar[arr[i]]);
    }
    printf("|\n");
}
void printRowLableLine() {
    printf("   ");
    for (int i = 0; i < SIZE; i++) {
        printf(" %c", 'A' + i);
    }
    printf("\n");
}
void displayGrid() {
    for (int i = 0; i < SIZE; i++) {
        printSeparatorLine();
        printGridLine(i, grid[i]);
    }
    printSeparatorLine();
    printRowLableLine();
}
std::string getJSONText(Json::Value & json) {
    Json::FastWriter writer;
    return writer.write(json);
}
void initJSONMessage() {
    message["requests"] = Json::Value(Json::arrayValue);
    message["responses"] = Json::Value(Json::arrayValue);
}
bool player(bool inputFlag = true) {
    int row = -1, column = -1;
    char columnC;

    if (inputFlag) {
        clearScreen();
        displayGrid();
        printf("Please input placement position (e.g. H7):");
        scanf(" %c%d", &columnC, &row);
        if (columnC < 'A' || columnC > 'O') return false;
        if (row < 0 || row > 14) return false;
        column = columnC - 'A';

        grid[row][column] = PLAYER;
    }
    
    Json::Value placement;
    placement["x"] = row;
    placement["y"] = column;
    message["requests"].append(placement);
    return true;
}
bool robot() {
    return true;
}
int main() {
    bool flag;
    int choice;

    initJSONMessage();

    clearScreen();
    printf("Choose who goes first:\n");
    printf("0. Player\n");
    printf("1. Robot\n");
    if (scanf(" %d", &choice) != 1) return 1;
    if (choice != 0 && choice != 1) return 2;

    if (choice == 1) player(false);

    while (1) {
        if (choice == 0) flag = player();
        else if (choice == 1) flag = robot();
        if (!flag) return 0;
        choice = (choice + 1) % 2;
    }
}