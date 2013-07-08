//---------- Do not edit --------------------
// Project Constants
// Values from 0xF000->0xFFFF are reserved for Atmel Kits
// values from 0x0000->0xEFFF are available for other projects
#define 	QT8			   0xF001
#define 	QT16				0xF002
#define 	QM64				0xF003
#define  ATSAM3S_EK     0xF006
#define  ATSAM3U_QTVB   0xF007
#define  ATSAM3N_EK     0xF008

// Interface constans
#define 	TWI				1
#define 	SPI1W				2
#define 	SPI2W				3
#define 	UART				4
#define  BitBangSPI     5

//---------- Edit Project Info -------------

// Select the type of interface to use for the debug protocol.
// Comment out the interface not used.
// Only one interface should be active.
//#define	QDEBUG_SPI
//#define QDEBUG_TWI
#define QDEBUG_SPI_BB

// Set up project info
//#define		PROJECT_ID			QT16
#define		PROJECT_ID			0xF0FF


#if defined(QDEBUG_SPI)
#define		INTERFACE           SPI2W
#elif defined(QDEBUG_TWI)
#define		INTERFACE           TWI
#elif defined(QDEBUG_SPI_BB)
#define     INTERFACE           BitBangSPI
#endif
