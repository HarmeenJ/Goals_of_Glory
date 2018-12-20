
#include <stdio.h>
#include "scheduler_task.hpp"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "tasks.hpp"
#include "queue.h"

#include "scheduler_task.hpp"
#include "singleton_template.hpp"
#include "i2c2.hpp"
#include "io.hpp"
#include "math.h"
#include "nrf_stream.hpp"
#include "string.h"

#define A 0.962
#define dt 0.020

int calx,caly;

double accel_x_cal,accel_y_cal,accel_z_cal;
double accelX, accelY, accelZ;

float rollangle,pitchangle;
float roll,pitch,yaw;

double gyroX, gyroY, gyroZ;
double gyro_x_cal,gyro_y_cal,gyro_z_cal;

float PI = 3.14159265358979323846f;
int minVal=265;
int maxVal=402;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float constrain(float x, float a, float b)
{
    if(x > a && x < b)
        return x;
    else if( x < a)
        return a;
    else if(x > b)
        return b;
}

void Gyro_Task(void *pvParameter)
{
    const char* data = "";
    mesh_packet_t MasterRcvPkt;
    float arx, ary, arz;
    while(1)
    {
        accelX = MPU_SEN.ACCEL_XOUT();
        accelY = MPU_SEN.ACCEL_YOUT();
        accelZ = MPU_SEN.ACCEL_ZOUT();

        gyroX = MPU_SEN.GYRO_XOUT();
        gyroY = MPU_SEN.GYRO_YOUT();
        gyroZ = MPU_SEN.GYRO_ZOUT();

        accelX = accelX / 16384.0;
        accelY = accelY / 16384.0;
        accelZ = accelZ / 16384.0;

        arx = (180/3.141592) * atan(accelX / sqrt(pow(accelY,2) + pow(accelZ,2)));
        ary = (180/3.141592) * atan(accelY / sqrt(pow(accelX,2) + pow(accelZ,2)));
        arz = (180/3.141592) * atan(sqrt(pow(accelY,2) + pow(accelX,2)) / accelZ);

        printf("AccX: %f\n", arx);
        printf("AccY: %f\n", ary);
        printf("AccZ: %f\n\n", arz);

        if((arx > 25 && arx < 45) && (arz > 27 && arz < 47))
        {
            printf("right\n");
            data = "right";
        }
        else if((arx < 25) && (arz < 27))
        {
            printf("ext right\n");
            data = "ext right";
        }
        else if((arx > 70 && arx < 80) && (arz < -75 && arz > -85))
        {
            printf("left\n");
            data = "left";
        }
        else if((arx > 45 && arx < 70) && (arz < -55 && arz > -75))
        {
            printf("ext left\n");
            data = "ext left";
        }
        else if(arx < 60 && ary > 20)
        {
            printf("straight\n");
            data = "straight";
        }

//        printf("AccX: %f\n", accelX);
//        printf("AccY: %f\n", accelY);
//        printf("AccZ: %f\n\n", accelZ);

        gyroX = gyroX / 131.0;
        gyroY = gyroY / 131.0;
        gyroZ = gyroZ / 131.0;

        //        printf("gyroX: %f\n", gyroX);
        //        printf("gyroY: %f\n", gyroY);
        //        printf("gyroZ: %f\n\n", gyroZ);

        rollangle=atan2(accelY,accelZ)*180/PI; // FORMULA FOUND ON INTERNET
        pitchangle=atan2(accelX,sqrt(accelY*accelY+accelZ*accelZ))*180/PI; //FORMULA FOUND ON INTERNET

        //Using Complementary Filter
        roll=A*(roll+gyroX*dt)+(1-A)*rollangle;
        pitch=A*(pitch+gyroY*dt)+(1-A)*pitchangle;

        yaw=gyroZ;

//        printf("Roll: %f\n", roll);
//        printf("Pitch: %f\n", pitch);
//        printf("Yaw: %f\n\n", yaw);
        //        data = "right";

//        if(roll > 0 && roll < 1 && pitch < 1.5)
//        {
//            data = "right";
//            //            wireless_send(221, mesh_pkt_nack, "right", 5, 0);
//        }
//        else if(roll > 5)
//        {
//            data = "left";
//            //            wireless_send(221, mesh_pkt_nack, "left", 4, 0);
//        }
//        else if(roll < 0 && pitch < 1.5)
//        {
//            data = "straight";
//            //            wireless_send(221, mesh_pkt_nack, "straight", 7, 0);
//        }

//        data = "right";
        if(data != NULL)
        {
//            printf("%s\n", data);
            wireless_send(221, mesh_pkt_ack, data, strlen(data), 0);
            //            wireless_get_rx_pkt(&MasterRcvPkt, 500);
            //            printf("%s", MasterRcvPkt.data);
            //            while(memcmp(MasterRcvPkt.data, "Send Data", strlen("Send Data")));
        }



        data = NULL;
        rollangle=0;
        pitchangle=0;

        roll=0;
        pitch=0;
        yaw=0;

        vTaskDelay(100);

        //        printf("%d\n", MPU_SEN.MOTION_DURATION());
    }
}

int main(void)
{
    MPU_SEN.MPU6050init();
    wireless_init();

    xTaskCreate(Gyro_Task, "GYRO", 512, NULL, PRIORITY_HIGH, NULL);

    //    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_add_task(new wirelessTask(PRIORITY_HIGH));
    scheduler_start();
    vTaskStartScheduler();
    return 0;
}
