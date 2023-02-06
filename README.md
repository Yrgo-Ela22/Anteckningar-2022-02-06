# Anteckningar 2022-02-06
Avbrottsbaserad timerimplementering av Timer 0 - Timer 2 för mikrodator ATmega328P i AVR assembler
samt slutförande av stackimplementering för CPU-emulator.

Tre lysdioder anslutna till pin 8 - 10 (PORTB0 - PORTB2) togglas via var sin timerkrets. 
LED1 ansluten till pin 8 (PORTB0) togglas var 100:e ms via Timer 0.
LED2 ansluten till pin 9 (PORTB1) togglas var 200:e ms via Timer 1.
LED3 ansluten till pin 10 (PORTB2) togglas var 500:e ms via Timer 2.

Följande avbrottsvektorer används:
TIMER0_OVF_vect (0x20)  : Avbrottsvektor för Timer 0 i Normal Mode.
TIMER1_COMPA_vect (0x16): Avbrottsvektor för Timer 1 i CTC Mode.
TIMER2_OVF_vect (0x12)  : Avbrottsvektor för Timer 2 i Normal Mode.

Varje timerkrets genererar ett avbrott var 16.384:e ms. 

I filen "timer_demo.asm" implementeras systemet i assembler. 
I filen "timer_demo.c" demonstreras motsvarande C-program.

Övriga filer utgörs av CPU-emulatorn med slutförd stackimplementering, 
där instruktioner CALL, RET, PUSH samt POP, som alla använder sig av stacken, har realiserats.
I filen "program_memory.c". skrevs ett program innehållande ett flertal funktionsanrop samt 
återhopp för test av stacken. 

