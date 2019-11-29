/*
 *    Header file for gps application
 *
 *    */

#include <common.h>
#include <exports.h>
/*#include "sst230includes.h"
 * #include "pdltype.h"
 * #include "gloinitprot.h"*/
#include "usrapi.h"
//#include "pandaspi.h"
//#include "spi.h"
//#include "gpio.h"
//#include "gic.h"


typedef struct
{
  T_U16 word4;

  T_U16 word3;

  T_U16 word2;

  T_U32 msword:4;

  T_U32 exp:11;

  T_U32 sign:1; 

}T_DOUBLE_OLD;

double d1;

#ifdef DUMP_20INT_DEBUG
T_U16 dumpdata[2902*20];
#endif
 
#ifdef TRACK_DATA_DEBUG
T_U16 track_data[16*1000];
#endif

int __errno;

#define GPS_DATA_TO_RX_IN_WORDS 1451
#define GPS_DATA_TO_TX_IN_WORDS 512

T_GPS_DATA_TX gps_data_buf;

T_U8 Nmea_Buf[1000];
T_U16 Nmea_Data_Size;
T_U16 Nmea_Buf_Read_Index;

int global_i;
int tenmsec_interrupt_count;
int twohundredmsec_count;
T_U8 Ignore_Reads;
T_U16 csum;

T_U16 down_counter;
T_U16 GloDataRead[20];
T_U8 GloDataReadIndex;

T_U16 GpsDataRead[20];
T_U8 GpsDataReadIndex;

T_U16 PatData[20];

T_U16 glonass;
T_U16 gps;
T_U16 glo_acq;
T_U16 gps_acq;
T_U16 pattern;
T_U16 pattern_gps;

static void tenms_interrupt ();
static void sw_interrupt(void *arg);
static void delay(T_U32 value);
void uart_write (void);
void VerifyChecksum(void);
void Init_GlobalData(void);

