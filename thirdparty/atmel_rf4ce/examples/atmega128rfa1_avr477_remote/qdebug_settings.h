//---------- Do not edit --------------------
// Project Constants
// Values from 0xF000->0xFFFF are reserved for Atmel Kits
// values from 0x0000->0xEFFF are available for other projects

//! @ingroup QDebug-Remote
//! @{

#define 	QT8			        0xF001
#define 	QT16				0xF002
#define 	QM64				0xF003

// Interface constans
#define 	TWI				1
#define 	SPI1W				2
#define 	SPI2W				3
#define 	UART				4
#define         BitBangSPI                      5
#define         RF4CE_QDEBUG                    6

//---------- Edit Project Info -------------

// Select the type of interface to use for the debug protocol.
// Comment out the interface not used.
// Only one interface should be active.
// Set up project info
#define         QDEBUG_RF4CE
#define		PROJECT_ID	    0x1
#define         INTERFACE           RF4CE_QDEBUG

//! @}

