using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort_NS;
using System.IO.Ports;
using System.Windows.Threading;
using System.Collections;


namespace RobotInterface
{
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();
        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload = new byte[0];
        int msgDecodedPayloadIndex = 0;
        bool isAutomatiqueActive = false;
        private const int MaxQueueSize = 1000; // Limite de la taille de la file d'attente

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum,
        }

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ExtendedSerialPort("COM10", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }

        //////////////////////////////////////////////// PROGRAMMATION DES BOUTONS ////////////////////////////////////////////////

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            SendMessage();
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                SendMessage();

        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Text = "";
        }

        private void buttonAutomatique_Click(object sender, RoutedEventArgs e)
        {
            isAutomatiqueActive = !isAutomatiqueActive;
            if (isAutomatiqueActive)
            {
                buttonTest.Background = Brushes.Green;
            }
            else
            {
                buttonTest.Background = Brushes.Red;
            }
        }

        private void checkBoxLED_Checked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = sender as CheckBox;
            int ledNumber = int.Parse(checkBox.Tag.ToString());
            SetLEDState(ledNumber, true);
        }

        private void checkBoxLED_Unchecked(object sender, RoutedEventArgs e)
        {
            CheckBox checkBox = sender as CheckBox;
            int ledNumber = int.Parse(checkBox.Tag.ToString());
            SetLEDState(ledNumber, false);
        }

        private void SetLEDState(int ledNumber, bool state)
        {
            byte[] byteList = new byte[2];
            byteList[0] = (byte)ledNumber;
            if (state)
            {
                byteList[1] = 1;
            }
            else
            {
                byteList[1] = 0;
            }
            int msgFunction = 0x0020;
            UartEncodeAndSendMessage(msgFunction, 2, byteList);
        }

        private void ScrollToEnd()
        {
            textBoxReception.Text += Environment.NewLine;
            textBoxReception.ScrollToEnd();
        }

        //////////////////////////////////////////////// RECEPTION DES MESSAGES ////////////////////////////////////////////////
        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            if (robot.byteListReceived.Count > 0)
            {
                while (robot.byteListReceived.Count > 0)
                {
                    var c = robot.byteListReceived.Dequeue();
                    //textBoxReception.Text += Convert.ToChar(c);
                    //textBoxReception.Text += "0x" + c.ToString("X2") + " ";
                    DecodeMessage(c);
                }
            }
        }

        private async void SerialPort1_DataReceived(object? sender, DataReceivedArgs e)
        {
            for (int i = 0; i < e.Data.Length; i++)
            {
                if (robot.byteListReceived.Count < MaxQueueSize)
                {
                    robot.byteListReceived.Enqueue(e.Data[i]);
                }
                else
                {
                    break;
                }
            }
        }

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction |= c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength |= c;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    msgDecodedPayloadIndex = 0;
                    rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:
                    if (msgDecodedPayloadIndex < msgDecodedPayload.Length)
                    {
                        msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                        if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                        {
                            rcvState = StateReception.CheckSum;
                        }
                    }
                    else
                    {
                        rcvState = StateReception.Waiting;
                    }
                    break;

                case StateReception.CheckSum:
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    byte receivedChecksum = c;

                    if (calculatedChecksum == receivedChecksum)
                    {
                        ProcessMessage();
                    }
                    else
                    {
                        textBoxReception.Text += "Checksum error";
                        ScrollToEnd();
                    }

                    rcvState = StateReception.Waiting;
                    msgDecodedPayloadIndex = 0;
                    break;

                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        //////////////////////////////////////////////// ENVOIE DES MESSAGES ////////////////////////////////////////////////

        private void SendMessage()
        {
            string value = textBoxEmission.Text;
            textBoxEmission.Text = "";
            if (value.Length > 0)
            {
                value = value.Replace("\r", "").Replace("\n", "");
                string[] parts = value.Split('/');
                string message = parts[0];
                int msgFunction;

                if (parts.Length == 2)
                {
                    msgFunction = Convert.ToInt32(parts[1], 16);
                }
                else
                {
                    msgFunction = 0x0080;
                }

                byte[] byteList = Encoding.ASCII.GetBytes(message);
                int msgPayloadLength = byteList.Length;

                UartEncodeAndSendMessage(msgFunction, msgPayloadLength, byteList);
            }
        }

        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;

            checksum ^= 0xFE;
            checksum ^= 0x00;
            checksum ^= (byte)msgFunction;
            checksum ^= (byte)msgPayloadLength;

            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }
            return checksum;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] trame = new byte[msgPayloadLength + 6];

            trame[0] = 0xFE;
            trame[1] = 0x00;
            trame[2] = (byte)msgFunction;
            trame[3] = (byte)(msgPayloadLength >> 8);
            trame[4] = (byte)(msgPayloadLength & 0xFF);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                trame[i + 5] = msgPayload[i];
            }

            trame[msgPayloadLength + 5] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            serialPort1.Write(trame, 0, trame.Length);
        }

        void ProcessMessage()
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                switch (msgDecodedFunction)
                {
                    case 0x0080:
                        textBoxReception.Text += Encoding.ASCII.GetString(msgDecodedPayload);
                        ScrollToEnd();
                        break;



                    case 0x0030:
                        textBoxDistanceTelemetreExtGauche.Text = msgDecodedPayload[0] + " cm";
                        textBoxDistanceTelemetreGauche.Text = msgDecodedPayload[1] + " cm";
                        textBoxDistanceTelemetreCentre.Text = msgDecodedPayload[2] + " cm";
                        textBoxDistanceTelemetreDroit.Text = msgDecodedPayload[3] + " cm";
                        textBoxDistanceTelemetreExtDroit.Text = msgDecodedPayload[4] + " cm";
                        break;

                    case 0x0040:
                        textBoxValeurMoteurGauche.Text = ((sbyte)msgDecodedPayload[0]) + " %";
                        textBoxValeurMoteurDroit.Text = ((sbyte)msgDecodedPayload[1]) + " %";
                        break;

                    case 0x0051:
                        buttonTest.Background = Brushes.Green;
                        break;

                    case 0x0052:
                        buttonTest.Background = Brushes.Red;
                        break;

                    default:
                        textBoxReception.Text += Encoding.ASCII.GetString(msgDecodedPayload);
                        textBoxReception.Text += " / fonction inconnue: 0x" + msgDecodedFunction.ToString("X4");
                        ScrollToEnd();
                        break;
                }
            }));
        }
    }
}

