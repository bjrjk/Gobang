#include <string>
#include <cstdio>
#include <cstdlib>
#include "jsoncpp/json.h"
#include "gobang.h"
#include "exec.h"

ChessPiece grid[SIZE][SIZE];
const char chessPieceChar[] = " XO";
const char chessPieceCurrentChar[] = " *#";
int curPlayerX = -1, curPlayerY = -1;
int curRobotX = -1, curRobotY = -1;
int timeout = 15;

Json::Value message;

void clearScreen() {
    printf("\033c");
    printf("Configurations: \n");
    printf("  - Current timeout: %ds\n", timeout);
    printf("\n");
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
        char c;
        c = chessPieceChar[arr[i]];
        if (curRobotX == line && curRobotY == i) c = chessPieceCurrentChar[1];
        if (curPlayerX == line && curPlayerY == i) c = chessPieceCurrentChar[2];
        printf("|%c", c);
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

        while (1) {
            printf("Please input placement position (e.g. H7):");
            if (scanf(" %c%d", &columnC, &row) != 2) return false;
            if (columnC < 'A' || columnC > 'O') return false;
            if (row < 0 || row > 14) return false;
            column = columnC - 'A';

            if (grid[row][column] != EMPTY) {
                clearScreen();
                printf("On %c%d already placed chess!\n", columnC, row);
                displayGrid();
                continue;
            }
            grid[row][column] = PLAYER;
            break;
        }
        
        curPlayerX = row;
        curPlayerY = column;
    }
    
    Json::Value placement;
    placement["x"] = row;
    placement["y"] = column;
    message["requests"].append(placement);
    return true;
}
bool robot() {
    message["type"] = 0;
    std::string serializedJSONMessage = getJSONText(message);
    serializedJSONMessage.append("\n");
    printf("Debug: %s", serializedJSONMessage.c_str());
    
    char responseBuf[0x1000];
    unsigned long responseSize;
    int retValue = createProcessWithGivenStdinAndGetStdout(
        "./gobang",
        serializedJSONMessage.c_str(), serializedJSONMessage.size(),
        responseBuf, sizeof(responseBuf) - 1, &responseSize,
        timeout, 1
    );
    if (!retValue) return false;
    responseBuf[responseSize] = '\0';

    Json::Reader reader;
    Json::Value response;
    reader.parse(responseBuf, response);

    int x, y;
    x = response["response"]["x"].asInt();
    y = response["response"]["y"].asInt();

    grid[x][y] = BOT;
    curRobotX = x;
    curRobotY = y;

    Json::Value placement;
    placement["x"] = x;
    placement["y"] = y;
    message["responses"].append(placement);

    return true;
}
bool judgeWins() {
    message["type"] = 1;
    std::string serializedJSONMessage = getJSONText(message);
    serializedJSONMessage.append("\n");
    
    char responseBuf[0x1000];
    unsigned long responseSize;
    int retValue = createProcessWithGivenStdinAndGetStdout(
        "./gobang",
        serializedJSONMessage.c_str(), serializedJSONMessage.size(),
        responseBuf, sizeof(responseBuf) - 1, &responseSize,
        0, 0
    );
    if (!retValue) return false;
    responseBuf[responseSize] = '\0';

    Json::Reader reader;
    Json::Value response;
    reader.parse(responseBuf, response);

    int status = response["status"].asInt();
    if (status == 1) {
        clearScreen();
        displayGrid();
        printf("%s\n", response["prompt"].asCString());
        return false;
    }
    return true;
}
void init() {
    initJSONMessage();

    char * timeoutEnv = std::getenv("TIMEOUT");
    if (timeoutEnv) timeout = std::strtol(timeoutEnv, NULL, 10);
}
int main() {
    bool flag;
    int choice;

    init();

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
        if (!judgeWins()) return 0;
        choice = (choice + 1) % 2;
    }
}