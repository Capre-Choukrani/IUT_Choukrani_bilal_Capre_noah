#include <xc.h>
#include <stdint.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "PWM.h"
#include "IO.h"
#include "robot.h"
#include "timer.h"


int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
int rcvState = 0;
unsigned char calculatedChecksum;


unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    unsigned char checksum = 0;

    checksum ^= 0xFE;
    checksum ^= (unsigned char)(msgFunction >> 8);      // func MSB
    checksum ^= (unsigned char)(msgFunction & 0xFF);    // func LSB
    checksum ^= (unsigned char)(msgPayloadLength >> 8); // len MSB
    checksum ^= (unsigned char)(msgPayloadLength & 0xFF);// len LSB

    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }

    return checksum;
}


void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload)
{
    unsigned char trame[msgPayloadLength + 6];

    trame[0] = 0xFE;
    trame[1] = (unsigned char)(msgFunction >> 8);     // MSB
    trame[2] = (unsigned char)(msgFunction & 0xFF);   // LSB
    trame[3] = (unsigned char)(msgPayloadLength >> 8);
    trame[4] = (unsigned char)(msgPayloadLength & 0xFF);

    for (int i = 0; i < msgPayloadLength; i++) {
        trame[i + 5] = msgPayload[i];
    }

    trame[msgPayloadLength + 5] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

    SendMessage(trame, msgPayloadLength + 6);
}


void UartDecodeMessage(unsigned char c) {

    switch (rcvState) {
        case RCV_STATE_WAITING:
            if (c == 0xFE) rcvState = RCV_STATE_FUNCTION_MSB;
            msgDecodedPayloadLength = 0;
            msgDecodedPayloadIndex = 0;
            msgDecodedFunction = 0;
            break;

        case RCV_STATE_FUNCTION_MSB:
            msgDecodedFunction = c << 8;
            rcvState = RCV_STATE_FUNCTION_LSB;
            break;

        case RCV_STATE_FUNCTION_LSB:
            msgDecodedFunction |= c;
            rcvState = RCV_STATE_LENGTH_MSB;
            break;

        case RCV_STATE_LENGTH_MSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = RCV_STATE_LENGTH_LSB;
            break;

        case RCV_STATE_LENGTH_LSB:
            msgDecodedPayloadLength |= c;

            rcvState = RCV_STATE_PAYLOAD;
            break;

        case RCV_STATE_PAYLOAD:
            if (msgDecodedPayloadIndex < msgDecodedPayloadLength) {
                msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                    rcvState = RCV_STATE_CHECKSUM;
            }
            break;

        case RCV_STATE_CHECKSUM:
            if (UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload) == c) {
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = RCV_STATE_WAITING;
            break;

        default:
            rcvState = RCV_STATE_WAITING;
            break;
    }
}

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload) {

    switch (function) {

        case SET_ROBOT_STATE:
    if (payloadLength >= 1) {
        SetRobotState(payload[0]);   // force l'état demandé
    }
    break;

case SET_ROBOT_MANUAL_CONTROL:
    if (payloadLength >= 1) {
        SetRobotAutoControlState(payload[0]); // 0 = manuel, 1 = auto
    }
    break;


        case SET_ROBOT_MOTOR:
            // généralement autorisé seulement si manuel
            if (payloadLength >= 2 && robotState.mode == 0) {
                PWMSetSpeedConsigne((int8_t) payload[0], MOTEUR_DROIT);
                PWMSetSpeedConsigne((int8_t) payload[1], MOTEUR_GAUCHE);
            }
            break;

        case SET_ROBOT_LED:
            if (payloadLength >= 5 && robotState.mode == 0) {
                LED_BLANCHE_1 = payload[0];
                LED_BLEUE_1   = payload[1];
                LED_ORANGE_1  = payload[2];
                LED_ROUGE_1   = payload[3];
                LED_VERTE_1   = payload[4];
            }
            break;

        default:
            break;
    }
}
void UartSendRobotStep(unsigned char step)
{
    unsigned char payload[5];
    uint32_t t = _millis;

    payload[0] = step;
    payload[1] = (unsigned char)(t >> 24);
    payload[2] = (unsigned char)(t >> 16);
    payload[3] = (unsigned char)(t >> 8);
    payload[4] = (unsigned char)(t & 0xFF);

    UartEncodeAndSendMessage(MSG_ROBOT_STATE, 5, payload);
}


