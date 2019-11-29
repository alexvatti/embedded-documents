#include <spi.h>
#include <common.h>
#include <lcd.h>
#include <asm/arch/shikhara_tc.h>

// Display width in pixels.
#define BOARD_LCD_WIDTH             480
/// Display height in pixels.
#define BOARD_LCD_HEIGHT            272
/// Size in pixels of calibration points.
#define POINTS_SIZE         4

// Maximum difference in pixels between the test point and the measured point.
#define POINTS_MAX_ERROR    0xF

extern inline void lcd_puts_xy (ushort x, ushort y, uchar *s);

typedef struct _CalibrationPoint {

	/// Coordinate of point along the X-axis of the screen.
	unsigned int x;
	/// Coordinate of point along the Y-axis of the screen.
	unsigned int y;
	/// Calibration data of point.
	unsigned int data[2];

} CalibrationPoint;

/// Calibration points
static CalibrationPoint calibrationPoints[] = {

	// Top-left corner calibration point
	{
		BOARD_LCD_WIDTH / 10,
		BOARD_LCD_HEIGHT / 10,
		{0, 0}
	},
	// Top-right corner calibration point
	{
		BOARD_LCD_WIDTH  - (BOARD_LCD_WIDTH / 10),
		BOARD_LCD_HEIGHT / 10,
		{0, 0}
	},
	// Bottom-right corner calibration point
	{
		BOARD_LCD_WIDTH  - (BOARD_LCD_WIDTH / 10) ,   
		BOARD_LCD_HEIGHT -( BOARD_LCD_HEIGHT / 10) ,
		{0, 0}
	},
	// Bottom-left corner calibration point
	{
		(BOARD_LCD_WIDTH / 10),
		BOARD_LCD_HEIGHT  -( BOARD_LCD_HEIGHT / 10),
		{0, 0}
	}
};
/// Test point
static const CalibrationPoint testpoint = {
	BOARD_LCD_WIDTH / 2,
	BOARD_LCD_HEIGHT / 2,
	{0, 0}
};
/// Slope for interpoling touchscreen measurements along the X-axis.
static signed int xSlope;
/// Slope for interpoling touchscreen measurements along the Y-axis.
static signed int ySlope;
//------------------------------------------------------------------------------
/// Display a calibration point on the given buffer.
/// \param pLcdBuffer  LCD buffer to draw on.
/// \param pPoint  Calibration point to display.
//------------------------------------------------------------------------------
static void DrawCalibrationPoint(const CalibrationPoint *pPoint)
{
	lcd_draw_rectangle(pPoint->x - POINTS_SIZE / 2,
			pPoint->y - POINTS_SIZE / 2,
			POINTS_SIZE,
			POINTS_SIZE,
			CONSOLE_COLOR_BLACK);
}

//------------------------------------------------------------------------------
/// Clears a calibration point from the given buffer.
/// \param pLcdBuffer  LCD buffer to draw on.
/// \param pPoint  Calibration point to clear.
//------------------------------------------------------------------------------
static void ClearCalibrationPoint(const CalibrationPoint *pPoint)
{
	lcd_draw_rectangle(pPoint->x - POINTS_SIZE / 2,
			pPoint->y - POINTS_SIZE / 2,
			POINTS_SIZE,
			POINTS_SIZE,
			CONSOLE_COLOR_WHITE);
}
void measurement(const unsigned int *pData, unsigned int *pPoint)
{
	pPoint[0] = calibrationPoints[0].x
		- (((signed int) calibrationPoints[0].data[0] - (signed int) pData[0]) * 1024)
		/ xSlope;

	pPoint[1] = calibrationPoints[0].y
		- (((signed int) calibrationPoints[0].data[1] - (signed int) pData[1]) * 1024)
		/ ySlope;

	if(pPoint[0] & 0x80000000) // Is pPoint[0] negative ?
	{
		pPoint[0] = 0;
	}

	if(pPoint[0] > BOARD_LCD_WIDTH) // Is pPoint[0] bigger than the LCD width ?
	{
		pPoint[0] = BOARD_LCD_WIDTH;
	}

	if(pPoint[1] & 0x80000000) // Is pPoint[1] negative ?
	{
		pPoint[1] = 0;
	}

	if(pPoint[1] > BOARD_LCD_HEIGHT) // Is pPoint[1] bigger than the LCD width ?
	{
		pPoint[1] = BOARD_LCD_HEIGHT;
	}
}

int ts_calibrate(struct spi_slave *slave){

	int i;
	signed int slope1, slope2;
	unsigned char xOk, yOk;
	signed int xDiff, yDiff;
	CalibrationPoint measuredPoint;	

	fill_screen(CONSOLE_COLOR_WHITE);
	lcd_puts_xy(40,40,(uchar *)"LCD Calibration");
	lcd_puts_xy(40,60,(uchar *)"Touch the dots to calibrate the screen");

	for (i=0; i < 4; i++) {
		printf("\n Please touch  at position %d to sample \n\n",i+1);
		DrawCalibrationPoint(&calibrationPoints[i]);
		get_position(&calibrationPoints[i].data[0],&calibrationPoints[i].data[1],slave);
		ClearCalibrationPoint(&calibrationPoints[i]);
		printf("Calibration points are x= %X and y= %X\n",calibrationPoints[i].data[0],calibrationPoints[i].data[1]);		
		udelay(500000);			
	}

	slope1 = ((signed int) calibrationPoints[0].data[0]) - ((signed int) calibrationPoints[1].data[0]);
	slope1 *= 1024;
	slope1 /= ((signed int) calibrationPoints[0].x) - ((signed int) calibrationPoints[1].x);
	slope2 = ((signed int) calibrationPoints[2].data[0]) - ((signed int) calibrationPoints[3].data[0]);
	slope2 *= 1024;
	slope2 /= ((signed int) calibrationPoints[2].x) - ((signed int) calibrationPoints[3].x);
	xSlope = (slope1 + slope2) / 2;

	slope1 = ((signed int) calibrationPoints[0].data[1]) - ((signed int) calibrationPoints[2].data[1]);
	slope1 *= 1024;
	slope1 /= ((signed int) calibrationPoints[0].y) - ((signed int) calibrationPoints[2].y);
	slope2 = ((signed int) calibrationPoints[1].data[1]) - ((signed int) calibrationPoints[3].data[1]);
	slope2 *= 1024;
	slope2 /= ((signed int) calibrationPoints[1].y) - ((signed int) calibrationPoints[3].y);
	ySlope = (slope1 + slope2) / 2;

	printf("\n\n X- slope is %d and Y- slope is %d \n Touch to take Sample for testing \n\n",xSlope,ySlope);

	/*Take Sample point */  
	DrawCalibrationPoint(&testpoint);	
	get_position(&measuredPoint.data[0],&measuredPoint.data[1],slave);
	ClearCalibrationPoint(&testpoint);
	measurement(&measuredPoint.data, (unsigned int *) &measuredPoint);

	// Check resulting x and y
	xDiff = (signed int) measuredPoint.x - (signed int) testpoint.x;
	yDiff = (signed int) measuredPoint.y - (signed int) testpoint.y;
	xOk = (xDiff >= -POINTS_MAX_ERROR) && (xDiff <= POINTS_MAX_ERROR);
	yOk = (yDiff >= -POINTS_MAX_ERROR) && (yDiff <= POINTS_MAX_ERROR);
	fill_screen(CONSOLE_COLOR_WHITE);
	lcd_puts_xy(100,60, (uchar *)"Calibration successful");
	printf("\n\n Calibration Completed...\n\n");
	udelay(3000000); 
	fill_screen(CONSOLE_COLOR_WHITE);
	printf("Touch to print the co-ordinates\n\n");

	return 0;
}

