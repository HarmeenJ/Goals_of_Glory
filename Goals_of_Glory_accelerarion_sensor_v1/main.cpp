/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 *          FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 *          @see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */

#include "tasks.hpp"
#include "examples/examples.hpp"
#include "io.hpp"
#include "stdio.h"
#include "printf_lib.h"
#include "math.h"
#include "FreeRTOS.h"
#include "handlers.hpp"
#include "FreeRTOSConfig.h"
#include "string.h"
#include "nrf_stream.hpp"


void GoalkeeperLocation(void *p)
{

    float orientationAngle = 0;
    int wireless_ret = -99;
    int16_t x,y,z;
    const char *data = "";

    while(1)
    {
        x = AS.getX();      // Get X-axis values
        y = AS.getY();      // Get Y-axis values
        z = AS.getZ();      // Get Z-axis values

        //u0_dbg_printf("X: %d, Y: %d, Z: %d\n", x, y, z);

        orientationAngle = sin(z/x);

        // Left orientation
        if ((orientationAngle > 0.025) && (x > 50))
        {
            u0_dbg_printf("LEFT.\n");
            //wireless_ret=wireless_send(221,mesh_pkt_nack,"left",4,0);
            data = "left";
        }
        // Right orientation
        else if ((orientationAngle < (-0.025)) && (x < 0))
        {
            u0_dbg_printf("RIGHT.\n");
            //wireless_ret=wireless_send(221,mesh_pkt_nack,"right",5,0);
            data = "right";
        }

/*      Goalkeeper up and down movement inside the D location
        // Up orientation
        else if ((directionAngle > (0.022)) && (y > 0))
        {
            u0_dbg_printf("UP.\n");
            wireless_send(221,mesh_pkt_nack,"up",2,0);
        }
        // Down orientation
        else if ((directionAngle <= (-0.022)) && (y < 0))
        {
            u0_dbg_printf("DOWN.\n");
            wireless_send(221,mesh_pkt_nack,"down",4,0);
        }
*/

        // Default orientation
        else if((orientationAngle > (-0.025)) && (orientationAngle < 0.025) && (orientationAngle != 0.0) && (z > 0))
        {
            //u0_dbg_printf("DEFAULT\n");
            //wireless_ret=wireless_send(221,mesh_pkt_nack,"default",7,0);
            data = "default";
        }

        // Invalid orientation or up/down orientation.
        else
        {
            //u0_dbg_printf("Invalid\n");
        }

        wireless_ret=wireless_send(221,mesh_pkt_ack,data,strlen(data),0);
        data = "";
        printf("\n%d\n", wireless_ret);
        wireless_ret=-99;
        vTaskDelay(50);
    }
}

int main(void)
{
    wireless_init();
    wireless_service();

    // Initialize the on-board accelerometer sensor.
    bool start = AS.init();


    if (!start)
        u0_dbg_printf("Accelerometer has not yet been initialized!\n");
    else
    {
        xTaskCreate(GoalkeeperLocation, "GKLocation", 1024, (void *) 1, PRIORITY_CRITICAL,NULL);
    }


    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));
    scheduler_start();
    return 0;
}
