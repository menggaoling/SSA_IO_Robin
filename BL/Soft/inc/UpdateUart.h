
//..............User Define..........................
#define UPD_PORT                        USART1
#define UPD_IRQChannel                  USART1_IRQChannel
#define UPD_UART_TX_GPIO                GPIOA
#define UPD_UART_TX_GPIO_Pin            GPIO_Pin_9
#define UPD_UART_RX_GPIO                GPIOA
#define UPD_UART_RX_GPIO_Pin            GPIO_Pin_10
//#define UPD_UART_BaudRate               115200
#define UPD_UART_BaudRate               9600


//..............Update Data index....................
#define UPD_HEAD_INDEX                  0x00
#define UPD_CMD_INDEX                   0x01
#define UPD_ADR0_INDEX                  0x02
#define UPD_ADR1_INDEX                  0x03
#define UPD_ADR2_INDEX                  0x04
#define UPD_ADR3_INDEX                  0x05
#define UPD_LEN0_INDEX                  0x06
#define UPD_LEN1_INDEX                  0x07
#define UPD_LEN2_INDEX                  0x08
#define UPD_LEN3_INDEX                  0x09
#define UPD_WDAT_INDEX                  0x0A

//..............Update Command List..................
#define UPD_DAT_EXTEND                  0xE0
#define UPD_DAT_START                   0xE1
#define UPD_DAT_END                     0xE2

#define UPD_RX_BUF_SIZE                 600
#define UPD_TX_BUF_SIZE                 600

#define MCU_WriteFlashHeader            7    //UPD_DAT_START(1 byte) + MCU_WriteFlash(1 byte) + StartAddr(4 bytes) + UPD_DAT_END(1 byte)

#define MCU_EraseFlash                  0x10
#define MCU_WriteFlash                  0x12
#define MCU_ReadFlash                   0x14
#define MCU_ReadProgramState            0x20
#define MCU_FlashUnlock                 0x24
#define MCU_WriteCheckCode              0x26
#define MCU_ReadBSLInfo                 0x30
#define MCU_ReadAPVersion               0x32
//#define MCU_ReadProduceID               0x30
//#define MCU_ReadMcuType                 0x32
//#define MCU_ReadMcuID                   0x34
#define MCU_SendReboot                  0x36
#define MCU_ReadUpdateMode              0x40
#define MCU_IsRequestUpdate             0x42

#define MCU_ReturnEraseFlash            0x11
#define MCU_ReturnWriteFlash            0x13
#define MCU_ReturnReadFlash             0x15
#define MCU_ReturnProgramState          0x21
#define MCU_ReturnFlashState            0x25
#define MCU_ReturnCheckCode             0x27
#define MCU_ReturnBSLInfo               0x31
#define MCU_ReturnAPVersion             0x33
//#define MCU_ReturnProduceID             0x31
//#define MCU_ReturnMcuType               0x33
//#define MCU_ReturnMcuID                 0x35
#define MCU_ReturnReboot                0x37
#define MCU_ReturnUpdateMode            0x41
#define MCU_ReturnRequestUpdate         0x43
#define MCU_ReturnInvalidCommand        0x7F

extern  const unsigned char CU8_BslMessage[];
extern  U8  Sys_Reboot;
//..............Public Function......................
extern  void UPD_UARTInit(void);
extern  void UPD_UARTGpioConfig(void);
extern  void UPD_SetBaudRate(void);
extern  U8   GetUPDCommandData(void);
extern  void UPD_CheckAndPackageCommand(void);
extern  void GenerateSystemColdReset(void);

extern void Delay1ms(u16 Delay1ms);
extern U16 APReboot_TIME;;







