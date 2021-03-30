/* WORK */
void enable_interrupt(void);
void work(void);
void start(void);
void death (void);

/* Main */
void quicksleep(int cyc);
void time2string( char *, int );
void delay(int);

/* Extra */
int getbtns(void);
int getsw(void);

/* FUNC - Display */
char * itoaconv( int num );
void reset(void);
void pixel_on(char x, char y);
void pixel_off(char x, char y);
void pixel_set(char x, char y, char set);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
void display_update_string(void);

char display_array[128][32];
void display_image(int x, const uint8_t *data);
uint8_t spi_send_recv(uint8_t data);
extern char textbuffer[4][16];
extern const uint8_t const icon [128];
extern const uint8_t const font [128*8];

/* FUNC - Tamagotchi */
int mat (int add);
int sleep (int add);
int clean (int add);
int fun (int add);


/*VIEW - Tamagotchi*/
void display_gamestatus(int game_status);
void show_poop(void);
void show_sleep(void);
void show_heart(void);
void show_food(void);
void show_lev(int x_offset , int y_offset);
void sleep_box (int sleep_stage);
void clean_box (int clean_stage);
void fun_box (int fun_stage);
void mat_box (int mat_stage);
//void heart_animation(char heart_icon[25][33],int x_offset , int y_offset);
void show_tam_move(char tam[25][33],int x_offset , int y_offset);
void show_poop_move(char tam[14][25],int x_offset , int y_offset);
void display_tama_life(int life_stage);


/* Global Variables */
extern int timeoutcounter;
extern int life_stage;
extern int animation_enable;

extern int mat_stage;
extern int sleep_stage;
extern int clean_stage;
extern int fun_stage;

extern int M;
extern int S;
extern int C;
extern int F;
extern int game_status;


/* CHAR */
char start_bitmap[32][128];
char egg_icon[25][33];
char mellan_icon[25][33];
char stor_icon[25][33];
char sleep_icon[6][20];
char heart_icon[9][13];
char poop_icon[14][25];
char food_icon[10][11];
char level_bit[7][4];




/* Declare display_debug - a function to help debugging.
 
 After calling display_debug,
 the two middle lines of the display show
 an address and its current contents.
 
 There's one parameter: the address to read and display.
 
 Note: When you use this function, you should comment out any
 repeated calls to display_image; display_image overwrites
 about half of the digits shown by display_debug.
 */
void display_debug( volatile int * const addr );
