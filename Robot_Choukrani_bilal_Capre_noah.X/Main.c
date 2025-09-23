#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "pwm.h"
#include "ADC.h"
#include "robot.h"

int main (void){

InitOscillator();

InitIO();
LED_BLANCHE_1 = 1;
LED_BLEUE_1 = 1;
LED_ORANGE_1 = 1;
LED_ROUGE_1 = 1;
LED_VERTE_1 = 1;

InitTimer1();
InitTimer23();
InitTimer4();
InitPWM(); 
InitADC1();



while(1)
{
} 
// fin main
}
void infra_rouge(){
if (ADCIsConversionFinished() == 1)
{
ADCClearConversionFinishedFlag();
unsigned int * result = ADCGetResult();
float volts = ((float) result [0])* 3.3 / 4096;
robotState.distanceTelemetreGauche = 34 / volts - 5;
volts = ((float) result [1])* 3.3 / 4096;
robotState.distanceTelemetreCentre = 34 / volts - 5;
volts = ((float) result [2])* 3.3 / 4096;
robotState.distanceTelemetreDroit = 34 / volts - 5;
}
}
