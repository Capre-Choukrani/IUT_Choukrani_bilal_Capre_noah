/* 
 * File:   CB_TX1.h
 * Author: E306-PC7
 *
 * Created on 5 décembre 2025, 14:31
 */

#ifndef CB_TX1_H
#define	CB_TX1_H
void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void);
void SendOne();
unsigned char CB_TX1_IsTranmitting(void);
int CB_TX1_GetDataSize(void);
int CB_TX1_GetRemainingSize(void);
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* CB_TX1_H */

