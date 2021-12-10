#include "spi.h"

/*
arm-linux-gnueabihf-gcc-10 -Wall -Wextra -Wconversion -Warray-bounds=2 
-ansi -O2 -o ../TestFiles/spiMaster spiMaster.c
*/
/*int main(int argc, char *argv[]){
    printf("with pic 1\n");

    uint8_t mode = 0;
    uint8_t bits = 0;
    uint32_t speed= atoi(argv[1]);
    const char name[]= "/dev/spidev1.0";
    uint8_t askBytes = BEGIN_SEND;
    uint8_t end = END_SEND;
    uint8_t amountToRead;
    uint8_t data;
    uint8_t i,j;
    spi_t spi;
    
    int file=spi_init(&spi, name, mode, bits, speed);
   
    int returnValue = spi_write(&spi, &askBytes, sizeof(askBytes));
   
    usleep(1000);
    returnValue = spi_read(&spi, &amountToRead, sizeof(amountToRead));
    printf("amount to read %d\n", amountToRead);
   
    uint8_t dataArray[amountToRead][arrayDepth];
    for(i=0;i<amountToRead;i++){
        for(j=0;j<arrayDepth;j++){
            spi_read(&spi, &data, sizeof(data));   
            dataArray[i][j] = data;
            
        }
    }
    PrintArray(dataArray, amountToRead);
    spi_write(&spi, &end, sizeof(end));
    uint16_t timer[amountToRead];

    for(i=0;i<amountToRead;i++){
        timer[i] = (uint16_t)((uint16_t)(dataArray[i][1] << 8) | (dataArray[i][0] << 0));
        printf("timer[%d] = %d\n", i, timer[i]);
        printf("time = timer * 0.000100 = %f\n\n", timer[i] * 0.000100f);
    }

    
    
    spi_free(&spi);
    return 0;
}*/
int spi_init(spi_t *spi, const char* devName, uint8_t mode, uint8_t bits, uint32_t speed){
    memset(&spi->xfer, 0, sizeof(spi->xfer));
    
    if ((spi->fd = open(devName,O_RDWR)) < 0)
    {
        printf("Failed to open the bus.");
        return(1);
    }
   

    if(mode){
        spi->mode = mode;
        if(ioctl(spi->fd, SPI_IOC_WR_MODE, &spi->mode)<0){
            perror("cant send wr mode");
            return(1);
        }
    }
    

    if(ioctl(spi->fd, SPI_IOC_RD_MODE, &spi->mode)<0){
        perror("cant send rd mode");
        return(1);
    }
    

    if(ioctl(spi->fd, SPI_IOC_RD_LSB_FIRST, &spi->lsd)<0){
        perror("cant set lsb first");
        return(1);
    }
    

    if(bits){
        if(ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &spi->bits)<0){
            perror("cant send bits p w");
            return(1);
        }   
    }
   

    if(ioctl(spi->fd, SPI_IOC_RD_BITS_PER_WORD, &spi->bits)<0){
        perror("cant send bits p w");
        return(1);
    }   
   
    
    if(speed){
        spi->speed = speed;
        if(ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi->speed)<0){
            perror("cant send speed");
            return(1);
        }
    }
    

    if(ioctl(spi->fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi->speed)<0){
        perror("cant send speed");
        return(1);
    }
    //spi->xfer.delay_usecs = 1000;
    printf("open dev: %s, mode %d, bits %d, lsd %d, speed %d, delay %d\n",\
     devName, (int)spi->mode, (int)spi->bits, (int)spi->lsd,(int)spi->speed, spi->xfer.delay_usecs);
    
    return spi->fd;
}

void spi_free(spi_t* spi){
    close(spi->fd);
}
 
int spi_read(spi_t *spi, uint8_t *rxBuf, int len){
    int returnValue;
    uint8_t *zero;
    uint8_t v = 0;
    zero = &v;
    spi->xfer.tx_buf = (uintptr_t)zero;
    spi->xfer.rx_buf = (uint64_t)(uintptr_t)rxBuf;
    spi->xfer.len = (uint32_t) len;
    

    returnValue = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &spi->xfer);
    if(returnValue<0){
        perror("cant send messgage");
        return(-1);
    }

    return returnValue;
}

int spi_write(spi_t* spi, uint8_t *txBuf, int len){
    int returnValue;
    
    spi->xfer.tx_buf = (uintptr_t)(txBuf);
    spi->xfer.rx_buf = (uint64_t)0x00;
    spi->xfer.len = (uint32_t) len;
    
    /*printf("write: %d\n", *txBuf);*/
    returnValue = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &spi->xfer);
    if(returnValue<0){
        perror("cant send messgage");
        return(-1);
    }
   

    return returnValue;
}

int spi_R_W(spi_t* spi, uint8_t *txBuf, uint8_t *rxBuf, int len){
    int returnValue;
    spi->xfer.tx_buf = (uint64_t)(uintptr_t)txBuf;
    spi->xfer.rx_buf = (uint64_t)(uintptr_t)rxBuf;
    spi->xfer.len = (uint32_t) len;
    
    returnValue = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &spi->xfer);
    if(returnValue<0){
        perror("cant send messgage");
        return(-1);
    }
    return returnValue;
}