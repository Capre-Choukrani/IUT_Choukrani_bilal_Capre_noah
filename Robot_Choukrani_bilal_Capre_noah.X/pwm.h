/* 
 * File:   pwm.h
 * Author: E306-PC7
 *
 * Created on 10 septembre 2025, 09:26
 */

#ifndef PWM_H
#define	PWM_H
#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

void PWMUpdateSpeed();
void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur);
void InitPWM(void);

#endif	/* PWM_H */

