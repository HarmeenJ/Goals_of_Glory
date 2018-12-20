/* *** TEAM GOALS OF GLORY ***
 *
 * File: Led_Matrix.hpp
 *
 * Created on: October 28, 2018
 * Author: Harmeen Joshi & Satya Naidu
 */

#ifndef LED_MATRIX_HPP_
#define LED_MATRIX_HPP_

#define MATRIX_HEIGHT 64
#define MATRIX_WIDTH 64
#define MATRIX_NROWS 32

#define GOALPOST 9
#define GOALKEEPER_X_MIN 3  // Depth Min
#define GOALKEEPER_X_MAX 27 //// Depth Max
#define GOALKEEPER_Y_MIN (GOALPOST) // Width Max
#define GOALKEEPER_Y_MAX (MATRIX_WIDTH - 1) - GOALKEEPER_Y_MIN  // Width Max
#define GOALKEEPER_COLOR yellow1
#define BALL_COLOR skyblue1
#define GOALPOST_COLOR cyan1

#define DELAY 10
#define X_START 58

#include "stdint.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define swap(a, b) { int16_t temp = a; a = b; b = temp; }

typedef enum
{
    black = 0,

    cyan1 = 7,  // Red, Green, Blue
    cyan2 = 56,
    cyan = 63,

    blue1 = 1,
    blue2 = 8,
    blue = 9,

    green1 = 2,
    green2 = 16,
    green = 18,

    red1 = 4,
    red2 = 32,
    red = 36,

    yellow1 = 6,    // Red, Green
    yellow2 = 48,
    yellow = 54,

    skyblue1 = 3,   // Green, Blue
    skyblue2 = 24,
    skyblue = 27,

    purple1 = 5,    // Blue, Red
    purple2 = 40,
    purple = 45

}color_codes;

typedef enum {
    pending,
    goal,
    noGoal
}score_status;

class ledMat
{
public:

    ledMat();
    void displayInit();

    void resetGame();
    void resetShoot();

    void drawStartScreen(uint8_t x, uint8_t y);
    void drawEndScreen(uint8_t x, uint8_t y);
    void drawGameName(uint8_t x, uint8_t y);
    void drawWinDesign(uint8_t x, uint8_t y, uint8_t num, uint8_t color);
    void clearUpperRightCorner();
    void clearUpperLeftCorner();

    void updateDisplay();
    void clearDisplay(uint8_t color);

    void drawPixel(int8_t x, int8_t y, uint8_t color);
    void clearPixel(int8_t x, int8_t y, uint8_t color);
    void drawLine(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t color);
    void clearLine(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t color);
    void drawDigit(uint8_t x, uint8_t y, uint8_t value, uint8_t color);
    void drawLetter(uint8_t x, uint8_t y, char alpha, uint8_t color);

    void drawGoalPost(uint8_t dist_left, uint8_t color);
    void drawBall(uint8_t x, uint8_t y, uint8_t color);
    void clearBall(uint8_t x, uint8_t y, uint8_t color);
    void drawGoalkeeper(uint8_t x, uint8_t y, uint8_t color);
    void clearGoalkeeper(uint8_t x, uint8_t y, uint8_t color);
    void drawGoalkeeperWithBall(uint8_t x, uint8_t y, uint8_t color);
    void clearGoalkeeperWithBall(uint8_t x, uint8_t y, uint8_t color);

    void drawPlayer1Scoreboard(uint8_t x, uint8_t y);
    void drawPlayer2Scoreboard(uint8_t x, uint8_t y);
    void updateScore(uint8_t player, uint8_t shoot_num, score_status status);

    void drawArrow(uint8_t x,uint8_t y,uint8_t color );
    void clearArrow(uint8_t color );
    void drawGoalKeeperDemoPose1(uint8_t x, uint8_t y,uint8_t color);
    void clearGoalKeeperDemoPose1(uint8_t x, uint8_t y);
    void drawGoalKeeperDemoPose2(uint8_t x, uint8_t y,uint8_t color);
    void clearGoalKeeperDemoPose2(uint8_t x, uint8_t y);

    bool ballBlocked;
    uint8_t goalkeeper_x = 3, goalkeeper_y = 30;
    unsigned char goalpost_color = GOALPOST_COLOR;
    bool p1Shooting;
    uint8_t p1Color = red1;
    uint8_t p2Color = blue1;
private:
    score_status p1_score[5];
    score_status p2_score[5];

    void calculateMatrixPosition(int8_t *x, uint8_t *color);
    void drawScoreGoal(uint8_t x, uint8_t y,uint8_t color);
    void drawScoreNoGoal(uint8_t x, uint8_t y);
    void drawScorePending(uint8_t x, uint8_t y);
};

#endif /* LED_MATRIX_HPP_ */
