#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BOARD_SIZE 3

// Player symbols
#define PLAYER_X 'X'
#define PLAYER_O 'O'
#define EMPTY_CELL ' '

float q_values[2][BOARD_SIZE][BOARD_SIZE]; // Q-values for the AI agent

// Function to load Q-values from a file
void load_q_values(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file != NULL) {
        fread(q_values, sizeof(float), 2 * BOARD_SIZE * BOARD_SIZE, file);
        fclose(file);
    } else {
        printf("Error: Unable to open file for reading.\n");
        exit(1);
    }
}

// Function to save Q-values to a file
void save_q_values(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(q_values, sizeof(float), 2 * BOARD_SIZE * BOARD_SIZE, file);
        fclose(file);
    } else {
        printf("Error: Unable to open file for writing.\n");
    }
}

// Function to print the board
void print_board(char board[][BOARD_SIZE]) {
    printf("   0   1   2\n");
    printf("  -----------\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d |", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c |", board[i][j]);
        }
        printf("\n");
        printf("  -----------\n");
    }
}

// Function to check if the game is over
int game_over(char board[][BOARD_SIZE]) {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] != EMPTY_CELL && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return (board[i][0] == PLAYER_X) ? 1 : 2; // Player X wins (1), Player O wins (2)
    }
    // Check columns
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (board[0][j] != EMPTY_CELL && board[0][j] == board[1][j] && board[1][j] == board[2][j])
            return (board[0][j] == PLAYER_X) ? 1 : 2; // Player X wins (1), Player O wins (2)
    }
    // Check diagonals
    if (board[0][0] != EMPTY_CELL && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return (board[0][0] == PLAYER_X) ? 1 : 2; // Player X wins (1), Player O wins (2)
    if (board[0][2] != EMPTY_CELL && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return (board[0][2] == PLAYER_X) ? 1 : 2; // Player X wins (1), Player O wins (2)
    // Check for draw
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY_CELL)
                return 0; // Game continues
        }
    }
    return -1; // Draw
}

// Function to perform AI's action
void ai_action(char board[][BOARD_SIZE], char ai_symbol) {
    int row, col;
    float max_q_value = -9999.0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY_CELL && q_values[ai_symbol == PLAYER_X ? 0 : 1][i][j] > max_q_value) {
                max_q_value = q_values[ai_symbol == PLAYER_X ? 0 : 1][i][j];
                row = i;
                col = j;
            }
        }
    }
    board[row][col] = ai_symbol;
}

// Function to update Q-values based on game outcome
void update_q_values(char board[][BOARD_SIZE], char player_symbol, char ai_symbol, int outcome) {
    float reward;
    if (outcome == 1) { // Player X wins
        reward = (player_symbol == PLAYER_X) ? 1.0 : -1.0;
    } else if (outcome == 2) { // Player O wins
        reward = (player_symbol == PLAYER_O) ? 1.0 : -1.0;
    } else { // Draw
        reward = 0.0;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY_CELL) {
                q_values[ai_symbol == PLAYER_X ? 0 : 1][i][j] += reward;
            }
        }
    }
}

int main() {
    srand(time(NULL)); // Seed for random number generation
    char board[BOARD_SIZE][BOARD_SIZE];
    char player_symbol, ai_symbol;

    // Load Q-values from file
    load_q_values("q_values.dat");

    // Ask the user to choose X or O
    printf("Choose X or O (X goes first): ");
    scanf("%c", &player_symbol);
    getchar(); // Consume newline character

    if (player_symbol == PLAYER_X) {
        ai_symbol = PLAYER_O;
    } else if (player_symbol == PLAYER_O) {
        ai_symbol = PLAYER_X;
    } else {
        printf("Invalid input. Please choose X or O.\n");
        return 1;
    }

    // Initialize the board
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY_CELL;
        }
    }

    // Main game loop
    while (true) {
        // Player's turn
        if (player_symbol == PLAYER_X) {
            int row, col;
            printf("Your turn (enter row and column): ");
            scanf("%d %d", &row, &col);
            getchar(); // Consume newline character
            if (board[row][col] != EMPTY_CELL || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                printf("Invalid move. Try again.\n");
                continue;
            }
            board[row][col] = player_symbol;
        } else {
            // AI's turn
            ai_action(board, ai_symbol);
        }
        
        // Print the board after each move
        printf("Current board:\n");
        print_board(board);
        
        // Check if the game is over after each move
        int outcome = game_over(board);
        if (outcome == 1) {
            printf("Player X wins!\n");
            update_q_values(board, player_symbol, ai_symbol, 1);
            save_q_values("q_values.dat"); // Save Q-values after the game
            break;
        } else if (outcome == 2) {
            printf("Player O wins!\n");
            update_q_values(board, player_symbol, ai_symbol, 2);
            save_q_values("q_values.dat"); // Save Q-values after the game
            break;
        } else if (outcome == -1) {
            printf("It's a draw!\n");
            update_q_values(board, player_symbol, ai_symbol, -1);
            save_q_values("q_values.dat"); // Save Q-values after the game
            break;
        }
        
        // Switch player turns
        if (player_symbol == PLAYER_X) {
            player_symbol = PLAYER_O;
            ai_symbol = PLAYER_X;
        } else {
            player_symbol = PLAYER_X;
            ai_symbol = PLAYER_O;
        }
    }

    return 0;
}


