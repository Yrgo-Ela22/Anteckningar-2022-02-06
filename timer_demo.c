/********************************************************************************
* timer_demo.c: Demonstration av timerkretsar Timer 0 - Timer 2 i C. 
*               Tre lysdioder anslutna till pin 8 - 10 (PORTB0 - PORTB2) togglas 
*               via var sin timerkrets. Varje timerkrets genererar ett avbrott 
*               var 16.384:e ms.
*
*               LED1 ansluten till (PORTB0) togglas var 100:e ms via Timer 0.
*               LED2 ansluten till (PORTB1) togglas var 200:e ms via Timer 1.
*               LED3 ansluten till (PORTB2) togglas var 300:e ms via Timer 2.
*
*               Eftersom ett timeravbrott sker var 16.384:e ms r�knas antalet
*               avbrott N som kr�vs f�r en viss f�rdr�jningstid T enligt nedan:
*
*                                      N = T / 16.384,
*
*               d�r resultatet avrundas till n�rmaste heltal.
*
*               Vi har en klockfrekvens F_CPU p� 16 MHz. Vi anv�nder en 
*               prescaler p� 1024 f�r timerkretsarna, vilket medf�r att 
*               uppr�kningsfrekvensen f�r respektive timerkrets blir 
*               16M / 1024 = 15 625 Hz. D�rmed sker inkrementering av respektive 
*               timer var 1 / 15 625 = 0.064:e ms. Eftersom varje timerkrets 
*               r�knar upp till 256 innan avbrott passerar 0.064 * 256 ms = 
*               16.384 ms mellan varje avbrott.
********************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

/* Makrodefinitioner: */
#define LED1 PORTB0 /* Lysdiod 1 ansluten till pin 8 (PORTB0). */
#define LED2 PORTB1 /* Lysdiod 2 ansluten till pin 9 (PORTB1). */
#define LED3 PORTB2 /* Lysdiod 3 ansluten till pin 10 (PORTB2). */

#define LED1_TOGGLE PINB = (1 << LED1) /* Togglar lysdiod 1. */
#define LED2_TOGGLE PINB = (1 << LED2) /* Togglar lysdiod 2. */
#define LED3_TOGGLE PINB = (1 << LED3) /* Togglar lysdiod 3. */

#define TIMER0_MAX_COUNT 6  /* 6 timeravbrott f�r 100 ms f�rdr�jning. */
#define TIMER1_MAX_COUNT 12 /* 12 timeravbrott f�r 200 ms f�rdr�jning. */
#define TIMER2_MAX_COUNT 18 /* 18 timeravbrott f�r 300 ms f�rdr�jning. */

/********************************************************************************
* ISR: Avbrottsrutin f�r Timer 0 i Normal Mode, som �ger rum var 16.384:e ms
*      vid overflow (uppr�kning till 256, d� r�knaren blir �verfull).
*      Ungef�r var 100:e ms (var 6:e avbrott) togglas lysdiod LED1.
*
*      - TIMER0_OVF_vect: Avbrottsvektor 0x20 f�r Timer 0 i Normal Mode.
********************************************************************************/
ISR (TIMER0_OVF_vect)
{
   static volatile uint8_t counter0 = 0;

   if (++counter0 >= TIMER0_MAX_COUNT)
   {
      LED1_TOGGLE;
      counter0 = 0;
   }
   return;
}

/********************************************************************************
* ISR: Avbrottsrutin f�r Timer 1 i CTC Mode, som �ger rum var 16.384:e ms
*      vid uppr�kning till 256, som har s�tts till maxv�rde f�r uppr�kning.
*      Ungef�r var 200:e ms (var 12:e avbrott) togglas lysdiod LED2.
*
*      - TIMER1_COMPA_vect: Avbrottsvektor 0x16 f�r Timer 1 i CTC Mode.
********************************************************************************/
ISR (TIMER1_COMPA_vect)
{
   static volatile uint8_t counter1 = 0;

   if (++counter1 >= TIMER1_MAX_COUNT)
   {
      LED2_TOGGLE;
      counter1 = 0;
   }
   return;
}

/********************************************************************************
* ISR: Avbrottsrutin f�r Timer 2 i Normal Mode, som �ger rum var 16.384:e ms
*      vid overflow (uppr�kning till 256, d� r�knaren blir �verfull).
*      Ungef�r var 300:e ms (var 18:e avbrott) togglas lysdiod LED3.
*
*      - TIMER2_OVF_vect: Avbrottsvektor 0x12 f�r Timer 2 i Normal Mode.
********************************************************************************/
ISR (TIMER2_OVF_vect)
{
   static volatile uint8_t counter2 = 0;

   if (++counter2 >= TIMER2_MAX_COUNT)
   {
      LED3_TOGGLE;
      counter2 = 0;
   }
   return;
}

/********************************************************************************
* setup: S�tter lysdiodernas pinnar till utportar samt aktiverar timerkretsarna
*        s� att avbrott sker var 16.384:e millisekund f�r respektive timer.
********************************************************************************/
static inline void setup(void)
{
   DDRB = (1 << LED1) | (1 << LED2) | (1 << LED3);

   TCCR0B = (1 << CS02) | (1 << CS00);
   TIMSK0 = (1 << TOIE0);

   TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12);
   OCR1A = 256;
   TIMSK1 = (1 << OCIE1A);

   TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
   TIMSK2 = (1 << TOIE2);

   asm("SEI");
   return;
}

/********************************************************************************
* main: Initierar systemet vid start. Programmet h�lls sedan ig�ng s� l�nge
*       matningssp�nning tillf�rs.
********************************************************************************/
int main(void)
{
   setup();

   while (1)
   {

   }

   return 0;
}

