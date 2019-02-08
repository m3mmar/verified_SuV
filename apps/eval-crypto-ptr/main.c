#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "microvisor.h"

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

/**************** SPECK CRYPTO ********************/

#define SPECK_TYPE uint32_t
#define SPECK_ROUNDS 27
#define SPECK_KEY_LEN 4

#define ROR(x, r) ((x >> r) | (x << ((sizeof(SPECK_TYPE) * 8) - r)))
#define ROL(x, r) ((x << r) | (x >> ((sizeof(SPECK_TYPE) * 8) - r)))

#ifdef SPECK_32_64
#define R(x, y, k) (x = ROR(x, 7), x += y, x ^= k, y = ROL(y, 2), y ^= x)
#define RR(x, y, k) (y ^= x, y = ROR(y, 2), x ^= k, x -= y, x = ROL(x, 7))
#else
#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)
#define RR(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))
#endif

void speck_expand(SPECK_TYPE const K[static SPECK_KEY_LEN], SPECK_TYPE S[static
    SPECK_ROUNDS])
{
  SPECK_TYPE i, b = K[0];
  SPECK_TYPE a[SPECK_KEY_LEN - 1];

  for (i = 0; i < (SPECK_KEY_LEN - 1); i++)
  {
    a[i] = K[i + 1];
  }
  S[0] = b;
  for (i = 0; i < SPECK_ROUNDS - 1; i++) {
    R(a[i % (SPECK_KEY_LEN - 1)], b, i);
    S[i + 1] = b;
  }
}

void speck_encrypt(SPECK_TYPE const pt[static 2], SPECK_TYPE ct[static 2],
    SPECK_TYPE const K[static SPECK_ROUNDS])
{
  SPECK_TYPE i;
  ct[0]=pt[0]; ct[1]=pt[1];

  for(i = 0; i < SPECK_ROUNDS; i++){
    R(ct[1], ct[0], K[i]);
  }
}

void speck_decrypt(SPECK_TYPE const ct[static 2], SPECK_TYPE pt[static 2],
    SPECK_TYPE const K[static SPECK_ROUNDS])
{
  SPECK_TYPE i;
  pt[0]=ct[0]; pt[1]=ct[1];

  for(i = 0; i < SPECK_ROUNDS; i++){
    RR(pt[1], pt[0], K[(SPECK_ROUNDS - 1) - i]);
  }
}

void (*speck_expand_ptr)(SPECK_TYPE const K[static SPECK_KEY_LEN], SPECK_TYPE S[static
    SPECK_ROUNDS]) = speck_expand;
void (*speck_encrypt_ptr)(SPECK_TYPE const pt[static 2], SPECK_TYPE ct[static 2],
    SPECK_TYPE const K[static SPECK_ROUNDS]) = speck_encrypt;
void (*speck_decrypt_ptr)(SPECK_TYPE const ct[static 2], SPECK_TYPE pt[static 2],
    SPECK_TYPE const K[static SPECK_ROUNDS]) = speck_decrypt;

int main(void) {
  uart_init();
  uart_puts("eval_crypto_process\n");

  /* MOSI and MISO pin */
  DDRB |= ( _BV(PB5) | _BV(PB6) );

  while(1) {
    uart_puts("Waiting.\n");

    _delay_ms(5000);

    uart_puts("Starting crypto.\n");

    /* PB5 high */
    PORTB = _BV(PB5);

    uint32_t plain[2] = {987654321,987654321};
    uint32_t key[4] = {123456789, 123456789, 123456789, 123456789};

    SPECK_TYPE buffer[2] = {0};
    SPECK_TYPE enc[2] = {0};

    SPECK_TYPE exp[SPECK_ROUNDS];

    speck_expand_ptr(key, exp);
    speck_encrypt_ptr(plain, enc, exp);
    speck_decrypt_ptr(enc, buffer, exp);

    /* PB5 low */
    PORTB = 0x00;

    uart_puts("finished crypto.\n");
  }
}
