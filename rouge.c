#include<ncurses.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<regex.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pthread.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include<unistd.h>

typedef struct {
    int y;
    int x;
} Position;

typedef struct {
    char user_name[20];
    char password[20];
} User;

typedef struct {
    Position position;
    int distance;
} QueueNode;

typedef struct 
{
    Position position;

    /* data */
}Door;

typedef struct
{
    Position position;

}Trap;

typedef struct {
    Position position;
    int width;
    int height;
    Door* doors; 
    // Position window;
    Position* object;
    Trap* traps;
    int num_traps;
    int num_doors;
    int num_objects;
} Room;

typedef struct {
        int room1;
        int door1;
        int room2;
        int door2;
        int distance;
} Edge;

typedef struct {
    Position start; // Start of the corridor
    Position end;   // End of the corridor
} Corridor;

typedef struct {
    Position position;
    int hp;
} Player;

typedef struct {
    Room *rooms;
    int num_rooms;
    Corridor *corridors;
    int num_corridors;
    char **map; // 2D array representing the level
    char **drawMap;
    int width;
    int height;
} Level;

typedef struct {
    pthread_t thread;          // رشته موسیقی
    int is_playing;            // وضعیت پخش (1 = در حال پخش، 0 = متوقف)
    char current_track[100];   // مسیر فایل موسیقی فعلی
    int volume;                // میزان صدا (0 تا 128)
} Music;

typedef struct {
    Player player;
    Level *levels;
    int total_levels;
    int current_level;
} Game;



void firstMenu();
void rand_color();
void draw_border();
void signUp_menu();
void logIn_menu();
void create_textbox();
int validate_email();
int checkSignUp();
int checkLogIn();
void gameMenu(User user);
void init_music();
void *music_thread();
void play_music();
void stop_music();
void set_music_volume();
WINDOW* create_button();

// Function Prototypes
char **create_map(int width, int height);
void free_map(char **map, int height);
Room create_room(int y, int x, int width, int height);
int check_room_overlap(Room room, Room *rooms, int num_rooms);
void add_room_to_map(Room room, char **map, int s);
Level create_level(int width, int height, int num_rooms);
void draw_map(char **map, int width, int height);
void move_player(Game *game, int key);
void draw_game(Game *game);
void reveal_room(Room room, char **map, char** drawMap);
int bfs_corridor(char **map, Position start, Position target, int width, int height, Position** parent, int** visited);
// void bfs_corridor(char **map, Position start, Position target, int width, int height);
void connect_rooms_with_bfs(Room *rooms, int num_rooms, char **map, int width, int height, Corridor ** corridors , int *num_corridors);
void reveal_corridor(Position start, char **map, char **draw_map);
void next_level(Game* game);
void init_game(int n, User user);
void load_game(Game* game , User user);
void save_game(Game *game, User user);
// void free_level(Level *level);
// void connect_rooms_with_mst(Room *rooms, int num_rooms, char **map, int width, int height, Corridor **corridors, int *num_corridors);
// void connect_rooms(Room *rooms, int num_rooms, char **map, Corridor **corridors, int *num_corridors);

// typedef struct
// {
//     Music music;
//     int gameLevel;
//     Level* levels;
//     int level;
    


// }Game;


int main(){
    
    initscr();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();
    srand(time(0));
    // rand_color();
    firstMenu();
    endwin();
}

void firstMenu(){
    

    // attron(COLOR_PAIR(2));
    // attron(A_REVERSE);
    // mvprintw(LINES/2 , COLS/2 , "SignUp Menu");
    // attroff(A_REVERSE);
    // mvprintw(LINES/2 + 5 , COLS/2 , "LogIn Menu");
    // attroff(COLOR_PAIR(2));
    char *choices[] = {"SignUp Menu" , "LogIn Menu"};
    int choice = 0;


    while (1)
    {
        // clear();
        draw_border();
        for (int i = 0; i < 2; ++i)
        {
            WINDOW* button = newwin(3 , 25, LINES/2 - 6 + 4*i, COLS/2 - 12);
            box(button , 0 , 0);
            refresh();
            wrefresh(button);
            if (i == choice)
                wattron(button,A_BOLD);
            else
                wattroff(button,A_BOLD);
            
            mvwprintw(button , 1 , 12 - strlen(choices[i])/2 , "%s" , choices[i]);
            wrefresh(button);
            refresh();
            delwin(button);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 1 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 1) ? 0 : choice + 1;
        else if (ch == 10){
            break;
        }
        
    }
    if (choice == 1){
        clear(); 
        refresh(); 
        logIn_menu();
    }    
    else{
        clear(); 
        refresh();
        signUp_menu();
    }     
}

void signUp_menu() {
    
    char username[50] = {0};
    char password[20] = {0};
    char email[50] = {0};
    // FILE* file = fopen("userpass.txt" , "a+");


    while (1) {
        clear();
        refresh();
        draw_border();

        
        mvprintw(LINES / 2, COLS / 2 - 15, "Enter your username:");
        mvprintw(LINES / 2 + 4, COLS / 2 - 15, "Enter your password:");
        mvprintw(LINES / 2 + 8, COLS / 2 - 15, "Enter your email:");
        refresh();

        
        create_textbox(LINES / 2 + 1, COLS / 2 - 15, 30, 25, username);
        create_textbox(LINES / 2 + 5, COLS / 2 - 15, 30, 25, password);
        create_textbox(LINES / 2 + 9, COLS / 2 - 15, 30, 25, email);
        refresh();
        
        int valid = 1;

        
        int greatword = 0, smallword = 0, digit = 0;
        for (int i = 0; i < strlen(password); i++) {
            if (password[i] >= 'a' && password[i] <= 'z')
                smallword++;
            else if (password[i] >= 'A' && password[i] <= 'Z')
                greatword++;
            else if (password[i] >= '0' && password[i] <= '9')
                digit++;
        }
        if (strlen(password) < 7 || greatword == 0 || smallword == 0 || digit == 0) {
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Invalid Password. Please try again.");
            refresh();
            valid = 0;
        }

        // اعتبارسنجی ایمیل
        if (!validate_email(email)) {
            mvprintw(LINES / 2 -6, COLS / 2 - 15, "Invalid email address. Please try again.");
            refresh();
            valid = 0;
        }

        if (valid) {
            // mvprintw(LINES / 2 -8, COLS / 2 - 15, "Registration successful!");
            break;
            // getch();
            // break; 
        } else {
            
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            memset(email, 0, sizeof(email));
            getch(); 
        }
    }

    refresh();
    if (checkSignUp(username , password))
    {
        mvprintw(LINES / 2 -8, COLS / 2 - 15, "You are already Registered");
        refresh(); 
        sleep(1);
        clear(); 
        logIn_menu();
    }
    else{
        mvprintw(LINES / 2 -8, COLS / 2 - 15, "Registration successful!");
        refresh(); 
        sleep(1);
        clear(); 
        logIn_menu();
    }
}

void logIn_menu(){
    char username[50] = {0};
    char password[20] = {0};

    while (1) {
        clear();
        refresh();
        draw_border();

        // نمایش متن‌ها
        mvprintw(LINES / 2, COLS / 2 - 15, "Enter your username:");
        mvprintw(LINES / 2 + 4, COLS / 2 - 15, "Enter your password:");
        // mvprintw(LINES / 2 + 8, COLS / 2 - 15, "Enter your email:");
        refresh();


        create_textbox(LINES / 2 + 1, COLS / 2 - 15, 30, 25, username);
        create_textbox(LINES / 2 + 5, COLS / 2 - 15, 30, 25, password);
        // create_textbox(LINES / 2 + 9, COLS / 2 - 15, 30, 25, email);
        refresh();

        int valid = 1;

        int greatword = 0, smallword = 0, digit = 0;
        for (int i = 0; i < strlen(password); i++) {
            if (password[i] >= 'a' && password[i] <= 'z')
                smallword++;
            else if (password[i] >= 'A' && password[i] <= 'Z')
                greatword++;
            else if (password[i] >= '0' && password[i] <= '9')
                digit++;
        }
        if (strlen(password) < 7 || greatword == 0 || smallword == 0 || digit == 0) {
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Invalid Password. Please try again.");
            refresh();
            valid = 0;
        }
        // if (!validate_email(email)) {
        //     mvprintw(LINES / 2 -6, COLS / 2 - 15, "Invalid email address. Please try again.");
        //     refresh();
        //     valid = 0;
        // }

        if (valid) {
            // mvprintw(LINES / 2 -8, COLS / 2 - 15, "Registration successful!");
            break;
            // getch(); 
        } else {
            // پاک کردن ورودی‌ها برای تلاش مجدد
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            // memset(email, 0, sizeof(email));
            getch(); 
        }
    }

    refresh();
    if (checkLogIn(username , password))
    {
        User user;
        strcpy(user.user_name , username);
        strcpy(user.password , password);
        mvprintw(LINES / 2 -8, COLS / 2 - 15, "You");
        refresh();
        sleep(2);
        clear();
        gameMenu(user);
    }
    else{
        mvprintw(LINES / 2 -8, COLS / 2 - 15, "You should signing up first");
        refresh(); 
        sleep(1);
        clear(); 
        signUp_menu();
    }

}

void gameMenu(User user){

    clear();
    refresh();
    draw_border();

    // تعریف نام دکمه‌ها
    char *choices[] = {"New Game", "Load Game", "Point Schedule", "Settings"};
    int num_choices = 4;
    int choice = 0;

    // موقعیت دکمه‌ها
    int start_y = LINES / 2 - 6;
    int start_x = COLS / 2 - 12;


    while (1) {

        for (int i = 0; i < num_choices; i++) {
            WINDOW *button = newwin(3, 25, start_y + i * 4, start_x);
            box(button, 0, 0);
            refresh();
            // WINDOW *button = (COLS / 2 - 12 + i*4,LINES / 2 - 6,24, 3,choices[i]);
            if (i == choice) {
                // دکمه انتخاب‌شده
                // wbkgd(button, COLOR_PAIR(2)); // تغییر رنگ پس‌زمینه
                wattron(button, A_BOLD);      // حاشیه برجسته
            } else {
                // wbkgd(button, COLOR_PAIR(1)); 
                wattroff(button, A_BOLD);
            }
            mvwprintw(button, 1, 12 - strlen(choices[i]) / 2, "%s", choices[i]);
            wrefresh(button);
            refresh();
            delwin(button); 
        }

        int ch = getch();
        if (ch == KEY_UP) {
            choice = (choice == 0) ? num_choices - 1 : choice - 1;
        } else if (ch == KEY_DOWN) {
            choice = (choice == num_choices - 1) ? 0 : choice + 1;
        } else if (ch == 10) { // Enter
            break;
        }
    }

    if (choice == 0)
    {
        init_game(0 , user);
    }
    else if(choice == 1){
        init_game(1 , user);
    }
    


    clear();
    mvprintw(LINES / 2, COLS / 2 - 10, "You selected: %s", choices[choice]);
    refresh();
    getch();


    // clear();
    // char* choices[] = {"New game" , "Load game" , "Point schedual" , "Settings"};

    // for (int i = 0; i < 4; i++)
    // {
    //     creat_button(LINES/2 + 5*(i-2) , COLS/2 - 5 , 25 , 3 , choices[i]);
    // }
    

}

void save_game(Game *game, User user) {
    char file_path[100];
    sprintf(file_path , "%s/save.dat" , user.user_name);
    FILE *file = fopen(file_path, "wb");
    if (!file) {
        perror("Error opening file for saving");
        return;
    }

    // Save basic game information
    fwrite(&game->player, sizeof(Player), 1, file);
    fwrite(&game->total_levels, sizeof(int), 1, file);
    fwrite(&game->current_level, sizeof(int), 1, file);

    // Save levels
    for (int i = 0; i <= game->current_level; i++) {
        Level *level = &game->levels[i];
        fwrite(&level->width, sizeof(int), 1, file);
        fwrite(&level->height, sizeof(int), 1, file);
        fwrite(&level->num_rooms, sizeof(int), 1, file);
        fwrite(&level->num_corridors, sizeof(int), 1, file);

        // Save map
        for (int y = 0; y < level->height; y++) {
            fwrite(level->map[y], sizeof(char), level->width, file);
        }

        for (int x = 0; x < level->height; x++) {
            fwrite(level->drawMap[x], sizeof(char), level->width, file);
        }

        // Save rooms
        for (int j = 0; j < level->num_rooms; j++) {
            Room *room = &level->rooms[j];
            fwrite(&room->position, sizeof(Position), 1, file);
            fwrite(&room->width, sizeof(int), 1, file);
            fwrite(&room->height, sizeof(int), 1, file);
            fwrite(&room->num_doors, sizeof(int), 1, file);
            fwrite(&room->num_objects, sizeof(int), 1, file);
            fwrite(&room->num_traps, sizeof(int), 1, file);

            fwrite(room->doors, sizeof(Door), room->num_doors, file);
            fwrite(room->object, sizeof(Position), room->num_objects, file);
            fwrite(room->traps, sizeof(Trap), room->num_traps, file);
        }

        // Save corridors
        fwrite(level->corridors, sizeof(Corridor), level->num_corridors, file);
    }

    fclose(file);
    // printf("Game saved successfully!\n");
}

void load_game(Game *game, User user) {
    char file_path[100];
    sprintf(file_path , "%s/save.dat" , user.user_name);
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file for loading");
        return;
    }

    // Load basic game information
    fread(&game->player, sizeof(Player), 1, file);
    fread(&game->total_levels, sizeof(int), 1, file);
    fread(&game->current_level, sizeof(int), 1, file);

    // Allocate memory for levels
    game->levels = malloc(game->total_levels * sizeof(Level));

    for (int i = 0; i <= game->current_level; i++) {
        Level *level = &game->levels[i];
        fread(&level->width, sizeof(int), 1, file);
        fread(&level->height, sizeof(int), 1, file);
        fread(&level->num_rooms, sizeof(int), 1, file);
        fread(&level->num_corridors, sizeof(int), 1, file);

        // Allocate memory for map
        level->map = create_map(level->width, level->height);
        for (int y = 0; y < level->height; y++) {
            fread(level->map[y], sizeof(char), level->width, file);
        }

        level->drawMap = create_map(level->width, level->height);
        for (int x = 0; x < level->height; x++) {
            fread(level->drawMap[x], sizeof(char), level->width, file);
        }

        // Allocate memory for rooms
        level->rooms = malloc(level->num_rooms * sizeof(Room));
        for (int j = 0; j < level->num_rooms; j++) {
            Room *room = &level->rooms[j];
            fread(&room->position, sizeof(Position), 1, file);
            fread(&room->width, sizeof(int), 1, file);
            fread(&room->height, sizeof(int), 1, file);
            fread(&room->num_doors, sizeof(int), 1, file);
            fread(&room->num_objects, sizeof(int), 1, file);
            fread(&room->num_traps, sizeof(int), 1, file);

            room->doors = malloc(room->num_doors * sizeof(Door));
            fread(room->doors, sizeof(Door), room->num_doors, file);

            room->object = malloc(room->num_objects * sizeof(Position));
            fread(room->object, sizeof(Position), room->num_objects, file);

            room->traps = malloc(room->num_traps * sizeof(Trap));
            fread(room->traps, sizeof(Trap), room->num_traps, file);
        }

        // Allocate memory for corridors
        level->corridors = malloc(level->num_corridors * sizeof(Corridor));
        fread(level->corridors, sizeof(Corridor), level->num_corridors, file);
    }

    fclose(file);
    // printf("Game loaded successfully!\n");
}

void init_game(int n , User user){
    // initscr();
    noecho();
    // keypad(stdscr, TRUE);
    // curs_set(FALSE);
    // srand(time(NULL));

    // Game setup
    Game game;
    if (n==0)
    {
        game.total_levels = 3;
        game.current_level = 0;
        int map_width = COLS, map_height = LINES -5, num_rooms = 6 + rand() % (game.current_level + 1);
        game.levels = malloc(4 * sizeof(Level));
        game.levels[game.current_level] = create_level(map_width, map_height, num_rooms);


        // Initialize player
        game.player.position.x = game.levels[game.current_level].rooms[0].position.x + game.levels[game.current_level].rooms[0].width/2;
        game.player.position.y = game.levels[game.current_level].rooms[0].position.y + game.levels[game.current_level].rooms[0].height/2;

        game.player.hp = 100;

        // Initially reveal the starting room
        // reveal_room(game.levels[0].rooms[0], game.levels[0].map);
        reveal_room(game.levels[game.current_level].rooms[0],game.levels[game.current_level].map ,game.levels[game.current_level].drawMap);

    }
    else{
        load_game(&game , user);
        // game.current_level = 0;
    }
    // Game loop
    int key;
    while (1) {
        draw_game(&game);
        refresh();
        key = getch();
        if (key == 'q'){
            save_game(&game , user);
            break;
        }
        move_player(&game, key);
        refresh();
    }

    // Cleanup
    // free_map(game.levels[0].map, game.levels[0].height);
    // free_map(game.levels[0].drawMap, game.levels[0].height);
    // free(game.levels[0].rooms);
    // free(game.levels[0].corridors);
    // free(game.levels);

    endwin();
    // return 0;
}

void next_level(Game* game){

    // if (game->current_level >= 0) {
    //     free_level(&game->levels[game->current_level]);
    // }

    game->current_level++;
    // if (game->current_level >= game->total_levels) {
    //     game->total_levels *= 2;
    //     Level *new_levels = realloc(game->levels, game->total_levels * sizeof(Level));
    //     if (!new_levels) {
    //         perror("Failed to allocate memory for levels");
    //         exit(EXIT_FAILURE);
    //     }
    //     game->levels = new_levels;
    // }
    // game->levels[game->current_level] = create_level

    int map_width = COLS, map_height = LINES - 5;
    int num_rooms = 6 + rand() % (game->current_level + 1);

    game->levels[game->current_level] = create_level(map_width, map_height, num_rooms);

    game->player.position.x = game->levels[game->current_level].rooms[0].position.x + 
                              game->levels[game->current_level].rooms[0].width / 2;
    game->player.position.y = game->levels[game->current_level].rooms[0].position.y + 
                              game->levels[game->current_level].rooms[0].height / 2;

    reveal_room(game->levels[game->current_level].rooms[0], 
                game->levels[game->current_level].map, 
                game->levels[game->current_level].drawMap);
    
}


char **create_map(int width, int height) {
    char **map = malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        map[i] = malloc(width * sizeof(char));
        memset(map[i], ' ', width);
    }
    return map;
}

void free_map(char **map, int height) {
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}


Room create_room(int y, int x, int width, int height) {
    Room room;
    room.position.y = y;
    room.position.x = x;
    room.width = width;
    room.height = height;
    room.num_doors = 1 + rand() % 3;
    room.num_objects = rand() % 2;
    room.num_traps = 1 + rand() % 2;///level
    room.doors = malloc(room.num_doors * sizeof(Door));
    room.object = malloc(room.num_objects * sizeof(Position));
    room.traps = malloc(room.num_traps * sizeof(Trap));


    for (int i = 0; i < room.num_doors; i++) {
        int side = rand() % 4; 

        switch (side) {///if , else if
            case 0: 
                room.doors[i].position.y = y; 
                room.doors[i].position.x = x + 1 + rand() % (width - 2); 
                break;
            case 1: 
                room.doors[i].position.y = y + height - 1; 
                room.doors[i].position.x = x + 1 + rand() % (width - 2); 
                break;
            case 2: 
                room.doors[i].position.y = y + 1 + rand() % (height - 2); 
                room.doors[i].position.x = x; 
                break;
            case 3: 
                room.doors[i].position.y = y + 1 + rand() % (height - 2); 
                room.doors[i].position.x = x + width - 1; 
                break;
        }
    }
    for (int i = 0; i < room.num_objects; i++)
    {
        room.object[i].x = x + 1 + rand() % (width - 2);
        room.object[i].y = y + 1 + rand() % (height - 2);
    }
    for (int i = 0; i < room.num_traps; i++)
    {
        room.traps[i].position.x = x + 1 + rand() % (width - 2);
        room.traps[i].position.y = y + 1 + rand() % (height - 2);
    }
    
    
    // int num_win = rand()% 1;
    // for (int i = 0; i < num_win; i++)
    // {
    //     /* code */
    // }
    return room;
}

int check_room_overlap(Room room, Room *rooms, int num_rooms) {
    const int MIN_DISTANCE = 5; 

    for (int i = 0; i < num_rooms; i++) {
        Room existing = rooms[i];

        int expanded_x1 = existing.position.x - MIN_DISTANCE;
        int expanded_y1 = existing.position.y - MIN_DISTANCE;
        int expanded_x2 = existing.position.x + existing.width + MIN_DISTANCE;
        int expanded_y2 = existing.position.y + existing.height + MIN_DISTANCE;

        
        if (room.position.x < expanded_x2 &&
            room.position.x + room.width > expanded_x1 &&
            room.position.y < expanded_y2 &&
            room.position.y + room.height > expanded_y1) {
            return 1; 
        }
    }

    return 0; 
}

void add_room_to_map(Room room, char **map, int s) {
    for (int i = 0; i < room.height; i++) {
        for (int j = 0; j < room.width; j++) {
            if (i == 0 || i == room.height - 1) {
                map[room.position.y + i][room.position.x + j] = '-';
            } 
            else if (j == 0 || j == room.width - 1)
            {
                map[room.position.y + i][room.position.x + j] = '|';
            }
            else {
                map[room.position.y + i][room.position.x + j] = '.'; 
            }
        }
    }
    // Add doors

    for(int i = 0; i < room.num_doors; i++){
        map[room.doors[i].position.y][room.doors[i].position.x] = '+';
    }
    for (int i = 0; i < room.num_objects; i++)
    {
        map[room.object[i].y][room.object[i].x] = 'O';
    }
    for (int i = 0; i < room.num_traps; i++)
    {
        map[room.traps[i].position.y][room.traps[i].position.x] = 'T';
    }
    if (s == 1)
    {
        map[room.position.y + 1 +rand() % (room.height - 2)][room.position.x + 1 +rand() % (room.width - 2)] = 'S';
    }

}


void reveal_room(Room room, char **map, char** drawMap) {
    for (int i = 0; i < room.height; i++) {
        for (int j = 0; j < room.width; j++) {
            char current_char = map[room.position.y + i][room.position.x + j];
            if (current_char == '-' || current_char == '|' || current_char == '.' || current_char == '+' || current_char == 'O' || current_char == 'T' || current_char == 'S') {
                // map[room.position.y + i][room.position.x + j] = current_char;
                if (current_char == 'T')
                {
                    drawMap[room.position.y + i][room.position.x + j] = '.';
                }
                else{
                    drawMap[room.position.y + i][room.position.x + j] = current_char;
                }
            }
        }
    }
}
void reveal_corridor(Position start, char **map , char** draw_map){
    
        char current_char1 = map[start.y + 1][start.x];
        char current_char2 = map[start.y][start.x + 1];
        char current_char3 = map[start.y - 1][start.x];
        char current_char4 = map[start.y][start.x - 1];

        if (current_char1 == '#' || current_char1 == '+')
        {
            draw_map[start.y + 1][start.x] = current_char1;
        }
        if (current_char2 == '#' || current_char2 == '+')
        {
            draw_map[start.y][start.x + 1] = current_char2;
        }
        if (current_char3 == '#' || current_char3 == '+')
        {
            draw_map[start.y - 1][start.x] = current_char3;
        }
        if (current_char4 == '#' || current_char4 == '+')
        {
            draw_map[start.y][start.x - 1] = current_char4;
        }

}

int bfs_corridor(char **map, Position start, Position target, int width, int height, Position **parent, int **visited) {
    int deltaX[4] = {1, 0, -1, 0};
    int deltaY[4] = {0, 1, 0, -1};

    QueueNode *queue = malloc(width * height * sizeof(QueueNode));
    int front = 0, rear = 0;

    queue[rear++] = (QueueNode){start, 0};
    visited[start.y][start.x] = 1;

    while (front < rear) {
        QueueNode current = queue[front++];
        Position pos = current.position;

        if (pos.x == target.x && pos.y == target.y) {
            free(queue);
            return 1;
        }

        for (int i = 0; i < 4; i++) {
            int newX = pos.x + deltaX[i];
            int newY = pos.y + deltaY[i];

            if (newX >= 0 && newX < width && newY > 0 && newY < height &&
                (map[newY][newX] == ' '|| map[newY][newX] == '+' || map[newY][newX] == '#') && !visited[newY][newX]) {
                visited[newY][newX] = 1;
                parent[newY][newX] = pos;
                queue[rear++] = (QueueNode){{newY, newX}, current.distance + 1};
            }
        }
    }

    free(queue);
    return 0;
}

int compareEdges(const void *a, const void *b) {
    Edge *edgeA = (Edge *)a;
    Edge *edgeB = (Edge *)b;
    return edgeA->distance - edgeB->distance;
}


void connect_rooms_with_bfs(Room *rooms, int num_rooms, char **map, int width, int height, Corridor **corridors, int *num_corridors) {
    *num_corridors = 0;
    *corridors = malloc(1000 * sizeof(Corridor)); 

    int **visited = malloc(height * sizeof(int *));
    Position **parent = malloc(height * sizeof(Position *));
    for (int i = 0; i < height; i++) {
        visited[i] = calloc(width, sizeof(int));
        parent[i] = malloc(width * sizeof(Position));
    }


    // Edge *edges = malloc(num_rooms * num_rooms * sizeof(Edge));
    // int edge_count = 0;


    // محاسبه فاصله بین تمام جفت‌های درب‌ها
    for (int i = 0; i < num_rooms; i++) {
        for (int j = 0; j < rooms[i].num_doors; j++) {
            
            for (int k = 0; k < num_rooms; k++) {
                if (i != k)
                {
                    Edge *edges = malloc(num_rooms * num_rooms * sizeof(Edge));
                    int edge_count = 0;
                    for (int l = 0; l < rooms[k].num_doors; l++) {
                        int dx = rooms[i].doors[j].position.x - rooms[k].doors[l].position.x;
                        int dy = rooms[i].doors[j].position.y - rooms[k].doors[l].position.y;
                        int distance = dx * dx + dy * dy;

                        edges[edge_count++] = (Edge){i, j, k, l, distance};
                    }

                    qsort(edges, edge_count, sizeof(Edge), compareEdges);
                    Position start = rooms[edges[0].room1].doors[edges[0].door1].position;
                    Position target = rooms[edges[0].room2].doors[edges[0].door2].position;

                    
                    for (int y = 0; y < height; y++) {
                        memset(visited[y], 0, width * sizeof(int)); 
                        for (int x = 0; x < width; x++) {
                            parent[y][x] = (Position){-1, -1};
                        }
                    }

                    if (bfs_corridor(map, start, target, width, height, parent, visited)) {
                        Position current = target;
                        while (current.x != start.x || current.y != start.y) {
                            if (current.x == target.x || current.y == target.y)
                            {
                                current = parent[current.y][current.x];
                                continue;
                            }
                            map[current.y][current.x] = '#'; 
                            current = parent[current.y][current.x];
                        }

                        
                        (*corridors)[*num_corridors].start = start;
                        (*corridors)[*num_corridors].end = target;
                        (*num_corridors)++;
                    }
                    free(edges);
                }
            }
            

        }
    }

    // آزادسازی حافظه
    for (int i = 0; i < height; i++) {
        free(visited[i]);
        free(parent[i]);
    }
    free(visited);
    free(parent);
}




Level create_level(int width, int height, int num_rooms) {
    Level level;
    level.width = width;
    level.height = height;
    level.num_rooms = 0;
    level.map = create_map(width, height);
    level.drawMap = create_map(width , height);
    level.rooms = malloc(num_rooms * sizeof(Room));

    for (int i = 0; i < num_rooms; i++) {
        int room_width = 6 + rand() % 4;
        int room_height = 6 + rand() % 4;
        int room_y, room_x;
        Room room;
        // int attempts = 0;

        do {
            room_y = rand() % (height -1 - room_height);
            room_x = rand() % (width -1 - room_width);
            room = create_room(room_y, room_x, room_width, room_height);            
 
        } while (check_room_overlap(room, level.rooms, level.num_rooms));

        
        level.rooms[level.num_rooms++] = room;
        if (i == 5)
        {
            add_room_to_map(room, level.map, 1);  
        }
        else{
            add_room_to_map(room, level.map, 0);  
        }
        
    }

    connect_rooms_with_bfs(level.rooms, level.num_rooms, level.map, width , height ,&level.corridors, &level.num_corridors);//مشکل دارد
    return level;
}

void move_player(Game *game, int key) {
    Position new_pos = game->player.position;
    Level *level = &game->levels[game->current_level];
    char **map = level->map;
    char **drawMap = level->drawMap;


    if (key == KEY_UP && new_pos.y > 0) new_pos.y--;
    else if (key == KEY_DOWN && new_pos.y < level->height - 1) new_pos.y++;
    else if (key == KEY_LEFT && new_pos.x > 0) new_pos.x--;
    else if (key == KEY_RIGHT && new_pos.x < level->width - 1) new_pos.x++;

    char next_tile = map[new_pos.y][new_pos.x];

    if (next_tile == '.' || next_tile == '+' || next_tile == '#' || next_tile == 'T' || next_tile == 'S') {
        if (next_tile == '+') {
            for (int i = 0; i < level->num_rooms; i++) {
                Room room = level->rooms[i];
                for (int j = 0; j < room.num_doors; j++) {
                    if (new_pos.y == room.doors[j].position.y && new_pos.x == room.doors[j].position.x) {
                        reveal_room(room, map, drawMap);
                    }
                }
            }
        } else if (next_tile == '#') {
            reveal_corridor(new_pos, map, drawMap);
        } else if (next_tile == 'T') {
            game->player.hp--;
        } else if (next_tile == 'S') {
            next_level(game); 
            return;
        }

        game->player.position = new_pos;
    }
}


void draw_map(char **drawMap, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char tile = drawMap[y][x];
            mvaddch(y, x, tile);
        }
    }
    refresh();
}

void draw_game(Game *game) {
    clear();
    Level *level = &game->levels[game->current_level];

    draw_map(level->drawMap, level->width, level->height);
    refresh();
    mvaddch(game->player.position.y, game->player.position.x, '@');
    mvprintw(level->height + 2, 0, "Player HP: %d", game->player.hp);

    refresh();
}


void init_music(Music *music, const char *track_path) {
    music->is_playing = 0;
    strncpy(music->current_track, track_path, sizeof(music->current_track) - 1);
    music->volume = 64; // صدای متوسط
}

void *music_thread(void *arg) {
    Music *music = (Music *)arg;
    while (music->is_playing) {
        char command[150];
        snprintf(command, sizeof(command), "mpg123 -q %s", music->current_track);
        system(command);
    }
    return NULL;
}

void play_music(Music *music) {
    if (music->is_playing) return; // اگر در حال پخش است، تکرار نکن
    music->is_playing = 1;
    pthread_create(&music->thread, NULL, music_thread, music);
}

void stop_music(Music *music) {
    if (!music->is_playing) return; // اگر در حال پخش نیست، متوقف نکن
    music->is_playing = 0;
    pthread_join(music->thread, NULL);
}

void set_music_volume(Music *music, int volume) {
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    music->volume = volume;
    char command[100];
    snprintf(command, sizeof(command), "amixer set Master %d%%", volume);
    system(command);
}




int checkSignUp(char* username , char* password){
    FILE* file = fopen("userpass.txt" , "a+");
    char username_o[20];
    char password_o[30];
    while (fscanf(file , "%s %s" , username_o , password_o)!= EOF)
    {
        if (strcmp(username , username_o) == 0 && strcmp(password_o , password) == 0)
        {
            return 1;
        }
        
    }
    mkdir(username , 0775);
    fprintf(file , "\n%s %s" , username , password);
    fclose(file);
    return 0;

    
}

int checkLogIn(char* username , char* password){
    FILE* file = fopen("userpass.txt" , "a+");
    char username_o[20];
    char password_o[30];
    while (fscanf(file , "%s %s" , username_o , password_o)!= EOF)
    {
        if (strcmp(username , username_o) == 0 && strcmp(password_o , password) == 0)
        {
            return 1;
        }
        
    }
    return 0;
}


void rand_color(){
    for (int i = 0; i < 7; i++)
    {
        int r = rand() % 1001;
        int g = rand() % 1001;
        int b = rand() % 1001;
        init_color(i  + 7, r , g , b);
        init_pair(i + 7, i + 7, -1);
    }
    
}


void draw_border(){
    clear();
    // attron(COLOR_PAIR(1));
    for (int i = 0; i < COLS; i++)
    {
        mvprintw(0 , i , "#");
        mvprintw(LINES - 1 , i , "#");
    }
    for (int i = 0; i < LINES; i++)
    {
        mvprintw(i , 0 , "#");
        mvprintw(i , COLS-1 , "#");
    }
    // attroff(COLOR_PAIR(1));
}

void create_textbox(int start_y, int start_x, int width, int max_length, char *input) {
    WINDOW *textbox = newwin(3, width + 2, start_y, start_x);
    box(textbox, 0, 0); 
    wrefresh(textbox);

    memset(input, 0, max_length + 1); 
    wmove(textbox , 1  , 1); 
    int ch;
    int index = 0;

    while ((ch = wgetch(textbox)) != '\n') { 
        if (ch == KEY_BACKSPACE || ch == 127) { 
            if (index > 0) {
                input[--index] = '\0';
                mvwaddch(textbox, 1, index + 1, ' '); 
                wmove(textbox, 1, index + 1); 
                wrefresh(textbox);
            }
        } else if (index < max_length && isprint(ch)) { 
            input[index++] = ch;
            mvwaddch(textbox, 1, index, ch); 
            wrefresh(textbox);
        }
    }


    // werase(textbox);
    // box(textbox, 0, 0);
    // wrefresh(textbox);
    // delwin(textbox); // حذف پنجره
}
// WINDOW* creat_button(int start_y, int start_x, int width, int height , char* input){
//     WINDOW *button = newwin(3, width + 2, start_y, start_x);
//     box(button, 0, 0); // رسم حاشیه تکست‌باکس
//     wrefresh(button);
//     // wbkgd()

//     // memset(input, 0, max_length + 1); 
//     // wmove(textbox , 1  , 1); 
//     mvwprintw(button, 1, 12 - strlen(input) / 2, "%s", input);
//     wrefresh(button);
//     return button;
//     // mvwaddstr(textbox , width/2 -2 , height/2 , input);
//     // wrefresh(textbox);
// }
int validate_email(const char *email) {
    const char *pattern = "^[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)?@[a-zA-Z0-9]+\\.[a-zA-Z]{2,}$";
    regex_t regex;
    int reti;
    regcomp(&regex, pattern, REG_EXTENDED);
    reti = regexec(&regex, email, 0, NULL, 0);

    regfree(&regex);

    if (!reti) {
        return 1;
    } else if (reti == REG_NOMATCH) {
        return 0;
    } else {
        return 0;
    }
}
