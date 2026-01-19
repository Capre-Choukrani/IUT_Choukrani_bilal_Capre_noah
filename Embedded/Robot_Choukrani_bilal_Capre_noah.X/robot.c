#include "robot.h"
#include "PWM.h"
#include "ADC.h"
#include "main.h"
#include "UART_Protocol.h"








volatile ROBOT_STATE_BITS robotState;
volatile unsigned char autoControlActivated = 1;  // 1 = auto par défaut
extern unsigned char stateRobot;



   // 1 = automatique, 0 = manuel

void SetRobotAutoControlState(unsigned char state)
{
    if(state == 0)
    {
        autoControlActivated = 0;   // mode manuel
    }
    else
    {
        autoControlActivated = 1;   // mode automatique
    }
}


void SetRobotState(unsigned char state)
{
    if(autoControlActivated == 0)   // seulement en manuel
    {
        stateRobot = state;
        
    }
}

