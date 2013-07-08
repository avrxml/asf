#ifndef BITBANGSPI_MASTER_H_INCLUDED
#define BITBANGSPI_MASTER_H_INCLUDED

/*============================ PROTOTYPES ====================================*/
void BitBangSPI_Master_Init (void);
void BitBangSPI_Send_Message(void);

/*============================ MACROS ========================================*/
#define JOIN( x, y ) x ## y
#define REG( REGISTER, SIDE ) JOIN( REGISTER, SIDE )

// The definitions below should not clash with the SNS/SNSK port pins

// Select QDebug Pins.
#define SS_BB           QDEBUG_SPI_BB_SS_PIN
#define SCK_BB          QDEBUG_SPI_BB_SCK_PIN
#define MOSI_BB         QDEBUG_SPI_BB_MOSI_PIN
#define MISO_BB         QDEBUG_SPI_BB_MISO_PIN

//Select QDebug Ports.
#define SPI_BB_SS       QDEBUG_SPI_BB_SS_PORT
#define SPI_BB_SCK      QDEBUG_SPI_BB_SCK_PORT
#define SPI_BB_MOSI     QDEBUG_SPI_BB_MOSI_PORT
#define SPI_BB_MISO     QDEBUG_SPI_BB_MISO_PORT

#endif

/* EOF */
