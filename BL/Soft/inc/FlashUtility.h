
//..............Flash Define.........................
#define StartAddr                   ((U32)0x08004000)
#define ApplicationAddress          ((U32)0x08004000)

//#define EndAddr                     ((U32)0x08020000)
#define EndAddr                     ((U32)0x08040000)
#define PageSize                    ((U16)0x800)        // PageSize = 2K
//#define PageSize                    ((U16)0x200)        // PageSize = 512 byte

#define DFlashEraseMaskBufferSize   8   // Size = 512K / 2K / 32

#define DFlashMarkPageAddress       (EndAddr - PageSize)
#define DFlashMarkAddress           ((U32)0x0803FFF0)
#define DAPVersionAddress           ((U32)0x0803FFE0)
#define DFlashMarkSize              16
#define DFlashMark1                 0x55AA5AA5
#define DFlashMark2                 0x24681357
#define DFlashMark3                 0x9BDEACF0
#define DFlashMark4                 0x0FF0FF00


//..............User Define..........................
#define DeviceFamilyType_HI         0xF2
#define DeviceFamilyType_LO         0x6F
#define BSLVersion_HI               0x11
#define BSLVersion_LO               0x01
#define Process_HI                  0x04
#define Process_LO                  0x60

//..............Update Command List..................
#if     0
#define
#define
#define
#endif
//..............Public Function......................
extern  void EnterFlashProgramming(void);
extern  void CheckFlashEraseStatus( U32 PU32_Address );
//extern  void FlashErase(void);
extern  U8   FlashErase(U32  EraseAddr, U32 Length);
extern  U8   FlashWrite(U32  WriteAddr, U16 Length, U8 *updbuf);
extern  U8   FlashRead(U32  ReadAddr, U16 Length, U8 *pbuf);
extern  void ShowFlashContent(U8 *updbuf);
extern  void  ShowFlashIndex(void);

//..............Public Varable ......................
extern  U8  ProgramLock;

#if     0
extern
extern
extern
#endif


//.....................................................................................
