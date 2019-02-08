#include <avr/pgmspace.h>
#include <avr/io.h>

#define BAUD 57600
#include <util/setbaud.h>

#include "microvisor.h"

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

char uart_getchar() {
  char c;
  loop_until_bit_is_set(UCSR1A, RXC1);
  c = UDR1;
  return c;
}

void uart_putchar(char c) {
  if (c == '\n') {
    uart_putchar('\r');
  }
  loop_until_bit_is_set(UCSR1A, UDRE1);
  UDR1 = c;
}

void uart_puts(char *c) {
  while(*c) {
    uart_putchar(*c++);
  }
}

int main(void) {
  uart_init();
  uart_puts("test\n");

  // Never return from main()
  while(1);
}
