// twi_master.c
// R. Traylor
// 11.07.2011
// twi_master code
// (https://github.com/rltraylor/twi_master)
//
// W. Daniels 08.2016
// Patched to now also have repeated starts

#include "twi_master.h"

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* These macros calculate the I2C clock parameters */
#define F_SCL 100000UL // I2C SCL target frequency is 100 kHz
#define prescaler_val 1 // I2c prescaler off. Modify TWSR manually in setup() to change
#define TWBR_val ((((F_CPU / F_SCL) / prescaler_val) - 16 ) / 2) // Calc clock value

volatile uint8_t *twi_buf;      //pointer to the buffer we are xferred from/to
volatile uint8_t twi_msg_size;  //number of bytes to be xferred
volatile uint8_t twi_bus_addr;  //address of device on TWI bus
volatile uint8_t twi_state;     //status of transaction
volatile uint8_t twi_send_stop; //send stop or not after write

//****************************************************************************
//This is the TWI ISR. Different actions are taken depending upon the value
//of the TWI status register TWSR.
//****************************************************************************/
ISR(TWI_vect){
  static uint8_t twi_buf_ptr;  //index into the buffer being used

  switch(TWSR) {
    case TW_START:          //START has been xmitted, fall thorough
    case TW_REP_START:      //Repeated START was xmitted
      TWDR = twi_bus_addr;  //load up the twi bus address
      twi_buf_ptr = 0;      //initalize buffer pointer
      TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
      break;
    case TW_MT_SLA_ACK:     //SLA+W was xmitted and ACK rcvd, fall through
    case TW_MT_DATA_ACK:                //Data byte was xmitted and ACK rcvd
      if (twi_buf_ptr < twi_msg_size) {  //send data till done
        TWDR = twi_buf[twi_buf_ptr++];  //load next and postincrement index
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);  //send next byte
      } else {
        if(twi_send_stop)
          TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); //last byte sent, send STOP without interrupt
        else
          TWCR = _BV(TWEN); //done, disable interrupts
      }
      break;
    case TW_MR_DATA_ACK:                //Data byte has been rcvd, ACK xmitted, fall through
      twi_buf[twi_buf_ptr++] = TWDR;    //fill buffer with rcvd data
    case TW_MR_SLA_ACK:                 //SLA+R xmitted and ACK rcvd
      if (twi_buf_ptr < (twi_msg_size-1)) {
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) | _BV(TWIE); //ACK each byte
      } else {
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE); //NACK last byte
      }
      break;
    case TW_MR_DATA_NACK: //Data byte was rcvd and NACK xmitted
      twi_buf[twi_buf_ptr] = TWDR;      //save last byte to buffer
      TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); //last byte rxed, send STOP without interrupt
      break;
    case TW_MT_ARB_LOST: //Arbitration lost
      TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE); //initiate RESTART
      break;
    case TW_MT_SLA_NACK:
    case TW_MR_SLA_NACK:
    default:                            //Error occured, save TWSR
      twi_state = TWSR;
      TWCR = _BV(TWEN);                  //Reset TWI, disable interupts
  }//switch
}//TWI_isr
//****************************************************************************
//*****************************************************************************
//Call this function to test if the TWI unit is busy transferring data. The TWI
//code uses the the interrupt enable bit (TWIE) to indicate if the TWI unit
//is busy or not.  This protocol must be maintained for correct operation.
//*****************************************************************************
//*****************************************************************************
static uint8_t busy_wait() {
  while(bit_is_set(TWCR,TWIE));
  return 1;
}
//****************************************************************************
//Initiates a write transfer. Loads global variables. Sends START. ISR handles
//the rest.
//****************************************************************************
uint8_t twi_start_wr(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt){
  twi_bus_addr = (twi_addr<<1 & ~TW_READ); //set twi bus address, mark as write
  twi_buf = twi_data;                   //load pointer to write buffer
  twi_msg_size = byte_cnt;              //load size of xfer
  twi_send_stop = 1;
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE); //initiate START
  return busy_wait();
}

uint8_t twi_start_wr_no_stop(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt){
  twi_bus_addr = (twi_addr<<1 & ~TW_READ); //set twi bus address, mark as write
  twi_buf = twi_data;                   //load pointer to write buffer
  twi_msg_size = byte_cnt;              //load size of xfer
  twi_send_stop = 0;
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE); //initiate START
  return busy_wait();
}

//****************************************************************************
//Initiates a read transfer. Loads global variables. Sends START. ISR handles
//the rest.
//****************************************************************************
uint8_t twi_start_rd(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt){
  twi_bus_addr = (twi_addr<<1 | TW_READ); //set twi bus address, mark as read
  twi_buf = twi_data;                  //load pointer to write buffer
  twi_msg_size = byte_cnt;             //load size of xfer
  twi_send_stop = 1;
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE); //initiate START
  return busy_wait();
}
//******************************************************************************
//                            init_twi
//
//Uses PD1 as SDA and PD0 as SCL
//10K pullups are present on the board
//for alarm clock an additional 4.7K resistor is also there for pullup
//******************************************************************************

void twi_init(){
  TWDR = 0xFF;     //release SDA, default contents
  TWCR = _BV(TWEN); //Reset TWI, disable interrupts
  TWSR &= ~( _BV(TWPS0) | _BV(TWPS1) ); //prescaler value = 1
  TWBR = TWBR_val; //defined in twi_master.h
}
