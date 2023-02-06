;********************************************************************************
; timer_demo.asm: Togglar lysdioder anslutna till pin 8 - 10 (PORTB0 - PORTB2)
;                 via var sin timerkrets:
;
;                 - Timer 0 togglar LED1 ansluten till PORTB0 var 100:e ms.
;                 - Timer 1 togglar LED2 ansluten till PORTB1 var 200:e ms.
;                 - Timer 2 togglas LED3 ansluten till PORTB2 var 300:e ms.
;
;                 Vi anv�nder en prescaler p� 1024 f�r respektive timerkrets,
;                 vilket medf�r timeravbrott var 16.384:e ms. F�r att r�kna
;                 ut antalet avbrott N som kr�vs f�r en viss f�rdr�jning T
;                 kan f�ljande formel anv�ndas:
;
;                                       N = T / 16.384,
;
;                 d�r T �r f�rdr�jningstiden i ms.
;
;                 D�rmed kr�vs ca 100 / 16.384 = 6 avbrott f�r 100 ms, 
;                              ca 200 / 16.384 = 12 avbrott f�r 200 ms och 
;                              ca 300 / 16.384 = 18 avbrott f�r 300 ms.               
;********************************************************************************
.EQU LED1 = PORTB0            ; LED1 ansluten till pin 8 (PORTB0).
.EQU LED2 = PORTB1            ; LED2 ansluten till pin 9 (PORTB1).
.EQU LED3 = PORTB2            ; LED3 ansluten till pin 10 (PORT2).

.EQU TIMER0_MAX_COUNT = 6     ; Motsvarar ca 100 ms f�rdr�jning.
.EQU TIMER1_MAX_COUNT = 12    ; Motsvarar ca 200 ms f�rdr�jning.
.EQU TIMER2_MAX_COUNT = 18    ; Motsvarar ca 300 ms f�rdr�jning.

.EQU RESET_vect        = 0x00 ; Reset-vektor, programmets startpunkt.
.EQU TIMER0_OVF_vect   = 0x20 ; Avbrottsvektor f�r Timer 0 i Normal Mode.
.EQU TIMER1_COMPA_vect = 0x16 ; Avbrottsvektor f�r Timer 1 i CTC Mode.
.EQU TIMER2_OVF_vect   = 0x12 ; Avbrottsvektor f�r Timer 2 i Normal Mode.

;********************************************************************************
; .DSEG: Dataminnet, h�r lagras statiska variabler. F�r att allokera minne f�r
;        en variabel anv�nds f�ljande syntax:
;
;        variabelnamn: .datatyp antal_byte
;********************************************************************************
.DSEG
.ORG SRAM_START
counter0: .byte 1 ; static uint8_t counter0 = 0;
counter1: .byte 1 ; static uint8_t counter1 = 0;
counter2: .byte 1 ; static uint8_t counter2 = 0;

;********************************************************************************
; .CSEG: Programminnet - H�r lagrar programkoden.
;********************************************************************************
.CSEG

;********************************************************************************
; RESET_vect: Programmets startpunkt. Programhopp sker till subrutinen main
;             f�r att starta programmet.
;********************************************************************************
.ORG RESET_vect
   RJMP main

;********************************************************************************
; TIMER2_OVF_vect: Avbrottsvektor f�r overflow-avbrott p� Timer 2, vilket sker
;                  var 16.384:e ms. Programhopp sker till motsvarande 
;                  avbrottsrutin ISR_TIMER2_OVF f�r att hantera avbrottet.
;********************************************************************************
.ORG TIMER2_OVF_vect
   RJMP ISR_TIMER2_OVF

;********************************************************************************
; TIMER1_COMPA_vect: Avbrottsvektor f�r CTC-avbrott p� Timer 1, vilket sker
;                    var 16.384:e ms. Programhopp sker till motsvarande 
;                    avbrottsrutin ISR_TIMER1_COMPA f�r att hantera avbrottet.
;********************************************************************************
.ORG TIMER1_COMPA_vect
   RJMP ISR_TIMER1_COMPA

;********************************************************************************
; TIMER0_OVF_vect: Avbrottsvektor f�r overflow-avbrott p� Timer 0, vilket sker
;                  var 16.384:e ms. Programhopp sker till motsvarande 
;                  avbrottsrutin ISR_TIMER0_OVF f�r att hantera avbrottet.
;********************************************************************************
.ORG TIMER0_OVF_vect
   RJMP ISR_TIMER0_OVF

;********************************************************************************
; ISR_TIMER0_OVF: Avbrottsrutin f�r overflow-avbrott p� Timer 0, vilket sker
;                 var 16.384:e ms. Var 6:e avbrott (var 100:e ms) togglas LED1.
;
;                 Den statiska variabeln counter0 l�ses in fr�n dataminnet,
;                 inkrementeras och j�mf�rs sedan med TIMER0_MAX_COUNT.
;
;                 Om counter0 �r st�rre eller lika med TIMER0_MAX_COUNT togglas 
;                 LED1 f�ljt av att counter0 nollst�lls. Innan avbrottsrutinen
;                 avslutas skrivs det uppdaterade v�rdet p� counter0 tillbaka
;                 till dataminnet.
;********************************************************************************
ISR_TIMER0_OVF:
   LDS R24, counter0         
   INC R24                   
   CPI R24, TIMER0_MAX_COUNT 
   BRLO ISR_TIMER0_OVF_end   
   OUT PINB, R16             
   CLR R24                   
ISR_TIMER0_OVF_end:
   STS counter0, R24         
   RETI                      

;********************************************************************************
; ISR_TIMER1_COMPA: Avbrottsrutin f�r CTC-avbrott p� Timer 1, vilket sker var
;                   16.384:e ms. Var 12:e avbrott (var 200:e ms) togglas LED2.
;
;                   Den statiska variabeln counter1 l�ses in fr�n dataminnet,
;                   inkrementeras och j�mf�rs sedan med TIMER1_MAX_COUNT.
;
;                   Om counter1 �r st�rre eller lika med TIMER1_MAX_COUNT togglas 
;                   LED2 f�ljt av att counter1 nollst�lls. Innan avbrottsrutinen
;                   avslutas skrivs det uppdaterade v�rdet p� counter1 tillbaka
;                   till dataminnet.
;********************************************************************************
ISR_TIMER1_COMPA:
   LDS R24, counter1         
   INC R24                   
   CPI R24, TIMER1_MAX_COUNT
   BRLO ISR_TIMER1_COMPA_end 
   OUT PINB, R17             
   CLR R24                  
ISR_TIMER1_COMPA_end:
   STS counter1, R24        
   RETI                     

;********************************************************************************
; ISR_TIMER2_OVF: Avbrottsrutin f�r overflow-avbrott p� Timer 2, vilket sker
;                 var 16.384:e ms. Var 18:e avbrott (var 300:e ms) togglas LED3.
;
;                 Den statiska variabeln counter2 l�ses in fr�n dataminnet,
;                 inkrementeras och j�mf�rs sedan med TIMER2_MAX_COUNT.
;
;                 Om counter2 �r st�rre eller lika med TIMER2_MAX_COUNT togglas 
;                 LED1 f�ljt av att counter2 nollst�lls. Innan avbrottsrutinen
;                 avslutas skrivs det uppdaterade v�rdet p� counter2 tillbaka
;                 till dataminnet.
;********************************************************************************
ISR_TIMER2_OVF:
   LDS R24, counter2         
   INC R24                  
   CPI R24, TIMER2_MAX_COUNT 
   BRLO ISR_TIMER2_OVF_end   
   OUT PINB, R18             
   CLR R24                   
ISR_TIMER2_OVF_end:
   STS counter2, R24       
   RETI                 

;********************************************************************************
; main: Initierar systemet vid start. Programmet h�lls sedan ig�ng s� l�nge
;       matningssp�nning tillf�rs.
;********************************************************************************b
main:
   CALL setup
main_loop:
   RJMP main_loop

;********************************************************************************
; setup: Initierar I/O-portar samt aktiverar timerkretsar Timer 0 - Timer 2 s�
;        att timeravbrott sker var 16.384:e ms per timer.
;
;        F�rst s�tts lysdiodernas pinnar PORTB0 - PORTB2 till utportar. 
;        D�refter sparas v�rden i CPU-register R16 - R18 f�r att enkelt toggla
;        de enskilda lysdioderna. 
;
;        Sedan aktiveras avbrott globalt, f�ljt av av 8-bitars timerkretsar 
;        Timer 0 och Timer 2 aktiveras i Normal Mode med en prescaler p� 1024, 
;        vilket medf�r avbrott var 16.384:e ms vid overflow (uppr�kning till 256). 
;
;        F�r att den 16-bitars timerkretsen Timer 1 ska fungera likartat st�lls 
;        denna in i CTC Mode, d�r maxv�rdet s�tts till 256 genom att skriva 
;        detta v�rde till det 16-bitars registret OCR1A. Prescalern s�tts till
;        1024 �ven f�r denna timerkrets, s� att CTC-avbrott sker var 16.384:e ms 
;        efter uppr�kning till 256.
;********************************************************************************
setup:
   LDI R16, (1 << LED1) | (1 << LED2) | (1 << LED3)
   OUT DDRB, R16 
init_registers:
   LDI R16, (1 << LED1) 
   LDI R17, (1 << LED2) 
   LDI R18, (1 << LED3) 
init_timer0:
   SEI
   LDI R24, (1 << CS02) | (1 << CS00) 
   OUT TCCR0B, R24    
   STS TIMSK0, R16   
init_timer2:
   LDI R24, (1 << CS22) | (1 << CS21) | (1 << CS20)
   STS TCCR2B, R24   
   STS TIMSK2, R16   
init_timer1:
   LDI R24, (1 << WGM12) | (1 << CS12) | (1 << CS10)
   STS TCCR1B, R24    
   LDI R25, high(256) 
   LDI R24, low(256)  
   STS OCR1AH, R25   
   STS OCR1AL, R24  
   STS TIMSK1, R17    
   RET