#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int timeoutcounter = 0;
int switchcounter = 1;
int life_stage = 0;
int animation_enable = 1;

int mat_counter = 0;
int sleep_counter = 0;
int clean_counter = 0;
int fun_counter = 0;

int mat_stage = 1;
int sleep_stage = 1;
int clean_stage = 1;
int fun_stage = 1;

char textstring[] = "text, more text, and even more text!";

/* By Malin Hall & Johann Schubert */
void user_isr( void ){
    
    if (IFS(0) & 0x80){
        switchcounter++;                    // switch 1-tamagotchi, 2-mat, 3-sleep, 4-clean, 5-fun
        IFSCLR(0) = 0x80;
    }
        
        if (IFS(0) & 0x100){
            timeoutcounter ++;
            IFSCLR(0)=0x100;
            
            if (life_stage == 1 || life_stage == 2 || life_stage == 3 || life_stage == 4){
            if (timeoutcounter %350 == 0){
                life_stage++;
            }
            
            if (timeoutcounter %10 == 0){
                mat_counter++;
                sleep_counter++;
                clean_counter++;
                fun_counter++;
            }
            }
        }
}

/* By Malin Hall & Johann Schubert */
void initiate( void ){
    
    T2CON = 0x70;                    //1:256 prescale value (bits 111) 0111 0000 0x70
    PR2 = ((80000000/256) / 10);    // 80/256/10 (1/10 sekund)  (31 250 clocks/sek < 16 bits (32,767)
    TMR2 = 0x0;                      //nolla timern
    T2CONSET = 0x8000;             // Start the timer
    
    //timer2 interrupt
    IECSET(0) = 0x100;          //interrupt enable control
    IPCSET(2) = 0x7;          //4. Interrupt priority control, sätt högsta prioritet på bitar 0-4, t2ip ligger på 3 lsb.
    //0000 0000 0000 0000,0000 0000 000(0 01)00
    IPCSET(1) = 0X7000000;
    IECSET(0) = 0x80;
    //IFSCLR(0) = 0x100; // Clear the timer interrupt status flag
    enable_interrupt();
}

void start (void){
    int btns = getbtns();
        display_string(2, "Start your game?");
        display_update_string();
        if (btns == 0x4){
            delay(100);
            life_stage = 1;
            timeoutcounter = 1;
        }
}
void work( void ){
        ///Minska Levels///
    if (mat_counter == 10){
        mat_stage = mat(1);
        mat_counter = 0;
    }
    if (sleep_counter == 40){
        sleep_stage = sleep(1);
        sleep_counter = 0;
    }
    if (clean_counter == 30){
        clean_stage = clean(1);
        clean_counter = 0;
    }
    if (fun_counter ==  20 ){
        fun_stage = fun(1);
        fun_counter = 0;
    }
    if (mat_stage == 4 || sleep_stage == 4 || clean_stage == 4 || fun_stage == 4){
        life_stage = 4;
    }
    
    /////////////MENY///////////////
    int btns = getbtns();
    
    if (switchcounter == 1){
        display_gamestatus(2);
                    /////////////CALL TAMAGOTCHI///////////////
        display_tama_life(life_stage);
                                ///////SYMBOLER//////
        if (mat_stage == 3){
            show_food();
        }
        if (sleep_stage == 3){
            show_sleep();
        }
        if (clean_stage == 3){
            show_poop();
        }
        if (fun_stage == 3){
            show_heart();
        }
                                ////////BOXES///////
        mat_box(mat_stage);
        sleep_box(sleep_stage);
        clean_box(clean_stage);
        fun_box(fun_stage);
        display_update();
        
    }
    if (switchcounter == 2){
        
        display_string(2, "      SLEEP");
        display_update_string();
        if (btns == 0x4){
            sleep_stage = sleep(-1);
            delay(100);
            switchcounter = 1;
        }
    }
    if (switchcounter == 3){
        display_string(2, "      LOVE");
        display_update_string();
        if (btns == 0x4){
            fun_stage = fun(-1);
            delay(100);
            switchcounter = 1;
        }
    }
    if (switchcounter == 4){
        
        display_string(2, "      CLEAN");
        display_update_string();
        if (btns == 0x4){
            clean_stage = clean(-1);
            delay(100);
            switchcounter = 1;
        }
    }
    if (switchcounter == 5){
        display_string(2, "      FOOD");
        display_update_string();
        if (btns == 0x4){
            mat_stage = mat(-1);
            delay(100);
            switchcounter = 1;
        }
    }
        if (switchcounter == 6){
            switchcounter = 1;
        }

}
void death(void){
    
        int x_offset = -40;
        int y_offset = -2;

    while(x_offset < 128){
        if (timeoutcounter %1 == 0)
        {
            x_offset += 1;
            y_offset += 1;
            outro(y_offset, x_offset);
            display_update();
        }
        delay(20);
        reset();
    }
    display_string(2, "   G");
    display_update_string();
    delay(200);
    display_string(2, "   GA");
    display_update_string();
    delay(200);
    display_string(2, "   GAM");
    display_update_string();
    delay(200);
    display_string(2, "   GAME");
    display_update_string();
    delay(200);
    display_string(2, "   GAME ");
    display_update_string();
    delay(200);
    display_string(2, "   GAME O");
    display_update_string();
    delay(200);
    display_string(2, "   GAME OV");
    display_update_string();
    delay(200);
    display_string(2, "   GAME OVE");
    display_update_string();
    delay(200);
    display_string(2, "   GAME OVER");
    display_update_string();
    delay(200);
    display_string(2, "   GAME OVER");
    display_update_string();
    delay(200);
}
