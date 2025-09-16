

#ifndef ADC_H
#define	ADC_H
void InitADC1(void);
void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void);
void ADC1StartConversionSequence();


#endif	/* ADC_H */

