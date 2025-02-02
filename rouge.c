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
#include <locale.h>
#include <signal.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define CUSTOM_ORANGE 10
#define CUSTOM_PINK 11
// #define start_x 

int weapon_menu_visible = 0;
int food_menu_visible = 0;
int spell_menu_visible = 0;

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
    int password;
    int is_open;
    /* data */
}Door;

typedef struct
{
    Position position;

}Trap;

typedef enum {
    FOOD_Apple,  
    FOOD_Egg,   
    FOOD_Bread,  
    FOOD_Carot, 
    FOOD_Fish   
} FoodType;

typedef struct {
    Position position;
    FoodType type;
    int hp_restore; 
} Food;

typedef enum {
    WEAPON_Mace,
    WEAPON_Sword,      
    WEAPON_Normal_Arrow,    
    WEAPON_Magic_Wand,  
    WEAPON_Dagger  
} WeaponType;

typedef enum {
    SPELL_Speed,
    SPELL_Health,
    SPELL_Damage
} SpellType;

typedef struct {
    Position position; 
    int value;         
} Gold;

typedef struct {
    Position position;  
    WeaponType type;    
    int attack_power;   
    // int durability;     
    // int special_effect; 
} Weapon;

typedef struct {
    Position position;
    SpellType type;
    int effect_type;   
    int duration;      
} Spell;

typedef enum {
    ROOM_NORMAL,
    ROOM_GOLD,
    ROOM_SPELL,
    // ROOM_TRAP,
    // ROOM_BOSS
} RoomType;

typedef enum {
    Deaⅿon,
    Dragon,      
    Giant,     
    Snake,
    Undead       
} MonsterType;

typedef struct {
    Position position;         
    // int follow_steps;          
    int max_follow_steps;     
    char last_tile;            
    // bool can_move_out;        
    MonsterType type;         
    int health;               
    int damage;               
    bool is_active;            
} Monster;



typedef struct {
    Position position;
    int width;
    int height;
    Door* doors; 
    Door pass_door;
    RoomType type;
    // Position window;
    Position* object;
    Position pass_key;
    Trap* traps;
    Food *foods;
    Gold *golds;   
    Weapon *weapons;  
    Spell *spells;  
    int num_foods;
    int num_golds;  
    int num_weapons; 
    int num_spells;
    int num_traps;
    int num_doors;
    int num_objects;
    int num_pass_door;
    int num_monsters;
    Monster* monsters;
    // char music_path[100];
    char *music_path;
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
    int num_sword;
    int num_mace;
    int num_normal_arrow;
    int num_magic_wand;
    int num_dagger;
    int num_weapons;
    int hungry;
    int num_health_spell;
    int num_speed_spell;
    int num_damage_spell;
    int num_apple;
    int num_egg;
    int num_fish;
    int num_bread;
    int num_carrot;
    int gold_collected;
    int color;
    Weapon* weapons;
    Weapon selected_weapon;
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
    Mix_Music *music; 
    int is_playing;    
} Music;


typedef struct {
    int total_time; 
    int penalty;
    int update_interval; 
} GameTimer;

typedef struct {
    int minute;
    int second;
}Timer;

typedef struct {
    int difficulty; 
    int player_color; 
} GameSettings;

typedef struct {
    Player player;
    Level *levels;
    int total_levels;
    int current_level;
    Timer timer;
    char* game_message;
    Music music;
    int difficulty;
} Game;

typedef struct {
    Game* game;
    Room* room;
} ThreadArgs;

User curent_user;



void firstMenu();
void creat_color();
// void rand_color();
void draw_border();
void signUp_menu();
void logIn_menu();
void create_textbox();
int validate_email();
int checkSignUp();
int checkLogIn();
void gameMenu(User user , GameSettings* settings);
// void init_music();
// void *music_thread();
// void play_music();
// void stop_music();
// void set_music_volume();
WINDOW* create_button();

// Function Prototypes
char **create_map(int width, int height);
void free_map(char **map, int height);
Room create_room(int y, int x, int width, int height);
int check_room_overlap(Room room, Room *rooms, int num_rooms);
void add_room_to_map(Room room, char **map, int s);
Level create_level(int width, int height, int num_rooms);
void draw_doors(Door* door , int start_x , int start_y);
void draw_map(Level *level);
void move_player(Game *game, int key);
void draw_game(Game *game);
void reveal_room(Room room, char **map, char** drawMap);
int bfs_corridor(char **map, Position start, Position target, int width, int height, Position** parent, int** visited);
// void bfs_corridor(char **map, Position start, Position target, int width, int height);
void connect_rooms_with_bfs(Room *rooms, int num_rooms, char **map, int width, int height, Corridor ** corridors , int *num_corridors);
void reveal_corridor(Position start, char **map, char **draw_map);
void next_level(Game* game);
void init_game(int n, User user, GameSettings *settings);
void load_game(Game* game , User user);
void save_game(Game *game, User user);
void init_game_timer(GameTimer *timer);
void *game_timer_thread(void *arg);
int create_window(int start_y, int start_x, int height, int width, int max_length, char *input, int pass);
int open_pass_door(Room* room , Game* game);
void* creat_password(void* arg);
void draw_weapon(Weapon *weapon , int start_x , int start_y);
void reveal_weapons(Weapon* weapon , Level* Level , Game* game);
void reveal_spells(Spell* spell , Level* Level , Game* game);
// void reveal_foods(Food* food, Level* Level , Game* game);
void reveal_foods(Food* food, Room* room , Level* level , Game* game);
void reveal_golds(Gold* gold , Level* Level , Game* game);
void toggle_weapon_menu(Game *game);
void draw_weapon_menu(Game *game);
void draw_player_info(Game *game);
void draw_message_box(Game* game , int start_x, int start_y, int width, int height);
void draw_food_menu(Game *game);
void toggle_food_menu(Game *game);
void toggle_spell_menu(Game *game);
void draw_spell_menu(Game *game);
void play_room_music(Game *game, Room *room);
void init_audio();
void load_music(Music *music, const char *file_path);
void stop_music(Music *music);
void play_music(Music *music);
void setting_menu(GameSettings *settings , User user);
void change_player_color(GameSettings *settings , User user);
void change_difficulty(GameSettings *settings , User user);
void move_monsters(Game* game);
void move_monster(Game* game , Monster* monster);
void active_monsters(Room* room);
void dis_active_monsters(Level* level);
void draw_monster(Monster* monster , int start_x , int start_y);
Monster* find_monster_at(Game *game, Position pos);
void handle_battle(Game *game, Monster *monster);
const char* weapon_type_to_str(WeaponType type);
int check_weapon_ammo(Player *player);
void reduce_weapon_ammo(Player *player);
void remove_monster(Level *level, Monster *monster);
void game_over(Game *game, int won);
int check_weapon_exists(Player *player, WeaponType type);
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
    setlocale(LC_ALL, "");
    initscr();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();

    init_color(CUSTOM_ORANGE, 1000, 500, 0);
    init_color(CUSTOM_PINK, 1000, 400, 800);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(4, CUSTOM_ORANGE, COLOR_BLACK);
    // init_pair(5, CUSTOM_ORANGE, COLOR_BLACK);
    init_pair(6, CUSTOM_PINK, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_BLUE , COLOR_BLACK);

    srand(time(0));
    init_audio(); 
    // rand_color();
    firstMenu();
    endwin();
}

// void creat_color(){

// }

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
    WINDOW *username_box, *password_box, *email_box;
    int username_index = 0, password_index = 0, email_index = 0;
    int active_box = 0; // 0: username, 1: password, 2: email

    
    curs_set(1);

    
    clear();
    draw_border();  
    refresh();

    
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Sign Up Menu");
    mvprintw(LINES / 2, COLS / 2 - 15, "Enter your username:");
    mvprintw(LINES / 2 + 4, COLS / 2 - 15, "Enter your password:");
    mvprintw(LINES / 2 + 8, COLS / 2 - 15, "Enter your email:");
    refresh();

    
    username_box = newwin(3, 32, LINES / 2 + 1, COLS / 2 - 15);
    password_box = newwin(3, 32, LINES / 2 + 5, COLS / 2 - 15);
    email_box = newwin(3, 32, LINES / 2 + 9, COLS / 2 - 15);

    box(username_box, 0, 0);
    box(password_box, 0, 0);
    box(email_box, 0, 0);

    wrefresh(username_box);
    wrefresh(password_box);
    wrefresh(email_box);

    while (1) {
        wmove(active_box == 0 ? username_box : (active_box == 1 ? password_box : email_box),
              1, active_box == 0 ? username_index + 1 : (active_box == 1 ? password_index + 1 : email_index + 1));
        wrefresh(active_box == 0 ? username_box : (active_box == 1 ? password_box : email_box));

        int ch = wgetch(active_box == 0 ? username_box : (active_box == 1 ? password_box : email_box));

        if (ch == '\t') {
            
            active_box = (active_box + 1) % 3;
        } else if (ch == '\n') {
            
            if (active_box == 2) break;
            active_box = (active_box + 1) % 3;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
           
            if (active_box == 0 && username_index > 0) {
                username[--username_index] = '\0';
                mvwaddch(username_box, 1, username_index + 1, ' ');
                wmove(username_box, 1, username_index + 1);
                wrefresh(username_box);
            } else if (active_box == 1 && password_index > 0) {
                password[--password_index] = '\0';
                mvwaddch(password_box, 1, password_index + 1, ' ');
                wmove(password_box, 1, password_index + 1);
                wrefresh(password_box);
            } else if (active_box == 2 && email_index > 0) {
                email[--email_index] = '\0';
                mvwaddch(email_box, 1, email_index + 1, ' ');
                wmove(email_box, 1, email_index + 1);
                wrefresh(email_box);
            }
        } else if (isprint(ch)) {
            
            if (active_box == 0 && username_index < sizeof(username) - 1) {
                username[username_index++] = ch;
                mvwaddch(username_box, 1, username_index, ch);
                wrefresh(username_box);
            } else if (active_box == 1 && password_index < sizeof(password) - 1) {
                password[password_index++] = ch;
                mvwaddch(password_box, 1, password_index, '*'); 
                wrefresh(password_box);
            } else if (active_box == 2 && email_index < sizeof(email) - 1) {
                email[email_index++] = ch;
                mvwaddch(email_box, 1, email_index, ch);
                wrefresh(email_box);
            }
        }
    }


    delwin(username_box);
    delwin(password_box);
    delwin(email_box);
    refresh();


    curs_set(0);


    int valid = 1;
    int greatword = 0, smallword = 0, digit = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (password[i] >= 'a' && password[i] <= 'z') smallword++;
        else if (password[i] >= 'A' && password[i] <= 'Z') greatword++;
        else if (password[i] >= '0' && password[i] <= '9') digit++;
    }
    if (strlen(password) < 7 || greatword == 0 || smallword == 0 || digit == 0) {
        mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Invalid Password. Must have 7+ chars, uppercase, lowercase, digit.");
        refresh();
        sleep(2);
        signUp_menu(); 
        return;
    }


    if (!validate_email(email)) {
        mvprintw(LINES / 2 - 6, COLS / 2 - 15, "Invalid Email Address!");
        refresh();
        sleep(2);
        signUp_menu();
        return;
    }


    if (checkSignUp(username, password)) {
        mvprintw(LINES / 2 - 8, COLS / 2 - 15, "You are already registered. Redirecting to Login.");
        refresh();
        sleep(2);
        logIn_menu();
    } else {
        mvprintw(LINES / 2 - 8, COLS / 2 - 15, "Registration successful!");
        refresh();
        sleep(2);
        logIn_menu();
    }
}

void logIn_menu() {
    char username[50] = {0};
    char password[20] = {0};
    WINDOW *username_box, *password_box;
    int username_index = 0, password_index = 0;
    int active_box = 0;
    
    curs_set(1);

    
    clear();
    draw_border();  
    refresh();

    
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Login Menu");
    mvprintw(LINES / 2, COLS / 2 - 15, "Enter your username:");
    mvprintw(LINES / 2 + 4, COLS / 2 - 15, "Enter your password:");
    refresh();

    
    username_box = newwin(3, 32, LINES / 2 + 1, COLS / 2 - 15);
    password_box = newwin(3, 32, LINES / 2 + 5, COLS / 2 - 15);
    box(username_box, 0, 0);
    box(password_box, 0, 0);
    wrefresh(username_box);
    wrefresh(password_box);

    while (1) {
        
        wmove(active_box == 0 ? username_box : password_box, 1, active_box == 0 ? username_index + 1 : password_index + 1);
        wrefresh(active_box == 0 ? username_box : password_box);

        int ch = wgetch(active_box == 0 ? username_box : password_box);

        if (ch == '\t') {
            
            active_box = 1 - active_box;
        } else if (ch == '\n') {
            
            if (active_box == 1) break;
            active_box = 1; 
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            
            if (active_box == 0 && username_index > 0) {
                username[--username_index] = '\0';
                mvwaddch(username_box, 1, username_index + 1, ' ');
                wmove(username_box, 1, username_index + 1);        
                wrefresh(username_box);
            } else if (active_box == 1 && password_index > 0) {
                password[--password_index] = '\0';
                mvwaddch(password_box, 1, password_index + 1, ' ');
                wmove(password_box, 1, password_index + 1);        
                wrefresh(password_box);
            }
        } else if (isprint(ch)) {

            if (active_box == 0 && username_index < sizeof(username) - 1) {
                username[username_index++] = ch;
                mvwaddch(username_box, 1, username_index, ch); 
                wrefresh(username_box);
            } else if (active_box == 1 && password_index < sizeof(password) - 1) {
                password[password_index++] = ch;
                mvwaddch(password_box, 1, password_index, '*');
                wrefresh(password_box);
            }
        }
    }


    delwin(username_box);
    delwin(password_box);
    refresh();


    curs_set(0);


    if (checkLogIn(username, password)) {
        User user;
        strcpy(user.user_name, username);
        strcpy(user.password, password);
        mvprintw(LINES / 2 - 8, COLS / 2 - 15, "Login successful!");
        refresh();
        sleep(2);
        clear();
        GameSettings settings = {0, 0};
        gameMenu(user , &settings);
    } else {
        mvprintw(LINES / 2 - 8, COLS / 2 - 15, "Invalid credentials! Please try signing up.");
        refresh();
        sleep(2);
        clear();
        signUp_menu();
    }
}

void gameMenu(User user , GameSettings* settings){

    clear();
    refresh();
    draw_border();


    char *choices[] = {"New Game", "Load Game", "Point Schedule", "Settings"};
    int num_choices = 4;
    int choice = 0;

    // GameSettings settings = {0, 0};

    
    int start_y = LINES / 2 - 6;
    int start_x = COLS / 2 - 12;


    while (1) {

        for (int i = 0; i < num_choices; i++) {
            WINDOW *button = newwin(3, 25, start_y + i * 4, start_x);
            box(button, 0, 0);
            refresh();
            // WINDOW *button = (COLS / 2 - 12 + i*4,LINES / 2 - 6,24, 3,choices[i]);
            if (i == choice) {
                // wbkgd(button, COLOR_PAIR(2));
                wattron(button, A_BOLD);      
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
        init_game(0 , user , settings);
    }
    else if(choice == 1){
        init_game(1 , user , settings);
    }
    else if(choice == 2){
        
    }
    else if(choice == 3){
        setting_menu(settings , user);
    }

}

void change_difficulty(GameSettings *settings , User user) {
    char *difficulty_levels[] = {"Easy 🟢", "Medium 🟡", "Hard 🔴"};
    int diff_choice = settings->difficulty;

    while (1) {
        clear();
        mvprintw(LINES / 2 - 3, COLS / 2 - 10, "Choose Difficulty Level:");
        for (int i = 0; i < 3; i++) {
            if (i == diff_choice) attron(A_BOLD);
            mvprintw(LINES / 2 + i, COLS / 2 - 5, "%s", difficulty_levels[i]);
            attroff(A_BOLD);
        }
        refresh();

        int ch = getch();
        if (ch == KEY_UP) diff_choice = (diff_choice == 0) ? 2 : diff_choice - 1;
        else if (ch == KEY_DOWN) diff_choice = (diff_choice == 2) ? 0 : diff_choice + 1;
        else if (ch == 10) break;
    }

    settings->difficulty = diff_choice;

    setting_menu(settings , user);
}


void change_player_color(GameSettings *settings , User user) {
    char *colors[] = {"Red 🔴", "Blue 🔵", "Green 🟢"};
    int color_choice = settings->player_color;

    while (1) {
        clear();
        mvprintw(LINES / 2 - 3, COLS / 2 - 10, "Choose Player Color:");
        for (int i = 0; i < 3; i++) {
            if (i == color_choice) attron(A_BOLD);
            mvprintw(LINES / 2 + i, COLS / 2 - 5, "%s", colors[i]);
            attroff(A_BOLD);
        }
        refresh();

        int ch = getch();
        if (ch == KEY_UP) color_choice = (color_choice == 0) ? 2 : color_choice - 1;
        else if (ch == KEY_DOWN) color_choice = (color_choice == 2) ? 0 : color_choice + 1;
        else if (ch == 10) break;
    }


    settings->player_color = color_choice;

    setting_menu(settings , user);
}


void setting_menu(GameSettings *settings , User user) {
    clear();
    refresh();

    char *choices[] = {"Difficulty ⚔️", "Player Color 🎨", "Back 🔙"};
    int num_choices = 3;
    int choice = 0;

    int start_y = LINES / 2 - 6;
    int start_x = COLS / 2 - 12;

    while (1) {
        for (int i = 0; i < num_choices; i++) {
            WINDOW *button = newwin(3, 30, start_y + i * 4, start_x);
            box(button, 0, 0);
            if (i == choice) wattron(button, A_BOLD);
            mvwprintw(button, 1, 15 - strlen(choices[i]) / 2, "%s", choices[i]);
            wattroff(button, A_BOLD);
            wrefresh(button);
            delwin(button);
        }

        int ch = getch();
        if (ch == KEY_UP) choice = (choice == 0) ? num_choices - 1 : choice - 1;
        else if (ch == KEY_DOWN) choice = (choice == num_choices - 1) ? 0 : choice + 1;
        else if (ch == 10) break;
    }

    if (choice == 0) {
        change_difficulty(settings , user);
    } else if (choice == 1) {
        change_player_color(settings , user);
    }
    else if(choice == 2){
        gameMenu(user , settings);
    }
}

void save_game(Game *game, User user) {
    FILE *file = fopen("save_game.dat", "wb");
    if (!file) {
        printf("Failed to open save file for writing.\n");
        return;
    }

    
    fwrite(&game->current_level, sizeof(int), 1, file);
    fwrite(&game->player.position, sizeof(Position), 1, file);
    fwrite(&game->player.hp, sizeof(int), 1, file);
    fwrite(&game->player.gold_collected, sizeof(int), 1, file);
    fwrite(&game->player.num_dagger, sizeof(int), 1, file);
    fwrite(&game->player.num_mace, sizeof(int), 1, file);
    fwrite(&game->player.num_sword, sizeof(int), 1, file);
    fwrite(&game->player.num_magic_wand, sizeof(int), 1, file);
    fwrite(&game->player.num_normal_arrow, sizeof(int), 1, file);
    fwrite(&game->player.num_damage_spell, sizeof(int), 1, file);
    fwrite(&game->player.num_health_spell, sizeof(int), 1, file);
    fwrite(&game->player.num_speed_spell, sizeof(int), 1, file);
    fwrite(&game->player.num_apple, sizeof(int), 1, file);
    fwrite(&game->player.num_bread, sizeof(int), 1, file);
    fwrite(&game->player.num_carrot, sizeof(int), 1, file);
    fwrite(&game->player.num_fish, sizeof(int), 1, file);
    fwrite(&game->player.num_egg, sizeof(int), 1, file);
    fwrite(&game->player.selected_weapon, sizeof(Weapon), 1, file);
    fwrite(&game->music, sizeof(Music), 1, file);
    fwrite(&game->difficulty, sizeof(int), 1, file);
    fwrite(&game->player.color, sizeof(int), 1, file);

    
    for (int i = 0; i < game->current_level; i++) {
        Level *level = &game->levels[i];

        
        fwrite(&level->width, sizeof(int), 1, file);
        fwrite(&level->height, sizeof(int), 1, file);
        fwrite(&level->num_rooms, sizeof(int), 1, file);
        fwrite(&level->num_corridors, sizeof(int), 1, file);

        
        for (int j = 0; j < level->height; j++) {
            fwrite(level->map[j], sizeof(char), level->width, file);
            fwrite(level->drawMap[j], sizeof(char), level->width, file);
        }

        
        for (int j = 0; j < level->num_rooms; j++) {
            Room *room = &level->rooms[j];
            fwrite(&room->position, sizeof(Position), 1, file);
            fwrite(&room->type, sizeof(int), 1, file);
            fwrite(&room->num_doors, sizeof(int), 1, file);
            fwrite(&room->num_objects, sizeof(int), 1, file);
            fwrite(&room->num_foods, sizeof(int), 1, file);
            fwrite(&room->num_golds, sizeof(int), 1, file);
            fwrite(&room->num_weapons, sizeof(int), 1, file);
            fwrite(&room->num_spells, sizeof(int), 1, file);
            fwrite(&room->num_monsters, sizeof(int), 1, file);

            
            for (int k = 0; k < room->num_doors; k++) {
                fwrite(&room->doors[k], sizeof(Door), 1, file);
            }

            
            for (int k = 0; k < room->num_objects; k++) {
                fwrite(&room->object[k], sizeof(Position), 1, file);
            }

            
            for (int k = 0; k < room->num_foods; k++) {
                fwrite(&room->foods[k], sizeof(Food), 1, file);
            }

            
            for (int k = 0; k < room->num_golds; k++) {
                fwrite(&room->golds[k], sizeof(Gold), 1, file);
            }

            
            for (int k = 0; k < room->num_weapons; k++) {
                fwrite(&room->weapons[k], sizeof(Weapon), 1, file);
            }

            
            for (int k = 0; k < room->num_spells; k++) {
                fwrite(&room->spells[k], sizeof(Spell), 1, file);
            }

            
            for (int k = 0; k < room->num_monsters; k++) {
                fwrite(&room->monsters[k], sizeof(Monster), 1, file);
            }
        }

        fwrite(level->corridors, sizeof(Corridor), level->num_corridors, file);
    }

    fclose(file);
    printf("Game saved successfully.\n");
}


// void save_game(Game *game, User user) {
//     char file_path[100];
//     sprintf(file_path , "%s/save.dat" , user.user_name);
//     FILE *file = fopen(file_path, "wb");
//     if (!file) {
//         perror("Error opening file for saving");
//         return;
//     }

//     // Save basic game information
//     fwrite(&game->player, sizeof(Player), 1, file);
//     fwrite(&game->total_levels, sizeof(int), 1, file);
//     fwrite(&game->current_level, sizeof(int), 1, file);

//     // Save levels
//     for (int i = 0; i <= game->current_level; i++) {
//         Level *level = &game->levels[i];
//         fwrite(&level->width, sizeof(int), 1, file);
//         fwrite(&level->height, sizeof(int), 1, file);
//         fwrite(&level->num_rooms, sizeof(int), 1, file);
        // fwrite(&level->num_corridors, sizeof(int), 1, file);

//         // Save map
//         for (int y = 0; y < level->height; y++) {
//             fwrite(level->map[y], sizeof(char), level->width, file);
//         }

//         for (int x = 0; x < level->height; x++) {
//             fwrite(level->drawMap[x], sizeof(char), level->width, file);
//         }

//         // Save rooms
//         for (int j = 0; j < level->num_rooms; j++) {
//             Room *room = &level->rooms[j];
//             fwrite(&room->position, sizeof(Position), 1, file);
//             fwrite(&room->width, sizeof(int), 1, file);
//             fwrite(&room->height, sizeof(int), 1, file);
//             fwrite(&room->num_doors, sizeof(int), 1, file);
//             fwrite(&room->num_objects, sizeof(int), 1, file);
//             fwrite(&room->num_traps, sizeof(int), 1, file);

//             fwrite(room->doors, sizeof(Door), room->num_doors, file);
//             fwrite(room->object, sizeof(Position), room->num_objects, file);
//             fwrite(room->traps, sizeof(Trap), room->num_traps, file);
//         }

//         // Save corridors
//         fwrite(level->corridors, sizeof(Corridor), level->num_corridors, file);
//     }

//     fclose(file);
//     // printf("Game saved successfully!\n");
// }

void load_game(Game *game, User user) {
    FILE *file = fopen("save_game.dat", "rb");
    if (!file) {
        printf("Failed to open save file for reading.\n");
        return;
    }

    
    fread(&game->current_level, sizeof(int), 1, file);
    fread(&game->player.position, sizeof(Position), 1, file);
    fread(&game->player.hp, sizeof(int), 1, file);
    fread(&game->player.gold_collected, sizeof(int), 1, file);
    fread(&game->player.num_dagger, sizeof(int), 1, file);
    fread(&game->player.num_mace, sizeof(int), 1, file);
    fread(&game->player.num_sword, sizeof(int), 1, file);
    fread(&game->player.num_magic_wand, sizeof(int), 1, file);
    fread(&game->player.num_normal_arrow, sizeof(int), 1, file);
    fread(&game->player.num_damage_spell, sizeof(int), 1, file);
    fread(&game->player.num_health_spell, sizeof(int), 1, file);
    fread(&game->player.num_speed_spell, sizeof(int), 1, file);
    fread(&game->player.num_apple, sizeof(int), 1, file);
    fread(&game->player.num_bread, sizeof(int), 1, file);
    fread(&game->player.num_carrot, sizeof(int), 1, file);
    fread(&game->player.num_fish, sizeof(int), 1, file);
    fread(&game->player.num_egg, sizeof(int), 1, file);
    fread(&game->player.selected_weapon, sizeof(Weapon), 1, file);
    fread(&game->music, sizeof(Music), 1, file);
    fread(&game->difficulty, sizeof(int), 1, file);
    fread(&game->player.color, sizeof(int), 1, file);

    
    for (int i = 0; i < game->current_level; i++) {
        Level *level = &game->levels[i];
        fread(&level->width, sizeof(int), 1, file);
        fread(&level->height, sizeof(int), 1, file);
        fread(&level->num_rooms, sizeof(int), 1, file);
        fread(&level->num_corridors, sizeof(int), 1, file);

        
        for (int j = 0; j < level->height; j++) {
            fread(level->map[j], sizeof(char), level->width, file);
            fread(level->drawMap[j], sizeof(char), level->width, file);
        }

        
        for (int j = 0; j < level->num_rooms; j++) {
            Room *room = &level->rooms[j];
            fread(&room->position, sizeof(Position), 1, file);
            fread(&room->type, sizeof(int), 1, file);
            fread(&room->num_doors, sizeof(int), 1, file);
            fread(&room->num_objects, sizeof(int), 1, file);
            fread(&room->num_foods, sizeof(int), 1, file);
            fread(&room->num_golds, sizeof(int), 1, file);
            fread(&room->num_weapons, sizeof(int), 1, file);
            fread(&room->num_spells, sizeof(int), 1, file);
            fread(&room->num_monsters, sizeof(int), 1, file);

            
            for (int k = 0; k < room->num_doors; k++) {
                fread(&room->doors[k], sizeof(Door), 1, file);
            }

            
            for (int k = 0; k < room->num_objects; k++) {
                fread(&room->object[k], sizeof(Position), 1, file);
            }

            
            for (int k = 0; k < room->num_foods; k++) {
                fread(&room->foods[k], sizeof(Food), 1, file);
            }

            
            for (int k = 0; k < room->num_golds; k++) {
                fread(&room->golds[k], sizeof(Gold), 1, file);
            }

            
            for (int k = 0; k < room->num_weapons; k++) {
                fread(&room->weapons[k], sizeof(Weapon), 1, file);
            }

            
            for (int k = 0; k < room->num_spells; k++) {
                fread(&room->spells[k], sizeof(Spell), 1, file);
            }

            
            for (int k = 0; k < room->num_monsters; k++) {
                fread(&room->monsters[k], sizeof(Monster), 1, file);
            }
        }

        level->corridors = malloc(level->num_corridors * sizeof(Corridor));
        fread(level->corridors, sizeof(Corridor), level->num_corridors, file);
    }

    fclose(file);
    printf("Game loaded successfully.\n");
}


// void load_game(Game *game, User user) {
//     char file_path[100];
//     sprintf(file_path , "%s/save.dat" , user.user_name);
//     FILE *file = fopen(file_path, "rb");
//     if (!file) {
//         perror("Error opening file for loading");
//         return;
//     }

//     // Load basic game information
//     fread(&game->player, sizeof(Player), 1, file);
//     fread(&game->total_levels, sizeof(int), 1, file);
//     fread(&game->current_level, sizeof(int), 1, file);

//     // Allocate memory for levels
//     game->levels = malloc(game->total_levels * sizeof(Level));

//     for (int i = 0; i <= game->current_level; i++) {
//         Level *level = &game->levels[i];
//         fread(&level->width, sizeof(int), 1, file);
//         fread(&level->height, sizeof(int), 1, file);
//         fread(&level->num_rooms, sizeof(int), 1, file);
//         fread(&level->num_corridors, sizeof(int), 1, file);

//         // Allocate memory for map
//         level->map = create_map(level->width, level->height);
//         for (int y = 0; y < level->height; y++) {
//             fread(level->map[y], sizeof(char), level->width, file);
//         }

//         level->drawMap = create_map(level->width, level->height);
//         for (int x = 0; x < level->height; x++) {
//             fread(level->drawMap[x], sizeof(char), level->width, file);
//         }

//         // Allocate memory for rooms
//         level->rooms = malloc(level->num_rooms * sizeof(Room));
//         for (int j = 0; j < level->num_rooms; j++) {
//             Room *room = &level->rooms[j];
//             fread(&room->position, sizeof(Position), 1, file);
//             fread(&room->width, sizeof(int), 1, file);
//             fread(&room->height, sizeof(int), 1, file);
//             fread(&room->num_doors, sizeof(int), 1, file);
//             fread(&room->num_objects, sizeof(int), 1, file);
//             fread(&room->num_traps, sizeof(int), 1, file);

//             room->doors = malloc(room->num_doors * sizeof(Door));
//             fread(room->doors, sizeof(Door), room->num_doors, file);

//             room->object = malloc(room->num_objects * sizeof(Position));
//             fread(room->object, sizeof(Position), room->num_objects, file);

//             room->traps = malloc(room->num_traps * sizeof(Trap));
//             fread(room->traps, sizeof(Trap), room->num_traps, file);
//         }

//         // Allocate memory for corridors
//         level->corridors = malloc(level->num_corridors * sizeof(Corridor));
//         fread(level->corridors, sizeof(Corridor), level->num_corridors, file);
//     }

//     fclose(file);
//     // printf("Game loaded successfully!\n");
// }

void init_game(int n , User user , GameSettings *settings){
    // initscr();
    noecho();
    // keypad(stdscr, TRUE);
    // curs_set(FALSE);
    // srand(time(NULL));


    strcpy(curent_user.user_name , user.user_name);
    strcpy(curent_user.password , user.password);

    // Game setup
    Game game;
    if (n==0)
    {
        game.total_levels = 3;
        game.current_level = 0;
        int map_width = COLS - 80, map_height = LINES -6, num_rooms = 6 + rand() % (game.current_level + 1);
        game.levels = malloc(4 * sizeof(Level));
        game.levels[game.current_level] = create_level(map_width, map_height, num_rooms);
        game.game_message = (char*)malloc(100 * sizeof(char));


        // Initialize player
        game.player.position.x = game.levels[game.current_level].rooms[0].position.x + game.levels[game.current_level].rooms[0].width/2;
        game.player.position.y = game.levels[game.current_level].rooms[0].position.y + game.levels[game.current_level].rooms[0].height/2;

        game.player.hp = 100;
        game.player.gold_collected = 0;
        game.player.num_dagger = 0;
        game.player.num_mace = 0;
        game.player.num_sword = 0;
        game.player.num_magic_wand = 0;
        game.player.num_normal_arrow = 0;
        game.player.num_damage_spell = 0;
        game.player.num_health_spell = 0;
        game.player.num_speed_spell = 0;
        game.player.num_apple = 0;
        game.player.num_bread = 0;
        game.player.num_carrot = 0;
        game.player.num_fish = 0;
        game.player.num_egg = 0;
        game.music.music = NULL;
        game.music.is_playing = 0;
        game.difficulty = settings->difficulty;
        game.player.color = settings->player_color;
        game.game_message = "Lets go!";
        game.player.weapons = malloc(1 * sizeof(Weapon));
        Weapon mace;
        mace.attack_power = 5;
        mace.position.y =-1;
        mace.position.x = -1;
        mace.type = WEAPON_Mace;
        game.player.weapons[0] = mace;
        game.player.num_mace = 1000;
        game.player.num_weapons = 1;
        game.player.selected_weapon = game.player.weapons[0];
        // game.music.is_playing = 0;
        // game.music.volume = 64;


        // Initially reveal the starting room
        // reveal_room(game.levels[0].rooms[0], game.levels[0].map);
        reveal_room(game.levels[game.current_level].rooms[0],game.levels[game.current_level].map ,game.levels[game.current_level].drawMap);

    }
    else{
        load_game(&game , user);
        // game.current_level = 0;
    }


    pthread_t timer_thread;
    // TimerArgs timer_args = {&game, 300}; 
    pthread_create(&timer_thread, NULL, game_timer_thread, &game);
    // Game loop
    int key;
    while (1) {
        draw_game(&game);
        refresh();
        key = getch();
        if (key == 'q'){
            // pause_menu();
            // save_game(&game , user);
            break;
        }
        move_player(&game, key);
        move_monsters(&game);
        draw_game(&game);
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

void init_game_timer(GameTimer *timer) {
    timer->total_time = 300;
    timer->penalty = 10;     
    timer->update_interval = 30;
}

void *game_timer_thread(void *arg) {
    Game *game = (Game *)arg;
    GameTimer timer;
    init_game_timer(&timer);
    Level *level = &game->levels[game->current_level];

    while (timer.total_time > 0) {
        sleep(1);
        timer.total_time--;

        
        if (timer.total_time % timer.update_interval == 0) {
            game->player.hp -= timer.penalty;
        }

        game->timer.minute = timer.total_time / 60;
        game->timer.second = timer.total_time % 60;
        
        // mvprintw(level->height + 3, 0, "Time Remaining: %02d:%02d", timer.total_time / 60, timer.total_time % 60);
        // mvprintw(level->height + 4, 0, "Player HP: %d", game->player.hp);

        
        if (game->player.hp <= 0) {
            clear();
            mvprintw(LINES/2, COLS/2-10, "Game Over! You lost.");
            refresh();
            sleep(3);
            endwin();
            exit(0);
        }

        refresh();
    }

    
    clear();
    mvprintw(LINES/2, COLS/2-10, "Game Over! Time's up.");
    refresh();
    sleep(3);
    endwin();
    exit(0);
}


void next_level(Game* game){

    if (game->current_level == 3) {
        save_game(game, curent_user);
        game_over(game , 1);
    }

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

    int map_width = COLS - 80, map_height = LINES - 6;
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
    room.num_doors = 1 + rand() % 2;
    room.num_objects = rand() % 2;
    room.doors = malloc(room.num_doors * sizeof(Door));
    room.object = malloc(room.num_objects * sizeof(Position));
    room.num_pass_door = rand()%2;
    room.num_weapons = rand() % 2;
    room.weapons = malloc(room.num_weapons * sizeof(Weapon));
    room.num_foods = rand() % 3;
    room.foods = malloc(room.num_foods * sizeof(Food));
    room.num_golds = rand() % 3;
    room.golds = malloc(room.num_golds * sizeof(Gold));
    room.music_path = NULL;/////
    room.num_monsters = 1 + rand() % 2;
    room.monsters = malloc(room.num_monsters * sizeof(Monster));

    int rand_type = rand() % 100;
    if (rand_type < 60) room.type = ROOM_NORMAL;   
    else if (rand_type < 75) room.type = ROOM_GOLD; 
    else if (rand_type < 100) room.type = ROOM_SPELL; 
    // else room.type = ROOM_TRAP;

    if (room.type == ROOM_NORMAL)
    {
        room.num_traps = 1 + rand() % 2;///level
        room.num_spells = rand() % 2;
        // room.music_track = malloc(100 * sizeof(char));
        // room.music_track = strdup("Musics/Butcher_mp3.mp3"); 
        // strcpy(room.music_path, "Musics/Butcher_mp3.mp3");
        room.music_path = strdup("Musics/Butcher_mp3.mp3");


        /* code */
    }
    else if (ROOM_GOLD)
    {
        room.num_traps = 4 + rand() % 2;///level
        room.num_spells = rand() % 2;
        // room.music_path = strdup("Musics/Best_Shot_mp3.mp3"); 
        // strcpy(room.music_path, "Musics/Best_Shot_mp3.mp3");
        room.music_path = strdup("Musics/Best_Shot_mp3.mp3");


        /* code */
    }
    else if (ROOM_SPELL){
        room.num_traps = 1 + rand() % 2;///level
        room.num_spells = 2 + rand() % 2;
        // room.music_path = strdup("Musics/Halloween_Party_Night_mp3.mp3"); 
        // strcpy(room.music_path, "Musics/Halloween_Party_Night_mp3.mp3");
        room.music_path = strdup("Musics/Halloween_Party_Night_mp3.mp3");




    }

    room.traps = malloc(room.num_traps * sizeof(Trap));
    room.spells = malloc(room.num_spells * sizeof(Spell));

    
    


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

    if (room.num_pass_door)
    {
        int side = rand() % 4; 

        switch (side) {///if , else if
            case 0: 
                room.pass_door.position.y = y; 
                room.pass_door.position.x = x + 1 + rand() % (width - 2);
                room.pass_key.y = y+1;
                room.pass_key.x = x+1;
                break;
            case 1: 
                room.pass_door.position.y = y + height - 1; 
                room.pass_door.position.x = x + 1 + rand() % (width - 2);
                room.pass_key.y = y + height - 2;
                room.pass_key.x = x + width - 2; 
                break;
            case 2: 
                room.pass_door.position.y = y + 1 + rand() % (height - 2); 
                room.pass_door.position.x = x;
                room.pass_key.y = y + height - 2;
                room.pass_key.x = x +1; 
                break;
            case 3: 
                room.pass_door.position.y = y + 1 + rand() % (height - 2); 
                room.pass_door.position.x = x + width - 1; 
                room.pass_key.y = y + 1;
                room.pass_key.x = x + width - 2;
                break;
        }
        room.pass_door.is_open = 0;
    }

    for (int i = 0; i < room.num_foods; i++) {
        room.foods[i].position.x = x + 1 + rand() % (width - 2);
        room.foods[i].position.y = y + 1 + rand() % (height - 2);
        room.foods[i].type = rand() % 2;
        room.foods[i].hp_restore = (rand() % 15) + 5;
    }

    for (int i = 0; i < room.num_weapons; i++) {
        room.weapons[i].type = 1 + rand() % 4;
        switch (room.weapons[i].type)
        {
        case WEAPON_Sword:
            room.weapons[i].attack_power = 10;
            break;
        case WEAPON_Dagger:
            room.weapons[i].attack_power = 12;
            break;
        case WEAPON_Magic_Wand:
            room.weapons[i].attack_power = 15;
            break;
        case WEAPON_Normal_Arrow:
            room.weapons[i].attack_power = 5;
            break;           
        } 
        room.weapons[i].position.x =  x + 1 + rand() % (width - 2);
        room.weapons[i].position.y =  y + 1 + rand() % (height - 2);
        // room.weapons[i].durability = (rand() % 5) + 1;
        // room.weapons[i].special_effect = rand() % 3; 
    }
    // room.pass_door = malloc(room.num_pass_door * sizeof(Door));

    
    for (int i = 0; i < room.num_golds; i++) {
        room.golds[i].position.x = x + 1 + rand() % (width - 2);
        room.golds[i].position.y = y + 1 + rand() % (height - 2);
        room.golds[i].value = (rand() % 20) + 10;
    }

    
    for (int i = 0; i < room.num_spells; i++) {
        room.spells[i].position.x = x + 1 + rand() % (width - 2);
        room.spells[i].position.y = y + 1 + rand() % (height - 2);
        room.spells[i].effect_type = rand() % 3;
        room.spells[i].duration = (rand() % 10) + 5;
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
    for (int i = 0; i < room.num_monsters; i++)
    {
        Monster* monster = &room.monsters[i];
        monster->type = rand() % 5;
        switch (monster->type)
        {
        case Deaⅿon:
            monster->damage = 1;
            monster->health = 5;
            monster->is_active = 0;
            monster->last_tile = '.';
            monster->max_follow_steps = 5;
            monster->position.x = x + 1 + rand() % (width - 2);
            monster->position.y = y + 1 + rand() % (height - 2);
            break;
        case Dragon:
            monster->damage = 2;
            monster->health = 10;
            monster->is_active = 0;
            monster->last_tile = '.';
            monster->max_follow_steps = 7;
            monster->position.x = x + 1 + rand() % (width - 2);
            monster->position.y = y + 1 + rand() % (height - 2);
            break;
        case Giant:
            monster->damage = 3;
            monster->health = 15;
            monster->is_active = 0;
            monster->last_tile = '.';
            monster->max_follow_steps = 7;
            monster->position.x = x + 1 + rand() % (width - 2);
            monster->position.y = y + 1 + rand() % (height - 2);
            break;
        case Snake:
            monster->damage = 4;
            monster->health = 20;
            monster->is_active = 0;
            monster->last_tile = '.';
            monster->max_follow_steps = 1000;
            monster->position.x = x + 1 + rand() % (width - 2);
            monster->position.y = y + 1 + rand() % (height - 2);
            break;
        case Undead:
            monster->damage = 5;
            monster->health = 30;
            monster->is_active = 0;
            monster->last_tile = '.';
            monster->max_follow_steps = 1000;
            monster->position.x = x + 1 + rand() % (width - 2);
            monster->position.y = y + 1 + rand() % (height - 2);
            break;        
        }

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
    for(int i =0; i< room.num_pass_door; i++){
        map[room.pass_door.position.y][room.pass_door.position.x] = '@';
        map[room.pass_key.y][room.pass_key.x] = '&';
    }
    for (int i = 0; i < room.num_foods; i++) {
        map[room.foods[i].position.y][room.foods[i].position.x] = 'F'; 
    }


    for (int i = 0; i < room.num_golds; i++) {
        map[room.golds[i].position.y][room.golds[i].position.x] = 'G';
    }


    for (int i = 0; i < room.num_weapons; i++) {
        map[room.weapons[i].position.y][room.weapons[i].position.x] = 'W'; 
    }


    for (int i = 0; i < room.num_spells; i++) {
        map[room.spells[i].position.y][room.spells[i].position.x] = 'S'; 
    }

    for (int i = 0; i < room.num_monsters; i++) {
        map[room.spells[i].position.y][room.spells[i].position.x] = 'M'; 
    }

    if (s == 1)
    {
        map[room.position.y + 1 +rand() % (room.height - 2)][room.position.x + 1 +rand() % (room.width - 2)] = 'P';
    }

}


void reveal_room(Room room, char **map, char** drawMap) {
    for (int i = 0; i < room.height; i++) {
        for (int j = 0; j < room.width; j++) {
            char current_char = map[room.position.y + i][room.position.x + j];
            if (current_char == '-' || current_char == '|' || current_char == '.' || current_char == '+' || current_char == 'O' || current_char == 'T' || current_char == 'P'|| current_char == '@' || current_char == '&' || current_char == 'F' || current_char == 'W' || current_char == 'G' || current_char == 'S' || current_char == 'M') {
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

        if (current_char1 == '#' || current_char1 == '+' || current_char1 == '@')
        {
            draw_map[start.y + 1][start.x] = current_char1;
        }
        if (current_char2 == '#' || current_char2 == '+' || current_char2 == '@')
        {
            draw_map[start.y][start.x + 1] = current_char2;
        }
        if (current_char3 == '#' || current_char3 == '+' || current_char3 == '@')
        {
            draw_map[start.y - 1][start.x] = current_char3;
        }
        if (current_char4 == '#' || current_char4 == '+' || current_char4 == '@')
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
                (map[newY][newX] == ' '|| map[newY][newX] == '+'|| map[newY][newX] == '@' || map[newY][newX] == '#') && !visited[newY][newX]) {
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

    for (int i = 0; i < num_rooms; i++) {

        for (int j = 0; j < rooms[i].num_doors; j++) {
            for (int k = 0; k < num_rooms; k++) {
                if (i == k) continue;

                for (int l = 0; l < rooms[k].num_doors; l++) {

                    Position start = rooms[i].doors[j].position;
                    Position target = rooms[k].doors[l].position;

                    for (int y = 0; y < height; y++) {
                        memset(visited[y], 0, width * sizeof(int));
                        for (int x = 0; x < width; x++) {
                            parent[y][x] = (Position){-1, -1};
                        }
                    }


                    if (bfs_corridor(map, start, target, width, height, parent, visited)) {
                        Position current = target;
                        while (current.x != start.x || current.y != start.y) {
                            if (map[current.y][current.x] == ' ') {
                                map[current.y][current.x] = '#';
                            } 
                            current = parent[current.y][current.x];
                        }

                        (*corridors)[*num_corridors].start = start;
                        (*corridors)[*num_corridors].end = target;
                        (*num_corridors)++;
                    }
                }
            }
        }


        if (rooms[i].num_pass_door > 0) {
            Position pass_door_pos = rooms[i].pass_door.position;
            for (int k = 0; k < num_rooms; k++) {
                if (i == k) continue;

                for (int l = 0; l < rooms[k].num_doors; l++) {
                    Position target = rooms[k].doors[l].position;

                    for (int y = 0; y < height; y++) {
                        memset(visited[y], 0, width * sizeof(int));
                        for (int x = 0; x < width; x++) {
                            parent[y][x] = (Position){-1, -1};
                        }
                    }


                    if (bfs_corridor(map, pass_door_pos, target, width, height, parent, visited)) {
                        Position current = target;
                        while (current.x != pass_door_pos.x || current.y != pass_door_pos.y) {
                            if (map[current.y][current.x] == ' ') {
                                map[current.y][current.x] = '#';
                            } 
                            current = parent[current.y][current.x];
                        }


                        (*corridors)[*num_corridors].start = pass_door_pos;
                        (*corridors)[*num_corridors].end = target;
                        (*num_corridors)++;
                    }
                }


                if (rooms[k].num_pass_door > 0) {
                    Position target_pass_door_pos = rooms[k].pass_door.position;


                    for (int y = 0; y < height; y++) {
                        memset(visited[y], 0, width * sizeof(int));
                        for (int x = 0; x < width; x++) {
                            parent[y][x] = (Position){-1, -1};
                        }
                    }


                    if (bfs_corridor(map, pass_door_pos, target_pass_door_pos, width, height, parent, visited)) {
                        Position current = target_pass_door_pos;
                        while (current.x != pass_door_pos.x || current.y != pass_door_pos.y) {
                            if (map[current.y][current.x] == ' ') {
                                map[current.y][current.x] = '#';
                            } 
                            current = parent[current.y][current.x];
                        }

                        (*corridors)[*num_corridors].start = pass_door_pos;
                        (*corridors)[*num_corridors].end = target_pass_door_pos;
                        (*num_corridors)++;
                    }
                }
            }
        }
    }


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

void reveal_weapons(Weapon* weapon , Level* level , Game* game){
    switch (weapon->type) {
        case WEAPON_Sword: game->player.num_sword++; break;
        case WEAPON_Mace: game->player.num_mace++; break;
        case WEAPON_Normal_Arrow: game->player.num_normal_arrow++; break;
        case WEAPON_Magic_Wand: game->player.num_magic_wand++; break;
        case WEAPON_Dagger: game->player.num_dagger++; break;
        // default: symbol = "?";
    }


    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_weapons; j++) {
            if (&level->rooms[i].weapons[j] == weapon) {

                for (int k = j; k < level->rooms[i].num_weapons - 1; k++) {
                    level->rooms[i].weapons[k] = level->rooms[i].weapons[k + 1];
                }
                level->rooms[i].num_weapons--;
                break;
            }
        }
    }


    level->map[weapon->position.y][weapon->position.x] = '.';
    level->drawMap[weapon->position.y][weapon->position.x] = '.';

}

void reveal_foods(Food* food, Room* room , Level* level , Game* game){
    switch (food->type) {
        case FOOD_Apple:  {
            game->player.hungry -= 2;
            game->player.num_apple++;
            break;
        }
        case FOOD_Egg:  {
            game->player.hungry -= 3; 
            game->player.num_egg++;
            break;
        }
        case FOOD_Bread:  {
            game->player.hungry -= 4; 
            game->player.num_bread++;
            break;
        }
        case FOOD_Carot:  {
            game->player.hungry -= 2; 
            game->player.num_carrot++;
            break;
        }
        case FOOD_Fish:  {
            game->player.hungry -= 5; 
            game->player.num_fish++;
            break;
        }
    }


    // for (int i = 0; i < level->num_rooms; i++) {
    //     for (int j = 0; j < level->rooms[i].num_foods; j++) {
    //         if (&level->rooms[i].foods[j] == food) {

    //             for (int k = j; k < level->rooms[i].num_foods - 1; k++) {
    //                 level->rooms[i].foods[k] = level->rooms[i].foods[k + 1];
    //             }
    //             level->rooms[i].num_foods--;
    //             break;
    //         }
    //     }
    // }

    // for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < room->num_foods; j++) {
            if (room->foods[j].position.x == food->position.x &&
                room->foods[j].position.y == food->position.y) {

            // حذف غذا از آرایه
                for (int k = j; k < room->num_foods - 1; k++) {
                    room->foods[k] = room->foods[k + 1];
                }
                room->num_foods--;  // کاهش تعداد غذاهای موجود
                break;
            }
        }
    // }


    // for (int j = 0; j < room->num_foods; j++) {
        

    //         for (int k = j; k < level->rooms[i].num_foods - 1; k++) {
    //             level->rooms[i].foods[k] = level->rooms[i].foods[k + 1];
    //         }
    //         level->rooms[i].num_foods--;
    //         break;
        
    // }


    level->map[food->position.y][food->position.x] = '.';
    level->drawMap[food->position.y][food->position.x] = '.';
}

void reveal_spells(Spell* spell , Level* level , Game* game){
    switch (spell->type) {
        case SPELL_Health:  game->player.num_health_spell++; break;
        case SPELL_Damage:  game->player.num_damage_spell++; break;
        case SPELL_Speed:  game->player.num_speed_spell++; break;
    }




    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_spells; j++) {
            if (&level->rooms[i].spells[j] == spell) {

                for (int k = j; k < level->rooms[i].num_spells - 1; k++) {
                    level->rooms[i].spells[k] = level->rooms[i].spells[k + 1];
                }
                level->rooms[i].num_spells--;
                break;
            }
        }
    }


    level->map[spell->position.y][spell->position.x] = '.';
    level->drawMap[spell->position.y][spell->position.x] = '.';
}

void reveal_golds(Gold* gold , Level* level , Game* game){
    game->player.gold_collected++;

    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_golds; j++) {
            if (&level->rooms[i].golds[j] == gold) {

                for (int k = j; k < level->rooms[i].num_golds - 1; k++) {
                    level->rooms[i].golds[k] = level->rooms[i].golds[k + 1];
                }
                level->rooms[i].num_golds--;
                break;
            }
        }
    }

    level->map[gold->position.y][gold->position.x] = '.';
    level->drawMap[gold->position.y][gold->position.x] = '.';
}


void init_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Failed to initialize SDL_mixer: %s\n", Mix_GetError());
        exit(1);
    }
}


void load_music(Music *music, const char *file_path) {
    if (music == NULL) return; // بررسی مقداردهی
    if (music->music != NULL) {
        Mix_FreeMusic(music->music);
    }

    music->music = Mix_LoadMUS(file_path);
    if (!music->music) {
        printf("Failed to load music: %s\n", Mix_GetError());
    }
}

void stop_music(Music *music) {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    music->is_playing = 0;
}

void play_music(Music *music) {
    if (music == NULL || music->music == NULL) return;

    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }

    Mix_PlayMusic(music->music, -1);
    music->is_playing = 1;
}

void play_room_music(Game *game, Room *room) {
    if (room->music_path == NULL || room->music_path[0] == '\0') { 
        return;
    }

    if (!Mix_PlayingMusic()) {
        load_music(&game->music, room->music_path);
        play_music(&game->music);
    }
}

void active_monsters(Room* room){
    for (int i = 0; i < room->num_monsters; i++)
    {
        room->monsters[i].is_active = 1;
    }
}

void dis_active_monsters(Level* level){
    for (int i = 0; i < level->num_rooms; i++)
    {
        for (int j = 0; j < level->rooms[i].num_monsters; j++)
        {
            level->rooms[i].monsters[j].is_active = 0;
        }
        
    }
    
}

void add_weapon_to_player(Game* game, Weapon* new_weapon){
    for (int i = 0; i < game->player.num_weapons; i++) {
        if (game->player.weapons[i].type == new_weapon->type) {
            // اگر اسلحه از قبل موجود باشد، تعداد آن را افزایش می‌دهیم
            switch (new_weapon->type) {
                case WEAPON_Sword: game->player.num_sword++; break;
                // case WEAPON_Mace: symbol = "⚒"; break;
                case WEAPON_Normal_Arrow: game->player.num_normal_arrow++; break;
                case WEAPON_Magic_Wand: game->player.num_magic_wand; break;
                case WEAPON_Dagger: game->player.num_dagger++; break; 
            }
            return;
        }
    }
    
    // اگر اسلحه جدید است، آن را به لیست سلاح‌ها اضافه می‌کنیم
    game->player.num_weapons += 1;
    switch (new_weapon->type) {
        case WEAPON_Sword: game->player.num_sword++; break;
                // case WEAPON_Mace: symbol = "⚒"; break;
        case WEAPON_Normal_Arrow: game->player.num_normal_arrow++; break;
        case WEAPON_Magic_Wand: game->player.num_magic_wand; break;
        case WEAPON_Dagger: game->player.num_dagger++; break; 
    }
    game->player.weapons = realloc(game->player.weapons, game->player.num_weapons * sizeof(Weapon));
    game->player.weapons[game->player.num_weapons - 1] = *new_weapon;
}

void move_player(Game *game, int key) {

    if (key == 'g') {
        // toggle_weapon_menu(game);
        draw_weapon_menu(game);
        return;
    }

    if (key == 'f') {
        toggle_food_menu(game);
        return;
    }
    if (key == 't') {
        toggle_spell_menu(game);
        return;
    }

    Position new_pos = game->player.position;
    Level *level = &game->levels[game->current_level];
    char **map = level->map;
    char **drawMap = level->drawMap;

    int start_y = 0;
    int start_x =  (COLS - level->width) / 2;
    if (key == KEY_UP && new_pos.y > 0) new_pos.y--;
    else if (key == KEY_DOWN && new_pos.y < level->height - 1) new_pos.y++;
    else if (key == KEY_LEFT && new_pos.x > 0) new_pos.x--;
    else if (key == KEY_RIGHT && new_pos.x < level->width - 1) new_pos.x++;

    char next_tile = map[new_pos.y][new_pos.x];

    if (next_tile == '.' || next_tile == '+' || next_tile == '#' || next_tile == 'T' || next_tile == 'P' || next_tile == '@' || next_tile == '&' || next_tile == 'W' || next_tile == 'F' || next_tile == 'G' || next_tile == 'S' || next_tile == 'M') {
        if (next_tile == '+') {
            for (int i = 0; i < level->num_rooms; i++) {
                Room room = level->rooms[i];
                for (int j = 0; j < room.num_doors; j++) {
                    if (new_pos.y == room.doors[j].position.y && new_pos.x == room.doors[j].position.x) {
                        reveal_room(room, map, drawMap);
                        active_monsters(&room);
                    }
                }
            }
        } else if (next_tile == '#') {
            stop_music(&game->music);
            dis_active_monsters(level);

            // curs_set(0);
            reveal_corridor(new_pos, map, drawMap);
        } else if (next_tile == 'T') {
            game->player.hp--;
        } else if (next_tile == 'P') {
            next_level(game); 
            return;
        }
        else if(next_tile == '.'){

            for (int i = 0; i < level->num_rooms; i++) {
                Room room = level->rooms[i];
                if (new_pos.y >= room.position.y && new_pos.y < room.position.y + room.height && new_pos.x >= room.position.x && new_pos.x < room.position.x + room.width)
                {
                    play_room_music(game, &room);
                    break;
                }
            }
        }

        else if (next_tile == '@')
        {

            int n;
            for (int i = 0; i < level->num_rooms; i++) {
                Room* room = &level->rooms[i];
                if (new_pos.y == room->pass_door.position.y && new_pos.x == room->pass_door.position.x) {   
                    n = open_pass_door(room, game);
                    break;
                }
            }
            if (n == 0) return;
            
        }
        else if (next_tile == '&'){
            for (int i = 0; i < level->num_rooms; i++) {
                Room* room = &level->rooms[i];
                if (new_pos.y == room->pass_key.y && new_pos.x == room->pass_key.x) {   
                    ThreadArgs* args = malloc(sizeof(ThreadArgs)); 
                    args->game = game;
                    args->room = room; 

                    pthread_t password_timer;
                    pthread_create(&password_timer, NULL, creat_password, args);
                    break;
                }
            }
            refresh();
            
        }
        else if (next_tile == 'W')
        {
                for (int i = 0; i < level->num_rooms; i++) {
                    for (int j = 0; j < level->rooms[i].num_weapons ; j++)
                    {
                        if (new_pos.y == level->rooms[i].weapons[j].position.y && new_pos.x == level->rooms[i].weapons[j].position.x)
                        {
                            reveal_weapons(&level->rooms[i].weapons[j] , level , game);
                            add_weapon_to_player(game, &level->rooms[i].weapons[j]);
                        }
                    }
                }
        }

        else if (next_tile == 'F')
        {
                for (int i = 0; i < level->num_rooms; i++) {
                    for (int j = 0; j < level->rooms[i].num_foods ; j++)
                    {
                        if (new_pos.y == level->rooms[i].foods[j].position.y && new_pos.x == level->rooms[i].foods[j].position.x)
                        {
                            reveal_foods(&level->rooms[i].foods[j], &level->rooms[i] , level , game);
                        }
                    }
                }
        }

        else if (next_tile == 'S')
        {
                for (int i = 0; i < level->num_rooms; i++) {
                    for (int j = 0; j < level->rooms[i].num_spells; j++)
                    {
                        if (new_pos.y == level->rooms[i].spells[j].position.y && new_pos.x == level->rooms[i].spells[j].position.x)
                        {
                            reveal_spells(&level->rooms[i].spells[j] , level , game);
                        }
                    }
                }
        }

        else if (next_tile == 'G')
        {
                for (int i = 0; i < level->num_rooms; i++) {
                    for (int j = 0; j < level->rooms[i].num_golds; j++)
                    {
                        if (new_pos.y == level->rooms[i].golds[j].position.y && new_pos.x == level->rooms[i].golds[j].position.x)
                        {
                            reveal_golds(&level->rooms[i].golds[j] , level , game);
                        }
                    }
                }
        }

        else if(next_tile == 'M'){
            Monster *monster = find_monster_at(game, new_pos);
            if (monster) {
                handle_battle(game, monster);
                return;
            }
        }

        
        

        game->player.position = new_pos;
    }
}

Monster* find_monster_at(Game *game, Position pos) {
    Level *level = &game->levels[game->current_level];
    for (int i = 0; i < level->num_rooms; i++) {
        Room *room = &level->rooms[i];
        for (int j = 0; j < room->num_monsters; j++) {
            if (room->monsters[j].position.x == pos.x && 
                room->monsters[j].position.y == pos.y) {
                return &room->monsters[j];
            }
        }
    }
    return NULL;
}

void handle_battle(Game *game, Monster *monster) {
    Level *level = &game->levels[game->current_level];
    WINDOW *battle_win = newwin(10, 40, LINES/2-5, COLS/2-20);
    keypad(battle_win, TRUE);
    
    while (1) {
        wclear(battle_win);
        box(battle_win, 0, 0);
        mvwprintw(battle_win, 1, 15, "⚔️ BATTLE ⚔️");
        mvwprintw(battle_win, 3, 2, "Player HP: %d", game->player.hp);
        mvwprintw(battle_win, 4, 2, "Monster HP: %d", monster->health);
        mvwprintw(battle_win, 5, 2, "Selected Weapon: %s (Power: %d)", 
            weapon_type_to_str(game->player.selected_weapon.type),
            game->player.selected_weapon.attack_power
        );
        
        int ch = wgetch(battle_win);
        if (ch == ' ') {
            // Player attack
            if (check_weapon_ammo(&game->player)) {
                monster->health -= game->player.selected_weapon.attack_power;
                reduce_weapon_ammo(&game->player);
                
                // Monster counter attack
                game->player.hp -= monster->damage;
                
                if (monster->health <= 0) {
                    remove_monster(level, monster);
                    wclear(battle_win);
                    delwin(battle_win);
                    return;
                }
                
                if (game->player.hp <= 0) {
                    game_over(game, 0);
                    return;
                }
            }
            else{
                game->game_message = "this weapon is ended.choose another one.";
            }
        } else if (ch == 'q') {
            delwin(battle_win);
            return;
        }
    }
}

const char* weapon_type_to_str(WeaponType type) {
    switch(type) {
        case WEAPON_Mace: return "Mace";
        case WEAPON_Sword: return "Sword";
        case WEAPON_Dagger: return "Dagger";
        case WEAPON_Magic_Wand: return "Magic Wand";
        case WEAPON_Normal_Arrow: return "Normal Arrow";
        default: return "Unknown";
    }
}

int check_weapon_ammo(Player *player) {
    switch(player->selected_weapon.type) {
        case WEAPON_Mace: return 1; // Unlimited
        case WEAPON_Sword: return (player->num_sword > 0);
        case WEAPON_Dagger: return (player->num_dagger > 0);
        case WEAPON_Magic_Wand: return (player->num_magic_wand > 0);
        case WEAPON_Normal_Arrow: return (player->num_normal_arrow > 0);
        default: return 0;
    }
}

void reduce_weapon_ammo(Player *player) {
    switch(player->selected_weapon.type) {
        case WEAPON_Sword: 
            if(player->num_sword > 0) {
                player->num_sword--;
            }
            if(player->num_sword == 0){
                for (int i = 0; i < player->num_weapons; i++)
                {
                    if (player->weapons[i].type == WEAPON_Sword)
                    {
                        for (int j = i; j < player->num_weapons - 1; j++)
                        {
                            player->weapons[j] = player->weapons[j+1];
                        }
                        player->num_weapons--;
                        break;
                    }
                    
                }
                
            }
            break;
        case WEAPON_Dagger: 
            if(player->num_dagger > 0) {
                player->num_dagger--;
            }
            if(player->num_dagger == 0){
                for (int i = 0; i < player->num_weapons; i++)
                {
                    if (player->weapons[i].type == WEAPON_Dagger)
                    {
                        for (int j = i; j < player->num_weapons - 1; j++)
                        {
                            player->weapons[j] = player->weapons[j+1];
                        }
                        player->num_weapons--;
                        break;
                    }
                    
                }
                
            }
            break;
        case WEAPON_Magic_Wand: 
            if(player->num_magic_wand > 0) {
                player->num_magic_wand--;
            }
            if(player->num_magic_wand == 0){
                for (int i = 0; i < player->num_weapons; i++)
                {
                    if (player->weapons[i].type == WEAPON_Magic_Wand)
                    {
                        for (int j = i; j < player->num_weapons - 1; j++)
                        {
                            player->weapons[j] = player->weapons[j+1];
                        }
                        player->num_weapons--;
                        break;
                    }
                    
                }
                
            }
            break;
        case WEAPON_Normal_Arrow: 
            if(player->num_normal_arrow > 0) {
                player->num_normal_arrow--;
            }
            if(player->num_normal_arrow == 0){
                for (int i = 0; i < player->num_weapons; i++)
                {
                    if (player->weapons[i].type == WEAPON_Normal_Arrow)
                    {
                        for (int j = i; j < player->num_weapons - 1; j++)
                        {
                            player->weapons[j] = player->weapons[j+1];
                        }
                        player->num_weapons--;
                        break;
                    }
                    
                }
                
            }
            break;
        default: break;
    }

    if (!check_weapon_exists(player, player->selected_weapon.type)) {
        if (player->num_weapons > 0) {
            player->selected_weapon = player->weapons[0];
            weapon_menu_visible = 0; // Select the first weapon in the list
        } 
        // else {
        //     // No weapons left, set selected weapon to a default or invalid state
        //     player->selected_weapon.type = WEAPON_NONE; // Assuming WEAPON_NONE is defined
        // }
    }
}

int check_weapon_exists(Player *player, WeaponType type) {
    for (int i = 0; i < player->num_weapons; i++) {
        if (player->weapons[i].type == type) {
            return 1; // Weapon exists
        }
    }
    return 0; // Weapon does not exist
}

void remove_monster(Level *level, Monster *monster) {
    for (int i = 0; i < level->num_rooms; i++) {
        Room *room = &level->rooms[i];
        for (int j = 0; j < room->num_monsters; j++) {
            if (&room->monsters[j] == monster) {
                // Restore original tile
                level->map[monster->position.y][monster->position.x] = monster->last_tile;
                level->drawMap[monster->position.y][monster->position.x] = monster->last_tile;
                
                // Remove from array
                for (int k = j; k < room->num_monsters-1; k++) {
                    room->monsters[k] = room->monsters[k+1];
                }
                room->num_monsters--;
                return;
            }
        }
    }
}

void game_over(Game *game, int won) {
    clear();
    if (won) {
        mvprintw(LINES/2, COLS/2-10, "🎉 VICTORY! Score: %d", game->player.gold_collected);
    } else {
        mvprintw(LINES/2, COLS/2-10, "💀 GAME OVER! Score: %d", game->player.gold_collected);
    }
    mvprintw(LINES/2+2, COLS/2-15, "Press any key to return to menu");
    sleep(2);
    refresh();
    getch();
    
    // Disable continue functionality
    game->current_level = -1;
    // save_game(game, current_user);
}

void move_monster(Game *game, Monster *monster) {
    Level *level = &game->levels[game->current_level];
    Position new_pos = monster->position;
    Position player_pos = game->player.position;

    // Determine the direction the monster should move toward the player
    if (player_pos.x > monster->position.x) new_pos.x++;
    else if (player_pos.x < monster->position.x) new_pos.x--;

    if (player_pos.y > monster->position.y) new_pos.y++;
    else if (player_pos.y < monster->position.y) new_pos.y--;

    char next_tile = level->map[new_pos.y][new_pos.x];

    // If the next tile is a wall, door, or something the monster can't pass, don't move
    if (next_tile == '-' || next_tile == '|' || next_tile == '+' || next_tile == '@' || next_tile == 'M' ||monster->max_follow_steps == 0) {
        return;
    }

    if (player_pos.y == new_pos.y && player_pos.x == new_pos.x)
    {
        // collision();
        return;
    }

    // Before moving, restore the last tile the monster was on

    
    level->map[monster->position.y][monster->position.x] = monster->last_tile;
    if (monster->last_tile == 'T')
    {
        level->drawMap[monster->position.y][monster->position.x] = '.';
    }
    else{
        level->drawMap[monster->position.y][monster->position.x] = monster->last_tile;
    }

    // Update the monster's last tile with the current tile
    
    monster->last_tile = next_tile;
    monster->position = new_pos;
    monster->max_follow_steps--;

    // If the next tile is not food/gold, we replace it with the monster symbol ('M')
    // if (next_tile != 'F' && next_tile != 'G') {
    //     level->map[new_pos.y][new_pos.x] = 'M';
    //     level->drawMap[new_pos.y][new_pos.x] = 'M';
    // }
    level->map[new_pos.y][new_pos.x] = 'M';
    level->drawMap[new_pos.y][new_pos.x] = 'M';
}

// void collison(){

// }

void move_monsters(Game* game){
    Level* level = &game->levels[game->current_level];

    for (int i = 0; i < level->num_rooms; i++)
    {
        Room* room = &level->rooms[i];
        for (int j = 0; j < room->num_monsters; j++)
        {
            Monster *monster = &room->monsters[j];
            if (monster->is_active == 1)
            {
                move_monster(game ,monster);
            }
            
        }
        
    }
    
}

void* creat_password(void* arg){

    ThreadArgs* args = (ThreadArgs*)arg;

    Game* game = args->game;
    Room* room = args->room;

    Level *level = &game->levels[game->current_level];

    float total_time = 30.0;
    int pass = rand()%10000;
    room->pass_door.password = pass;
    // return pass;
    while (total_time > 0)
    {
        usleep(100000);
        total_time-=0.1;
        mvprintw(level->height + 4, COLS/2 - 7, "secret password: %d", pass);
        refresh();

    }
    refresh();
    pthread_exit(NULL);
    
}

int open_pass_door(Room* room , Game* game){
    char password[5];
    if (room->pass_door.is_open == 1)
    {
        return 1;
    }
    
    // int pass = creat_password();
    int unlocked = create_window(LINES/2 - 5 , COLS / 2 -20, 10 , 40 , 4 , password , room->pass_door.password);
    refresh();
    if (unlocked == 1)
    {
        room->pass_door.is_open = 1;
        refresh();
        return 1;
    }
    refresh();
    return 0;  
}



void toggle_weapon_menu(Game *game) {
    weapon_menu_visible = !weapon_menu_visible;
    draw_game(game);
}

void toggle_food_menu(Game *game) {
    food_menu_visible = !food_menu_visible;
    draw_game(game); 
}

void toggle_spell_menu(Game *game) {
    spell_menu_visible = !spell_menu_visible;
    draw_game(game);
}

void draw_food_menu(Game *game) {
    if (!food_menu_visible) return;

    int start_x = COLS - 36; 
    int start_y = 3;
    int width = 30;
    int height = 10;

    WINDOW *food_win = newwin(height, width, start_y, start_x);
    box(food_win, 0, 0);
    mvwprintw(food_win, 1, 10, "🍽 Food 🍽");

    
    for (int i = 0; i < 5; i++) {
        char *food_icon;
        int food_count;

        switch (i) {
            case 0: {
                food_icon = "🍎 Apple : ";
                food_count = game->player.num_apple;
                break;
            }
            case 1: {
                food_icon = "🥚 Egg : ";
                food_count = game->player.num_egg;
                break;
            }
            case 2: {
                food_icon = "🍞 Bread : ";
                food_count = game->player.num_bread;
                break;
            }
            case 3: {
                food_icon = "🥕 Carrot : ";
                food_count = game->player.num_carrot;
                break;
            }
            case 4: {
                food_icon = "🐟 Fish : ";
                food_count = game->player.num_fish;
                break;
            }        }
        mvwprintw(food_win, i + 3, 2, "%s %d", food_icon, food_count);
    }

    wrefresh(food_win);
}

void draw_spell_menu(Game *game) {
    if (!spell_menu_visible) return;

    int start_x = COLS - 36;
    int start_y = 20;
    int width = 30;
    int height = 10;

    WINDOW *spell_win = newwin(height, width, start_y, start_x);
    box(spell_win, 0, 0);
    mvwprintw(spell_win, 1, 10, "🧙‍♂️ Spells 🔮");


    for (int i = 0; i < 3; i++) {
        char *spell_icon;
        int spell_count;

        switch (i) {
            case 0: {
                spell_icon = "❤️ Health : ";
                spell_count = game->player.num_health_spell;
                break;
            }
            case 1: {
                spell_icon = "⚡ Damage : ";
                spell_count = game->player.num_damage_spell;
                break;
            }
            case 2: {
                spell_icon = "🏃‍♂️ Speed : ";
                spell_count = game->player.num_speed_spell;
                break;
            }
        }
        mvwprintw(spell_win, i + 3, 2, "%s %d", spell_icon, spell_count);
    }

    wrefresh(spell_win);
}

void draw_weapon_menu(Game *game) {
    // if (!weapon_menu_visible) return;

    int start_x = COLS - 36; 
    int start_y = 35; 
    int width = 30;
    int height = 10;
    int choice = weapon_menu_visible;

    while (1) {
        WINDOW *weapon_win = newwin(height, width, start_y, start_x);
        box(weapon_win, 0, 0);
        mvwprintw(weapon_win, 1, 10, "🏹 Weapons 🏹");
        wrefresh(weapon_win);

        // نمایش لیست سلاح‌ها
        for (int i = 0; i < game->player.num_weapons; i++) {
            char *weapon_icon;
            int weapon_count;
            switch (game->player.weapons[i].type)
            {
            case WEAPON_Mace:
                weapon_icon = "⚒ Mace";
                weapon_count = game->player.num_mace;
                break;
            case WEAPON_Sword:
                weapon_icon = "⚔️ Sword";
                weapon_count = game->player.num_sword;
                break;
            case WEAPON_Dagger:
                weapon_icon = "🗡️ Dagger";
                weapon_count = game->player.num_dagger;
                break;
            case WEAPON_Normal_Arrow:
                weapon_icon = "➳ Normal Arrow";
                weapon_count = game->player.num_normal_arrow;
                break;
            case WEAPON_Magic_Wand:
                weapon_icon = "🪄 Magic Wand";
                weapon_count = game->player.num_magic_wand;
                break;
            }
            int weapon_power = game->player.weapons[i].attack_power;

            if (choice == i) {
                wattron(weapon_win, A_REVERSE); 
            } else {
                wattroff(weapon_win, A_REVERSE);
            }

            mvwprintw(weapon_win, i + 3, 2, "%s %d %d", weapon_icon, weapon_count, weapon_power);
            wrefresh(weapon_win);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? game->player.num_weapons - 1 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == game->player.num_weapons - 1) ? 0 : choice + 1;
        else if (ch == 10) { // Enter key
            weapon_menu_visible = choice;
            game->player.selected_weapon = game->player.weapons[choice];
            game->game_message = "You selected a weapon!";
            break; 
        }

        delwin(weapon_win);
    }


}





void draw_doors(Door* door , int start_x , int start_y) {
    // for (int i = 0; i < num_doors; i++) {
        // Door *door = doors;
        // int start_x = (COLS - level->width) / 2;
        // int start_y;

        if (door->is_open) {
            attron(COLOR_PAIR(3));
            mvaddch(door->position.y + start_y, door->position.x + start_x , '@');
            attroff(COLOR_PAIR(3));
        } else {
            attron(COLOR_PAIR(2)); 
            mvaddch(door->position.y + start_y, door->position.x + start_x, '@');
            attroff(COLOR_PAIR(2));
        }
    // }
}

void draw_weapon(Weapon *weapon , int start_x , int start_y) {
    char *symbol;
    switch (weapon->type) {
        case WEAPON_Sword: symbol = "⚔"; break;
        case WEAPON_Mace: symbol = "⚒"; break;
        case WEAPON_Normal_Arrow: symbol = "➳"; break;
        case WEAPON_Magic_Wand: symbol = "🪄"; break;
        case WEAPON_Dagger: symbol = "🗡"; break;
        default: symbol = "?";
    }
    mvprintw(weapon->position.y + start_y, weapon->position.x + start_x, "%s", symbol);
}

void draw_food(Food *food , int start_x , int start_y) {
    char *food_icon;
    switch (food->type) {
        case FOOD_Apple:  food_icon = ":"; break;
        case FOOD_Egg:   food_icon = "'"; break;
        case FOOD_Bread:  food_icon = "🥜"; break;
        case FOOD_Carot: food_icon = "🥕"; break;
        case FOOD_Fish:   food_icon = "🍦"; break;
        default:          food_icon = "?"; break;
    }
    mvprintw(food->position.y + start_y, food->position.x + start_x, "%s", food_icon);
}

void draw_gold(Gold *gold , int start_x , int start_y) {
    mvprintw(gold->position.y + start_y, gold->position.x + start_x, "*");
}

void draw_spell(Spell *spell , int start_x , int start_y) {

    mvprintw(spell->position.y + start_y, spell->position.x + start_x, "🔮");
}


void draw_monster(Monster *monster, int start_x, int start_y) {
    char *monster_icon;
    switch (monster->type) {
        case Deaⅿon:  monster_icon = "D"; break;
        case Dragon:   monster_icon = "F"; break;
        case Giant:  monster_icon = "G"; break;
        case Snake: monster_icon = "S"; break;
        case Undead:   monster_icon = "U"; break;
    }
    mvprintw(monster->position.y + start_y, monster->position.x + start_x, "%s", monster_icon);
}

void draw_border_around_map(int start_x, int start_y, int width, int height) {

    mvprintw(start_y - 1, start_x - 1, "🔲");
    mvprintw(start_y - 1, start_x + width, "🔲");
    mvprintw(start_y + height, start_x - 1, "🔲");
    mvprintw(start_y + height, start_x + width, "🔲");


    for (int x = start_x; x < start_x + width; x++) {
        mvprintw(start_y - 1, x, "─");
        mvprintw(start_y + height, x, "─");
    }


    for (int y = start_y; y < start_y + height; y++) {
        mvprintw(y, start_x - 1, "❘");
        mvprintw(y, start_x + width, "❘");
    }
}


void draw_map(Level *level) {
    clear();

    int start_x = (COLS - level->width) / 2;
    int start_y = (LINES - level->height) / 2;

    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            char tile = level->drawMap[y][x];

            if (tile == 'G' || tile == 'W' || tile == 'M' || tile == '@' || tile == 'F' || tile == 'S')
            {
                continue;
            }
            
            int found_room = 0; 

            
            for (int i = 0; i < level->num_rooms; i++) {
                Room room = level->rooms[i];

                if (x >= room.position.x && x < room.position.x + room.width &&
                    y >= room.position.y && y < room.position.y + room.height) {

                    found_room = 1; 

                    if (room.type == ROOM_GOLD)
                        attron(COLOR_PAIR(3)); 
                    else if (room.type == ROOM_SPELL)
                        attron(COLOR_PAIR(7)); 
                    else
                        attron(COLOR_PAIR(1)); 

                    break;
                }
            }

            
            mvaddch(y + start_y, x + start_x, tile);

            
            if (found_room)
                attroff(COLOR_PAIR(1)); 
        }
    }

    draw_border_around_map(start_x, start_y, level->width, level->height);


    for (int i = 0; i < level->num_rooms; i++) {
        if (level->rooms[i].num_pass_door > 0)
        {
            if (level->drawMap[level->rooms[i].pass_door.position.y][level->rooms[i].pass_door.position.x] == '@')
            {
                draw_doors(&level->rooms[i].pass_door , start_x , start_y);
            }
            
        }
    }

    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_weapons ; j++)
        {
            if (level->drawMap[level->rooms[i].weapons[j].position.y][level->rooms[i].weapons[j].position.x] == 'W')
            {
                draw_weapon(&level->rooms[i].weapons[j] , start_x , start_y);
            }
        }
    }

    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_foods; j++) {
            if (level->drawMap[level->rooms[i].foods[j].position.y][level->rooms[i].foods[j].position.x] == 'F')
            {
                draw_food(&level->rooms[i].foods[j] , start_x , start_y);
            }
            
        }
    }

    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_golds; j++) {

        if (level->drawMap[level->rooms[i].golds[j].position.y][level->rooms[i].golds[j].position.x] == 'G')
            {
                draw_gold(&level->rooms[i].golds[j] , start_x , start_y);
            }
        }
    }

    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_spells; j++) {
            if (level->drawMap[level->rooms[i].spells[j].position.y][level->rooms[i].spells[j].position.x] == 'S')
            {
                draw_spell(&level->rooms[i].spells[j] , start_x , start_y);
            }
        }
    }

    for (int i = 0; i < level->num_rooms; i++) {
        for (int j = 0; j < level->rooms[i].num_monsters; j++) {
            if (level->drawMap[level->rooms[i].monsters[j].position.y][level->rooms[i].monsters[j].position.x] == 'M')
            {
                draw_monster(&level->rooms[i].monsters[j] , start_x , start_y);
            }
        }
    }


    refresh();
}

void draw_player_info(Game *game) {
    int start_x = 4;
    int start_y = 25;
    int width = 30;
    int height = 10;

    WINDOW *info_win = newwin(height, width, start_y, start_x);
    box(info_win, 0, 0);
    mvwprintw(info_win, 1, 8, "📜 Stats 📜");

    mvwprintw(info_win, 3, 2, "🏆 Level: %d/%d", game->current_level + 1, game->total_levels);
    mvwprintw(info_win, 5, 2, "⏳ Time: %02d:%02d", game->timer.minute, game->timer.second);
    mvwprintw(info_win, 7, 2, "❤️ HP: %d", game->player.hp);
    mvwprintw(info_win, 9, 2, "💰 Gold: %d", game->player.gold_collected);

    wrefresh(info_win);
}

void draw_message_box(Game* game , int start_x, int start_y, int width, int height){

    WINDOW* message_box = newwin(height , width, start_y , start_x);
    box(message_box , 0 , 0);
    wrefresh(message_box);
    mvwprintw(message_box , 1 , 6 , "💬 Input Message 💬");
    mvwprintw(message_box , height/2 -1  , 5 , "%s" ,game->game_message);
    wrefresh(message_box);


}

void draw_game(Game *game) {
    clear();
    Level *level = &game->levels[game->current_level];

    int start_x = (COLS - level->width) / 2;
    int start_y = (LINES - level->height) / 2;
    draw_map(level);
    refresh();
    switch (game->player.color)
    {
    case 1:
        attron(COLOR_PAIR(5));
        mvaddch(game->player.position.y + start_y, game->player.position.x + start_x, '@');
        attroff(COLOR_PAIR(5));
        break;
    case 2:
        attron(COLOR_PAIR(8));
        mvaddch(game->player.position.y + start_y, game->player.position.x + start_x, '@');
        attroff(COLOR_PAIR(8));
        break;
    case 3:
        attron(COLOR_PAIR(2));
        mvaddch(game->player.position.y + start_y, game->player.position.x + start_x, '@');
        attroff(COLOR_PAIR(2));
        break;
    default:
        mvaddch(game->player.position.y + start_y, game->player.position.x + start_x, '@');
        break;
    }
    // attron(COLOR_PAIR(1));
    // mvaddch(game->player.position.y + start_y, game->player.position.x + start_x, '@');

    draw_message_box(game , 4 , start_y , 30 , 10);
    draw_player_info(game);
    // mvprintw(level->height + 4, start_x, "Player HP: %d", game->player.hp);
    // mvprintw(level->height + 5, start_x, "Time Remaining: %02d:%02d", game->timer.minute, game->timer.second);

    // draw_weapon_menu(game);
    draw_food_menu(game); 
    draw_spell_menu(game);

    refresh();
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

int create_window(int start_y, int start_x, int height, int width, int max_length, char *input, int pass) {
    WINDOW *textbox = newwin(height, width, start_y, start_x);
    box(textbox, 0, 0); 
    wrefresh(textbox);
    refresh();

    int n = 0;
    const char *prompt = "Enter the password: "; 
    int prompt_len = strlen(prompt);            

    while (1) {
        memset(input, 0, max_length + 1);

        
        mvwaddstr(textbox, height / 2, 1, prompt);
        wmove(textbox, height / 2, 1 + prompt_len); 
        wrefresh(textbox);

        int ch;
        int index = 0;

        while ((ch = wgetch(textbox)) != '\n') {
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (index > 0) {
                    input[--index] = '\0';
                    mvwaddch(textbox, height / 2, 1 + prompt_len + index, ' ');
                    wmove(textbox, height / 2, 1 + prompt_len + index);        
                    wrefresh(textbox);
                }
            } else if (ch == 'e') {
                delwin(textbox);
                return 0;
            } else if (index < max_length && isprint(ch)) {
                input[index++] = ch;
                mvwaddch(textbox, height / 2, 1 + prompt_len + index - 1, ch); 
                wrefresh(textbox);
            }
        }

        int input_pass = atoi(input);
        if (input_pass == pass) {
            delwin(textbox);
            return 1;
        } else if (input_pass != pass && n == 0) {
            wattron(textbox, COLOR_PAIR(3));
            mvwprintw(textbox, 5, 1, "Warning! you entered a wrong password");
            wattroff(textbox, COLOR_PAIR(2));
            wrefresh(textbox);
            refresh();
            n++;
            sleep(1);
            werase(textbox);
            box(textbox, 0, 0);
            wrefresh(textbox);
        } else if (input_pass != pass && n == 1) {
            wattron(textbox, COLOR_PAIR(4));
            mvwprintw(textbox, 5, 1, "Warning2! you entered a wrong password");
            wattroff(textbox, COLOR_PAIR(2));
            wrefresh(textbox);
            refresh();
            n++;
            sleep(1);
            werase(textbox);
            box(textbox, 0, 0);
            wrefresh(textbox);
        } else if (input_pass != pass && n == 2) {
            wattron(textbox, COLOR_PAIR(5));
            mvwprintw(textbox, 5, 1, "The door was locked!");
            wattroff(textbox, COLOR_PAIR(2));
            wrefresh(textbox);
            refresh();
            sleep(1);
            delwin(textbox);
            return 0;
        }
    }
}

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
