
#ifndef PWM_H
#define	PWM_H

#define MOTEUR_DROIT 1
#define MOTEUR_GAUCHE 0

//void PWMSetSpeed(float vitesseEnPourcents, int moteur);

void InitPWM(void);
void PWMUpdateSpeed(void);
void PWMSetSpeedConsigne(float vitesseEnPourcents, int moteur);


#endif


