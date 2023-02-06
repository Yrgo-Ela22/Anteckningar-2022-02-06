/********************************************************************************
* program_memory.c: Contains function definitions and macro definitions for
*                   implementation of a 6 kB program memory, capable of storing
*                   up to 256 24-bit instructions. Since C doesn't support
*                   unsigned 24-bit integers (without using structs or unions),
*                   the program memory is set to 32 bits data width, but only
*                   24 bits are used.
********************************************************************************/
#include "program_memory.h"

/* Macro definitions: */
#define main           1  /* Start address for subroutine main. */
#define main_loop      2  /* Start address for loop in subroutine main. */
#define led_blink      4  /* Start address for subroutine led_blink */
#define setup          9  /* Start address for subroutine setup. */
#define init_ports     12 /* Start address for subroutine init_ports. */
#define init_registers 15 /* Start address for subroutine init_register. */
#define end            19 /* End address for current program. */

#define LED1 PORTB0 /* LED 1 connected to pin 8 (PORTB0). */
#define LED2 PORTB1 /* LED 2 connected to pin 9 (PORTB1). */
#define LED3 PORTB2 /* LED 3 connected to pin 10 (PORTB2). */

/* Static functions: */
static inline uint32_t assemble(const uint8_t op_code,
                                const uint8_t op1,
                                const uint8_t op2);

/********************************************************************************
* data: Program memory with capacity for storing 256 instructions.
********************************************************************************/
static uint32_t data[PROGRAM_MEMORY_ADDRESS_WIDTH];

/********************************************************************************
* program_memory_write: Writes machine code to the program memory. This function
*                       should be called once when the program starts.
********************************************************************************/
void program_memory_write(void)
{
   static bool program_memory_initialized = false;
   if (program_memory_initialized) return;

   /********************************************************************************
   * RESET_vect: Reset vector and start address for the program. A jump is made
   *             to the main subroutine in order to start the program.
   ********************************************************************************/
   data[0]  = assemble(JMP, main, 0x00); /* JMP main */

   /********************************************************************************
   * main: Initiates the system at start. The program is kept running as long
   *       as voltage is supplied. The leds connected to PORTB0 - PORTB2 are
   *       blinkning continuously. Values for enabling each LED is stored in
   *       CPU registers R16 - R18 for direct write to data register PORTB.
   ********************************************************************************/
   data[1]  = assemble(CALL, setup, 0x00); /* CALL setup */

   /********************************************************************************
   * main_loop: Blinks the leds in a loop continuously.
   ********************************************************************************/
   data[2]  = assemble(CALL, led_blink, 0x00); /* CALL led_blink */
   data[3]  = assemble(JMP, main_loop, 0x00);  /* JMP main_loop */

   /********************************************************************************
   * led_blink: Blinks leds connected to PORTB0 - PORTB2 in a sequence.
   ********************************************************************************/
   data[4]  = assemble(OUT, PORTB, R16); /* OUT PORTB, R16 */
   data[5]  = assemble(OUT, PORTB, R17); /* OUT PORTB, R17 */
   data[6]  = assemble(OUT, PORTB, R18); /* OUT PORTB, R18 */
   data[7]  = assemble(OUT, PORTB, R19); /* OUT PORTB, R19 */
   data[8]  = assemble(RET, 0x00, 0x00); /* RET */

   /********************************************************************************
   * setup: Initiates I/O-ports and CPU registers.
   ********************************************************************************/
   data[9]  = assemble(CALL, init_ports, 0x00);     /* CALL init_ports */
   data[10] = assemble(CALL, init_registers, 0x00); /* CALL init_registers */
   data[11] = assemble(RET, 0x00, 0x00);            /* RET */

   /********************************************************************************
   * init_ports: Sets PORTB0 - PORTB2 to outputs and enables the internal 
   *             pullup-resistor of PORTB5.
   ********************************************************************************/
   data[12] = assemble(LDI, R16, (1 << LED1) | (1 << LED2) | (1 << LED3)); 
   data[13] = assemble(OUT, DDRB, R16);  /* OUT DDRB, R16 */
   data[14] = assemble(RET, 0x00, 0x00); /* RET */

   /********************************************************************************
   * init_registers: Stores values for toggling leds in R16 - R18.
   ********************************************************************************/
   data[15] = assemble(LDI, R16, (1 << LED1)); /* LDI R16, (1 << LED1) */
   data[16] = assemble(LDI, R17, (1 << LED2)); /* LDI R17, (1 << LED2) */
   data[17] = assemble(LDI, R18, (1 << LED3)); /* LDI R18, (1 << LED3) */
   data[18] = assemble(RET, 0x00, 0x00);       /* RET */

   program_memory_initialized = true;
   return;
}

/********************************************************************************
* program_memory_read: Returns the instruction at specified address. If an
*                      invalid address is specified (should be impossible as
*                      long as the program memory address width isn't increased)
*                      no operation (0x00) is returned.
*
*                      - address: Address to instruction in program memory.
********************************************************************************/
uint32_t program_memory_read(const uint8_t address)
{
   if (address < PROGRAM_MEMORY_ADDRESS_WIDTH)
   {
      return data[address];
   }
   else
   {
      return 0x00;
   }
}

/********************************************************************************
* program_memory_subroutine_name: Returns the name of the subroutine at
*                                 specified address.
*
*                                 - address: Address within the subroutine.
********************************************************************************/
const char* program_memory_subroutine_name(const uint8_t address)
{
   if (address >= RESET_vect && address < main)                return "RESET_vect";
   else if (address >= main && address < led_blink)            return "main";
   else if (address >= led_blink && address < setup)           return "led_blink";
   else if (address >= setup && address < init_ports)          return "setup";
   else if (address >= init_ports && address < init_registers) return "init_ports";
   else if (address >= init_registers && address < end)        return "init_registers";
   else                                                        return "Unknown";
}

/********************************************************************************
* assemble: Returns instruction assembled to machine code.
********************************************************************************/
static inline uint32_t assemble(const uint8_t op_code,
                                const uint8_t op1,
                                const uint8_t op2)
{
   const uint32_t instruction = (op_code << 16) | (op1 << 8) | op2;
   return instruction;
}