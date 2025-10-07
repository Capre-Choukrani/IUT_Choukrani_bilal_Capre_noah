#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>

#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "ADC.h"
#include "Toolbox.h"
#include "PWM.h"
#include "main.h"
#include "robot.h"
#include "uart.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "UART_Protocol.h"

unsigned char stateRobot = STATE_ATTENTE;
unsigned char nextStateRobot = 0;
unsigned char lastStateRobot = 0;
float vitesse;

int main(void) {
    vitesse = VITESSE_ROBOT;
    robotState.mode = 0;
    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer4();
    InitTimer23();
    InitPWM();
    InitADC1();
    InitUART();

    int counter = 0;
    unsigned int last;

    while (1) {

        if (BP1 != 0) {
            tempAction = 0;
            robotState.mode = 1;
        }

        if (BP2 != 0 || (tempAction > 60000 && robotState.mode)) {
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            robotState.mode = 0;
        }
        for (int i = 0; i < CB_RX1_GetDataSize(); i++) {
            UartDecodeMessage(CB_RX1_Get());
        }

        if (_millis - last > 500) {
            switch (counter) {
                case 0:
                    ft_send_telemetre();
                    break;
                case 1:
                    ft_send_motor();
                    break;
                case 2:
                    ft_send_mode();
                    break;
                case 3:
                    ft_send_led();
                    break;
            }
            last = _millis;
            counter = (counter + 1) % 4;
        }

        if (robotState.mode)
            ft_LED();
    }
}

void ft_send_mode(void) {
   
}
                                                                                                                                                                                                                                                                                                                                                                                            
void ft_send_telemetre() {
    unsigned char telemetre[5];
    telemetre[0] = (unsigned char) (robotState.distanceTelemetreExtGauche);
    telemetre[1] = (unsigned char) (robotState.distanceTelemetreGauche);
    telemetre[2] = (unsigned char) (robotState.distanceTelemetreCentre);
    telemetre[3] = (unsigned char) (robotState.distanceTelemetreDroit);
    telemetre[4] = (unsigned char) (robotState.distanceTelemetreExtDroit);
    UartEncodeAndSendMessage(0x0030, 5, telemetre);
}

void ft_send_motor() {
    unsigned char motor[2];
    motor[0] = robotState.vitesseGaucheCommandeCourante;
    motor[1] = robotState.vitesseDroiteCommandeCourante;
    UartEncodeAndSendMessage(0x0040, 2, motor);
}

void ft_send_led(void) {
    unsigned char led[5];
    led[0] = LED_BLANCHE_1;
    led[1] = LED_BLEUE_1;
    led[2] = LED_ORANGE_1;
    led[3] = LED_ROUGE_1;
    led[4] = LED_VERTE_1;
    UartEncodeAndSendMessage(0x0020, 5, led);
}

void ft_LED(void) {
    LED_BLANCHE_1 = (robotState.distanceTelemetreExtGauche < DISTANCE_LIM);
    LED_BLEUE_1 = (robotState.distanceTelemetreGauche < DISTANCE_LIM);
    LED_ORANGE_1 = (robotState.distanceTelemetreCentre < DISTANCE_LIM);
    LED_ROUGE_1 = (robotState.distanceTelemetreDroit < DISTANCE_LIM);
    LED_VERTE_1 = (robotState.distanceTelemetreExtDroit < DISTANCE_LIM);

    unsigned char robot = (vitesse == 30);
    LED_VERTE_2 = LED_ROUGE_2 = LED_ORANGE_2 = LED_BLEUE_2 = LED_BLANCHE_2 = robot;
}

void OperatingSystemLoop() {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
            break;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(VITESSE_VIRAGE, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_VIRAGE / 2.5, MOTEUR_GAUCHE);
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(VITESSE_VIRAGE / 2.5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_VIRAGE, MOTEUR_GAUCHE);
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(VITESSE_T, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_T, MOTEUR_GAUCHE);
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-VITESSE_T, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_T, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

void SetNextRobotStateInAutomaticMode() {
    nextStateRobot = Operating(StateBinary());
    if (nextStateRobot != lastStateRobot) {
        lastStateRobot = stateRobot;
        stateRobot = nextStateRobot;
    } else {
        lastStateRobot = nextStateRobot;
    }
}

char Operating(unsigned char binary) {
    switch (binary) {

        case 0b00000:
            return (STATE_AVANCE);
            break;
        case 0b00001:
        case 0b00010:
        case 0b00011:
            return (STATE_TOURNE_GAUCHE);
            break;
        case 0b10000:
        case 0b01000:
        case 0b11000:
            return (STATE_TOURNE_DROITE);
            break;
        case 0b00100:
        case 0b00101:
        case 0b00110:
        case 0b00111:
        case 0b01001:
        case 0b01010:
        case 0b01011:
            return (STATE_TOURNE_SUR_PLACE_GAUCHE);
            break;
        case 0b10010:
        case 0b10011:
        case 0b10100:
        case 0b10101:
        case 0b10110:
        case 0b10111:
            return (STATE_TOURNE_SUR_PLACE_DROITE);
            break;
        case 0b10001:
        case 0b01100:
        case 0b01101:
        case 0b01110:
        case 0b01111:
        case 0b11001:
        case 0b11010:
        case 0b11011:
        case 0b11100:
        case 0b11101:
        case 0b11110:
        case 0b11111:
            return (STATE_TOURNE_SUR_PLACE_DROITE);
            break;
        default:
            return (STATE_ATTENTE);
            break;
    }
}

unsigned char StateBinary() {
    return (BinaryCapteur(robotState.distanceTelemetreExtGauche, DISTANCE_LIM_EXT) << 4 |
            BinaryCapteur(robotState.distanceTelemetreGauche, DISTANCE_LIM) << 3 |
            BinaryCapteur(robotState.distanceTelemetreCentre, DISTANCE_LIM_CENTRE) << 2 |
            BinaryCapteur(robotState.distanceTelemetreDroit, DISTANCE_LIM) << 1 |
            BinaryCapteur(robotState.distanceTelemetreExtDroit, DISTANCE_LIM_EXT));
}

unsigned char BinaryCapteur(float capteur, int val) {
    if (capteur < val)
        return (1);
    else
        return (0);
}

void Infrarouge_Conversion() {
    if (ADCIsConversionFinished() == 1) {
        ADCClearConversionFinishedFlag();
        unsigned int *result = ADCGetResult();
        robotState.distanceTelemetreExtGauche = 34 / (((float) result[0]) * 3.3 / 4096) - 5;
        robotState.distanceTelemetreGauche = 34 / (((float) result[1]) * 3.3 / 4096) - 5;
        robotState.distanceTelemetreCentre = 34 / (((float) result[2]) * 3.3 / 4096) - 5;
        robotState.distanceTelemetreDroit = 34 / (((float) result[3]) * 3.3 / 4096) - 5;
        robotState.distanceTelemetreExtDroit = 34 / (((float) result[4]) * 3.3 / 4096) - 5;
    }
}

