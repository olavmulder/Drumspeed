#ifndef SPI_H
#define SPI_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

typedef struct spi_{
  int fd;
  uint32_t speed;
  uint8_t  mode;
  uint8_t  lsd;
  uint8_t bits;
  uint16_t delay;
  struct spi_ioc_transfer xfer;
}spi_t;
static const char name[]= "/dev/spidev1.0";




static const uint8_t BEGIN_SEND = 0x01;
static const uint8_t END_SEND = 0xFF;
static const uint8_t DUMMY_DATA = 0x00;
int spi_init(spi_t *spi, const char * devName, uint8_t mode, uint8_t bits, uint32_t speed);
void spi_free(spi_t* spi);
int spi_read(spi_t*, uint8_t *rxBuf, int len);
int spi_write(spi_t*, uint8_t *txBuf, int len);
int spi_R_W(spi_t*spi, uint8_t *txBuf, uint8_t *rxBuf, int len);
/*-------------------*/
/*uint8_t  spi_read(int nbytes,int file, unsigned char *tx, unsigned char *rx, uint8_t transfers);*/
/*int spi_init(char filename[40]);*/
/*void spi_write(int nbytes,uint8_t *value,int file);*/


#endif