#include <stdio.h>
#include <stdbool.h>

char board[3][3]; // 3x3 Tic-Tac-Toe board

// Function to initialize the board
void initialize_board() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

// Function to print the board
void print_board() {
    printf("   0   1   2\n");
    printf("0  %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
    printf("  ---|---|---\n");
    printf("1  %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
    printf("  ---|---|---\n");
    printf("2  %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
}

// Function to check if a player has won
bool check_winner(char player) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
            return true;
    }
    // Check columns
    for (int j = 0; j < 3; j++) {
        if (board[0][j] == player && board[1][j] == player && board[2][j] == player)
            return true;
    }
    // Check diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
        return true;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
        return true;
    return false;
}

// Function to check if the board is full (draw)
bool check_draw() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ')
                return false;
        }
    }
    return true;
}

// Function to make a move
void make_move(int row, int col, char player) {
    board[row][col] = player;
}

// Function to reset the game
void reset_game() {
    initialize_board();
}

int main() {
    char play_again = 'y';

    printf("Welcome to Tic-Tac-Toe!\n");
    printf("Player 1: X, Player 2: O\n");

    while (play_again == 'y' || play_again == 'Y') {
        initialize_board();
        char current_player = 'X';
        bool game_over = false;

        while (!game_over) {
            printf("\nCurrent board:\n");
            print_board();

            int row, col;
            printf("\nPlayer %c's turn. Enter row (0-2) and column (0-2) separated by a space: ", current_player);
            scanf("%d %d", &row, &col);

            if (row < 0 || row > 2 || col < 0 || col > 2 || board[row][col] != ' ') {
                printf("Invalid move. Try again.\n");
                continue;
            }

            make_move(row, col, current_player);

            if (check_winner(current_player)) {
                printf("\nPlayer %c wins!\n", current_player);
                game_over = true;
            } else if (check_draw()) {
                printf("\nThe game ends in a draw.\n");
                game_over = true;
            } else {
                // Switch player
                current_player = (current_player == 'X') ? 'O' : 'X';
            }
        }

        printf("\nFinal board:\n");
        print_board();

        // Ask if the player wants to play again
        printf("\nDo you want to play again? (y/n): ");
        scanf(" %c", &play_again);

        if (play_again == 'y' || play_again == 'Y') {
            reset_game();
        }
    }

    printf("\nThank you for playing Tic-Tac-Toe!\n");

    return 0;
}
