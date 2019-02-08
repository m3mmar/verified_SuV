#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "microvisor.h"

/************************ UART INIT CODE *************************/

#define BAUD 57600
#include <util/setbaud.h>
void uart_init(void) {
  UBRR1H = UBRRH_VALUE;
  UBRR1L = UBRRL_VALUE;
#if USE_2X
  UCSR1A |= _BV(U2X1);
#else
  UCSR1A &= ~(_BV(U2X1));
#endif
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);
  UCSR1B = _BV(TXEN1) | _BV(RXEN1);
}

void uart_putchar(char c) {
  loop_until_bit_is_set(UCSR1A, UDRE1);
  UDR1 = c;
}

void uart_puts(char *c) {
  while(*c) {
    if(*c == '\n')
      uart_putchar('\r');
    uart_putchar(*c++);
  }
}

/**************** READ AND WRITE TO EEPROM  ******************/
int main(void) {
  uint8_t wr_array[256];
  uint8_t rd_array[256];

  uart_init();
  uart_puts("eval_storage_process\n");

  /* MOSI and MISO pin */
  DDRB |= ( _BV(PB5) | _BV(PB6) );

  /* Generate some data */
  uint16_t i;
  for(i=0; i<256; i++){
    wr_array[i] = i+5;
  }

  while(1) {
    _delay_ms(5000);

    uart_puts("Start writing\n");
    PORTB = _BV(PB5);

    while(!eeprom_is_ready());
    eeprom_write_block(wr_array, (void *) 0x0000, 256);

    PORTB = 0x00;
    uart_puts("end  writing\n");

    _delay_ms(500);

    uart_puts("start reading\n");
    PORTB = _BV(PB5);

    while(!eeprom_is_ready());
    eeprom_read_block(rd_array, (void *) 0x0000, 256);

    PORTB = 0x00;
    uart_puts("end reading\n");
  }
}
