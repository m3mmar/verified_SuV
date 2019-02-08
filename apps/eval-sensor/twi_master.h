#ifndef __TWI_H__
#define __TWI_H__
#include <stdint.h>

uint8_t twi_busy(void);
uint8_t twi_start_wr(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt);
uint8_t twi_start_wr_no_stop(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt);
uint8_t twi_start_rd(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt);
uint8_t twi_start_rd_no_stop(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt);
void twi_init();

#endif
