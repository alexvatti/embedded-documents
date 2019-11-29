/*Source burst size 32 bits  length 2 = 64 bytes at a time
  Destination burst size 16 bits Length 4 = 64*/
// 8 bytes per Xfer
static unsigned int tx_code_daf_8[] = {
/*
DMAMOV CCR SP7 DP7 SB2 SS32 DAF DB4 DS16
DMAMOV   SAR, 0x40000000  ;0x40000000 is Source Address, This need to change.
DMAMOV   DAR, 0x5F000000  ;0x5F000000 is Destination Address, This need to change.
; Flush Peripheral clears the state in the DMAC. 27 is Peripheral number
DMAFLUSHP P27 
; Specify no of counts, each count transfers 8-bytes
DMALP 20
; Wait For Peripheral instructs the DMAC to halt execution of the thread until the specified
;		peripheral signals a DMA request for that DMA channel. 27 is Peripheral number
DMAWFP 27, burst 
DMALDB
;27 is Peripheral number
DMASTPB P27 
DMAWMB
DMALPENDB
;Send Event instructs the DMAC to modify an event-interrupt resource. 27 is event number
DMASEV E27  
DMAEND
*/
0x871501BC,
0x00BC01CC,
0x40000000,
0x000002BC,
0xD8355F00,
0xD8321320,
0x13D82B07,
0xD834063B,
0xFFFFFF00,
0xFFFFFFFF,
};

static unsigned int tx_code_daf_4[] = {
/*
;SA burst size 32 bits , SA burst length 1
; DA burst size 8 bits, DA burst length 4 

DMAMOV CCR SP7 DP7 SB1 SS32 DAF DB4 DS8
DMAMOV   SAR, 0x40000000  ;0x40000000 is Source Address, This need to change.
DMAMOV   DAR, 0x5F000000  ;0x5F000000 is Destination Address, This need to change.
; Flush Peripheral clears the state in the DMAC. 20 is Peripheral number
DMAFLUSHP P20
; Specify no of counts, each count transfers 4-bytes
DMALP 20
; Wait For Peripheral instructs the DMAC to halt execution of the thread until the specified
;               peripheral signals a DMA request for that DMA channel. 20 is Peripheral number
DMAWFP 20, burst
DMALDB
; 20 is Peripheral number
DMASTPB P20
DMAWMB
DMALPENDB
;Send Event instructs the DMAC to modify an event-interrupt resource. 20 is event number
DMASEV E20
DMAEND
*/
0x070501BC,
0x00BC01CC,
0x40000000,
0x000002BC,
0xA0355F00,
0xA0321320,
0x13A02B07,
0xA034063B,
0xFFFFFF00,
0xFFFFFFFF,
};

static unsigned int rx_code_saf_4[] = {
/*
;SA burst size 8 bits , SA burst length 4
; DA burst size 32 bits, DA burst length 1 

DMAMOV CCR SP7 DP7 SAF SB4 SS8 DB1 DS32
DMAMOV   SAR, 0x40000000  ;0x40000000 is Source Address, This need to change.
DMAMOV   DAR, 0x5F000000  ;0x5F000000 is Destination Address, This need to change.
; Flush Peripheral clears the state in the DMAC. 21 is Peripheral number
DMAFLUSHP P21
; Specify no of counts, each count transfers 4-bytes
DMALP 20
; Wait For Peripheral instructs the DMAC to halt execution of the thread until the specified
;               peripheral signals a DMA request for that DMA channel. 21 is Peripheral number
DMAWFP 21, burst
DMALDB
; 21 is Peripheral number
DMASTPB P21
DMAWMB
DMALPENDB
;Send Event instructs the DMAC to modify an event-interrupt resource. 21 is event number
DMASEV E21
DMAEND
*/
0x473001BC,
0x00BC01C1,
0x40000000,
0x000002BC,
0xA8355F00,
0xA8321320,
0x13A82B07,
0xA834063B,
0xFFFFFF00,
0xFFFFFFFF,
};

static unsigned int rx_code_saf_8[] = {
/*
;SA burst size 16 bits , SA burst length 4
; DA burst size 32 bits, DA burst length 2 

DMAMOV CCR SP7 DP7 SAF SB4 SS16 DB2 DS32
DMAMOV   SAR, 0x40000000  ;0x40000000 is Source Address, This need to change.
DMAMOV   DAR, 0x5F000000  ;0x5F000000 is Destination Address, This need to change.
; Flush Peripheral clears the state in the DMAC. 28 is Peripheral number
DMAFLUSHP P28
; Specify no of counts, each count transfers 8-bytes
DMALP 20
; Wait For Peripheral instructs the DMAC to halt execution of the thread until the specified
;               peripheral signals a DMA request for that DMA channel. 28 is Peripheral number
DMAWFP 28, burst
DMALDB
; 28 is Peripheral number
DMASTPB P28
DMAWMB
DMALPENDB
;Send Event instructs the DMAC to modify an event-interrupt resource. 28 is event number
DMASEV E28
DMAEND
*/
0x473201BC,
0x00BC01C5,
0x40000000,
0x000002BC,
0xE0355F00,
0xE0321320,
0x13E02B07,
0xE034063B,
0xFFFFFF00,
0xFFFFFFFF,
};


