#ifndef ROBOT_H
#define ROBOT_H

void SetControlMotor(unsigned char* payload);

typedef struct robotStateBITS {

    union {

        struct {
            unsigned char taskEnCours;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;

            float distanceTelemetreExtDroit;
            float distanceTelemetreDroit;

            float distanceTelemetreExtGauche;
            float distanceTelemetreGauche;

            float distanceTelemetreCentre;

            int mode;

            double vitesseDroitFromOdometry;
            double vitesseGaucheFromOdometry;
            double vitesseLineaireFromOdometry;
            double vitesseAngulaireFromOdometry;
            double angleRadianFromOdometry;
            double xPosFromOdometry_1;
            double xPosFromOdometry;
            double yPosFromOdometry_1;
            double yPosFromOdometry;
            double angleRadianFromOdometry_1;
        };
    };
} ROBOT_STATE_BITS;

extern volatile ROBOT_STATE_BITS robotState;

#endif /* ROBOT_H */