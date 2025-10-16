#ifndef IO_H
#define IO_H

//Affectation des pins des LEDS    
#define LED_BLANCHE_1 _LATJ6 //RJ6
#define LED_BLEUE_1 _LATJ5   //RJ5
#define LED_ORANGE_1 _LATJ4  //RJ4
#define LED_ROUGE_1 _LATJ11  //RJ11
#define LED_VERTE_1 _LATH10  //RH10

#define LED_BLANCHE_2 _LATA0
#define LED_BLEUE_2 _LATA9
#define LED_ORANGE_2 _LATK15
#define LED_ROUGE_2 _LATA10
#define LED_VERTE_2 _LATH3

#define BP1 _RH1
#define BP2 _RH0
// Prototypes fonctions
void InitIO();
void LockIO();
void UnlockIO();

#endif /* IO_H */

