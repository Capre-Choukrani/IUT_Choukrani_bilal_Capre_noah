#include <xc.h>      // OBLIGATOIRE pour les registres (QEI1..., POS1...)
#include <math.h>
#include "robot.h"   // pour robotState
#include "QEI.h"     // si tu as un QEI.h, sinon retire

#define PI 3.14159265358979323846

// Écartement entre les roues (à la bonne unité !)
#define DISTROUES 0.2812

// Fréquence d'échantillonnage du QEI (Hz)
#define FREQ_ECH_QEI 100.0


// Positions roues (distance totale)
double QeiDroitPosition = 0.0;
double QeiGauchePosition = 0.0;

// Positions précédentes (t-1)
double QeiDroitPosition_T_1 = 0.0;
double QeiGauchePosition_T_1 = 0.0;

// Deltas de déplacement (distance sur l'échantillon)
double delta_d = 0.0;
double delta_g = 0.0;


void InitQEI1()
{
QEI1IOCbits.SWPAB = 1; //QEAx and QEBx are swapped
QEI1GECL = 0xFFFF;
QEI1GECH = 0xFFFF;
QEI1CONbits.QEIEN = 1; // Enable QEI Module
}
void InitQEI2(){
QEI2IOCbits.SWPAB = 1; //QEAx and QEBx are not swapped
QEI2GECL = 0xFFFF;
QEI2GECH = 0xFFFF;
QEI2CONbits.QEIEN = 1; // Enable QEI Module
}


void QEIUpdateData()
{
    // On sauvegarde les anciennes valeurs
    QeiDroitPosition_T_1  = QeiDroitPosition;
    QeiGauchePosition_T_1 = QeiGauchePosition;

    // On actualise les valeurs des positions
    long QEI1RawValue = POS1CNTL;
    QEI1RawValue += ((long)POS1HLD << 16);

    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long)POS2HLD << 16);

    // Conversion en distance (m ou mm, selon ta constante)
    QeiDroitPosition  =  0.00001620 * QEI1RawValue;
    QeiGauchePosition = -0.00001620 * QEI2RawValue;

    // Calcul des deltas de position
    delta_d = QeiDroitPosition  - QeiDroitPosition_T_1;
    delta_g = QeiGauchePosition - QeiGauchePosition_T_1;

    // Vitesses (delta * fréquence d?échantillonnage)
    robotState.vitesseDroitFromOdometry   = delta_d * FREQ_ECH_QEI;
    robotState.vitesseGaucheFromOdometry  = delta_g * FREQ_ECH_QEI;
    robotState.vitesseLineaireFromOdometry  = (robotState.vitesseDroitFromOdometry + robotState.vitesseGaucheFromOdometry) / 2.0;
    robotState.vitesseAngulaireFromOdometry = (robotState.vitesseDroitFromOdometry - robotState.vitesseGaucheFromOdometry) / DISTROUES;

    // Mise à jour du positionnement terrain à t-1
    robotState.xPosFromOdometry_1       = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1       = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 = robotState.angleRadianFromOdometry;

    // Odométrie (mise à jour pose)
    double dtheta = (delta_d - delta_g) / DISTROUES;
    double ds     = (delta_d + delta_g) / 2.0;
    double theta  = robotState.angleRadianFromOdometry_1;

    robotState.xPosFromOdometry     = robotState.xPosFromOdometry_1 + ds * cos(theta + dtheta / 2.0);
    robotState.yPosFromOdometry     = robotState.yPosFromOdometry_1 + ds * sin(theta + dtheta / 2.0);
    robotState.angleRadianFromOdometry = theta + dtheta;

    // Normalisation de l?angle entre [-PI ; PI]
    if (robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2.0 * PI;
    if (robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry += 2.0 * PI;
}
