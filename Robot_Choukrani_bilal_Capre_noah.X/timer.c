#include <xc.h>
#include "timer.h"
#include "IO.h"
#include "pwm.h"
#include "ADC.h"
#include "main.h"
#include "ChipConfig.h"
//Initialisation d?un timer 16 bits
float freq = 2.5;
float freq4 = 1000.0;
unsigned long timestamp = 0;
unsigned long _millis = 0;

void InitTimer1(void) {
    //Timer1 pour horodater les mesures (1ms)
    SetFreqTimer1();
    T1CONbits.TON = 0; // Disable Timer
    //T1CONbits.TCKPS = 0b10; //Prescaler
    //11 = 1:256 prescale value
    //10 = 1:64 prescale value
    //01 = 1:8 prescale value
    //00 = 1:1 prescale value
    T1CONbits.TCS = 0; //clock source = internal clock
    //PR1 = 60000000/100/64; //493E pour 50 Hz et 2710 pour 6khz
    IFS0bits.T1IF = 0; // Clear Timer Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer interrupt
    T1CONbits.TON = 1; // Enable Timer
}
//Interruption du timer 1

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;
    InitADC1();
    PWMUpdateSpeed();

    //LED_BLEUE_1 = !LED_BLEUE_1;
}
//Initialisation d?un timer 32 bits

void InitTimer23(void) {
    T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
    T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
    T2CONbits.T32 = 1; // Enable 32-bit Timer mode
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR3 = 0x00; // Clear 32-bit Timer (msw)
    TMR2 = 0x00; // Clear 32-bit Timer (lsw)
    PR3 = 0x0727; // Load 32-bit period value (msw)
    PR2 = 0x0E00; // Load 32-bit period value (lsw)
    IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    T2CONbits.TON = 1; // Start 32-bit Timer

}

unsigned char toggle = 0;

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    //LED_ORANGE_1 = !LED_ORANGE_1;
    if (toggle == 0) {
        PWMSetSpeedConsigne(20, MOTEUR_DROIT);
        PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
        toggle = 1;
    } else {
        PWMSetSpeedConsigne(-20, MOTEUR_DROIT);
        PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
        toggle = 0;
    }

}

void SetFreqTimer1(void) {
    T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR1 = (int) (FCY / freq / 64);
        } else
            PR1 = (int) (FCY / freq / 8);
    } else
        PR1 = (int) (FCY / freq);
}

void InitTimer4(void) {
    //Timer1 pour horodater les mesures (1ms)
    SetFreqTimer4();
    T4CONbits.TON = 0; // Disable Timer
    T4CONbits.TCS = 0; //clock source = internal clock
    //PR1 = 60000000/100/64; //493E pour 50 Hz et 2710 pour 6khz
    IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
    IEC1bits.T4IE = 1; // Enable Timer interrupt
    T4CONbits.TON = 1; // Enable Timer
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    IFS1bits.T4IF = 0;
    timestamp++;
    _millis++;
    ADC1StartConversionSequence();
    infra_rouge();

    OperatingSystemLoop();




    //WMUpdateSpeed();
    //itADC1();
    //ED_BLANCHE_1 = !LED_BLANCHE_1;
}

void SetFreqTimer4(void) {
    T4CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq4 > 65535) {
        T4CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq4 / 8 > 65535) {
            T4CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq4 / 64 > 65535) {
                T4CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR4 = (int) (FCY / freq4 / 256);
            } else
                PR4 = (int) (FCY / freq4 / 64);
        } else
            PR4 = (int) (FCY / freq4 / 8);
    } else
        PR4 = (int) (FCY / freq4);
}










// timer en 16 bits permet pas de compter assez longtemps donc on prend 2 timer 16 bits et on les additionne pour avoir un 32 bits ce qui nous permettra avec les prescaler de comper plus longtemps