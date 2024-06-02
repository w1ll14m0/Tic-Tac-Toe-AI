#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BOARD_SIZE 3
#define NUM_INSTANCES 100
#define NUM_GENERATIONS 500

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

char boards[NUM_INSTANCES][BOARD_SIZE][BOARD_SIZE]; // Tic-Tac-Toe boards for multiple instances
int current_players[NUM_INSTANCES]; // Current players for each instance
float q_values[NUM_INSTANCES][2][BOARD_SIZE][BOARD_SIZE]; // Q-values for each instance
int wins[NUM_INSTANCES] = {0}; // Number of wins for each instance

// Function to initialize the boards and Q-values for all instances
void initialize_instances() {
    for (int i = 0; i < NUM_INSTANCES; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                boards[i][j][k] = EMPTY_CELL;
                q_values[i][0][j][k] = 0;
                q_values[i][1][j][k] = 0;
            }
        }
        current_players[i] = PLAYER_X;
    }
}

// Check if a specific instance's game is over
bool game_over_instance(int instance) {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (boards[instance][i][0] != EMPTY_CELL && boards[instance][i][0] == boards[instance][i][1] && boards[instance][i][1] == boards[instance][i][2])
            return true;
    }
    // Check columns
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (boards[instance][0][j] != EMPTY_CELL && boards[instance][0][j] == boards[instance][1][j] && boards[instance][1][j] == boards[instance][2][j])
            return true;
    }
    // Check diagonals
    if (boards[instance][0][0] != EMPTY_CELL && boards[instance][0][0] == boards[instance][1][1] && boards[instance][1][1] == boards[instance][2][2])
        return true;
    if (boards[instance][0][2] != EMPTY_CELL && boards[instance][0][2] == boards[instance][1][1] && boards[instance][1][1] == boards[instance][2][0])
        return true;
    // Check for draw
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (boards[instance][i][j] == EMPTY_CELL)
                return false;
        }
    }
    return true;
}

// Get available actions (empty cells) for a specific instance
void get_available_actions_instance(int instance, int available_actions[][2], int* num_actions) {
    *num_actions = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (boards[instance][i][j] == EMPTY_CELL) {
                available_actions[*num_actions][0] = i;
                available_actions[*num_actions][1] = j;
                (*num_actions)++;
            }
        }
    }
}

// Perform a random action for a specific instance
void random_action_instance(int instance, int* row, int* col) {
    int available_actions[BOARD_SIZE * BOARD_SIZE][2];
    int num_actions;
    get_available_actions_instance(instance, available_actions, &num_actions);
    int random_index = rand() % num_actions;
    *row = available_actions[random_index][0];
    *col = available_actions[random_index][1];
}

// Perform an action based on epsilon-greedy policy for a specific instance
void epsilon_greedy_action_instance(int instance, float q_values_instance[][BOARD_SIZE][BOARD_SIZE], int* row, int* col) {
    int available_actions[BOARD_SIZE * BOARD_SIZE][2];
    int num_actions;
    get_available_actions_instance(instance, available_actions, &num_actions);
    
    if ((float)rand() / RAND_MAX < EPSILON) {
        // Random action
        random_action_instance(instance, row, col);
    } else {
        // Greedy action
        float max_q_value = -9999.0;
        for (int i = 0; i < num_actions; i++) {
            int r = available_actions[i][0];
            int c = available_actions[i][1];
            if (q_values_instance[current_players[instance] == PLAYER_X ? 0 : 1][r][c] > max_q_value) {
                max_q_value = q_values_instance[current_players[instance] == PLAYER_X ? 0 : 1][r][c];
                *row = r;
                *col = c;
            }
        }
    }
}

// Update Q-values based on the outcome of the game for a specific instance
void update_q_values_instance(int instance, char winner) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (boards[instance][i][j] == EMPTY_CELL)
                continue;
            if (winner == PLAYER_X)
                q_values[instance][0][i][j] += LEARNING_RATE * (WIN_REWARD - q_values[instance][0][i][j]);
            else if (winner == PLAYER_O)
                q_values[instance][1][i][j] += LEARNING_RATE * (WIN_REWARD - q_values[instance][1][i][j]);
            else
                q_values[instance][0][i][j] += LEARNING_RATE * (DRAW_REWARD - q_values[instance][0][i][j]);
        }
    }
}

// Function to save Q-values to a file for a specific instance
void save_q_values_instance(int instance, float q_values_instance[][BOARD_SIZE][BOARD_SIZE], const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(q_values_instance, sizeof(float), 2 * BOARD_SIZE * BOARD_SIZE, file);
        fclose(file);
    } else {
        printf("Error: Unable to open file for writing.\n");
    }
}

// Function to load Q-values from a file for a specific instance
void load_q_values_instance(int instance, float q_values_instance[][BOARD_SIZE][BOARD_SIZE], const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file != NULL) {
        fread(q_values_instance, sizeof(float), 2 * BOARD_SIZE * BOARD_SIZE, file);
        fclose(file);
    } else {
        printf("Error: Unable to open file for reading.\n");
    }
}


// Play a game between two Q-learning agents for a specific instance
void play_game_instance(int instance) {
    while (!game_over_instance(instance)) {
        int row, col;
        epsilon_greedy_action_instance(instance, q_values[instance], &row, &col);
        boards[instance][row][col] = current_players[instance];
        current_players[instance] = (current_players[instance] == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }

    // Determine the winner and update Q-values
    char winner = EMPTY_CELL;
    if (game_over_instance(instance)) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (boards[instance][i][0] != EMPTY_CELL && boards[instance][i][0] == boards[instance][i][1] && boards[instance][i][1] == boards[instance][i][2])
                winner = boards[instance][i][0];
        }
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (boards[instance][0][j] != EMPTY_CELL && boards[instance][0][j] == boards[instance][1][j] && boards[instance][1][j] == boards[instance][2][j])
                winner = boards[instance][0][j];
        }
        if (boards[instance][0][0] != EMPTY_CELL && boards[instance][0][0] == boards[instance][1][1] && boards[instance][1][1] == boards[instance][2][2])
            winner = boards[instance][0][0];
        if (boards[instance][0][2] != EMPTY_CELL && boards[instance][0][2] == boards[instance][1][1] && boards[instance][1][1] == boards[instance][2][0])
            winner = boards[instance][0][2];
    }
    if (winner == PLAYER_X)
        wins[instance]++;

    update_q_values_instance(instance, winner);
}

// Find the instance with the highest win rate
int find_best_instance() {
    int best_instance = 0;
    float max_win_rate = (float)wins[0] / NUM_GENERATIONS;
    for (int i = 1; i < NUM_INSTANCES; i++) {
        float win_rate = (float)wins[i] / NUM_GENERATIONS;
        if (win_rate > max_win_rate) {
            max_win_rate = win_rate;
            best_instance = i;
        }
    }
    return best_instance;
}

// Function to initialize the board for a specific instance
void initialize_board_instance(int instance) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            boards[instance][i][j] = EMPTY_CELL;
        }
    }
}

// Function to print the board for a specific instance
void print_board_instance(int instance) {
    printf("   0   1   2\n");
    printf("  -----------\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d |", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c |", boards[instance][i][j]);
        }
        printf("\n");
        printf("  -----------\n");
    }
}


// Display the full game of the best instance in the last generation
void display_best_instance_game() {
    int best_instance = find_best_instance();
    printf("\nBest instance's game (Player X vs Player O):\n");
    printf("Instance: %d\n", best_instance + 1);
    initialize_board_instance(best_instance);
    char current_player = PLAYER_X;
    while (!game_over_instance(best_instance)) {
        printf("\nCurrent board:\n");
        print_board_instance(best_instance);
        printf("Player %c's turn.\n", current_player);

        int row, col;
        if (current_player == PLAYER_X) {
            // Player X's turn
            epsilon_greedy_action_instance(best_instance, q_values[best_instance], &row, &col);
        } else {
            // Player O's turn
            epsilon_greedy_action_instance(best_instance, q_values[best_instance], &row, &col);
        }
        printf("Player %c chooses position (%d, %d).\n", current_player, row, col);
        boards[best_instance][row][col] = current_player;
        current_player = (current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }

    printf("\nFinal board:\n");
    print_board_instance(best_instance);

    // Determine the winner
    char winner = EMPTY_CELL;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (boards[best_instance][i][0] != EMPTY_CELL && boards[best_instance][i][0] == boards[best_instance][i][1] && boards[best_instance][i][1] == boards[best_instance][i][2])
            winner = boards[best_instance][i][0];
    }
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (boards[best_instance][0][j] != EMPTY_CELL && boards[best_instance][0][j] == boards[best_instance][1][j] && boards[best_instance][1][j] == boards[best_instance][2][j])
            winner = boards[best_instance][0][j];
    }
    if (boards[best_instance][0][0] != EMPTY_CELL && boards[best_instance][0][0] == boards[best_instance][1][1] && boards[best_instance][1][1] == boards[best_instance][2][2])
        winner = boards[best_instance][0][0];
    if (boards[best_instance][0][2] != EMPTY_CELL && boards[best_instance][0][2] == boards[best_instance][1][1] && boards[best_instance][1][1] == boards[best_instance][2][0])
        winner = boards[best_instance][0][2];

    printf("Game Over!\n");
    if (winner == PLAYER_X)
        printf("Player X wins!\n");
    else if (winner == PLAYER_O)
        printf("Player O wins!\n");
    else
        printf("It's a draw!\n");
}

int main() {
    srand(time(NULL)); // Seed for random number generation

    // Train the Q-learning agents by playing multiple games
    for (int generation = 0; generation < NUM_GENERATIONS; generation++) {
        initialize_instances();
        for (int instance = 0; instance < NUM_INSTANCES; instance++) {
            play_game_instance(instance);
        }
    }
    
    //Save the q_values for the best instance in the generation
    int best_instance = find_best_instance();
    save_q_values_instance(best_instance, q_values[best_instance], "best_instance_q_values.dat");

    // Display the full game of the best instance in the last generation
    display_best_instance_game();

    return 0;
}
