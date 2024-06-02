#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARD_SIZE 3

// Q-learning parameters
#define LEARNING_RATE 0.1
#define DISCOUNT_FACTOR 0.9
#define EPSILON 0.1

// Reward values
#define WIN_REWARD 1
#define DRAW_REWARD 0
#define LOSS_REWARD -1

// Player symbols
#define PLAYER_X 'X'
#define PLAYER_O 'O'
#define EMPTY_CELL ' '

char board[BOARD_SIZE][BOARD_SIZE]; // Tic-Tac-Toe board

// Initialize the board
void initialize_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY_CELL;
        }
    }
}

// Print the board
void print_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Check if the game is over
bool game_over() {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] != EMPTY_CELL && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return true;
    }
    // Check columns
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (board[0][j] != EMPTY_CELL && board[0][j] == board[1][j] && board[1][j] == board[2][j])
            return true;
    }
    // Check diagonals
    if (board[0][0] != EMPTY_CELL && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return true;
    if (board[0][2] != EMPTY_CELL && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return true;
    // Check for draw
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY_CELL)
                return false;
        }
    }
    return true;
}

// Get available actions (empty cells)
void get_available_actions(int available_actions[][2], int* num_actions) {
    *num_actions = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY_CELL) {
                available_actions[*num_actions][0] = i;
                available_actions[*num_actions][1] = j;
                (*num_actions)++;
            }
        }
    }
}

// Perform a random action
void random_action(int* row, int* col) {
    int available_actions[BOARD_SIZE * BOARD_SIZE][2];
    int num_actions;
    get_available_actions(available_actions, &num_actions);
    int random_index = rand() % num_actions;
    *row = available_actions[random_index][0];
    *col = available_actions[random_index][1];
}

// Perform an action based on epsilon-greedy policy
void epsilon_greedy_action(char player, float q_values[][BOARD_SIZE][BOARD_SIZE], int* row, int* col) {
    int available_actions[BOARD_SIZE * BOARD_SIZE][2];
    int num_actions;
    get_available_actions(available_actions, &num_actions);
    
    if ((float)rand() / RAND_MAX < EPSILON) {
        // Random action
        random_action(row, col);
    } else {
        // Greedy action
        float max_q_value = -9999.0;
        for (int i = 0; i < num_actions; i++) {
            int r = available_actions[i][0];
            int c = available_actions[i][1];
            if (q_values[player == PLAYER_X ? 0 : 1][r][c] > max_q_value) {
                max_q_value = q_values[player == PLAYER_X ? 0 : 1][r][c];
                *row = r;
                *col = c;
            }
        }
    }
}

// Update Q-values based on the outcome of the game
void update_q_values(float q_values[][BOARD_SIZE][BOARD_SIZE], char winner) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY_CELL)
                continue;
            if (winner == PLAYER_X)
                q_values[0][i][j] += LEARNING_RATE * (WIN_REWARD - q_values[0][i][j]);
            else if (winner == PLAYER_O)
                q_values[1][i][j] += LEARNING_RATE * (WIN_REWARD - q_values[1][i][j]);
            else
                q_values[0][i][j] += LEARNING_RATE * (DRAW_REWARD - q_values[0][i][j]);
        }
    }
}

// Play a game between two Q-learning agents
void play_game(float q_values[][BOARD_SIZE][BOARD_SIZE]) {
    initialize_board();
    char current_player = PLAYER_X;
    int num_moves = 0;

    while (!game_over()) {
        int row, col;
        if (current_player == PLAYER_X) {
            // Player X's turn
            epsilon_greedy_action(PLAYER_X, q_values, &row, &col);
        } else {
            // Player O's turn
            epsilon_greedy_action(PLAYER_O, q_values, &row, &col);
        }
        board[row][col] = current_player;
        num_moves++;
        current_player = (current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }

    // Determine the winner and update Q-values
    char winner = EMPTY_CELL;
    if (game_over()) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[i][0] != EMPTY_CELL && board[i][0] == board[i][1] && board[i][1] == board[i][2])
                winner = board[i][0];
        }
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[0][j] != EMPTY_CELL && board[0][j] == board[1][j] && board[1][j] == board[2][j])
                winner = board[0][j];
        }
        if (board[0][0] != EMPTY_CELL && board[0][0] == board[1][1] && board[1][1] == board[2][2])
            winner = board[0][0];
        if (board[0][2] != EMPTY_CELL && board[0][2] == board[1][1] && board[1][1] == board[2][0])
            winner = board[0][2];
    }
    update_q_values(q_values, winner);
}

int main() {
    srand(time(NULL)); // Seed for random number generation

    float q_values[2][BOARD_SIZE][BOARD_SIZE] = {0}; // Q-values for player X and player O

    // Train the Q-learning agent by playing multiple games
    int num_episodes = 10000;
    for (int episode = 0; episode < num_episodes; episode++) {
        play_game(q_values);
    }

    // Print the learned Q-values
    printf("Learned Q-values for Player X:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%.2f ", q_values[0][i][j]);
        }
        printf("\n");
    }

    printf("\nLearned Q-values for Player O:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%.2f ", q_values[1][i][j]);
        }
        printf("\n");
    }

    return 0;
}
