/*
 By Liyanboy74
 https://github.com/liyanboy74
*/

#include <mega8.h>
#include <delay.h>
#include <stdio.h>

#define Button      PINB.0
#define Data_In     PINB.1
#define LED_Port    PORTC.5
#define SignalTest  PORTB.3
#define Buffer_Size 900

#define ON  1
#define OFF 0

unsigned char Buffer[Buffer_Size];
unsigned int Index=0,BitIndex=0,BitBuffer=0;

void GetSample()
{
  BitBuffer|=(Data_In<<BitIndex);

  BitIndex++;
  if(BitIndex>=8)
  {
    BitIndex=0;
    if(Index<Buffer_Size)
    {
      Buffer[Index]=BitBuffer;
      Index++; 
      BitBuffer=0;
    }
  }
}

// Timer1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
  // Reinitialize Timer1 value
  TCNT1H=0xFCE0 >> 8;
  TCNT1L=0xFCE0 & 0xff;

  // Get Sample
  GetSample();
}

// Timer2 overflow interrupt service routine
interrupt [TIM2_OVF] void timer2_ovf_isr(void)
{
  static int i=1;

  // Reinitialize Timer2 value
  TCNT2=0x9C;

  // Test Signal
  if(i)
  {
    SignalTest=1;
    i=0;   
  }
  else
  {
    SignalTest=0;
    i=1;
  }
}

unsigned char CheckFullBuffer()
{
  if(Index<Buffer_Size)return 0;
  else return 1;
}

void ClearBuffer()
{
  Index=0;  
  BitIndex=0;
  BitBuffer=0;
}

void TransferBuffer()
{
  int i,j;
  for(i=0;i<Buffer_Size;i++)
  {
    for(j=0;j<8;j++)
    {
      printf("%d\n",(Buffer[i]>>j)&1); 
    }
  }
}

void main(void)
{
  // Input/Output Ports initialization
  DDRB = (1<<DDB3); 
  PORTB= (1<<PORTB0);
  DDRC = (1<<DDC5);
  PORTC= (0<<0);
  DDRD = (1<<DDD1);
  PORTD= (1<<PORTD2)| (1<<PORTD0);

  // Timer/Counter 1 initialization
  // Clock source: System Clock
  // Clock value: 8000/000 kHz
  // Mode: Normal top=0xFFFF
  // OC1A output: Disconnected
  // OC1B output: Disconnected
  // Noise Canceler: Off
  // Input Capture on Falling Edge
  // Timer Period: 0/1 ms
  // Timer1 Overflow Interrupt: On
  // Input Capture Interrupt: Off
  // Compare A Match Interrupt: Off
  // Compare B Match Interrupt: Off
  TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
  TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
  TCNT1H=0xFC;
  TCNT1L=0xE0;
  ICR1H=0x00;
  ICR1L=0x00;
  OCR1AH=0x00;
  OCR1AL=0x00;
  OCR1BH=0x00;
  OCR1BL=0x00;

  // Timer/Counter 2 initialization
  // Clock source: System Clock
  // Clock value: 250/000 kHz
  // Mode: Normal top=0xFF
  // OC2 output: Disconnected
  // Timer Period: 0/4 ms
  ASSR=0<<AS2;
  TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (1<<CS21) | (1<<CS20);
  TCNT2=0x9C;
  OCR2=0x00;

  // Timer(s)/Counter(s) Interrupt(s) initialization
  TIMSK=(0<<OCIE2) | (1<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (1<<TOIE1) | (0<<TOIE0);

  // USART initialization
  // Communication Parameters: 8 Data, 1 Stop, No Parity
  // USART Receiver: On
  // USART Transmitter: On
  // USART Mode: Asynchronous
  // USART Baud Rate: 9600
  UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
  UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
  UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
  UBRRH=0x00;
  UBRRL=0x33;

  // Global enable interrupts
  #asm("sei")

  while (1)
  {
    LED_Port=OFF;
    while(Button);
    LED_Port=ON;

    ClearBuffer();

    while(!CheckFullBuffer())
    {
      delay_ms(10);
    }

    LED_Port=OFF;
    while(!Button); 
    delay_ms(50);

    while(Button); 
    LED_Port=ON; 
    TransferBuffer();
    while(!Button);
    delay_ms(50);
  }
}
