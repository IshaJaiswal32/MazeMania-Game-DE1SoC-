/*  Title:       MAZE MANIA
    Name:        Isha Jaiwal
    Roll Number: 2201CS32
    Course:      Computer Architecture Lab
    Course Code: CS210

    Declaration: I affirm that this project is my original work, and while I authored the code, I sought assistance from ChatGPT to debug errors encountered during 
    development. Throughout the process, I have applied the knowledge gained from lab assignments and have encountered various learning opportunities, contributing
    to my overall understanding and skill development.
    */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // For usleep function
#include <string.h> // Include cstring for strlen
// VGA base address
#define VGA_BASE 0x08000000
#define CHAR_WIDTH 16  // Custom character width (e.g., 16 pixels)
#define CHAR_HEIGHT 16 // Custom character height (e.g., 16 pixels)
// VGA dimensions
#define VGA_WIDTH 320
#define VGA_HEIGHT 240
// Square size for the maze
#define SQUARE_SIZE 10
// Colors
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_PINK 0xF99F
#define COLOR_BLUE 0x0007
#define COLOR_ORANGE 0xFDA0
#define COLOR_YELLOW 0xFFE0
#define COLOR_LBLUE 0x001F
int game_over = 0; // 0 means game is ongoing, 1 means game is over
// Function prototypes
void write_pixel(int x, int y, short color);
void clear_screen(short color);
void draw_square(int x, int y, short color);
void draw_maze(short col);
void draw_player(int x, int y);
void display_winning_message();
void delay(int microseconds);
void write_char(int x, int y, char c, short color);
void erase_text_on_vga();
void move_player(int dx, int dy, char *msg, short color);
// #include <stdint.h>
//  Base addresses for character and color buffers
#define CHARACTER_BUFFER_BASE 0x09000000
#define COLOR_BUFFER_BASE 0x09040000
// Maze configuration (1 = wall, 0 = open path)
int maze[24][32];
// Starting and ending points in the maze as constant expressions
int start_x = 1;
int start_y = 1;
int end_x = 30;
int end_y = 21;
// Player's current position
int player_x;
int player_y;
// Function to erase the text characters written on the VGA display
void erase_text_on_vga()
{
    // Calculate the starting position in the character and color buffers
    for (int x = 0; x < 80; x++)
    {
        for (int y = 0; y < 60; y++)
        {
            write_char(x, y, ' ', COLOR_BLACK); // Write space character to clear
        }
    }
}
// Function to write a pixel with a specific color
void write_pixel(int x, int y, short color)
{
    volatile short *vga_ptr = (volatile short *)(VGA_BASE + (y << 10) + (x << 1));
    *vga_ptr = color;
}
// Function to write a character at a specific position with a specified color
void write_char(int x, int y, char c, short color)
{
    // VGA character buffer base address
    volatile char *character_buffer = (char *)(0x09000000 + (y << 7) + x);

    // Write the character to the character buffer
    *character_buffer = c;
}
// Function to clear the VGA display
void clear_screen(short color)
{
    for (int x = 0; x < VGA_WIDTH; x++)
    {
        for (int y = 0; y < VGA_HEIGHT; y++)
        {
            write_pixel(x, y, color);
        }
    }
}
// Function to draw a square at the specified position and color
void draw_square(int x, int y, short color)
{
    int start_x = x * SQUARE_SIZE;
    int start_y = y * SQUARE_SIZE;
    int end_x = start_x + SQUARE_SIZE;
    int end_y = start_y + SQUARE_SIZE;
    for (int i = start_x; i < end_x; i++)
    {
        for (int j = start_y; j < end_y; j++)
        {
            write_pixel(i, j, color);
        }
    }
}
// Function to draw the maze on the VGA display
void draw_maze(short col)
{
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            short color = maze[y][x] == 1 ? COLOR_WHITE : col;
            draw_square(x, y, color);
        }
    }
    // Draw the finish point in green
    draw_square(end_x, end_y, COLOR_GREEN);
}
// Function to draw the player at their current position
void draw_player(int x, int y)
{
    draw_square(x, y, COLOR_RED);
}
// Function to display a message using larger characters
void display_winning_message(char *msg)
{
    // For simplicity, we'll clear the screen and display the message
    clear_screen(COLOR_BLUE);
    // const char *msg = "Congratulations! You've won!";

    // Display the message by writing each character at a specific position
    // Calculate the starting position of the message
    int msg_length = strlen(msg); // Length of the message
   
    int x_start = 10; // Center the message
    int y_start = 24; // Vertically center the message

    for (int i = 0; i < msg_length; i++)
    {
        // Write each character in the message
        write_char(x_start, y_start, msg[i], COLOR_GREEN);
        x_start += 2;
    }
}
// Function to handle player movement
void move_player(int dx, int dy, char *msg, short color)
{
    // Calculate new player position
    int new_x = player_x + dx;
    int new_y = player_y + dy;
    // Check if the new position is within the maze boundaries and is not a wall
    if (new_x >= 0 && new_x < 32 && new_y >= 0 && new_y < 24 && maze[new_y][new_x] == 0)
    {
        if (maze[new_y][new_x + 1] == 2 || (new_x == end_x && new_y == end_y) ||
            (maze[new_y + 1][new_x] == 2))
        {
            // Display the winning message
            display_winning_message(msg);
            game_over = 1;
            // clear_screen();
            // display_winning_message();
        }
        // Update the player's position
        player_x = new_x;
        player_y = new_y;
        // Redraw the maze and player
        if (!game_over)
        {
            draw_maze(color);
            draw_player(player_x, player_y);
        }
        // Check if the player has reached the end point
    }
}
// Function to add a delay (in microseconds) to slow down player movements
// Simple busy-wait delay function (in microseconds)
void delay(int microseconds)
{
    long long i;
    for (i = 0; i < microseconds; i++)
    {
        // Busy wait
    }
}
void display_intro()
{
    clear_screen(COLOR_BLUE);
    erase_text_on_vga();
    // Display game title
    const char *game_title = "Maze Game";
    int title_length = 9;
    int title_x = 23;
    int title_y = 10; // Adjust position as needed
    for (int i = 0; i < title_length; i++)
    {
        write_char(title_x, title_y, game_title[i], COLOR_BLACK);
        title_x += 2;
    }
    // Display your name
    const char *author_name = "Isha Jaiswal 2201CS32";
    int author_length = strlen(author_name); // Use strlen to find the length
    int author_x = 13;
    int author_y = title_y + 30; // Adjust position as needed
    for (int i = 0; i < author_length; i++)
    {
        write_char(author_x, author_y, author_name[i], COLOR_WHITE);
        author_x += 2;
    }
    const char *options = "Press switches:";
    int options_length = strlen(options); // Use strlen to find the length
    author_x = 8;
    author_y = title_y + 10; // Adjust position as needed
    for (int i = 0; i < options_length; i++)
    {
        write_char(author_x, author_y, options[i], COLOR_WHITE);
        author_x += 2;
    }
    //Display the options
    const char *options1 = "4: Easy   5: Medium   6: Difficult";
    int options_length1 = strlen(options1); // Use strlen to find the length
    author_x = 3;
    author_y = title_y + 15; // Adjust position as needed
    for (int i = 0; i < options_length1; i++)
    {
        write_char(author_x, author_y, options1[i], COLOR_WHITE);
        author_x += 2;
    }
}
int main()
{
    // Display the introduction screen
    display_intro();

    // Initialize player's position
    player_x = start_x;
    player_y = start_y;

    // Wait for the user to press the switch to start the maze
    while (1)
    {
        // Read the switch input
        volatile uint32_t *switches_ptr1 = (volatile uint32_t *)0xFF200040;
        uint32_t switches1 = *switches_ptr1;

        if (switches1 & 0x10)
        {
            // Draw the maze and player
            // erase_text_on_vga(0,0,320,240);
            //  Initialize the VGA display
            // Maze Pattern for easy level
            int maze_easy[24][32] = {
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
                {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1},
                {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
            for (int i = 0; i < 24; i++)
            {
                for (int j = 0; j < 32; j++)
                {
                    maze[i][j] = maze_easy[i][j];
                }
            }
            clear_screen(COLOR_BLACK);
            erase_text_on_vga();
            draw_maze(COLOR_YELLOW);
            draw_player(player_x, player_y);
            // Main game loop
            while (!game_over)
            {
                // Check switches for movement input
                volatile uint32_t *switches_ptr = (volatile uint32_t *)0xFF200040;
                uint32_t switches = *switches_ptr;
                char *msg = "Press switch 5 to move to level 2";
                // Move the player based on switch input
                if (switches & 0x01)
                { // Move left
                    move_player(-1, 0, msg, COLOR_YELLOW);
                }
                if (switches & 0x02)
                { // Move right
                    move_player(1, 0, msg, COLOR_YELLOW);
                }
                if (switches & 0x04)
                { // Move up
                    move_player(0, -1, msg, COLOR_YELLOW);
                }
                if (switches & 0x08)
                { // Move down
                    move_player(0, 1, msg, COLOR_YELLOW);
                }

                // Introduce a delay of 10,000 microseconds (0.01 seconds) between movements
                delay(10000);
            }
            start_x = 1;
            start_y = 1;
            continue; // Exit the loop after the game is over
        }
        else if (switches1 & 0x20)
        {
            // Draw the maze and player
            // erase_text_on_vga(0,0,320,240);
            //  Initialize the VGA display
            int maze_medium[24][32] = {
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
                {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
                {1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},
                {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
                {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
                {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
                {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                {1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
                {1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1},
                {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
            for (int i = 0; i < 24; i++)
            {
                for (int j = 0; j < 32; j++)
                {
                    maze[i][j] = maze_medium[i][j];
                }
            }
            player_x = start_x;
            player_y = start_y;
            game_over = 0;
            clear_screen(COLOR_BLACK);
            erase_text_on_vga();
            draw_maze(COLOR_LBLUE);
            draw_player(player_x, player_y);
            // Main game loop
            while (!game_over)
            {
                // Check switches for movement input
                volatile uint32_t *switches_ptr = (volatile uint32_t *)0xFF200040;
                uint32_t switches = *switches_ptr;
                char *msg = "Press switch 6 to move to level 3";
                // Move the player based on switch input
                if (switches & 0x01)
                { // Move left
                    move_player(-1, 0, msg, COLOR_LBLUE);
                }
                if (switches & 0x02)
                { // Move right
                    move_player(1, 0, msg, COLOR_LBLUE);
                }
                if (switches & 0x04)
                { // Move up
                    move_player(0, -1, msg, COLOR_LBLUE);
                }
                if (switches & 0x08)
                { // Move down
                    move_player(0, 1, msg, COLOR_LBLUE);
                }

                // Introduce a delay of 10,000 microseconds (0.01 seconds) between movements
                delay(10000);
            }
            delay(10000);
            start_x = 1;
            start_y = 1;
            continue; // Exit the loop after the game is over
        }
        // Check if the user has pressed switch 1
        else if (switches1 & 0x40)
        {
            // Draw the maze and player
            // erase_text_on_vga(0,0,320,240);
            //  Initialize the VGA display
            int maze_difficult[24][32] = {
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1},
                {1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
                {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
                {1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},
                {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1},
                {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
                {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
                {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
                {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
                {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1},
                {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
            for (int i = 0; i < 24; i++)
            {
                for (int j = 0; j < 32; j++)
                {
                    maze[i][j] = maze_difficult[i][j];
                }
            }
            player_x = start_x;
            player_y = start_y;
            game_over = 0;
            clear_screen(COLOR_BLACK);
            erase_text_on_vga();
            draw_maze(COLOR_BLUE);
            draw_player(player_x, player_y);
            // Main game loop
            while (!game_over)
            {
                // Check switches for movement input
                volatile uint32_t *switches_ptr = (volatile uint32_t *)0xFF200040;
                uint32_t switches = *switches_ptr;
                char *msg = "Congratulations! You've won!";
                // Move the player based on switch input
                if (switches & 0x01)
                { // Move left
                    move_player(-1, 0, msg, COLOR_BLUE);
                }
                if (switches & 0x02)
                { // Move right
                    move_player(1, 0, msg, COLOR_BLUE);
                }
                if (switches & 0x04)
                { // Move up
                    move_player(0, -1, msg, COLOR_BLUE);
                }
                if (switches & 0x08)
                { // Move down
                    move_player(0, 1, msg, COLOR_BLUE);
                }

                // Introduce a delay of 10,000 microseconds (0.01 seconds) between movements
                delay(10000);
            }
            break; // Exit the loop after the game is over
        }
        // Introduce a short delay before checking the switches again
        delay(10000);
    }
    return 0;
}
