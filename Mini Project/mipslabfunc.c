/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)



/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
 
 */
void display_debug( volatile int * const addr )
{
    display_string( 1, "Addr" );
    display_string( 2, "Data" );
    num32asc( &textbuffer[1][6], (int) addr );
    num32asc( &textbuffer[2][6], *addr );
    display_update();
}


//sends the information to the screen
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

// set a pixel to a value
void pixel_set(char x, char y, char set){
    display_array [x][y] = set;
}

// turns a pixel on
void pixel_on(char x, char y){
    display_array[x][y] = 1;
}

// turns a pixel off
void pixel_off(char x, char y){
    display_array[x][y] = 0;
}

// turns all pixels off
void reset(void){
    int x, y;
    for(y = 0; y < 32; y++){
        for(x = 0; x < 128; x++){
            display_array[x][y] = 0;
        }
    }
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 32; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}

void display_update(void) {
    int i, j, k;
    int value;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);
        
        spi_send_recv(0x0);
        spi_send_recv(0x10);
        
        DISPLAY_CHANGE_TO_DATA_MODE;
        
        for(j = 0; j < 128; j++) {
            value = display_array[j][i*8];
            for(k = 1; k < 8; k++){
                value += (display_array[j][i*8+k]<<k);
            }
            spi_send_recv(value);
        }
    }
}
void display_update_string(void) {
    int i, j, k;
    int c;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);
        
        spi_send_recv(0x0);
        spi_send_recv(0x10);
        
        DISPLAY_CHANGE_TO_DATA_MODE;
        
        for(j = 0; j < 16; j++) {
            c = textbuffer[i][j];
            if(c & 0x80)
                continue;
            
            for(k = 0; k < 8; k++)
                spi_send_recv(font[c*8 + k]);
        }
    }
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}


/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}

void reset_string(void){
    display_string(0, "");
    display_string(1, "");
    display_string(2, "");
    display_string(3, "");
    display_update_string();
}



                /* TAMAGOTCHI FUNKTIONER BY MALIN HALL & JOHANNA SCHUBERT */


int mat (int add){
    int M;
    if (mat_stage == 1 && add == -1){
        M = 1;
    }
    else{
        M = mat_stage + add;
    }
    return M;
}
int sleep (int add){
    int S;
    if (sleep_stage == 1 && add == -1){
    S = 1;
    }
    else{
    S = sleep_stage + add;
    }
    return S;
}
int clean (int add){
    int C;
    if (clean_stage == 1 && add == -1){
        C = 1;
    }
    else{
        C = clean_stage + add;
    }
    return C;
}
int fun (int add){
    int F;
    if (fun_stage == 1 && add == -1){
        F = 1;
    }
    else{
        F = fun_stage + add;
    }
    return F;
}


void display_gamestatus(int game_status){
    //START
    if (game_status == 1){
        //display_tama_life(0);
        //display_levels(0,0,0,0);
        int x,y;
        for(x = 0; x < 128; x++){
            for(y = 0; y < 32; y++){
                pixel_set(x, y, start_bitmap[y][x]);
            }
        }
    }
    //GAME
    if (game_status == 2){
        int x,y;
        for(x = 0; x < 128; x++){
            for(y = 0; y < 32; y++){
                pixel_set(x, y, start_bitmap[y][x]);
    }
        }
    }
    //DEATH
    if (game_status == 0){
        int x,y;
        for(x = 0; x < 128; x++){
            for(y = 0; y < 32; y++){
                pixel_set(x, y, start_bitmap[y][x]);
            }
        }
    }
}


        ///////////Tamagotchi-FUNKTIONER///////////

void show_tam(char tam[25][33]){
    int x,y;
    for (x = 0 ;x < 33 ; x++){
        for(y=0 ; y < 25; y++){
            if(tam[y][x]){
                pixel_set(x + 34, y + 6 ,tam[y][x]);
            }
        }
    }
}

void outro(int x_offset , int y_offset){
        int x;
        int y;
        for (x = 0 ;x < 13 ; x++){
            for(y = 0 ; y < 9; y++){
                    pixel_set(x + x_offset, y + y_offset,heart_icon[y][x]);
            }
        }
    }

void display_tama_life(int life_stage){
    
    if (life_stage == 1){
        show_tam(egg_icon);
    }
    if (life_stage == 2){
        show_tam(mellan_icon);
    }
    if (life_stage == 3){
        show_tam(stor_icon);
    }
}

                ///////////SYMBOLER///////////

void show_poop(void){
    int x,y;
    for (x = 0 ;x < 25 ; x++){
        for(y = 0 ; y < 14 ; y++){
            if(poop_icon[y][x]){
                pixel_set(x + 67, y + 17, poop_icon[y][x]);
            }
        }
    }
}
void show_sleep(void){
    int x,y;
    for (x = 0 ;x < 20 ; x++){
        for(y=0 ; y < 6; y++){
            if(sleep_icon[y][x]){
                pixel_set(x + 70, y + 8,sleep_icon[y][x]);
            }
        }
    }
}
void show_heart(void){
    int x,y;
    for (x = 0 ;x < 13 ; x++){
        for(y=0 ; y < 9; y++){
            if(heart_icon[y][x]){
                pixel_set(x + 92, y + 7,heart_icon[y][x]);
            }
        }
    }
}

void show_food(void){
    int x,y;
    for (x = 0 ;x < 11 ; x++){
        for(y=0 ; y < 10; y++){
            if(food_icon[y][x]){
                pixel_set(x + 94, y + 21,food_icon[y][x]);
            }
        }
    }
}

                    ////// SYMBOLER _ BOXES /////////
void show_lev(int x_offset , int y_offset){
    int x,y;
    for (x = 0 ;x < 4 ; x++){
        for(y=0 ; y < 8; y++){
            if(level_bit[y][x]){
                pixel_set(x + x_offset, y + y_offset,level_bit[y][x]);
            }
        }
    }
}
void mat_box (int mat_stage){
    if(mat_stage == 1){
        show_lev(27,22); /*foodlevelbit l??ngst ned*/
        show_lev(27,15); /*foodlevelbit i mitten*/
        show_lev(27,8); /*foodlevelbit l??ngst upp*/
    }
    if(mat_stage == 2){
        show_lev(27,22); /*foodlevelbit l??ngst ned*/
        show_lev(27,15); /*foodlevelbit i mitten*/
    }
    if(mat_stage == 3){
        show_lev(27,22); /*foodlevelbit l??ngst ned*/
    }
}
void sleep_box (int sleep_stage){
    if(sleep_stage == 1){
        show_lev(3,22); /*sleeplevelbit l??ngst ned*/
        show_lev(3,15); /*sleeplevelbit i mitten*/
        show_lev(3,8); /*sleeplevelbit l??ngst upp*/
    }
    if(sleep_stage == 2){
        show_lev(3,22); /*sleeplevelbit l??ngst ned*/
        show_lev(3,15); /*sleeplevelbit i mitten*/
    }
    if(sleep_stage == 3){
        show_lev(3,22); /*sleeplevelbit l??ngst ned*/
    }
}
void clean_box (int clean_stage){
    if(clean_stage == 1){
        show_lev(19,22); /*pooplevelbit l??ngst ned*/
        show_lev(19,15); /*pooplevelbit i mitten*/
        show_lev(19,8); /*pooplevelbit l??ngst upp*/
    }
    if(clean_stage == 2){
        show_lev(19,22); /*pooplevelbit l??ngst ned*/
        show_lev(19,15); /*pooplevelbit i mitten*/
    }
    if(clean_stage == 3){
        show_lev(19,22); /*pooplevelbit l??ngst ned*/
    }
}
void fun_box (int fun_stage){
    if(fun_stage == 1){
        show_lev(11,22); /*heartlevelbit l??ngst ned*/
        show_lev(11,15); /*heartlevelbit i mitten*/
        show_lev(11,8); /*heartlevelbit l??ngst upp*/
    }
    if(fun_stage == 2){
        show_lev(11,22); /*heartlevelbit l??ngst ned*/
        show_lev(11,15); /*heartlevelbit i mitten*/
    }
    if(fun_stage == 3){
        show_lev(11,22); /*heartlevelbit l??ngst ned*/
    }
}
