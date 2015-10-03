#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "medianAlgo.h"

#define MAX_MEDIAN_WINDOW	(long)1000
#define MIN_MEDIAN_WINDOW	(long)3
#define BLOCK_SIZE	(size_t)8192

#define VERSION "1.1"

static struct globalArgs_t 
{
	int notBufferStdout;		// parameter -b : not buffer stdout
	size_t windowSize;			// parameter -w : size of median filter window
} globalArgs;

static uint32_t ioBuff[BLOCK_SIZE];

static void PrintHelp(void);
static void PrintVersion(void);
static void PrintInvalidArg(const char * string);
static void ParseComandArgs(int argc, char** argv);
static void GlobalArgsInit(void);
static size_t ReadWindowSize(const char * string);
static void PrintErrorAndExit(const char * string);

static void PrintInvalidArg(const char * string)
{
	fprintf(stderr, "median: invalid option: %s\n\
	Try median -h for more information.\n", string);
	exit(EXIT_FAILURE);
}

static void PrintVersion(void)
{
	fprintf(stdout, "median: version: %s\n", VERSION);
	exit(EXIT_SUCCESS);
}

static size_t ReadWindowSize(const char * string)
{
	long windowSize = 0;
	char *endptr;
	windowSize = strtol(string, &endptr, 10);
	if(*endptr != '\0') PrintInvalidArg(string);
	if(windowSize > MAX_MEDIAN_WINDOW) PrintInvalidArg("window size is too big");
	if(windowSize < MIN_MEDIAN_WINDOW) PrintInvalidArg("window size is too small");
	return (size_t)windowSize;
}

static void PrintErrorAndExit(const char * string)
{
	fprintf(stderr, "median: %s\n", string);
	exit(EXIT_FAILURE);
}

static void GlobalArgsInit(void)
{
	globalArgs.notBufferStdout = 0;
	globalArgs.windowSize = 0;
}

static void PrintHelp(void)
{
	fprintf(stdout, "Usage: median [OPTION]\n\
Read data from standard input, filter them using median filter \
and write data to standard output\n\
Data read and write as 32-bit unsigned number (uint32_t)\n\
Options:\n\
-w M         set size of median filter window. This is mandatory option. \
The minimum window size is %ld and maximum size is %ld\n\
-b           do not buffer standard output stream.\n\
-v           display program version and exit.\n\
-h           display this help and exit.\n\
Note that when median read number of data instance less M it calculates \
median using actual median filter window less then M\n\
(M is required size of median filter window)\n", MIN_MEDIAN_WINDOW, MAX_MEDIAN_WINDOW);
	fprintf(stdout, "Exit status:\n\
 %d  if OK,\n %d  if problems.\n", EXIT_SUCCESS, EXIT_FAILURE);
	
	exit(EXIT_SUCCESS);
}

//median -h
//median -v
//median -w window_size
//median -b -w window_size
static void ParseComandArgs(int argc, char** argv)
{
	static const char *optString = "bw:hv";
	int option;
	
	while((option = getopt(argc, argv, optString)) != -1)
	{
		switch(option)
		{			
			case 'b':
			globalArgs.notBufferStdout = 1;
			break;
			
			case 'h':
			PrintHelp();
			break;
			
			case 'w':
			globalArgs.windowSize = ReadWindowSize(optarg);
			break;
			
			case 'v':
			PrintVersion();
			break;
			
			default:
			exit(EXIT_FAILURE);
			break;
		}
	}
	if(!globalArgs.windowSize) PrintInvalidArg("option -w is mandatory");
}

int main(int argc, char** argv)
{
	size_t i, ioDataCount, buffCounter;
	node_t * circularBufferPtr;
	
	GlobalArgsInit();
	ParseComandArgs(argc, argv);

	circularBufferPtr = (node_t *)malloc(sizeof(node_t) * globalArgs.windowSize);
	if(circularBufferPtr == NULL) PrintErrorAndExit("can not allocate enough memory");
	if(globalArgs.notBufferStdout) setbuf(stdout, NULL);		
	InitMedianFilter(circularBufferPtr, globalArgs.windowSize);	
	
	for(buffCounter = 0; buffCounter < globalArgs.windowSize;)
	{
		ioDataCount = fread(&ioBuff, sizeof(uint32_t), (globalArgs.windowSize - buffCounter), stdin);
		if(!ioDataCount) 
		{
			free(circularBufferPtr);
			exit(EXIT_SUCCESS);
		}
		for(i = 0; i < ioDataCount; i++)
		{
			IncreaseWindowSize(buffCounter + i + 1);
			ioBuff[i] = MedianFilter(ioBuff[i]);
		}
		if(fwrite(&ioBuff, sizeof(uint32_t), ioDataCount, stdout) != ioDataCount) 
		{
			free(circularBufferPtr);			
			PrintErrorAndExit("can not write to standard output");
		}
		buffCounter += ioDataCount;
	}
	IncreaseWindowSize(globalArgs.windowSize);	
	
	while((ioDataCount = fread(&ioBuff, sizeof(uint32_t), BLOCK_SIZE, stdin)))
	{
		for(i = 0; i < ioDataCount; i++) ioBuff[i] = MedianFilter(ioBuff[i]);
		if(fwrite(&ioBuff, sizeof(uint32_t), ioDataCount, stdout) != ioDataCount) 
		{
			free(circularBufferPtr);			
			PrintErrorAndExit("can not write to standard output");
		}
	}	
	free(circularBufferPtr);
	exit(EXIT_SUCCESS);
}
