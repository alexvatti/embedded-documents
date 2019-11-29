/* Function prototypes for gps-glonass integration
   */
#ifndef __USRAPI_H_
#define __USRAPI_H_
typedef unsigned short int 				T_U16;
typedef short int						T_S16;
typedef unsigned long int				T_U32;
typedef long int						T_S32;
typedef char							T_CHAR;

typedef unsigned char					T_U8;
typedef signed char						T_S8;

typedef struct
{
	/*
	 *	X_velocity_kalman coordinate of ECEF position in metres, scaled by 100
	 */
	T_S32								XPOS;

	/*
	 *	Y coordinate of ECEF position in metres, scaled by 100
	 */
	T_S32								YPOS;

	/*
	 *	Y coordinate of ECEF position in metres, scaled by 100
	 */
	T_S32								ZPOS;

	/*
	 *	X component of ECEF velocity in metres per second, scaled by 10e7.
	 */
	T_S32								XVEL;

	/*
	 *	Y component of ECEF velocity in metres per second, scaled by 10e7.
	 */
	T_S32								YVEL;

	/*
	 *	Z component of ECEF velocity in metres per second, scaled by 10e7.
	 */
	T_S32								ZVEL;

	/*
	 *	Latitude of user position  in radians, scaled by 10e7, positive means
	 *	North, negative means South.
	 */
	T_S32								Lat;

	/*
	 *	Longitude of user position  in radians, scaled by 10e7, positive means
	 *	East, negative means West.
	 */
	T_S32								Lon;

	/*
	 *	Altitude in radians, scaled by 10e3.
	 */
	T_S32								Alt;

	/*
	 *	Position Dilution of Precision, scaled by 100.
	 */
	T_S32								PDop;

	/*
	 *	Horizontal Dilution of Precision, scaled by 100.
	 */
	T_S32								HDop;

	/*
	 *	Vertical Dilution of Precision, scaled by 100.
	 */
	T_S32								VDop;

	/*
	 *	Geometric Dilution of Precision, scaled by 100.
	 */
	T_S32								GDop;

	/*
	 *	Time Dilution of Precision, scaled by 100.
	 */
	T_S32								TDop;


	/*
	 *	User Speed in Kmph, scaled by 1000.
	 */
	T_S32								Speed;

	/*
	 *	User Heading w.r.t to north(0 to 360 degrees), scaled by 1000.
	 */
	T_S32								Heading;

	/*
	 *	Seconds elapsed in current week.
	 */
	T_S32								Seconds;

	/*
	 *	Nanoseconds elapsed in current second.
	 */
	T_S32								NanoSeconds;

	/*
	 *	Current Week number.
	 */
	T_U16			 					Week;

	/*
	 *	Day of the current month.
	 */
	T_U16			 					Day;

	/*
	 *	Month of the current year.
	 */
	T_U16			 					Month;

	/*
	 *	Current year
	 */
	T_U16			 					Year;

	/*
	 *	Hours field of Gps time.
	 */
	T_U16			 					Hour;

	/*
	 *	Minutes field of Gps time.
	 */
	T_U16			 					Minute;

	/*
	 *	Seconds field of Gps time.
	 */
	T_U16			 					Second;

	/*
	 *	Flag to indicate position availability.
	 *	0: position not available, 1: position available.
	 */
	T_U16			 					Pos_Avail				:1;

	/*
	 *	Flag to indicate the type of fix given
	 *	0: No fix
	 *	1: 2D fix
	 *	2: 3D fix
	 *	3 to 8: Reserved
	 */
	T_U16			 					Fix_Type				:3;

	/*
	 *	Flag to indicate Time validity.
	 *	0: satellite time synch not done, 1: satellite time synch done.
	 */
	T_U16			 					TimeValidity			:1;

	/*
	 *	Unused field
	 */
	T_U16			 					dummy					:11;

	/*
	 *	The datum corrections to be applied to the position.
	 *	The range is from 0 to 188.
	 */
	T_U16			 					Datum;

	/*
	 *	Meansea level correction to be applied to altitude
	 */
	T_S16								Meansea_corr;


	T_U16								dummy1;

	/*
	 *	Lower word of position timestamp
	 */
	T_U32			 					Position_latch_time_lower_word;

	/*
	 *	Upper word of position timestamp
	 */
	T_U32			 					Position_latch_time_upper_word;

	/*
	 *	Number of visible satellites, valid only in M_WARM/HOT start
	 */
	T_U16								No_of_visible_svs;

	/*
	 *	Satellite list in channels - 0 to 31, 255
	 *	means no satellite assigned to the channel
	 */
	T_U16								Svs_programmed_in_trk_channels [16];

	/*
	 *	Satellites used for User position computation
	 */
	T_U16								Svs_used_for_pos;

	/*
	 *	Signal strength in dB-Hz
	 */
	T_U16								SNR [16];

	/*
	 *	Antenna status message: 0- Connected 1- Antenna open 2-Antenna short
	 */
	T_U16								Antenna_status;


	T_U16								dummy2;

	/*
	 *	Clock Bias in nanoseconds
	 */
	T_S32								clock_bias;

	/*
	 *	Clock Drift in nanoseconds per second
	 */
	T_S32								clock_drift;

	/*
	 *	Duration for which the receiver is switched on in terms of ten milliseconds
	 */
	T_U32								receiver_on_time;

} T_GPS_DATA_TX;

extern T_U16  Glonass_dsp_tx_buffer[512],                                                                        
              Dsp_tx_buffer[512],                                                                                
              Dsp_rx_buffer[1451],
              Glonass_dsp_rx_buffer[1451];  

void GpsGlonassApplicationFunction(void);
void TenMillisecondsTask(void);
void GpsGlonassTwoFiftymsecTask(void);
void GetNmeaMsg(T_U8 *tx_buf, T_U16 *tx_size);
void GpsReadData(T_GPS_DATA_TX *input);

#endif

