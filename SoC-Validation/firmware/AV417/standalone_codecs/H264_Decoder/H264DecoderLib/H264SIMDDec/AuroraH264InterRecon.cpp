/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */

/* ------------------------------------------------------------------------ */
/* FILE:			AuroraH264Dec.h											*/
/* DATE:															        */
/* DESCRIPTION:		Interface for the AuroraH264Dec class.					*/
/* ------------------------------------------------------------------------ */
#if defined _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#include <assert.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"


#include "AuroraH264VerFiltering.h"
#include "AuroraH264HorFiltering.h"
#include "AuroraH264CtrPelFiltering.h"
#include "AuroraH264QPelFiltering.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	AuroraH264Dec::SIMDFilterLumaASM	                    */
/* DESCRIPTION:							                                    */
/* RETURNS:		void                                        */
/* ------------------------------------------------------------------------ */
void AuroraH264Dec::SIMDFilterLumaASM( SDM_Offset sdm_work_buf,
                                			    int fractional_hor_vector,
                                			    int fractional_ver_vector,
                                			    int block_hor_size,
                                			    int block_ver_size)
{
	int pixelNumber = (fractional_ver_vector<<2) + fractional_hor_vector;

	m_block_ver_size = block_ver_size;
	m_block_hor_size = block_hor_size;


    
    // calculate inter-prediction values
	switch( pixelNumber )
	{
	case 1:
		//perform vertical filtering on all input rows
		PerformInpHorFiltering( sdm_work_buf /*inp_ptr*/, 
					  m_hor_half_pel_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);
		ShiftBy2AndQPelFiltering( sdm_work_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */);

		break;

	
	case 2:
		//perform vertical filtering and store result in output array
		PerformInpHorFiltering( sdm_work_buf /*inp_ptr*/, 
					  sdm_work_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);
		break;


	case 3:
		//perform vertical filtering on all input rows
		PerformInpHorFiltering( sdm_work_buf /*inp_ptr*/, 
					  m_hor_half_pel_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);

		ShiftBy3AndQPelFiltering( sdm_work_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */);
		break;

	case 4:
        //perform vertical filtering on block_hor_size at offset 0
		VerticalFilteringAndRounding( sdm_work_buf, 
					  m_ver_half_pel_rnd_buf,
					  block_hor_size /* horSize */,
					  0); /* horOffset */

		QPelFiltering( sdm_work_buf+24*2 /* inp1_ptr */,
				m_ver_half_pel_rnd_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */); // d,f,n,q
		break;


	case 5:
		//perform vertical filtering on only block_hor_size at offset 2
		VerticalFilteringAndRounding( sdm_work_buf, 
					  m_ver_half_pel_rnd_buf,
					  block_hor_size /* horSize */,
					  2 /* horOffset */ );

		// calculate 'b' and store result in half pel buffer
		// discard the first 2 rows of the input
		PerformInpHorFiltering( sdm_work_buf+24*2 /*inp_ptr*/, 
					  m_hor_half_pel_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);

		// Different Hor half pel samples are used for 5 & 13 but 
		// this re-arrangement is done in hor-half pel calculation
		ShiftBy2AndQPelFiltering( m_ver_half_pel_rnd_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */);

		break;

	case 6:
		{
			//perform vertical filtering on all columns
		    int input_hor_size;
			if (0 != fractional_hor_vector)
			{
				input_hor_size = block_hor_size + 5;
			}
			else
			{
				input_hor_size = block_hor_size;
			}
			VerticalFilteringWORounding(sdm_work_buf /* inp_ptr */, 
						  m_ver_half_pel_unrnd_buf /* out_ptr */,
						  input_hor_size /* horSize */,
						  0 /* horOffset */);
	
			// calculate 'b' and store result in half pel buffer
			// discard the first 2 rows of the input
			PerformInpHorFiltering( sdm_work_buf+24*2 /*inp_ptr*/, 
						  m_hor_half_pel_buf /*out_ptr*/ ,
						  24 /*output_line_length*/);
			// calculate 'j'
			CentrePelFilterWOInpRounding( m_ver_half_pel_unrnd_buf,
							m_centre_pel_buf /* out_ptr */);
	
			QPelFiltering( m_centre_pel_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */); // d,f,n,q
			break;
		}
	case 7:
		// perform vertical filtering on block_hor_size columns at offset 3
		VerticalFilteringAndRounding( sdm_work_buf, 
					  m_ver_half_pel_rnd_buf,
					  block_hor_size /* horSize */,
					  3 /* horOffset */ );

		// calculate 'b' and store result in half pel buffer
		// discard the first 2 rows of the input
		PerformInpHorFiltering( sdm_work_buf+24*2 /*inp_ptr*/, 
					  m_hor_half_pel_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);

		// Different Hor half pel samples are used for 7 & 15 but 
		// this re-arrangement is done in hor-half pel calculation
		ShiftBy3AndQPelFiltering( m_ver_half_pel_rnd_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */);
		break;

	case 8:
		//perform vertical filtering and store result in output array
		VerticalFilteringAndRounding( sdm_work_buf, 
					  sdm_work_buf,
					  block_hor_size /* horSize */,
					  0); /* horOffset */

		break;

	case 9:
		{
			//perform vertical filtering on all columns
		    int input_hor_size;
			if (0 != fractional_hor_vector)
			{
				input_hor_size = block_hor_size + 5;
			}
			else
			{
				input_hor_size = block_hor_size;
			}
			VerticalFilteringWORounding(sdm_work_buf /* inp_ptr */, 
						  m_ver_half_pel_unrnd_buf /* out_ptr */,
						  input_hor_size /* horSize */,
						  0 /* horOffset */);
	
			// calculate 'j' and round the vertical half-pels
			CentrePelFilAndInpRounding(m_ver_half_pel_unrnd_buf,
										m_centre_pel_buf,
										m_ver_half_pel_rnd_buf);
	
			ShiftBy2AndQPelFiltering( m_ver_half_pel_rnd_buf /* inp1_ptr */,
					m_centre_pel_buf /* inp2_ptr */,
					sdm_work_buf /* out_ptr */ );
	
			break;
		}
	case 10:
		{
			//perform vertical filtering on all columns
		    int input_hor_size;
			if (0 != fractional_hor_vector)
			{
				input_hor_size = block_hor_size + 5;
			}
			else
			{
				input_hor_size = block_hor_size;
			}
			VerticalFilteringWORounding(sdm_work_buf /* inp_ptr */, 
						  m_ver_half_pel_unrnd_buf /* out_ptr */,
						  input_hor_size /* horSize */,
						  0 /* horOffset */);
	
			// calculate the final output 'j'
			CentrePelFilterWOInpRounding( m_ver_half_pel_unrnd_buf,
						sdm_work_buf );
	
			break;
		}
	case 11:
		{
			//perform vertical filtering on all columns
		    int input_hor_size;
			if (0 != fractional_hor_vector)
			{
				input_hor_size = block_hor_size + 5;
			}
			else
			{
				input_hor_size = block_hor_size;
			}
			VerticalFilteringWORounding(sdm_work_buf /* inp_ptr */, 
						  m_ver_half_pel_unrnd_buf /* out_ptr */,
						  input_hor_size /* horSize */,
						  0 /* horOffset */);
			// calculate 'j' and round the vertical half-pels
			CentrePelFilAndInpRounding(m_ver_half_pel_unrnd_buf,
										m_centre_pel_buf,
										m_ver_half_pel_rnd_buf);
			ShiftBy3AndQPelFiltering( m_ver_half_pel_rnd_buf /* inp1_ptr */,
					m_centre_pel_buf /* inp2_ptr */,
					sdm_work_buf /* out_ptr */ );
	
			break;
		}
	case 12:
        //perform vertical filtering on block_hor_size at offset 0
		VerticalFilteringAndRounding( sdm_work_buf, 
					  m_ver_half_pel_rnd_buf,
					  block_hor_size /* horSize */,
					  0); /* horOffset */

		QPelFiltering( sdm_work_buf+24*3 /* inp1_ptr */,
				m_ver_half_pel_rnd_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */); // d,f,n,q

		break;

	case 13:
		//perform vertical filtering on only block_hor_size at offset 2
		VerticalFilteringAndRounding( sdm_work_buf, 
					  m_ver_half_pel_rnd_buf,
					  block_hor_size /* horSize */,
					  2 /* horOffset */ );

		// calculate 's' and store result in half pel buffer
		// discard the first 3 rows of the input
		PerformInpHorFiltering( sdm_work_buf+24*3 /*inp_ptr*/, 
					  m_hor_half_pel_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);
		// Different Hor half pel samples are used for 5 & 13 but 
		// this re-arrangement is done in hor-half pel calculation
		ShiftBy2AndQPelFiltering( m_ver_half_pel_rnd_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */ );
		
		break;

	case 14:
		{
			//perform vertical filtering on all columns
		    int input_hor_size;
			if (0 != fractional_hor_vector)
			{
				input_hor_size = block_hor_size + 5;
			}
			else
			{
				input_hor_size = block_hor_size;
			}
			VerticalFilteringWORounding(sdm_work_buf /* inp_ptr */, 
						  m_ver_half_pel_unrnd_buf /* out_ptr */,
						  input_hor_size /* horSize */,
						  0 /* horOffset */);
			// calculate 's' and store result in half pel buffer
			// discard the first 3 rows of the input
			PerformInpHorFiltering( sdm_work_buf+24*3 /*inp_ptr*/, 
						  m_hor_half_pel_buf /*out_ptr*/ ,
						  24 /*output_line_length*/);
	
			// calculate 'j'
			CentrePelFilterWOInpRounding( m_ver_half_pel_unrnd_buf,
							m_centre_pel_buf /* out_ptr */);
	
			QPelFiltering( m_centre_pel_buf /* inp1_ptr */,
					m_hor_half_pel_buf /* inp2_ptr */,
					sdm_work_buf /* out_ptr */); // d,f,n,q
		}
		break;

	case 15:
		// perform vertical filtering on block_hor_size columns at offset 3
		VerticalFilteringAndRounding( sdm_work_buf, 
					  m_ver_half_pel_rnd_buf,
					  block_hor_size /* horSize */,
					  3 /* horOffset */ );
		// calculate 's' and store result in half pel buffer
		// discard the first 3 rows of the input
		PerformInpHorFiltering( sdm_work_buf+24*3 /*inp_ptr*/, 
					  m_hor_half_pel_buf /*out_ptr*/ ,
					  24 /*output_line_length*/);
		// Different Hor half pel samples are used for 7 & 15 but 
		// this re-arrangement is done in hor-half pel calculation
		ShiftBy3AndQPelFiltering( m_ver_half_pel_rnd_buf /* inp1_ptr */,
				m_hor_half_pel_buf /* inp2_ptr */,
				sdm_work_buf /* out_ptr */);

		break;

	default:
		assert(false);
		break;
	}
        
    return;

	
}
