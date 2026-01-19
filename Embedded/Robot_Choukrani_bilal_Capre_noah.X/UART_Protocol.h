#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

#define RCV_STATE_WAITING 0
#define RCV_STATE_FUNCTION_MSB 1
#define RCV_STATE_FUNCTION_LSB 2
#define RCV_STATE_LENGTH_MSB 3
#define RCV_STATE_LENGTH_LSB 4
#define RCV_STATE_PAYLOAD 5
#define RCV_STATE_CHECKSUM 6

#define MSG_ROBOT_STATE  0x0050
#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052
#define SET_ROBOT_MOTOR 0x0040
#define SET_ROBOT_LED 0x0020

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(int function,int payloadLength, unsigned char* payload);
void UartSendRobotStep(unsigned char step);


#endif	/* UART_PROTOCOL_H */

