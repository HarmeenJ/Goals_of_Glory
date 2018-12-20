/* *** TEAM GOALS OF GLORY ***
 *
 * File: main.cpp
 *
 * Created on: October 28, 2018
 * Author: Harmeen Joshi & Satya Naidu
 */

#include <Led_Matrix.hpp>
#include "FreeRTOS.h"
#include "LPC17xx.h"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "io.hpp"
#include "gpio.hpp"
#include "utilities.h"
#include "printf_lib.h"
#include "lpc_rit.h"
#include "nrf_stream.hpp"

#define Player1_Tx_Address 250
#define Player2_Tx_Address 210

ledMat mat;
int Y_HIT = 0;

bool colorSelected = false;
bool gameStart = false;
SemaphoreHandle_t semBeginGame;

uint8_t color_choice_x = 9;
uint8_t color_choice_y[6] = {12, 19, 26, 33, 40, 47};

TaskHandle_t goalkeeper_task;

void selectPlayer(void *p)
{
    GPIO left_switch(P1_9), right_switch(P1_15),
            p1_sel_switch(P1_10), p2_sel_switch(P1_14);
    int8_t i = 0;
    int8_t current_selection = 0;
    bool moveArrowUp = false;
    bool goalkeeperPos1 = false;
    bool goalkeeperStill = true;
    bool p1AnimationStop = false;
    bool p2AnimationStop = false;

    left_switch.setAsInput();
    right_switch.setAsInput();
    p1_sel_switch.setAsInput();
    p2_sel_switch.setAsInput();

    while (false == colorSelected) {
        if (right_switch.read()) {
            current_selection++;
            if (current_selection > 5)
                current_selection = 0;
        }
        if (left_switch.read()) {
            current_selection--;
            if (current_selection < 0)
                current_selection = 5;
        }

        vTaskDelay(150);
        mat.clearArrow(cyan1);

        mat.drawStartScreen(48,0);

        // Goalkeeper Dance Animation
        if (goalkeeperPos1) {
            if (!goalkeeperStill) {
                mat.clearGoalKeeperDemoPose1(2,3);
                mat.clearGoalKeeperDemoPose1(2,55);
                mat.clearGoalKeeperDemoPose2(2,3);
                mat.clearGoalKeeperDemoPose2(2,55);
                if (!p1AnimationStop) {
                    mat.drawGoalKeeperDemoPose1(2,3,mat.p1Color);
                } else {
                    mat.drawGoalKeeperDemoPose2(2,3,mat.p1Color);
                }
                if (!p2AnimationStop) {
                    mat.drawGoalKeeperDemoPose2(2,55,mat.p2Color);
                } else {
                    mat.drawGoalKeeperDemoPose2(2,55,mat.p2Color);
                }
                goalkeeperPos1 = false;
                goalkeeperStill = true;
            } else {
                goalkeeperStill = false;
            }
        } else {
            if (!goalkeeperStill) {
                mat.clearGoalKeeperDemoPose1(2,3);
                mat.clearGoalKeeperDemoPose1(2,55);
                mat.clearGoalKeeperDemoPose2(2,3);
                mat.clearGoalKeeperDemoPose2(2,55);
                if (!p1AnimationStop) {
                    mat.drawGoalKeeperDemoPose2(2,3,mat.p1Color);
                } else {
                    mat.drawGoalKeeperDemoPose2(2,3,mat.p1Color);
                }
                if (!p2AnimationStop) {
                    mat.drawGoalKeeperDemoPose1(2,55,mat.p2Color);
                } else {
                    mat.drawGoalKeeperDemoPose2(2,55,mat.p2Color);
                }
                goalkeeperPos1 = true;
                goalkeeperStill = true;
            } else {
                goalkeeperStill = false;
            }
        }

        // Arrow Animation
        if (i >= 3) {
            moveArrowUp = true;
        }
        if (i <= 0) {
            moveArrowUp = false;
        }
        if (moveArrowUp) {
            i--;
        } else {
            i++;
        }

        mat.drawArrow((color_choice_x + i),
                color_choice_y[current_selection], cyan1);

        if (p1_sel_switch.read() && (!p1AnimationStop)) {
            p1AnimationStop = true;
            switch (current_selection) {
                case 0:
                    mat.p1Color = blue1;
                    break;
                case 1:
                    mat.p1Color = green1;
                    break;
                case 2:
                    mat.p1Color = red1;
                    break;
                case 3:
                    mat.p1Color = yellow1;
                    break;
                case 4:
                    mat.p1Color = skyblue1;
                    break;
                case 5:
                    mat.p1Color = purple1;
                    break;
            }
        }
        if (p2_sel_switch.read() && (!p2AnimationStop)) {
            switch (current_selection) {
                case 0:
                    mat.p2Color = blue1;
                    break;
                case 1:
                    mat.p2Color = green1;
                    break;
                case 2:
                    mat.p2Color = red1;
                    break;
                case 3:
                    mat.p2Color = yellow1;
                    break;
                case 4:
                    mat.p2Color = skyblue1;
                    break;
                case 5:
                    mat.p2Color = purple1;
                    break;
            }
            p2AnimationStop = true;
        }

        if (p1AnimationStop && p2AnimationStop) {
            colorSelected = true;
            mat.clearArrow(cyan1);

            mat.clearGoalKeeperDemoPose1(2,3);
            mat.clearGoalKeeperDemoPose1(2,55);
            mat.clearGoalKeeperDemoPose2(2,3);
            mat.clearGoalKeeperDemoPose2(2,55);
            mat.drawGoalKeeperDemoPose2(2,3,mat.p1Color);
            mat.drawGoalKeeperDemoPose2(2,55,mat.p2Color);

            for (i = 12; i < 51; (i += 2)) {
                vTaskDelay(200);
                mat.drawPixel(14, i, cyan1);
            }
            vTaskDelay(500);

            // Let the GAME BEGIN !!!
            mat.clearDisplay(cyan);
            gameStart = true;
            xSemaphoreGive(semBeginGame);
            xSemaphoreGive(semBeginGame);
        }
    }

    while(1);
}

/*
 * Update LED Matrix every 2ms
 */
void UpdateDisplay(void *p)
{
    while(1) {
        if (gameStart) {
            mat.drawPlayer2Scoreboard(62,59);
            mat.drawPlayer1Scoreboard(62,1);
        }

        mat.updateDisplay();
        vTaskDelay(2);
    }
}

/*
 * Animate the ball movement based on the Y_HIT value
 */
void animateBall(uint8_t color)
{
    int x, y;
    int swing_var = 0;
    int y_var = 32 - Y_HIT;
    int direction_change_interval = X_START / abs(y_var);
    int swing_change_interval = 0;
    y = 32;

    swing_change_interval = direction_change_interval / 2;

    while (mat.ballBlocked);

    // Lesser swing for 2/3rd of the ball path
    for(x = X_START; x >= 17; x--) {

        if (!mat.ballBlocked) {
            if ((x % direction_change_interval) == 0) {
                if (y_var < 0) {
                    y--;
                } else if (y_var > 0) {
                    y++;
                }
            }
            if ((x % swing_change_interval) == 0) {
                swing_change_interval++;
                if (y_var < 0) {
                    swing_var += 1;
                } else if (y_var >= 0) {
                    swing_var -= 1;
                }
            }

            mat.drawBall(x, y + swing_var, color);

            mat.drawGoalPost(GOALPOST, mat.goalpost_color);
            vTaskDelay(DELAY);
            mat.clearBall(x + 1, y + swing_var , color);
        }
    }

    // Higher swing for 1/3rd of the remaining ball path
    for(x = 16; x >= 2; x--) {
        if (!mat.ballBlocked) {
            if ((x % direction_change_interval) == 0) {
                if (y_var < 0) {
                    y--;
                } else if (y_var >= 0) {
                    y++;
                }
            }
            if ((x % swing_change_interval) == 0) {
                if (y_var < 0) {
                    swing_var -= 1;
                } else if (y_var > 0) {
                    swing_var += 1;
                }
            }

            mat.drawBall(x, y + swing_var, color);

            mat.drawGoalPost(GOALPOST, mat.goalpost_color);
            vTaskDelay(DELAY);
            mat.clearBall(x, y + swing_var, color);
        }
    }
}
/*
 * Task to control the goalkeeper based on the accelerometer values
 */
void ControlGoalkeeper(void *p)
{
    int x = 4, y = 30;
    mesh_packet_t GKPkt;
    int timeout_ms = 500;

    // Wait till Both the players select their colors
    xSemaphoreTake(semBeginGame, portMAX_DELAY);

    while (1) {
        if (wireless_get_rx_pkt(&GKPkt, timeout_ms)
                && (Player2_Tx_Address == GKPkt.nwk.src)) {

            wireless_flush_rx();
            // Change the goalkeeper position based on the
            // data recieved through NORDIC
            if(!(memcmp(GKPkt.data, "right", 5))) {
                if(y != GOALKEEPER_Y_MAX) {
                    y += 3;
                }
            } else if(!(memcmp(GKPkt.data, "left", 4))) {
                if(y != GOALKEEPER_Y_MIN) {
                    y -= 3;
                }
            } else if(!(memcmp(GKPkt.data, "up", 2))) {
                if(x != GOALKEEPER_X_MIN) {
                    x -= 3;
                }
            } else if(!(memcmp(GKPkt.data, "down", 4))) {
                if(x != GOALKEEPER_X_MAX) {
                    x += 3;
                }
            }
        }
        // Change the goalkeeper color based on which player is defending
        if (mat.p1Shooting)
            mat.drawGoalkeeper(x, y, mat.p2Color);
        else
            mat.drawGoalkeeper(x, y, mat.p1Color);

        mat.drawGoalPost(GOALPOST, mat.goalpost_color);
        vTaskDelay(40);
    }
}

void ControlGame(void *p)
{
    int8_t shoot_num = -1;
    mesh_packet_t ballPkt;
    int timeout_ms = 500;
    bool endGame = false;

    // Wait till Both the players select their colors
    xSemaphoreTake(semBeginGame, portMAX_DELAY);

    mat.drawGoalPost(GOALPOST, mat.goalpost_color);
    mat.resetShoot();

    //draw player1 SHOOT
    mat.drawLetter(33, 29, 'P', cyan1);
    mat.drawDigit(33, 35, 1,cyan1);
    mat.drawLetter(39, 19, 's', red1);
    mat.drawLetter(39, 24, 'h', red1);
    mat.drawLetter(39, 29, 'Q', red1);
    mat.drawLetter(39, 35, 'Q', red1);
    mat.drawLetter(39, 40, 't', red1);
    mat.drawLetter(39, 46, '!', red1);

    // Wait for the player to press any of the 4 SJOne switch buttons
    // after they get ready
    while (!(SW.getSwitchValues()));

    mat.clearDisplay(cyan);

    mat.drawPixel(X_START, 32, green1);
    vTaskDelay(DELAY * 100);
    mat.clearPixel(X_START, 32, green1);

    while(1) {
        if (endGame) {
            vTaskSuspend(goalkeeper_task);
            gameStart = false;
            mat.clearDisplay(cyan);

            mat.drawEndScreen(48, 0);
            while(1);
        }

        // Draw Ball before Player shoots
        if (mat.p1Shooting)
            mat.drawBall(X_START, 32, mat.p1Color);
        else
            mat.drawBall(X_START, 32, mat.p2Color);

        if (wireless_get_rx_pkt(&ballPkt, timeout_ms)
                && (Player1_Tx_Address == ballPkt.nwk.src)) {

            wireless_flush_rx();

            // Change the Y_HIT location of the ball based on the gyro values
            if(!(memcmp(ballPkt.data, "right", 5)))
                Y_HIT = 25;
            else if(!(memcmp(ballPkt.data, "left", 4)))
                Y_HIT = 45;
            else if(!(memcmp(ballPkt.data, "straight", 7)))
                Y_HIT = 64-32;
            else if(!(memcmp(ballPkt.data, "ext left", 8)))
                Y_HIT = 55;
            else if(!(memcmp(ballPkt.data, "ext right", 9)))
                Y_HIT = 15;
            else
                continue;

            if (!mat.ballBlocked) {
                mat.drawPixel(X_START, 32, green1);
                mat.clearPixel(X_START, 32, green1);
                if (mat.p1Shooting)
                    mat.drawBall(X_START, 32, mat.p1Color);
                else
                    mat.drawBall(X_START, 32, mat.p2Color);
            }

            // Change the ball color based on which player is shooting
            if (mat.p1Shooting)
                animateBall(mat.p1Color);
            else
                animateBall(mat.p2Color);

            if (!mat.ballBlocked) {
                mat.goalpost_color = green1;
                shoot_num++;

                if (mat.p1Shooting)
                    mat.updateScore(1, shoot_num, goal);
                else
                    mat.updateScore(2, shoot_num, goal);

                if ((4 == shoot_num) && mat.p1Shooting) {
                    shoot_num = -1;
                    mat.p1Shooting = false;

                    mat.clearDisplay(cyan);
                    //draw player2 SHOOT
                    mat.drawLetter(33, 29, 'P', cyan1);
                    mat.drawDigit(33, 35, 2,cyan1);
                    mat.drawLetter(39, 19, 's', red1);
                    mat.drawLetter(39, 24, 'h', red1);
                    mat.drawLetter(39, 29, 'Q', red1);
                    mat.drawLetter(39, 35, 'Q', red1);
                    mat.drawLetter(39, 40, 't', red1);
                    mat.drawLetter(39, 46, '!', red1);

                    // Wait for the player to press any of the 4 SJOne switch buttons
                    // after they get ready
                    while (!(SW.getSwitchValues()));

                } else if ((4 == shoot_num) && !mat.p1Shooting) {
                    // End the game after P2 is done shooting
                    endGame = true;
                }
            }
            if (mat.ballBlocked) {
                shoot_num++;
                mat.drawPixel(X_START, 32, green1);
                mat.goalpost_color = red1;

                if (mat.p1Shooting)
                    mat.updateScore(1, shoot_num, noGoal);
                else
                    mat.updateScore(2, shoot_num, noGoal);

                if ((4 == shoot_num) && mat.p1Shooting) {
                    shoot_num = -1;
                    mat.p1Shooting = false;

                     mat.clearDisplay(cyan);
                    //draw player2 SHOOT
                    mat.drawLetter(33, 29, 'P', cyan1);
                    mat.drawDigit(33, 35, 2,cyan1);
                    mat.drawLetter(39, 19, 's', red1);
                    mat.drawLetter(39, 24, 'h', red1);
                    mat.drawLetter(39, 29, 'Q', red1);
                    mat.drawLetter(39, 35, 'Q', red1);
                    mat.drawLetter(39, 40, 't', red1);
                    mat.drawLetter(39, 46, '!', red1);

                    // Wait for the player to press any of the 4 SJOne switch buttons
                    // after they get ready
                    while (!(SW.getSwitchValues()));// & (1 << 0)));

                } else if ((4 == shoot_num) && !mat.p1Shooting) {
                    // End the game after P2 is done shooting
                    endGame = true;
                }
            }
            mat.clearDisplay(cyan);
            mat.drawGoalPost(GOALPOST, mat.goalpost_color);
            vTaskDelay(DELAY * 100);
            mat.resetShoot();
        }
        vTaskDelay(DELAY * 50);
    }
}

int main(void)
{
    // Initialize the global binary Semaphore
    semBeginGame =xSemaphoreCreateBinary();

    mat.displayInit();
    wireless_init();
    wireless_service();
    scheduler_add_task(new wirelessTask(PRIORITY_MEDIUM));

    // Task to check sensor values and show Ball movement animation.
    xTaskCreate(ControlGame, "Control Game", 1024, NULL, PRIORITY_MEDIUM, NULL);
    // Task to show Ball movement animation.
    xTaskCreate(ControlGoalkeeper, "Control Goalkeeper", 1024, NULL, PRIORITY_MEDIUM, &goalkeeper_task);
    // Dedicated task to update the LED Matrix
    xTaskCreate(UpdateDisplay, "Update LED Matrix", 1024, NULL, PRIORITY_HIGH, NULL);
    // Task for the start screen and player color selection
    xTaskCreate(selectPlayer, "Select Players", 1024, NULL, PRIORITY_LOW, NULL);

    scheduler_start();

    // Return failure if this line executes
    return -1;
}
