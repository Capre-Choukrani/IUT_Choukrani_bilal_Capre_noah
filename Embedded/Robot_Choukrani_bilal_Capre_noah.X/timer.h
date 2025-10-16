#ifndef TIMER_H
#define TIMER_H

    void InitTimer23();
    void InitTimer1();
    void SetFreqTimer1(float freq);
    void InitTimer4();
    void SetFreqTimer4(float freq);
    
    extern unsigned long timestamp;
    extern unsigned long tempAction;
    extern unsigned long _millis;
    
#endif /* TIMER_H */