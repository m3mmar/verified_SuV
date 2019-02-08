#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "microvisor.h"
#include "twi_master.h"

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

/**************** SAMPLE SENSIRION SHT25 TEMP/HUM SENSOR ******************/
#define TEMP_SLA_ADDR 0x40

int main(void) {
  uint8_t buf[2];
  uint32_t conv_buf;
  int16_t temp;

  uart_init();
  uart_puts("eval_sensor_process\n");

  /* Setup I2C On my specific demo board, PB4 needs to be high in order to use
   * I2C. Not required for all 1284p boards. */
  DDRB |= _BV(PB4);
  PORTB |= _BV(PB4);
  sei();
  twi_init();

  /* MOSI and MISO pin */
  DDRB |= ( _BV(PB5) | _BV(PB6) );

  while(1) {
    uart_puts("Waiting.\n");
    _delay_ms(5000);

    /* MOSI high */
    uart_puts("Starting sampling\n");
    PORTB = _BV(PB5) | _BV(PB4);

    buf[0] = 0xE3; //TEMP HOLD MEASUREMENT
    twi_start_wr_no_stop(TEMP_SLA_ADDR, buf, 1);
    twi_start_rd(TEMP_SLA_ADDR, buf, 2);

    /* Code for conversion from Contiki sht25.c driver. This number/100 is the
     * temperature in deg C */
    conv_buf = (uint32_t)( ((buf[0] << 8) + buf[1]) & ~0x0003);
    conv_buf *= 17572;
    conv_buf = conv_buf >> 16;
    temp = (int16_t)conv_buf - 4685;

    /* MOSI low */
    PORTB = _BV(PB4);
    uart_puts("Finished sampling\n");

    /* Dump raw signed value over UART */
    uart_putchar((uint8_t) ((temp&0xFF00)>>8) );
    uart_putchar((uint8_t) (temp&0x00FF) );
  }
}
