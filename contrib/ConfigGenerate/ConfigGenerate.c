// Copyright Jetty, Feb 4th 2015

#include <stdio.h>
#include <math.h>

/*
	Generates a basic Hdmilight configuration that has strips of LEDs wired in the following order on 1 output.  The intention
	is to provide a simple configuration to begin with, that can be adapted later:

	Wire in this order:
		Power/Data -> Top (Going Left to Right)
		Right (Going Top to Bottom)
		Bottom (Going Right to Left)
		Left (Going Bottom to Top)

	The following files are generated:
		area.conf
		output_N.conf

	It's assumed that top/bottom led strips  have the same number of leds and left/right leds strips have the same number of leds

	The total number of LEDS must be <=512
*/

// The number of the output we want to generate the files for
#define	OUTPUT_NUM 0 	// Valid values are 0-7 (and integer) and map to outputs 1-8 on the Hdmilight v2 board

// Set to the width / height of the picture (you can obtain this via the GF (get format) command in the serial interface
#define PIXELS_WIDTH  1916	// Must be integer
#define PIXELS_HEIGHT 1080	// Must be integer

// LEDS_X * 2 + LEDS_Y * 2  must be <= 512, if it's not, then your configuration will span more than 1 output, and this
// tool is not for you
#define LEDS_X	73	// Number of X leds (number of LEDs in each of the Top and Bottom Rows), must be integer
#define LEDS_Y	41	// Number of Y leds (number of LEDs in each of the Left and Right Columns, must be integer

// Fine tuning the bounds of the screen
// The screen is divided into a grid of cells of VIDEO_SCALING x VIDEO_SCALING in pixels by the FPGA
// Because the picture may not fit completely in the cell grid, we provide the opportunity here to adjust the 
// the bounds of the cell grid.  Negative values will shift the cell grid boudns to the left, and positive values to the right.
// The starting offsets should be >= 0, and values -2 to 2 would be normal.
// Start off with the values at 0, if you have a left most led in X that is not displaying a color, the video is out of bounds,
// increase by 1 and try again.  If the right most led in X is not correct, then you will want to decrease the end X offset by 1
//
// The cell grid could like this before adjustment, where P is the picture.  Both P and X are units of VIDEO_SCALING * VIDEO_SCALING
// in size:
//  X X X X X X X X X X X X X
//  X P P P P P P P P P P P X
//  X P P P P P P P P P P P X
//  X P P P P P P P P P P P X
//  X P P P P P P P P P P P X
//  X P P P P P P P P P P P X
//
//  To fix this and achieve:
//  P P P P P P P P P P P
//  P P P P P P P P P P P
//  P P P P P P P P P P P
//  P P P P P P P P P P P
//  P P P P P P P P P P P
//
//	CELLGRID_BOUNDS_START_X_OFFSET 	1
//	CELLGRID_BOUNDS_END_X_OFFSET	-1
//	CELLGRID_BOUNDS_START_Y_OFFSET 	1
//	CELLGRID_BOUNDS_END_Y_OFFSET	0
//
#define CELLGRID_BOUNDS_START_X_OFFSET 	0	// Adjustment to the first cell we want to use for the LEDS, must be >=0, typically <=2 and integer
#define CELLGRID_BOUNDS_END_X_OFFSET	0	// Adjustment to the last cell we want to use for the LEDS, typically -2 to +2 and integer
#define CELLGRID_BOUNDS_START_Y_OFFSET 	0	// Adjustment to the first cell we want to use for the LEDS, must be >=0, typically <=2 and integer
#define CELLGRID_BOUNDS_END_Y_OFFSET	0	// Adjustment to the last cell we want to use for the LEDS, typically -2 to +2 and integer

// The cell grid is further divided into areas which surround the outside the screen
// The following are in units of cells and are integers.  The minimum size is 1 cell.
// 
// For the top/bottom rows, AREA_WIDTH is in X and AREA_DEPTH is in Y, i.e AREA_WIDTH=2, AREA_DEPTH = 8, defines an area that is sized
// X: 2 * VIDEO_SCALING and Y:8 * VIDEO_SCALING
//
// For the left/right columns, AREA_WIDTH is in Y and AREA_DEPTH is in X, i.e AREA_WIDTH=2, AREA_DEPTH = 8, defines an area that is sized
// Y: 2 * VIDEO_SCALING and X:8 * VIDEO_SCALING
//
// Normally, where you have more LEDs than cells, you would have an AREA_WIDTH = 1
// If you have significantly fewer LEDS than cells, then AREA_WIWDTH = 2, or maybe 3
//
// AREA_DEPTH controls have far the sample area stretches from the edges of the screen.  Larger numbers take a large
// sample area to calculate the resulting color of the LED
//
// AREA_WIDTH * AREA_HEIGHT must be a power of 2 i.e. valid values are 1, 2, 4, 8, 16, 32 etc.
//
// The number of areas must be <= 255, therefore:
//	 ( ((ceilf(PIXELS_WIDTH / VIDEO_SCALING) / AREA_WIDTH) * 2 + 
//	   ((ceilf(PIXELS_HEIGHT / VIDEO_SCALING) / AREA_WIDTH) * 2 ) must be less than 255
#define AREA_WIDTH 1	// >=1 and integer:  Typically 1 or 2 (if you have fewer LEDs per strip)
#define AREA_DEPTH 8	// >=1 and integer:  Start at 8, and reduce after you've tuned CELLGRID_BOUNDS

// If the span of leds exceed the displayed picture area, there will be leds that should be dark,
// but because they are not are causing leds to not line up with the picture.
// Force the leds to be ignored by adjusting the bounds of the LED causing outermost leds to be ignored 
#define LEDS_BOUNDS_START_X_OFFSET 	0	// Adjustment to the first led we want to use, must be >=0, typically <=2 and integer
#define LEDS_BOUNDS_END_X_OFFSET	0	// Adjustment to the last led we want to use, must be <= 0, typically >=-2 and integer
#define LEDS_BOUNDS_START_Y_OFFSET 	0	// Adjustment to the first led we want to use, must be >=0, typically <=2 and integer
#define LEDS_BOUNDS_END_Y_OFFSET	0	// Adjustment to the last led we want to use, must be <=0, typically <=-2 and integer

// The color value for the output table for leds in use
#define COLOR_ACTIVE 0

// The color value for the output table for leds that are disabled
// Set this color in the color_N.conf to black (0's for everything)
#define COLOR_DISABLED 1

// The gamma value for the output table
#define GAMMA 1




// DO NOT CHANGE BELOW HERE
#define VIDEO_SCALING 32	// Incoming video is scaled down by this amount in X/Y by the FPGA.  I.e. 1920x1080 / 32 = 60x33
				// Scaling is independent of resolution, so 640x480 is 20x15

#define AREA_CONF		"area.conf"
#define OUTPUT_CONF_PART1	"output_"
#define OUTPUT_CONF_PART2	".conf"

#define LEDS_WIDTH  ((LEDS_X) + (LEDS_BOUNDS_END_X_OFFSET) - (LEDS_BOUNDS_START_X_OFFSET))
#define LEDS_HEIGHT ((LEDS_Y) + (LEDS_BOUNDS_END_Y_OFFSET) - (LEDS_BOUNDS_START_Y_OFFSET))



// Returns true if v is integer
// otherwise false

int isInteger(float v)
{
	if (((float)((int)v)) == v )	return 1;
	else				return 0;
}



// Verifies the settings are valid
// Returns true if okay, otherwise false

int verifySettings(void)
{
	if ( OUTPUT_NUM < 0 || OUTPUT_NUM > 7 || ( ! isInteger(OUTPUT_NUM) ) )	{fprintf(stderr, "Error: OUTPUT_NUM should be 0..7 and integer\n");return 0;}
	if ( PIXELS_WIDTH < 1 || (! isInteger(PIXELS_WIDTH) ) )			{fprintf(stderr, "Error: PIXELS_WIDTH should be >=1 and integer\n");return 0;}
	if ( PIXELS_HEIGHT < 1 || (! isInteger(PIXELS_HEIGHT) ) )		{fprintf(stderr, "Error: PIXELS_HEIGHT should be >=1 and integer\n");return 0;}
	if ( LEDS_X < 1 || (! isInteger(LEDS_X) ) )				{fprintf(stderr, "Error: LEDS_X should be >=1 and integer\n");return 0;}
	if ( LEDS_Y < 1 || (! isInteger(LEDS_Y) ) )				{fprintf(stderr, "Error: LEDS_Y should be >=1 and integer\n");return 0;}

	if (( LEDS_X * 2 + LEDS_Y * 2 ) > 512)
		{fprintf(stderr, "Error: Total number of LEDs is greater than 512 and will exceed 1 output, this utility is not for you!\n");return 0;}

	if ( CELLGRID_BOUNDS_START_X_OFFSET < 0 || (! isInteger(CELLGRID_BOUNDS_START_X_OFFSET) ) )
										{fprintf(stderr, "Error: CELLGRID_BOUNDS_START_X_OFFSET should be >=0 and integer\n");return 0;}
	if ( ! isInteger(CELLGRID_BOUNDS_END_X_OFFSET) )			{fprintf(stderr, "Error: CELLGRID_BOUNDS_END_X_OFFSET should be integer\n");return 0;}
	if ( CELLGRID_BOUNDS_START_Y_OFFSET < 0 || (! isInteger(CELLGRID_BOUNDS_START_Y_OFFSET) ) )
										{fprintf(stderr, "Error: CELLGRID_BOUNDS_START_Y_OFFSET should be >=0 and integer\n");return 0;}
	if ( ! isInteger(CELLGRID_BOUNDS_END_Y_OFFSET) )			{fprintf(stderr, "Error: CELLGRID_BOUNDS_END_Y_OFFSET should be integer\n");return 0;}
	if ( AREA_WIDTH < 1 || (! isInteger(AREA_WIDTH) ) )			{fprintf(stderr, "Error: AREA_WIDTH should be >=1 and integer\n");return 0;}
	if ( AREA_DEPTH < 1 || (! isInteger(AREA_DEPTH) ) )			{fprintf(stderr, "Error: AREA_DEPTH should be >=1 and integer\n");return 0;}
	
	// Test for a power of 2 by shifting to the right
	int size = AREA_WIDTH * AREA_DEPTH;
	while ( size > 0 )
	{
		// Tes for more than one bit set
		if ( size > 1 && (size & 1))	break;
		size >>= 1;	// Shift right
	}

	// If size is not zero, then we broke out of the while above due to more than one bit being set
	// If size is 0, then we only had one bit set (i.e. a power of 2)
	if ( size )	{fprintf(stderr, "Error: AREA_WIDTH * AREA_DEPTH should be a power of 2\n");return 0;}

	if  ( ((ceilf((float)PIXELS_WIDTH  / (float)VIDEO_SCALING) / (float)AREA_WIDTH) * 2.0 +
	      ((ceilf((float)PIXELS_HEIGHT / (float)VIDEO_SCALING) / (float)AREA_WIDTH) * 2.0) ) > 255.0 )
			{fprintf(stderr, "Error: 255 Areas exceeded, increase AREA_WIDTH by 1\n");return 0;}

	if ( LEDS_BOUNDS_START_X_OFFSET < 0 || ( ! isInteger(LEDS_BOUNDS_START_X_OFFSET) ) )
		{fprintf(stderr, "Error: LEDS_BOUNDS_START_X_OFFSET should be >=0 and integer\n");return 0;}
	if ( LEDS_BOUNDS_START_Y_OFFSET < 0 || ( ! isInteger(LEDS_BOUNDS_START_Y_OFFSET) ) )
		{fprintf(stderr, "Error: LEDS_BOUNDS_START_Y_OFFSET should be >=0 and integer\n");return 0;}
	if ( LEDS_BOUNDS_END_X_OFFSET > 0 || ( ! isInteger(LEDS_BOUNDS_END_X_OFFSET) ) )
		{fprintf(stderr, "Error: LEDS_BOUNDS_END_X_OFFSET should be <=0 and integer\n");return 0;}
	if ( LEDS_BOUNDS_END_Y_OFFSET > 0 || ( ! isInteger(LEDS_BOUNDS_END_Y_OFFSET) ) )
		{fprintf(stderr, "Error: LEDS_BOUNDS_END_Y_OFFSET should be <=0 and integer\n");return 0;}
	
	return 1;
}



void areaConfHeader(FILE *fp)
{
	fprintf(fp, "# xmin  xmax  ymin  ymax  divshift area_number\n");
}



FILE *areaConfCreate(void)
{
	FILE *fp, *fopen();

	if (( fp = fopen(AREA_CONF, "w")) == NULL )
		perror(AREA_CONF);

	return fp;
}



void outputConfHeader(FILE *fp)
{
	fprintf(fp, "# index    area    colour    gamma    enable\n");
}



FILE *outputConfCreate(void)
{
	char filename[20];
	FILE *fp, *fopen();

	sprintf(filename, "%s%d%s", OUTPUT_CONF_PART1, OUTPUT_NUM, OUTPUT_CONF_PART2);
	if (( fp = fopen(filename, "w")) == NULL )
		perror(filename);

	return fp;
}



int areaConfGenerate(FILE *fp, int *areasX, int *areasY)
{
	int i, divshift = 0;

	*areasX = *areasY = 0;

	// Figure out divshift, which is a power of 2, we do this by shifting until areaArea is 0
	int areaArea = AREA_WIDTH * AREA_DEPTH;

	while ( ( areaArea >>= 1 ) )
		divshift ++;
	
	printf("divshift calculated: %d\n", divshift);

	// Calculate the min and max cell values
	int cellMaxX = (int)ceilf((float)PIXELS_WIDTH  / (float)VIDEO_SCALING);
	int cellMaxY = (int)ceilf((float)PIXELS_HEIGHT / (float)VIDEO_SCALING);
	int cellMinX = 0;
	int cellMinY = 0;

	// Adjust the min/max cell values by the bounds offset
	cellMinX += CELLGRID_BOUNDS_START_X_OFFSET;
	cellMaxX += CELLGRID_BOUNDS_END_X_OFFSET;
	cellMinY += CELLGRID_BOUNDS_START_Y_OFFSET;
	cellMaxY += CELLGRID_BOUNDS_END_Y_OFFSET;

	int countAreas = 0;

	// Top row running left to right
	fprintf(fp, "# Top Row running left to right\n");
	areaConfHeader(fp);
	for ( i = cellMinX; i <= cellMaxX; i += AREA_WIDTH )
		{ fprintf(fp, "  %-5d %-5d %-5d %-5d %-8d # %d\n", i, (i + AREA_WIDTH) - 1, cellMinY, (cellMinY + AREA_DEPTH) - 1, divshift, countAreas++); (*areasX) ++; }
	
	// Right column running top to bottom
	fprintf(fp, "# Right Column running top to bottom\n");
	areaConfHeader(fp);
	for ( i = cellMinY; i <= cellMaxY; i += AREA_WIDTH )
		{ fprintf(fp, "  %-5d %-5d %-5d %-5d %-8d # %d\n", (cellMaxX - AREA_DEPTH) + 1, cellMaxX, i, (i + AREA_WIDTH) - 1, divshift, countAreas++); (*areasY) ++; }

	// Bottom row running left to right
	fprintf(fp, "# Bottom Row running left to right\n");
	areaConfHeader(fp);
	for ( i = cellMinX; i <= cellMaxX; i += AREA_WIDTH )
		{ fprintf(fp, "  %-5d %-5d %-5d %-5d %-8d # %d\n", i, (i + AREA_WIDTH) - 1, (cellMaxY - AREA_DEPTH) + 1, cellMaxY, divshift, countAreas++); }

	// Left column running top to bottom
	fprintf(fp, "# Left Column running top to bottom\n");
	areaConfHeader(fp);
	for ( i = cellMinY; i <= cellMaxY; i += AREA_WIDTH )
		{ fprintf(fp, "  %-5d %-5d %-5d %-5d %-8d # %d\n", cellMinX, (cellMinX + AREA_DEPTH ) - 1, i, (i + AREA_WIDTH) - 1, divshift, countAreas++); }

	printf("Number of areas: %d XAreas: %d YAreas: %d\n", countAreas, *areasX, *areasY);

	return countAreas;
}



int getAreaForLedX(int ind, int areasX)
{
	return (int)(((float)ind / (float)LEDS_WIDTH) * (float)areasX);
}



int getAreaForLedY(int ind, int areasY)
{
	return (int)(((float)ind / (float)LEDS_HEIGHT) * (float)areasY);
}



int dumpDisabledLed(FILE *fp, int count, int ledCount)
{
	int i;

	for ( i = 0; i < count; i ++, ledCount ++ )
		fprintf(fp, "%7d %7d %9d %8d %9d\n", ledCount, 0, COLOR_DISABLED, GAMMA, 1);

	return ledCount++;
}



int dumpActiveLed(FILE *fp, int from, int to, int ledCount, int baseArea, int x, int areas)
{
	int i;

	if 	( to > from )
	{
		for ( i = from; i < to; i ++, ledCount ++ )
			fprintf(fp, "%7d %7d %9d %8d %9d\n", ledCount, baseArea + ((x) ? getAreaForLedX(i, areas) : getAreaForLedY(i, areas)), COLOR_ACTIVE, GAMMA, 1);
	}
	else if ( to < from )
	{
		for ( i = from - 1; i >= to; i --, ledCount ++ )
			fprintf(fp, "%7d %7d %9d %8d %9d\n", ledCount, baseArea + ((x) ? getAreaForLedX(i, areas) : getAreaForLedY(i, areas)), COLOR_ACTIVE, GAMMA, 1);
	}


	return ledCount;
}



void outputConfGenerate(FILE *fp, int areasX, int areasY)
{
	int i, ledCount = 0, baseArea = 0;

	// Top Row running left to right
	fprintf(fp, "# Top Row running left to right\n");
	outputConfHeader(fp);
	ledCount = dumpDisabledLed(fp, LEDS_BOUNDS_START_X_OFFSET, ledCount);
	ledCount = dumpActiveLed(fp, 0, LEDS_WIDTH, ledCount, baseArea, 1, areasX);
	ledCount = dumpDisabledLed(fp, -LEDS_BOUNDS_END_X_OFFSET, ledCount);

	// Right Column running top to bottom
	fprintf(fp, "# Right Column running top to bottom\n");
	outputConfHeader(fp);
	baseArea += areasX;
	ledCount = dumpDisabledLed(fp, LEDS_BOUNDS_START_Y_OFFSET, ledCount);
	ledCount = dumpActiveLed(fp, 0, LEDS_HEIGHT, ledCount, baseArea, 0, areasY);
	ledCount = dumpDisabledLed(fp, -LEDS_BOUNDS_END_Y_OFFSET, ledCount);

	// Bottom Row running right to left
	fprintf(fp, "# Bottom Row running right to left\n");
	outputConfHeader(fp);
	baseArea += areasY;
	ledCount = dumpDisabledLed(fp, -LEDS_BOUNDS_END_X_OFFSET, ledCount);
	ledCount = dumpActiveLed(fp, LEDS_WIDTH, 0, ledCount, baseArea, 1, areasX);
	ledCount = dumpDisabledLed(fp, LEDS_BOUNDS_START_X_OFFSET, ledCount);

	// Left Column running bottom to top
	fprintf(fp, "# Left Column running bottom to top\n");
	outputConfHeader(fp);
	baseArea += areasX;
	ledCount = dumpDisabledLed(fp, -LEDS_BOUNDS_END_Y_OFFSET, ledCount);
	ledCount = dumpActiveLed(fp, LEDS_HEIGHT, 0, ledCount, baseArea, 0, areasY);
	ledCount = dumpDisabledLed(fp, LEDS_BOUNDS_START_Y_OFFSET, ledCount);

	printf("%d leds written to output, filled to 512\n", ledCount);

	// Disable the remainder of the leds in the channel
	fprintf(fp, "# Remaining LEDs are disabled\n");
	outputConfHeader(fp);
	ledCount = dumpDisabledLed(fp, 512-ledCount, ledCount);
}



int main(void)
{
	int areasX, areasY;
	FILE *areaConfFp, *outputConfFp;

	if ( ! verifySettings() )	return 1;

	// Open the files and create the headers
	if (( areaConfFp   = areaConfCreate()   ) == NULL )	return 2;
	if (( outputConfFp = outputConfCreate() ) == NULL )	return 3;

	// Generate area file contents
	areaConfGenerate(areaConfFp, &areasX, &areasY);

	// Generate output file contents
	outputConfGenerate(outputConfFp, areasX, areasY);

	// Close the files
	fclose(outputConfFp);
	fclose(areaConfFp);

	return 0;
}