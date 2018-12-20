/* *** TEAM GOALS OF GLORY ***
 *
 * File: Led_Matrix.cpp
 *
 * Created on: October 28, 2018
 * Author: Harmeen Joshi & Satya Naidu
 */
#include <Led_Matrix.hpp>
#include "FreeRTOS.h"
#include "LPC17xx.h"
#include <stdint.h>
#include "io.hpp"
#include "gpio.hpp"
#include "utilities.h"
#include "stdio.h"
#include "stdbool.h"
#include "printf_lib.h"
#include "string.h"
#include "stdlib.h"

GPIO R1(P1_20);
GPIO G1(P2_7);
GPIO B1(P1_22);
GPIO R2(P1_23);
GPIO G2(P2_6);
GPIO B2(P1_28);

GPIO A(P1_29);
GPIO B(P2_5);
GPIO C(P2_0);
GPIO D(P2_4);
GPIO E(P1_19);

GPIO clk(P2_1);
GPIO lat(P2_3);
GPIO oe(P2_2);

GPIO dir(P0_30);

uint8_t matrixbuff[MATRIX_NROWS][MATRIX_HEIGHT];

/*
 ** Initialize pins as output/input
 */
void ledMat::displayInit()
{
    R1.setAsOutput();
    G1.setAsOutput();
    B1.setAsOutput();
    R2.setAsOutput();
    G2.setAsOutput();
    B2.setAsOutput();
    A.setAsOutput();
    B.setAsOutput();
    C.setAsOutput();
    D.setAsOutput();
    E.setAsOutput();
    clk.setAsOutput();
    lat.setAsOutput();
    oe.setAsOutput();
    dir.setAsOutput();

    R1.setLow();
    G1.setLow();
    B1.setLow();
    R2.setLow();
    G2.setLow();
    B2.setLow();
    A.setLow();
    B.setLow();
    C.setLow();
    D.setLow();
    E.setLow();
    clk.setLow();
    lat.setLow();
    oe.setLow();
    dir.setHigh();

    memset(matrixbuff, 0, MATRIX_HEIGHT * MATRIX_NROWS);
}

void ledMat::drawPixel(int8_t x, int8_t y, uint8_t color)
{
    if ((x < 0) || (x > 63))
        return;
    if ((y < 0) || (y > 63))
        return;

    calculateMatrixPosition(&x, &color);

    matrixbuff[x][y] |= color;
}

void ledMat::clearPixel(int8_t x, int8_t y, uint8_t color)
{
    if ((x < 0) || (x > 63))
        return;
    if ((y < 0) || (y > 63))
        return;

    calculateMatrixPosition(&x, &color);

    matrixbuff[x][y] &= ~(color);
}

void ledMat::clearDisplay(uint8_t color)
{
    for(uint8_t row = 0; row < 32; row++)
    {
        for(uint8_t col = 0; col < 64; col++)
        {
            clearPixel(row, col, color);
        }
    }
}

void ledMat::updateDisplay()
{
    for(uint8_t row = 0; row < 32; row++) {
        for(uint8_t col = 0; col < 64; col++) {
            if(matrixbuff[row][col] & 0x1) {
                B1.setHigh();
            } else {
                B1.setLow();
            }
            if(matrixbuff[row][col] & 0x2) {
                G1.setHigh();
            } else {
                G1.setLow();
            }
            if(matrixbuff[row][col] & 0x4) {
                R1.setHigh();
            } else {
                R1.setLow();
            }
            if(matrixbuff[row][col] & 0x8) {
                B2.setHigh();
            } else {
                B2.setLow();
            }
            if(matrixbuff[row][col] & 0x10) {
                G2.setHigh();
            } else {
                G2.setLow();
            }
            if(matrixbuff[row][col] & 0x20) {
                R2.setHigh();
            } else {
                R2.setLow();
            }

            clk.setHigh();
            clk.setLow();
        }

        oe.setHigh();
        lat.setHigh();

        A.setLow();
        B.setLow();
        C.setLow();
        D.setLow();
        E.setLow();

        if(row & 0x1) {
            A.setHigh();
        }

        if(row & 0x2) {
            B.setHigh();
        }

        if(row & 0x4) {
            C.setHigh();
        }

        if(row & 0x8) {
            D.setHigh();
        }

        if(row & 0x10) {
            E.setHigh();
        }

        lat.setLow();
        oe.setLow();
    }
}

void ledMat::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
    int16_t slope = abs(y1 - y0) > abs(x1 - x0);
    int16_t dx, dy;
    int16_t err;
    int16_t ystep;

    if (slope) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    dx = x1 - x0;
    dy = abs(y1 - y0);
    err = dx / 2;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (slope) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }

        err -= dy;

        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ledMat::clearLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
    int16_t slope = abs(y1 - y0) > abs(x1 - x0);
    int16_t dx, dy;
    int16_t err;
    int16_t ystep;

    if (slope) {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    dx = x1 - x0;
    dy = abs(y1 - y0);
    err = dx / 2;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (slope) {
            clearPixel(y0, x0, color);
        } else {
            clearPixel(x0, y0, color);
        }

        err -= dy;

        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ledMat::drawDigit(uint8_t x0, uint8_t y0, uint8_t value, uint8_t color)
{
    if(value == 0)
    {
        drawPixel(x0, y0+1, color);
        drawPixel(x0, y0+2, color);
        drawPixel(x0, y0+3, color);
        drawPixel(x0, y0+4, color);
        drawPixel(x0, y0+5, color);
        drawPixel(x0, y0+6, color);
        drawPixel(x0+1, y0, color);
        drawPixel(x0+1, y0+7, color);
        drawPixel(x0+2, y0, color);
        drawPixel(x0+2, y0+7, color);
        drawPixel(x0+3, y0, color);
        drawPixel(x0+3, y0+7, color);
        drawPixel(x0+4, y0+1, color);
        drawPixel(x0+4, y0+2, color);
        drawPixel(x0+4, y0+3, color);
        drawPixel(x0+4, y0+4, color);
        drawPixel(x0+4, y0+5, color);
        drawPixel(x0+4, y0+6, color);
    }
    else if(value == 1)
    {
        drawLine(x0,y0,x0+4,y0,color);
        drawPixel(x0+1,y0-1,color);
        drawLine(x0+4,y0-1,x0+4,y0+1,color);
    }
    else if(value == 2)
    {
        drawPixel(x0+1,y0,color);
        drawPixel(x0,y0+1,color);drawPixel(x0,y0+2,color);
        drawPixel(x0+1,y0+3,color);
        drawPixel(x0+2,y0+2,color);
        drawPixel(x0+3,y0+1,color);
        drawPixel(x0+4,y0,color);
        drawPixel(x0+4,y0+1,color);
        drawPixel(x0+4,y0+2,color);
        drawPixel(x0+4,y0+3,color);

    }
    else if(value == 3)
    {
        drawPixel(x0,y0+1,color);drawPixel(x0,y0+2,color);
        drawPixel(x0+1,y0+3,color);
        drawPixel(x0+2,y0+2,color);
        drawPixel(x0+3,y0+3,color);
        drawPixel(x0+4,y0+1,color);drawPixel(x0+4,y0+2,color);
    }
    else if(value == 4)
    {
        drawPixel(x0, y0+3, color);
        drawPixel(x0+1, y0+3, color);drawPixel(x0+1, y0+2, color);
        drawPixel(x0+2, y0+3, color);drawPixel(x0+2, y0+1, color);
        drawPixel(x0+3, y0, color);drawPixel(x0+3, y0+1, color);drawPixel(x0+3, y0+2, color);drawPixel(x0+3, y0+3, color);drawPixel(x0+3, y0+4, color);
        drawPixel(x0+4, y0+3, color);
    }
    else if(value == 5)
    {
        drawPixel(x0, y0, color);drawPixel(x0, y0+1, color);drawPixel(x0, y0+2, color);drawPixel(x0, y0+3, color);
        drawPixel(x0+1, y0, color);
        drawPixel(x0+2, y0, color);drawPixel(x0+2, y0+1, color);drawPixel(x0+2, y0+2, color);
        drawPixel(x0+3, y0+3, color);
        drawPixel(x0+4, y0, color);drawPixel(x0+4, y0+1, color);drawPixel(x0+4, y0+2, color);
    }
    else if(value == 6)
    {
        drawPixel(x0, y0+1, color);
        drawPixel(x0, y0+2, color);
        drawPixel(x0, y0+3, color);
        drawPixel(x0, y0+4, color);
        drawPixel(x0, y0+5, color);
        drawPixel(x0, y0+6, color);
        drawPixel(x0+1, y0, color);
        drawPixel(x0+1, y0+4, color);
        drawPixel(x0+1, y0+7, color);
        drawPixel(x0+2, y0, color);
        drawPixel(x0+2, y0+4, color);
        drawPixel(x0+2, y0+7, color);
        drawPixel(x0+3, y0, color);
        drawPixel(x0+3, y0+4, color);
        drawPixel(x0+3, y0+7, color);
        drawPixel(x0+4, y0+1, color);
        drawPixel(x0+4, y0+2, color);
        drawPixel(x0+4, y0+3, color);
        drawPixel(x0+4, y0+6, color);
    }
    else if(value == 7)
    {
        drawPixel(x0, y0+7, color);
        drawPixel(x0+1, y0+7, color);
        drawPixel(x0+2, y0, color);
        drawPixel(x0+2, y0+1, color);
        drawPixel(x0+2, y0+2, color);
        drawPixel(x0+2, y0+3, color);
        drawPixel(x0+2, y0+7, color);
        drawPixel(x0+3, y0+4, color);
        drawPixel(x0+3, y0+7, color);
        drawPixel(x0+4, y0+5, color);
        drawPixel(x0+4, y0+6, color);
        drawPixel(x0+4, y0+7, color);
    }
    else if(value == 8)
    {
        drawPixel(x0, y0+1, color);
        drawPixel(x0, y0+2, color);
        drawPixel(x0, y0+3, color);
        drawPixel(x0, y0+5, color);
        drawPixel(x0, y0+6, color);
        drawPixel(x0+1, y0, color);
        drawPixel(x0+1, y0+4, color);
        drawPixel(x0+1, y0+7, color);
        drawPixel(x0+2, y0, color);
        drawPixel(x0+2, y0+4, color);
        drawPixel(x0+2, y0+7, color);
        drawPixel(x0+3, y0, color);
        drawPixel(x0+3, y0+4, color);
        drawPixel(x0+3, y0+7, color);
        drawPixel(x0+4, y0+1, color);
        drawPixel(x0+4, y0+2, color);
        drawPixel(x0+4, y0+3, color);
        drawPixel(x0+4, y0+5, color);
        drawPixel(x0+4, y0+6, color);
    }
    else if(value == 9)
    {
        drawPixel(x0, y0+1, color);
        drawPixel(x0, y0+4, color);
        drawPixel(x0, y0+5, color);
        drawPixel(x0, y0+6, color);
        drawPixel(x0+1, y0, color);
        drawPixel(x0+1, y0+3, color);
        drawPixel(x0+1, y0+7, color);
        drawPixel(x0+2, y0, color);
        drawPixel(x0+2, y0+3, color);
        drawPixel(x0+2, y0+7, color);
        drawPixel(x0+3, y0, color);
        drawPixel(x0+3, y0+3, color);
        drawPixel(x0+3, y0+7, color);
        drawPixel(x0+4, y0+1, color);
        drawPixel(x0+4, y0+2, color);
        drawPixel(x0+4, y0+3, color);
        drawPixel(x0+4, y0+4, color);
        drawPixel(x0+4, y0+5, color);
        drawPixel(x0+4, y0+6, color);
    }
}

void ledMat::drawLetter(uint8_t x, uint8_t y, char alpha, uint8_t color)
{
    if (alpha == 'a' || alpha == 'A') {
        drawPixel(x, y+1, color);drawPixel(x, y+4, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+1, color);drawPixel(x+1, y+2, color);drawPixel(x+1, y+3, color);drawPixel(x+1, y+4, color);drawPixel(x+1, y+5, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+1, color);drawPixel(x+2, y+4, color);drawPixel(x+2, y+5, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+1, color);drawPixel(x+3, y+2, color);drawPixel(x+3, y+3, color);drawPixel(x+3, y+4, color);drawPixel(x+3, y+5, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+1, color);drawPixel(x+4, y+2, color);drawPixel(x+4, y+3, color);drawPixel(x+4, y+4, color);drawPixel(x+4, y+5, color);
        drawPixel(x+5, y, color);drawPixel(x+5, y+1, color);drawPixel(x+5, y+4, color);drawPixel(x+5, y+5, color);
        drawPixel(x+6, y, color);drawPixel(x+6, y+1, color);drawPixel(x+6, y+4, color);drawPixel(x+6, y+5, color);
    }

    if (alpha == 'b' || alpha == 'B') {
        drawPixel(x, y+4, color); drawPixel(x+1, y+4, color); drawPixel(x+2, y+4, color); drawPixel(x+3, y+4, color);
        drawPixel(x, y+3, color); drawPixel(x+4, y+3, color);
        drawPixel(x, y+2, color); drawPixel(x+1, y+2, color); drawPixel(x+2, y+2, color); drawPixel(x+3, y+2, color);
        drawPixel(x, y+1, color); drawPixel(x+4, y+1, color);
        drawPixel(x, y, color); drawPixel(x+1, y, color); drawPixel(x+2, y, color); drawPixel(x+3, y, color);
    }

    if (alpha == 'c' || alpha == 'C') {
        drawPixel(x+1, y+4, color); drawPixel(x+2, y+4, color); drawPixel(x+3, y+4, color); //drawPixel(x+4, y+4, color);
        drawPixel(x, y+3, color);
        drawPixel(x, y+2, color);
        drawPixel(x, y+1, color);
        drawPixel(x+1, y, color); drawPixel(x+2, y, color); drawPixel(x+3, y, color); //drawPixel(x+4, y, color);
    }

    if (alpha == 'd' || alpha == 'D') {
        drawPixel(x, y+4, color); drawPixel(x+1, y+4, color); drawPixel(x+2, y+4, color); drawPixel(x+3, y+4, color);
        drawPixel(x, y+3, color); drawPixel(x+4, y+3, color);
        drawPixel(x, y+2, color); drawPixel(x+4, y+2, color);
        drawPixel(x, y+1, color); drawPixel(x+4, y+1, color);
        drawPixel(x, y, color); drawPixel(x+1, y, color); drawPixel(x+2, y, color); drawPixel(x+3, y, color);
    }

    if(alpha=='&') {
        drawPixel(x, y+1, color); drawPixel(x, y+2, color);
        drawPixel(x+1, y, color); drawPixel(x+1, y+3, color);
        drawPixel(x+2, y+1, color); drawPixel(x+2, y+2, color);
        drawPixel(x+3, y+1, color); drawPixel(x+3, y+2, color);drawPixel(x+3, y+4, color);
        drawPixel(x+4, y, color); drawPixel(x+4, y+3, color);
        drawPixel(x+5, y+1, color); drawPixel(x+5, y+2, color);drawPixel(x+5, y+4, color);
    }
    if (alpha == 'e' || alpha == 'E') {
        drawPixel(x, y, color); drawPixel(x, y+1, color); drawPixel(x, y+2, color); drawPixel(x, y+3, color); drawPixel(x, y+4, color);
        drawPixel(x+1, y, color);
        drawPixel(x+2, y, color); drawPixel(x+2, y+1, color); drawPixel(x+2, y+3, color); drawPixel(x+2, y+2, color);
        drawPixel(x+3, y, color);
        drawPixel(x+4, y, color); drawPixel(x+4, y+1, color); drawPixel(x+4, y+2, color); drawPixel(x+4, y+3, color); drawPixel(x+4, y+4, color);
    }

    if (alpha == 'f' || alpha == 'F') {
        drawPixel(x, y, color);   drawPixel(x, y+1, color);   drawPixel(x, y+2, color);   drawPixel(x, y+3, color);   drawPixel(x, y+4, color);
        drawPixel(x+1, y, color); drawPixel(x+1, y+1, color); drawPixel(x+1, y+2, color); drawPixel(x+1, y+3, color); drawPixel(x+1, y+4, color);
        drawPixel(x+2, y, color); drawPixel(x+2, y+1, color);
        drawPixel(x+3, y, color); drawPixel(x+3, y+1, color); drawPixel(x+3, y+2, color); drawPixel(x+3, y+3, color);
        drawPixel(x+4, y, color); drawPixel(x+4, y+1, color);drawPixel(x+4, y+2, color); drawPixel(x+4, y+3, color);
        drawPixel(x+5, y, color); drawPixel(x+5, y+1, color);
        drawPixel(x+6, y, color); drawPixel(x+6, y+1, color);

    }
    if (alpha ==  'G') {
        drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);drawPixel(x, y+4, color);drawPixel(x, y+5, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+1, color);drawPixel(x+1, y+2, color);drawPixel(x+1, y+3, color);drawPixel(x+1, y+4, color);drawPixel(x+1, y+5, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+1, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+1, color);drawPixel(x+3, y+3, color);drawPixel(x+3, y+4, color);drawPixel(x+3, y+5, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+1, color);drawPixel(x+4, y+4, color);drawPixel(x+4, y+5, color);
        drawPixel(x+5, y, color);drawPixel(x+5, y+1, color);drawPixel(x+5, y+2, color);drawPixel(x+5, y+3, color);drawPixel(x+5, y+4, color);drawPixel(x+5, y+5, color);
        drawPixel(x+6, y+1, color);drawPixel(x+6, y+2, color);drawPixel(x+6, y+3, color);drawPixel(x+6, y+4, color);drawPixel(x+6, y+5, color);
    }
    if (alpha == 'g') {
        drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+3, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+2, color);drawPixel(x+2, y+3, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+3, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+3, color);
    }

    if (alpha =='h' || alpha =='H') {
        drawPixel(x, y, color);drawPixel(x, y+3, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+3, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+1, color);drawPixel(x+2, y+2, color);drawPixel(x+2, y+3, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+3, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+3, color);
    }

    if (alpha == '-') {
        drawPixel(x, y, color);drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);
    }

    if (alpha == '!') {
        drawPixel(x, y, color);
        drawPixel(x+1, y, color);
        drawPixel(x+2, y, color);
        drawPixel(x+4, y, color);
    }

    if (alpha == 'i' || alpha == 'I') {
        drawPixel(x, y, color);drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);drawPixel(x, y+4, color);
        drawPixel(x+1, y+2, color);
        drawPixel(x+2, y+2, color);
        drawPixel(x+3, y+2, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+1, color);drawPixel(x+4, y+2, color);drawPixel(x+4, y+3, color);drawPixel(x+4, y+4, color);
    }

    if (alpha == 'l' || alpha == 'L') {
        drawPixel(x, y, color);drawPixel(x, y+1, color);//drawPixel(x, y+2, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+1, color);//drawPixel(x+1, y+2, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+1, color);//drawPixel(x+2, y+2, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+1, color);//drawPixel(x+3, y+2, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+1, color);//drawPixel(x+4, y+2, color);drawPixel(x+4, y+3, color);drawPixel(x+4, y+4, color);drawPixel(x+4, y+5, color);
        drawPixel(x+5, y, color);drawPixel(x+5, y+1, color);drawPixel(x+5, y+2, color);drawPixel(x+5, y+3, color);drawPixel(x+5, y+4, color);//drawPixel(x+5, y+5, color);
        drawPixel(x+6, y, color);drawPixel(x+6, y+1, color);drawPixel(x+6, y+2, color);drawPixel(x+6, y+3, color);drawPixel(x+6, y+4, color);//drawPixel(x+6, y+5, color);
    }

    if (alpha =='n' || alpha =='N') {
        drawPixel(x, y, color);drawPixel(x, y+4, color);
        drawPixel(x+1, y, color);  drawPixel(x+1, y+4, color);  drawPixel(x+1, y+1, color);
        drawPixel(x+2, y, color); drawPixel(x+2, y+4, color);   drawPixel(x+2, y+2, color);
        drawPixel(x+3, y, color); drawPixel(x+3, y+4, color);   drawPixel(x+3, y+3, color);
        drawPixel(x+4, y, color);  drawPixel(x+4, y+4, color);

    }

    if (alpha == 'O') {
        drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);drawPixel(x, y+4, color);drawPixel(x, y+5, color);
        drawPixel(x+1, y+1, color); drawPixel(x+1, y+5, color);drawPixel(x+1, y, color);drawPixel(x+1, y+2, color);drawPixel(x+1, y+3, color);drawPixel(x+1, y+4, color);drawPixel(x+1, y+6, color);
        drawPixel(x+2, y+0, color);drawPixel(x+2, y+1, color);drawPixel(x+2, y+5, color);drawPixel(x+2, y+6, color);
        drawPixel(x+3, y+0, color);drawPixel(x+3, y+1, color);drawPixel(x+3, y+5, color);drawPixel(x+3, y+6, color);
        drawPixel(x+4, y+0, color);drawPixel(x+4, y+1, color);drawPixel(x+4, y+5, color);drawPixel(x+4, y+6, color);
        drawPixel(x+5, y+1, color);drawPixel(x+5, y+5, color);drawPixel(x+5, y, color);drawPixel(x+5, y+2, color);drawPixel(x+5, y+3, color);drawPixel(x+5, y+4, color);drawPixel(x+5, y+6, color);
        drawPixel(x+6, y+1, color);drawPixel(x+6, y+2, color);drawPixel(x+6, y+3, color);drawPixel(x+6, y+4, color);drawPixel(x+6, y+5, color);
    }

    if (alpha == 'Q') {
        drawLine(x,y+1,x,y+3,color);
        drawPixel(x+1,y,color);drawPixel(x+1,y+4,color);
        drawPixel(x+2,y,color);drawPixel(x+2,y+4,color);
        drawPixel(x+3,y,color);drawPixel(x+3,y+4,color);
        drawLine(x+4,y+1,x+4,y+3,color);

    }

    if (alpha == 'o') {
        uint8_t p=x;
        uint8_t q=y;

        drawLine(p,q+1,p,q+2,color);
        drawPixel(p-1,q,color);
        drawPixel(p-1,q+3,color);
        drawPixel(p-2,q,color);
        drawPixel(p-2,q+3,color);
        drawLine(p-3,q+1,p-3,q+2,color);
    }

    if (alpha == 'p' || alpha == 'P') {
        drawPixel(x, y+1, color);
        drawPixel(x, y+2, color);
        drawPixel(x+1, y+3, color);
        drawPixel(x+2, y+2, color);
        drawPixel(x+2, y+1, color);
        drawLine(x,y,x+4,y,color);
    }

    if (alpha == 'r' || alpha == 'R') {
        drawPixel(x, y+1, color); drawPixel(x, y+2, color); drawPixel(x, y+3, color);drawPixel(x, y+4, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+1, color); drawPixel(x+1, y+2, color); drawPixel(x+1, y+3, color);drawPixel(x+1, y+4, color);drawPixel(x+1, y+5, color);                                                               drawPixel(x+4, y+3, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+1, color);drawPixel(x+2, y+4, color);drawPixel(x+2, y+5, color); drawPixel(x+4, y+3, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+1, color);drawPixel(x+3, y+2, color);drawPixel(x+3, y+3, color);drawPixel(x+3, y+4, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+1, color);drawPixel(x+4, y+2, color);drawPixel(x+4, y+3, color);
        drawPixel(x+5, y, color);drawPixel(x+5, y+1, color);drawPixel(x+5, y+3, color);drawPixel(x+5, y+4, color);
        drawPixel(x+6, y, color);drawPixel(x+6, y+1, color);drawPixel(x+6, y+5, color);drawPixel(x+6, y+4, color);
    }

    if ( alpha == 'S') {
        drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);drawPixel(x, y+4, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+1, color);drawPixel(x+1, y+2, color);drawPixel(x+1, y+3, color);drawPixel(x+1, y+4, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+1, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+2, color);drawPixel(x+3, y+3, color);drawPixel(x+3, y+1, color);
        drawPixel(x+4, y+1, color);drawPixel(x+4, y+2, color);drawPixel(x+4, y+3, color);drawPixel(x+4, y+4, color);
        drawPixel(x+5, y+3, color);drawPixel(x+5, y+4, color);
        drawPixel(x+6, y, color);drawPixel(x+6, y+1, color);drawPixel(x+6, y+2, color);drawPixel(x+6, y+3, color);drawPixel(x+6, y+4, color);
        drawPixel(x+7, y, color);drawPixel(x+7, y+2, color);drawPixel(x+7, y+3, color);drawPixel(x+7, y+1, color);
    }

    if(alpha == 's' ) {
        drawPixel(x, y+1, color);drawPixel(x, y+2, color);drawPixel(x, y+3, color);
        drawPixel(x+1, y, color);
        drawPixel(x+2, y+1, color);drawPixel(x+2, y+2, color);
        drawPixel(x+3, y+3, color);
        drawPixel(x+4, y+1, color);drawPixel(x+4, y+2, color);drawPixel(x+4, y, color);
    }

    if (alpha == 't' || alpha == 'T') {
        drawPixel(x, y, color); drawPixel(x, y+1, color); drawPixel(x, y+2, color); drawPixel(x, y+4, color); drawPixel(x, y+3, color);
        drawPixel(x+1, y+2, color);
        drawPixel(x+2, y+2, color);
        drawPixel(x+3, y+2, color);
        drawPixel(x+4, y+2, color);
    }

    if (alpha == 'w' || alpha == 'W') {
        drawPixel(x, y, color); drawPixel(x, y+4, color);
        drawPixel(x+1, y, color);drawPixel(x+1, y+4, color);
        drawPixel(x+2, y, color);drawPixel(x+2, y+2, color);drawPixel(x+2, y+4, color);
        drawPixel(x+3, y, color);drawPixel(x+3, y+1, color);drawPixel(x+3, y+3, color);drawPixel(x+3, y+4, color);
        drawPixel(x+4, y, color);drawPixel(x+4, y+4, color);
    }

    if (alpha == 'y' || alpha == 'Y') {
        drawPixel(x,y,color);drawPixel(x,y+1,color);drawPixel(x,y+6,color);drawPixel(x,y+7,color);
        drawPixel(x+1,y+1,color);drawPixel(x+1,y+2,color);drawPixel(x+1,y+6,color);drawPixel(x+1,y+5,color);

        drawPixel(x+2,y+2,color);drawPixel(x+2,y+4,color);drawPixel(x+2,y+3,color);drawPixel(x+2,y+5,color);
        drawPixel(x+3,y+3,color);drawPixel(x+3,y+4,color);
        drawPixel(x+4,y+3,color);drawPixel(x+4,y+4,color);
        drawPixel(x+5,y+3,color);drawPixel(x+5,y+4,color);
        drawPixel(x+6,y+3,color);drawPixel(x+6,y+4,color);
    }

    if (alpha == '1') {
        drawPixel(x+2, y+4, color);
        drawPixel(x+1, y+3, color); drawPixel(x+2, y+3, color);
        drawPixel(x+2, y+2, color);
        drawPixel(x+2, y+1, color);
        drawPixel(x+1, y, color); drawPixel(x+2, y, color); drawPixel(x+3, y, color);
    }

    if (alpha == '2') {
        drawPixel(x, y+4, color);drawPixel(x+1, y+4, color); drawPixel(x+2, y+4, color); drawPixel(x+3, y+4, color);
        drawPixel(x+4, y+3, color);
        drawPixel(x+1, y+2, color); drawPixel(x+2, y+2, color); drawPixel(x+3, y+2, color);
        drawPixel(x, y+1, color);
        drawPixel(x, y, color); drawPixel(x+1, y, color); drawPixel(x+2, y, color); drawPixel(x+3, y, color);  drawPixel(x+4, y, color);
    }
}

void ledMat::drawGameName(uint8_t x, uint8_t y)
{
    drawLetter(x, y, 'G', red1);
    drawLetter(x, y+7, 'O', red1);
    drawLetter(x, y+15, 'A', red1);
    drawLetter(x, y+22, 'L', red1);
    drawLetter(x, y+28, 'S', red1);
    drawLine(x+7, y, x+7, y+33, skyblue1);

    drawLetter(x+10, y+12, 'O', red1);
    drawLetter(x+10, y+20, 'F', red1);
    drawLine(x+17, y+12, x+17, y+24, skyblue1);

    drawLetter(x+20, y, 'G', red1);
    drawLetter(x+20, y+7, 'L', red1);
    drawLetter(x+20, y+13, 'O', red1);
    drawLetter(x+20, y+21, 'R', red1);
    drawLetter(x+20, y+27, 'Y', red1);
    drawLine(x+27, y, x+27, y+33, skyblue1);
}

ledMat::ledMat()
{
    ballBlocked = false;
    p1Shooting = true;

    for (int i = 0; i < 5; i++) {
        p1_score[i] = pending;
        p2_score[i] = pending;
    }
}

void ledMat::updateScore(uint8_t player, uint8_t shoot_num, score_status status)
{
    switch(player) {
        case 1:
            p1_score[shoot_num] = status;
            break;
        case 2:
            p2_score[shoot_num] = status;
            break;
    }
}

void ledMat::drawScoreGoal(uint8_t p, uint8_t q,uint8_t color_goal)
{
    drawLine(p,q+1,p,q+2, color_goal);
    drawLine(p-1,q,p-1,q+3, color_goal);
    drawLine(p-2,q,p-2,q+3, color_goal);
    drawLine(p-3,q+1,p-3,q+2, color_goal);
}

void ledMat::drawScoreNoGoal(uint8_t p, uint8_t q)
{
    uint8_t color_nogoal = red2;

    drawLine(p,q,p-3,q+3,color_nogoal);
    drawLine(p,q+3,p-3,q,color_nogoal);
}

void ledMat::drawScorePending(uint8_t p, uint8_t q)
{
    drawLine(p,q+1,p,q+2,cyan1);
    drawPixel(p-1,q,cyan1);
    drawPixel(p-1,q+3,cyan1);
    drawPixel(p-2,q,cyan1);
    drawPixel(p-2,q+3,cyan1);
    drawLine(p-3,q+1,p-3,q+2,cyan1);
}


void ledMat::drawPlayer2Scoreboard(uint8_t x, uint8_t y)
{
    switch (p2_score[0]) {
        case pending:
            drawScorePending(x,y);
            break;
        case goal:
            drawScoreGoal(x,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x,y);
            break;
    }

    switch (p2_score[1]) {
        case pending:
            drawScorePending(x-5,y);
            break;
        case goal:
            drawScoreGoal(x-5,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-5,y);
            break;
    }

    switch (p2_score[2]) {
        case pending:
            drawScorePending(x-10,y);
            break;
        case goal:
            drawScoreGoal(x-10,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-10,y);
            break;
    }

    switch (p2_score[3]) {
        case pending:
            drawScorePending(x-15,y);
            break;
        case goal:
            drawScoreGoal(x-15,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-15,y);
            break;
    }

    switch (p2_score[4]) {
        case pending:
            drawScorePending(x-20,y);
            break;
        case goal:
            drawScoreGoal(x-20,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-20,y);
            break;
    }
}

void ledMat::drawPlayer1Scoreboard(uint8_t x, uint8_t y)
{
    switch (p1_score[0]) {
        case pending:
            drawScorePending(x,y);
            break;
        case goal:
            drawScoreGoal(x,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x,y);
            break;
    }

    switch (p1_score[1]) {
        case pending:
            drawScorePending(x-5,y);
            break;
        case goal:
            drawScoreGoal(x-5,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-5,y);
            break;
    }

    switch (p1_score[2]) {
        case pending:
            drawScorePending(x-10,y);
            break;
        case goal:
            drawScoreGoal(x-10,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-10,y);
            break;
    }

    switch (p1_score[3]) {
        case pending:
            drawScorePending(x-15,y);
            break;
        case goal:
            drawScoreGoal(x-15,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-15,y);
            break;
    }

    switch (p1_score[4]) {
        case pending:
            drawScorePending(x-20,y);
            break;
        case goal:
            drawScoreGoal(x-20,y,green1);
            break;
        case noGoal:
            drawScoreNoGoal(x-20,y);
            break;
    }
}

void ledMat::resetShoot()
{
    goalpost_color = GOALPOST_COLOR;
    clearGoalkeeper(goalkeeper_x, goalkeeper_y, GOALKEEPER_COLOR);
    ballBlocked = false;
}

void ledMat::resetGame()
{
    ballBlocked = false;

    for (int i = 0; i < 5; i++) {
        p1_score[i] = pending;
        p2_score[i] = pending;
    }
}

void ledMat::calculateMatrixPosition(int8_t *x, uint8_t *color)
{
    if (*x > 31) {
        *x = *x - 32;
        switch(*color) {
            case blue1:
                *color = blue2;
                break;
            case cyan1:
                *color = cyan2;
                break;
            case green1:
                *color = green2;
                break;
            case purple1:
                *color = purple2;
                break;
            case red1:
                *color = red2;
                break;
            case skyblue1:
                *color = skyblue2;
                break;
            case yellow1:
                *color = yellow2;
                break;
        }
    }
}

void ledMat::drawGoalPost(uint8_t dist_left, uint8_t color)
{
    uint8_t dist_right = 63 - dist_left;

    //Left Boundry line
    drawLine(10, 0, 10, dist_left, GOALPOST_COLOR);
    // Right Boundry Line
    drawLine(10, dist_right, 10, 63, GOALPOST_COLOR);

    // Outer left goalpost
    drawLine(0, dist_left, 9, dist_left, color);
    // Inner left goalpost
    drawLine(2, (dist_left + 2), 8, (dist_left + 2), color);

    // Outer Right Goal Post
    drawLine(0, dist_right, 9, dist_right, color);

    // Inner Right Goal Post
    drawLine(2, (dist_right - 2), 8, (dist_right - 2), color);

    // Outer Top Goal Post
    drawLine(0, (dist_left + 1), 0, dist_right, color);

    // Inner Top Goalpost
    drawLine(2, (dist_left + 2), 2, (dist_right - 2), color);

    // Top Left Corner Pixel
    drawPixel(1, (dist_left + 1), color);
    // Top Right Corner Pixel
    drawPixel(1, (dist_right - 1), color);
    // Bottom Left Corner Pixel
    drawPixel(9, (dist_left + 1), color);
    // Bottom Right Corner Pixel
    drawPixel(9, (dist_right - 1), color);

    // Left Field Marking
    drawLine((GOALKEEPER_X_MAX + 1), 2, 11, 2, GOALPOST_COLOR);
    // Middle Field Marking
    drawLine((GOALKEEPER_X_MAX + 1), 2, (GOALKEEPER_X_MAX + 1), 61, GOALPOST_COLOR);
    // Right Field Marking
    drawLine(11, 61, (GOALKEEPER_X_MAX + 1), 61, GOALPOST_COLOR);
}

void ledMat::drawBall(uint8_t ball_x, uint8_t ball_y, uint8_t color)
{
    // Collision detection mechanisam between Goalkeeper and Ball
    if ((ball_y - 2) >= goalkeeper_y && (ball_y - 2) <= (goalkeeper_y + 4)) {
        if ((ball_x - 1) >= goalkeeper_x && (ball_x - 1) <= (goalkeeper_x + 10)) {
            ballBlocked = true;
        }
    }
    if ((ball_y) >= goalkeeper_y && (ball_y) <= (goalkeeper_y + 4)) {
        if ((ball_x - 2) >= goalkeeper_x && (ball_x - 2) <= (goalkeeper_x + 10)) {
            ballBlocked = true;
        }
    }
    if ((ball_y + 1) >= goalkeeper_y && (ball_y + 1) <= (goalkeeper_y + 4)) {
        if ((ball_x - 2) >= goalkeeper_x && (ball_x - 2) <= (goalkeeper_x + 10)) {
            ballBlocked = true;
        }
    }
    if ((ball_y + 2) >= goalkeeper_y && (ball_y + 2) <= (goalkeeper_y + 4)) {
        if ((ball_x - 1) >= goalkeeper_x && (ball_x - 1) <= (goalkeeper_x + 10)) {
            ballBlocked = true;
        }
    }

    drawLine(ball_x-2, ball_y-1, ball_x+2, ball_y-1, color);
    drawLine(ball_x-2, ball_y, ball_x+2, ball_y, color);
    drawLine(ball_x-2, ball_y+1, ball_x+2, ball_y+1, color);
    // Horizontal Lines
    drawLine(ball_x-1, ball_y-2, ball_x-1, ball_y+2, color);
    drawLine(ball_x, ball_y-2, ball_x, ball_y+2, color);
    drawLine(ball_x+1, ball_y-2, ball_x+1, ball_y+2, color);
}

void ledMat::clearBall(uint8_t ball_x, uint8_t ball_y, uint8_t color)
{
    // Vertical Lines
    clearLine(ball_x-2, ball_y-1, ball_x+2, ball_y-1, color);
    clearLine(ball_x-2, ball_y, ball_x+2, ball_y, color);
    clearLine(ball_x-2, ball_y+1, ball_x+2, ball_y+1, color);
    // Horizontal Lines
    clearLine(ball_x-1, ball_y-2, ball_x-1, ball_y+2, color);
    clearLine(ball_x, ball_y-2, ball_x, ball_y+2, color);
    clearLine(ball_x+1, ball_y-2, ball_x+1, ball_y+2, color);
}

void ledMat::drawGoalkeeper(uint8_t new_x, uint8_t new_y, uint8_t color)
{
    if (ballBlocked) {
        drawGoalkeeperWithBall(new_x, new_y, color);
        return;
    }

    // Clear Previous Goalkeeper Image
    clearGoalkeeperWithBall(goalkeeper_x, goalkeeper_y, color);
    // Check Goalkeeper horizontal movement range
    if (new_y > GOALKEEPER_Y_MIN && (new_y + 4) < GOALKEEPER_Y_MAX) {
        goalkeeper_y = new_y;
    }
    // Check Goalkeeper vertical movement range
    if (new_x > GOALKEEPER_X_MIN && (new_x + 10) < GOALKEEPER_X_MAX) {
        goalkeeper_x = new_x;
    }

    // Draw Goalkeeper Head Top
    drawPixel(goalkeeper_x, goalkeeper_y+1, color);
    drawPixel(goalkeeper_x, goalkeeper_y+2, color);
    drawPixel(goalkeeper_x, goalkeeper_y+3, color);
    // Draw Goalkeeper Head Left
    drawPixel(goalkeeper_x+1, goalkeeper_y, color);
    drawPixel(goalkeeper_x+2, goalkeeper_y, color);
    drawPixel(goalkeeper_x+3, goalkeeper_y+1, color);
    // Draw Goalkeeper Head Right
    drawPixel(goalkeeper_x+1, goalkeeper_y+4, color);
    drawPixel(goalkeeper_x+2, goalkeeper_y+4, color);
    drawPixel(goalkeeper_x+3, goalkeeper_y+3, color);
    // Draw Goalkeeper Neck
    drawPixel(goalkeeper_x+4, goalkeeper_y+2, color);

    // Draw Goalkeeper Body
    drawLine(goalkeeper_x+5, goalkeeper_y+2, goalkeeper_x+8, goalkeeper_y+2, color);

    // Draw Goalkeeper Left Hand
    drawPixel(goalkeeper_x+6, goalkeeper_y+3, color); drawPixel(goalkeeper_x+5, goalkeeper_y+4, color);

    // Draw Goalkeeper Right Hand
    drawPixel(goalkeeper_x+6, goalkeeper_y+1, color); drawPixel(goalkeeper_x+5, goalkeeper_y, color);

    // Draw Goalkeeper Left Leg
    drawPixel(goalkeeper_x+9, goalkeeper_y+3, color); drawPixel(goalkeeper_x+10, goalkeeper_y+4, color);

    // Draw Goalkeeper Right Leg
    drawPixel(goalkeeper_x+9, goalkeeper_y+1, color); drawPixel(goalkeeper_x+10, goalkeeper_y, color);
}

void ledMat::drawGoalKeeperDemoPose1(uint8_t goalKeeperX, uint8_t goalKeeperY,uint8_t color){
    // Draw Goalkeeper Head Top
    drawPixel(goalKeeperX , goalKeeperY+1, color);
    drawPixel(goalKeeperX, goalKeeperY+2, color);
    drawPixel(goalKeeperX, goalKeeperY+3, color);
    // Draw Goalkeeper Head Left
    drawPixel(goalKeeperX+1, goalKeeperY, color);
    drawPixel(goalKeeperX+2, goalKeeperY, color);
    drawPixel(goalKeeperX+3, goalKeeperY+1, color);
    // Draw Goalkeeper Head Right
    drawPixel(goalKeeperX+1, goalKeeperY+4, color);
    drawPixel(goalKeeperX+2, goalKeeperY+4, color);
    drawPixel(goalKeeperX+3, goalKeeperY+3, color);
    // Draw Goalkeeper Neck
    drawPixel(goalKeeperX+4, goalKeeperY+2, color);

    // Draw Goalkeeper Body
    drawLine(goalKeeperX+5, goalKeeperY+2, goalKeeperX+8, goalKeeperY+2, color);

    // Draw Goalkeeper Left Hand
    drawPixel(goalKeeperX+6, goalKeeperY+3, color); drawPixel(goalKeeperX+5, goalKeeperY+4, color);

    // Draw Goalkeeper Right Hand
    drawPixel(goalKeeperX+6, goalKeeperY+1, color); drawPixel(goalKeeperX+5, goalKeeperY, color);

    // Draw Goalkeeper Left Leg
    drawPixel(goalKeeperX+9, goalKeeperY+3, color); drawPixel(goalKeeperX+10, goalKeeperY+4, color);

    // Draw Goalkeeper Right Leg
    drawPixel(goalKeeperX+9, goalKeeperY+1, color); drawPixel(goalKeeperX+10, goalKeeperY, color);
}

void ledMat::clearGoalKeeperDemoPose1(uint8_t goalKeeperX, uint8_t goalKeeperY){
    //    // Clear Goalkeeper Head Top
    clearPixel(goalKeeperX, goalKeeperY+1, cyan1);
    clearPixel(goalKeeperX, goalKeeperY+2, cyan1);
    clearPixel(goalKeeperX, goalKeeperY+3, cyan1);
    // Clear Goalkeeper Head Left
    clearPixel(goalKeeperX+1, goalKeeperY, cyan1);
    clearPixel(goalKeeperX+2, goalKeeperY, cyan1);
    clearPixel(goalKeeperX+3, goalKeeperY+1, cyan1);
    // clear Goalkeeper Head Right
    clearPixel(goalKeeperX+1, goalKeeperY+4, cyan1);
    clearPixel(goalKeeperX+2, goalKeeperY+4, cyan1);
    clearPixel(goalKeeperX+3, goalKeeperY+3, cyan1);
    // clear Goalkeeper Neck
    clearPixel(goalKeeperX+4, goalKeeperY+2, cyan1);

    // clear Goalkeeper Body
    clearLine(goalKeeperX+5, goalKeeperY+2, goalKeeperX+8, goalKeeperY+2, cyan1);

    // clear Goalkeeper Left Hand
    clearPixel(goalKeeperX+6, goalKeeperY+3, cyan1); clearPixel(goalKeeperX+5, goalKeeperY+4, cyan1);

    // clear Goalkeeper Right Hand
    clearPixel(goalKeeperX+6, goalKeeperY+1, cyan1); clearPixel(goalKeeperX+5, goalKeeperY, cyan1);

    // clear Goalkeeper Left Leg
    clearPixel(goalKeeperX+9, goalKeeperY+3, cyan1); clearPixel(goalKeeperX+10, goalKeeperY+4, cyan1);

    // clear Goalkeeper Right Leg
    clearPixel(goalKeeperX+9, goalKeeperY+1, cyan1); clearPixel(goalKeeperX+10, goalKeeperY, cyan1);
}

void ledMat::drawGoalKeeperDemoPose2(uint8_t goalKeeperX, uint8_t goalKeeperY,uint8_t color)
{
    //     Draw Goalkeeper Head Top
    drawPixel(goalKeeperX+1, goalKeeperY+1, color);
    drawPixel(goalKeeperX+1, goalKeeperY+2, color);
    drawPixel(goalKeeperX+1, goalKeeperY+3, color);
    // Draw Goalkeeper Head Left
    drawPixel(goalKeeperX+2, goalKeeperY, color);
    drawPixel(goalKeeperX+3, goalKeeperY, color);
    drawPixel(goalKeeperX+4, goalKeeperY+1, color);
    // Draw Goalkeeper Head Right
    drawPixel(goalKeeperX+2, goalKeeperY+4, color);
    drawPixel(goalKeeperX+3, goalKeeperY+4, color);
    drawPixel(goalKeeperX+4, goalKeeperY+3, color);
    // Draw Goalkeeper Neck
    drawPixel(goalKeeperX+5, goalKeeperY+2, color);

    // Draw Goalkeeper Body
    drawLine(goalKeeperX+5, goalKeeperY+2, goalKeeperX+8, goalKeeperY+2, color);

    // Draw Goalkeeper Left Hand
    drawPixel(goalKeeperX+6, goalKeeperY+3, color); drawPixel(goalKeeperX+7, goalKeeperY+4, color);

    // Draw Goalkeeper Right Hand
    drawPixel(goalKeeperX+6, goalKeeperY+1, color); drawPixel(goalKeeperX+7, goalKeeperY, color);

    // Draw Goalkeeper Left Leg
    drawPixel(goalKeeperX+9, goalKeeperY+3, color); drawPixel(goalKeeperX+10, goalKeeperY+3, color);

    // Draw Goalkeeper Right Leg
    drawPixel(goalKeeperX+9, goalKeeperY+1, color); drawPixel(goalKeeperX+10, goalKeeperY+1, color);
}

void ledMat::clearGoalKeeperDemoPose2(uint8_t goalKeeperX, uint8_t goalKeeperY)
{
    //     clear Goalkeeper Head Top
    clearPixel(goalKeeperX+1, goalKeeperY+1, cyan1);
    clearPixel(goalKeeperX+1, goalKeeperY+2, cyan1);
    clearPixel(goalKeeperX+1, goalKeeperY+3, cyan1);
    // clear Goalkeeper Head Left
    clearPixel(goalKeeperX+2, goalKeeperY, cyan1);
    clearPixel(goalKeeperX+3, goalKeeperY, cyan1);
    clearPixel(goalKeeperX+4, goalKeeperY+1, cyan1);
    // clear Goalkeeper Head Right
    clearPixel(goalKeeperX+2, goalKeeperY+4, cyan1);
    clearPixel(goalKeeperX+3, goalKeeperY+4, cyan1);
    clearPixel(goalKeeperX+4, goalKeeperY+3, cyan1);
    // clear Goalkeeper Neck
    clearPixel(goalKeeperX+5, goalKeeperY+2, cyan1);

    // clear Goalkeeper Body
    clearLine(goalKeeperX+5, goalKeeperY+2, goalKeeperX+8, goalKeeperY+2, cyan1);

    // clear Goalkeeper Left Hand
    clearPixel(goalKeeperX+6, goalKeeperY+3, cyan1); clearPixel(goalKeeperX+7, goalKeeperY+4, cyan1);

    // clear Goalkeeper Right Hand
    clearPixel(goalKeeperX+6, goalKeeperY+1, cyan1); clearPixel(goalKeeperX+7, goalKeeperY, cyan1);

    // clear Goalkeeper Left Leg
    clearPixel(goalKeeperX+9, goalKeeperY+3, cyan1); clearPixel(goalKeeperX+10, goalKeeperY+3, cyan1);

    // clear Goalkeeper Right Leg
    clearPixel(goalKeeperX+9, goalKeeperY+1, cyan1); clearPixel(goalKeeperX+10, goalKeeperY+1, cyan1);
}

void ledMat::drawGoalkeeperWithBall(uint8_t new_x, uint8_t new_y, uint8_t color)
{
    // Clear Previous Goalkeeper Image
    clearGoalkeeperWithBall(goalkeeper_x, goalkeeper_y, color);

    // Check Goalkeeper horizontal movement range
    if (new_y > GOALKEEPER_Y_MIN && (new_y + 4) < GOALKEEPER_Y_MAX) {
        goalkeeper_y = new_y;
    }
    // Check Goalkeeper vertical movement range
    if (new_x > GOALKEEPER_X_MIN && (new_x + 10) < GOALKEEPER_X_MAX) {
        goalkeeper_x = new_x;
    }

    // Draw Goalkeeper Head Top
    drawPixel(goalkeeper_x, goalkeeper_y+1, color);
    drawPixel(goalkeeper_x, goalkeeper_y+2, color);
    drawPixel(goalkeeper_x, goalkeeper_y+3, color);
    // Draw Goalkeeper Head Left
    drawPixel(goalkeeper_x+1, goalkeeper_y, color);
    drawPixel(goalkeeper_x+2, goalkeeper_y, color);
    // Draw Goalkeeper Head Right
    drawPixel(goalkeeper_x+1, goalkeeper_y+4, color);
    drawPixel(goalkeeper_x+2, goalkeeper_y+4, color);

    // Draw Goalkeeper Body
    drawPixel(goalkeeper_x+8, goalkeeper_y+2, color);

    // Draw Ball in hands of Goalkeeper
    drawBall(goalkeeper_x+5, goalkeeper_y+2, black);
    if (p1Shooting)
        drawBall(goalkeeper_x+5, goalkeeper_y+2, p1Color);
    else
        drawBall(goalkeeper_x+5, goalkeeper_y+2, p2Color);

    // Draw Extra Left Hand Pixel
    drawPixel(goalkeeper_x+4, goalkeeper_y+5, color);
    // Draw Extra Right Hand Pixel
    drawPixel(goalkeeper_x+4, goalkeeper_y-1, color);

    // Draw Goalkeeper Left Leg
    drawPixel(goalkeeper_x+9, goalkeeper_y+3, color); drawPixel(goalkeeper_x+10, goalkeeper_y+4, color);

    // Draw Goalkeeper Right Leg
    drawPixel(goalkeeper_x+9, goalkeeper_y+1, color); drawPixel(goalkeeper_x+10, goalkeeper_y, color);
}

void ledMat::clearGoalkeeper(uint8_t new_x, uint8_t new_y, uint8_t color)
{
    static uint8_t x = 3, y = 30;

    if (ballBlocked) {
        clearGoalkeeperWithBall(new_x, new_y, color);
        return;
    }

    // Check Goalkeeper horizontal movement range
    if (new_y > GOALKEEPER_Y_MIN && (new_y + 4) < GOALKEEPER_Y_MAX) {
        y = new_y;
    }
    // Check Goalkeeper vertical movement range
    if (new_x > GOALKEEPER_X_MIN && (new_x + 10) < GOALKEEPER_X_MAX) {
        x = new_x;
    }

    // Clear Goalkeeper Head Top
    clearPixel(x, y+1, color);
    clearPixel(x, y+2, color);
    clearPixel(x, y+3, color);
    // Clear Goalkeeper Head Left
    clearPixel(x+1, y, color);
    clearPixel(x+2, y, color);
    clearPixel(x+3, y+1, color);
    // Clear Goalkeeper Head Right
    clearPixel(x+1, y+4, color);
    clearPixel(x+2, y+4, color);
    clearPixel(x+3, y+3, color);
    // Clear Goalkeeper Neck
    clearPixel(x+4, y+2, color);

    // Clear Goalkeeper Body
    clearLine(x+5, y+2, x+8, y+2, color);

    // Clear Goalkeeper Left Hand
    clearPixel(x+6, y+3, color); clearPixel(x+5, y+4, color);

    // Clear Goalkeeper Right Hand
    clearPixel(x+6, y+1, color); clearPixel(x+5, y, color);

    // Clear Goalkeeper Left Leg
    clearPixel(x+9, y+3, color); clearPixel(x+10, y+4, color);

    // Clear Goalkeeper Right Leg
    clearPixel(x+9, y+1, color); clearPixel(x+10, y, color);
}

void ledMat::clearGoalkeeperWithBall(uint8_t new_x, uint8_t new_y, uint8_t color)
{
    static uint8_t x = 3, y = 30;

    // Check Goalkeeper horizontal movement range
    if (new_y > GOALKEEPER_Y_MIN && (new_y + 4) < GOALKEEPER_Y_MAX) {
        y = new_y;
    }
    // Check Goalkeeper vertical movement range
    if (new_x > GOALKEEPER_X_MIN && (new_x + 10) < GOALKEEPER_X_MAX) {
        x = new_x;
    }

    // Clear Goalkeeper Head Top
    clearPixel(x, y+1, color);
    clearPixel(x, y+2, color);
    clearPixel(x, y+3, color);
    // Clear Goalkeeper Head Left
    clearPixel(x+1, y, color);
    clearPixel(x+2, y, color);
    clearPixel(x+3, y+1, color);
    // Clear Goalkeeper Head Right
    clearPixel(x+1, y+4, color);
    clearPixel(x+2, y+4, color);
    clearPixel(x+3, y+3, color);
    // Clear Goalkeeper Neck
    clearPixel(x+4, y+2, color);

    // Clear Goalkeeper Body
    clearLine(x+5, y+2, x+8, y+2, color);

    // Clear Ball in hands of Goalkeeper
    if (p1Shooting)
        clearBall(goalkeeper_x+5, goalkeeper_y+2, p1Color);
    else
        clearBall(goalkeeper_x+5, goalkeeper_y+2, p2Color);

    // Draw Extra Left Hand Pixel
    clearPixel(x+4, y+5, color);
    // Draw Extra Right Hand Pixel
    clearPixel(x+4, y-1, color);

    // Clear Goalkeeper Left Hand
    clearPixel(x+6, y+3, color); clearPixel(x+5, y+4, color);

    // Clear Goalkeeper Right Hand
    clearPixel(x+6, y+1, color); clearPixel(x+5, y, color);

    // Clear Goalkeeper Left Leg
    clearPixel(x+9, y+3, color); clearPixel(x+10, y+4, color);

    // Clear Goalkeeper Right Leg
    clearPixel(x+9, y+1, color); clearPixel(x+10, y, color);
}

void ledMat::drawWinDesign(uint8_t x, uint8_t y, uint8_t num, uint8_t color)
{
    switch(num) {
        case 0:
            drawPixel(x,y,color);
            break;
        case 1:
            drawPixel(x,y,color);drawPixel(x,y-1,color);drawPixel(x,y+1,color);
            drawPixel(x+1,y,color);drawPixel(x-1,y,color);
            break;
        case 2:
            drawLine(x-2,y-2,x-4,y-4,color);
            drawLine(x-3,y,x-6,y,color);
            drawLine(x-2,y+2,x-4,y+4,color);
            drawLine(x,y+3,x,y+6,color);
            drawLine(x+2,y+2,x+4,y+4,color);
            drawLine(x+3,y,x+6,y,color);
            drawLine(x+2,y-2,x+4,y-4,color);
            drawLine(x,y-3,x,y-6,color);
            break;
    }
}

void ledMat::clearUpperLeftCorner()
{
    for (int x = 0; x < 17; x++) {
        for (int y = 0; y < 14; y++) {
            clearPixel(x, y, cyan1);
        }
    }
}

void ledMat::clearUpperRightCorner()
{
    for (int x = 0; x < 17; x++) {
        for (int y = 63; y > 49; y--) {
            clearPixel(x, y, cyan1);
        }
    }
}

void ledMat::drawEndScreen(uint8_t x, uint8_t y)
{
    uint8_t color1 = cyan1;
    uint8_t color2 = cyan2;
    uint8_t color3= skyblue1;
    uint8_t c_temp = 1;
    uint8_t p1_goal_tally = 0, p2_goal_tally = 0;
    int8_t game_result = 0;
    uint8_t i1 = 0;
    bool pose1 = false;

    // Calculate Final Goal Tally of P1 and P2
    for (int i = 0; i < 5; i++) {
        if (goal == p1_score[i])
            p1_goal_tally++;
        if (goal == p2_score[i])
            p2_goal_tally++;
    }
    if (p1_goal_tally > p2_goal_tally) {
        game_result--;
    } else if (p1_goal_tally < p2_goal_tally) {
        game_result++;
    }

    while(1) {
        // Draw (inner) Field outline
        drawLine(x,y,x-27,y+14,color3);
        drawLine(x,y+63,x-27,y+49,color3);
        drawLine(x-4,y,x-29,y+13,color3);
        // Draw (outer) Field outline
        drawLine(x-4,y+63,x-29,y+ 50, color3);
        drawLine(x-27,y+14,x-27, y+49,color3);
        drawLine(x-29,y+14,x-29,y+ 49,color3);

        if (game_result < 0) {      // P1 WINS!!
            drawLetter(x-45, y+27 , 'P', p1Color);
            drawDigit(x-45, y+32, 1, p1Color);

            drawLetter(x-37, y+18, 'w', red1);
            drawLetter(x-37, y+24, 'i', red1);
            drawLetter(x-37, y+30, 'n', red1);
            drawLetter(x-37, y+36, 's', red1);
            drawLetter(x-37, y+42, '!', red1);
            drawLetter(x-37, y+44, '!', red1);

            //draw player1
            if (pose1) {
                clearGoalKeeperDemoPose2(x-22,y+20);
                drawGoalKeeperDemoPose1(x-22,y+20,p1Color);
                pose1 = false;
            } else {
                clearGoalKeeperDemoPose1(x-22,y+20);
                drawGoalKeeperDemoPose2(x-22,y+20,p1Color);
                pose1 = true;
            }
            //draw player2
            drawGoalKeeperDemoPose2(x-22,y+37,p2Color);

        } else if (game_result > 0) {   // P2 WINS!!
            drawLetter(x-45, y+27 , 'P', p2Color);
            drawDigit(x-45, y+32, 2, p2Color);

            drawLetter(x-37, y+18, 'w', red1);
            drawLetter(x-37, y+24, 'i', red1);
            drawLetter(x-37, y+30, 'n', red1);
            drawLetter(x-37, y+36, 's', red1);
            drawLetter(x-37, y+42, '!', red1);
            drawLetter(x-37, y+44, '!', red1);
            //draw player2
            if (pose1) {
                clearGoalKeeperDemoPose2(x-22,y+37);
                drawGoalKeeperDemoPose1(x-22,y+37,p2Color);
                pose1 = false;
            } else {
                clearGoalKeeperDemoPose1(x-22,y+37);
                drawGoalKeeperDemoPose2(x-22,y+37,p2Color);
                pose1 = true;
            }
            //draw player1
            drawGoalKeeperDemoPose2(x-22,y+20,p1Color);
        } else  {   // P1 & P2 TIE
            drawLetter(x-45, y+41 , 'P', p2Color);
            drawDigit(x-45, y+46, 2, p2Color);

            drawLetter(x-45, y+30 , '&', cyan1);

            drawLetter(x-45, y+16 , 'P', p1Color);
            drawDigit(x-45, y+22, 1, p1Color);

            drawLetter(x-37, y+24, 'T', cyan1);
            drawLetter(x-37, y+30, 'I', cyan1);
            drawLetter(x-37, y+36, 'E', cyan1);

            //draw player1 & 2
            if (pose1) {
                clearGoalKeeperDemoPose1(x-22,y+20);
                clearGoalKeeperDemoPose2(x-22,y+37);
                drawGoalKeeperDemoPose2(x-22,y+20,p1Color);
                drawGoalKeeperDemoPose1(x-22,y+37,p2Color);
                pose1 = false;
            } else {
                clearGoalKeeperDemoPose2(x-22,y+20);
                clearGoalKeeperDemoPose1(x-22,y+37);
                drawGoalKeeperDemoPose1(x-22,y+20,p1Color);
                drawGoalKeeperDemoPose2(x-22,y+37,p2Color);
                pose1 = true;
            }
        }

        drawLetter(x-9, y+19, 'P', cyan1);
        drawDigit(x-9,y+25,1,color1);
        drawLetter(x-9, y+35, 'P', cyan1);
        drawDigit(x-9,y+40,2,color1);

        switch (p1_goal_tally) {
            case 0:
                drawLetter(x-2, y+21, 'Q', color1);
                break;
            case 1:
                drawDigit(x-2, y+21, 1, color1);
                break;
            case 2:
                drawDigit(x-2, y+21, 2, color1);
                break;
            case 3:
                drawDigit(x-2, y+21, 3, color1);
                break;
            case 4:
                drawDigit(x-2, y+21, 4, color1);
                break;
            case 5:
                drawDigit(x-2, y+21, 5, color1);
                break;
        }

        drawLetter(x, y+28, '-', red1);

        switch (p2_goal_tally) {
            case 0:
                drawLetter(x-2, y+37, 'Q', color2);
                break;
            case 1:
                drawDigit(x-2, y+37, 1, color2);
                break;
            case 2:
                drawDigit(x-2, y+37, 2, color2);
                break;
            case 3:
                drawDigit(x-2, y+37, 3, color2);
                break;
            case 4:
                drawDigit(x-2, y+37, 4, color2);
                break;
            case 5:
                drawDigit(x-2, y+37, 5, color2);
                break;
        }

        switch(i1) {
            case 0:
                drawWinDesign(10, 7, i1, c_temp);
                drawWinDesign(10, 56, i1, c_temp + 3);
                break;
            case 1:
                drawWinDesign(10, 7, i1, c_temp);
                drawWinDesign(10, 56, i1, c_temp + 3);
                break;
            case 2:
                drawWinDesign(10, 7, i1, c_temp);
                drawWinDesign(10, 56, i1, c_temp + 3);
                break;
        }
        vTaskDelay(DELAY * 25);
        if (i1 > 3) {
            i1 = 0;
            clearUpperLeftCorner();
            clearUpperRightCorner();
            if (c_temp > 3) {
                c_temp = 1;
            }
            c_temp++;
        }
        i1++;
        clearUpperLeftCorner();
        clearUpperRightCorner();
    }
}

void ledMat::drawStartScreen(uint8_t x, uint8_t y)
{
    uint8_t color1 = cyan1;
    uint8_t color3= skyblue1;

    //draw player1&2
    drawLetter(14, 2, 'P', cyan1);
    drawDigit(14,8,1,color1);
    drawLetter(14, 53, 'P', cyan1);
    drawDigit(14,58,2,color1);
    // Draw (inner) Field outline
    drawLine(x,y,x-27,y+14,color3);
    drawLine(x,y+63,x-27,y+49,color3);
    drawLine(x-4,y,x-29,y+13,color3);
    // Draw (outer) Field outline
    drawLine(x-4,y+63,x-29,y+ 50, color3);
    drawLine(x-27,y+14,x-27, y+49,color3);
    drawLine(x-29,y+14,x-29,y+ 49,color3);

    //select color cubes
    uint8_t x1=7;
    uint8_t y1=12;

    drawScoreGoal(x1,y1,blue1);
    drawScoreGoal(x1,y1+7,green1);
    drawScoreGoal(x1,y1+14,red1);
    drawScoreGoal(x1,y1+21,yellow1);
    drawScoreGoal(x1,y1+28,skyblue1);
    drawScoreGoal(x1,y1+35,purple1);

    drawGameName(32, 14);
}

void ledMat::drawArrow(uint8_t x,uint8_t y,uint8_t color )
{
    drawLine(x,y+1,x+5,y+1,color);
    drawLine(x,y+2,x+5,y+2,color);
    drawPixel(x+1,y,color);
    drawPixel(x+1,y+3,color);
    drawPixel(x+2,y-1,color);
    drawPixel(x+2,y+4,color);
}

void ledMat::clearArrow(uint8_t color)
{
    clearLine(9,9,9,51,color);
    clearLine(10,9,10,51,color);
    clearLine(11,9,11,51,color);
    clearLine(12,9,12,51,color);
    clearLine(13,9,13,51,color);
    clearLine(14,9,14,51,color);
    clearLine(15,9,15,51,color);
    clearLine(16,9,16,51,color);
    clearLine(17,9,17,51,color);
}
