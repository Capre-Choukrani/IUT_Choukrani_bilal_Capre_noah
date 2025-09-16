#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "pwm.h"

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
InitPWM(); 
InitADC1();


while(1)
{

} // fin main
}
