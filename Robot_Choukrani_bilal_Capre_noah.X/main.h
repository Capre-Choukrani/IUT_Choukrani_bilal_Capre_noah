    
#ifndef MAIN_H
#define	MAIN_H

void OperatingSystemLoop(void);
void SetNextRobotStateInAutomaticMode(void);
void Infrarouge_Conversion(void);
char Operating(unsigned char binary);
unsigned char BinaryCapteur(float capteur, int val);
unsigned char StateBinary(void);
void ft_LED(void);
void ft_send_info(void);
void ft_send_telemetre(void);
void ft_send_motor(void);
void ft_send_mode(void);
void ft_send_led(void);



 /////////////////////MODIF/////////////////////
 
#define FREQUENCE_TIMER_1 250
#define FREQUENCE_TIMER_4 1000

#define VITESSE_ROBOT 20
#define VITESSE_T 19
#define VITESSE_VIRAGE 21

#define DISTANCE_LIM 31
#define DISTANCE_LIM_CENTRE 38
#define DISTANCE_LIM_EXT 24


 //////////////////////////////////////////

#define STATE_ATTENTE 0
#define STATE_ATTENTE_EN_COURS 1
#define STATE_AVANCE 2
#define STATE_AVANCE_EN_COURS 3
#define STATE_TOURNE_GAUCHE 4
#define STATE_TOURNE_GAUCHE_EN_COURS 5
#define STATE_TOURNE_DROITE 6
#define STATE_TOURNE_DROITE_EN_COURS 7
#define STATE_TOURNE_SUR_PLACE_GAUCHE 8
#define STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS 9
#define STATE_TOURNE_SUR_PLACE_DROITE 10
#define STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS 11
#define STATE_ARRET 12
#define STATE_ARRET_EN_COURS 13
#define STATE_RECULE 14
#define STATE_RECULE_EN_COURS 15

#endif

