#include "medianAlgo.h"

#define INIT_VALUE	0
 
static uint32_t ( * CalcMedian)(const uint32_t, const uint32_t);

static uint32_t EvenCalcMedian(const uint32_t, const uint32_t);
static uint32_t OddCalcMedian(const uint32_t, const uint32_t);

static void IncrementDataPoiner(void);

static node_t * circularBuffer;
static node_t * dataPointer;
static size_t windowSize, dynamicWindowSize;
static node_t big, small;

static uint32_t OddCalcMedian(const uint32_t left, const uint32_t right)
{
	return left;
}

static uint32_t EvenCalcMedian(const uint32_t left, const uint32_t right)
{
	//right is always more or equal than left
	return (left + (right - left)/2);
}

//Init the chain nodes, clear nodes values, set CalcMedian function
void InitMedianFilter(node_t * circularBufferPtr, const size_t windowSizeVal)
{
	size_t i;
	circularBuffer = circularBufferPtr;
	windowSize = windowSizeVal;
	dynamicWindowSize = windowSizeVal;
	small.value = INIT_VALUE;
	big.value = INIT_VALUE;
	for(i = 0; i < windowSize; i++) 
	{
		circularBuffer[i].next = &circularBuffer[i+1];
		circularBuffer[i].last = &circularBuffer[i-1];
		circularBuffer[i].value = INIT_VALUE;
	}
	circularBuffer[0].last = &small;
	circularBuffer[i-1].next = &big;
	small.next = circularBuffer;
	big.last = &circularBuffer[i-1];
	dataPointer = circularBuffer;
	//check windowSize: even or odd
	if(windowSize%2) CalcMedian = OddCalcMedian;
	else CalcMedian = EvenCalcMedian;
}

//Can be used to increase window size at the beginning of filter process
void IncreaseWindowSize(const size_t dynamicWindowSizeVal)
{
	dynamicWindowSize = dynamicWindowSizeVal;
	//check dynamicWindowSize: even or odd and choice right way to calculate median
	if(dynamicWindowSize%2) CalcMedian = OddCalcMedian;
	else CalcMedian = EvenCalcMedian;
}

//Increment and wrap data in pointer.
static void IncrementDataPoiner(void)
{
	if ((++dataPointer - circularBuffer) >= windowSize)
		dataPointer = circularBuffer;
}

//remove oldest node and add inputData instead of it in correct position.
//calculate median value and return it.
uint32_t MedianFilter(const uint32_t inputData)
{	
	node_t * scan;		// Pointer used to scan up/down the sorted list
	size_t i = 0;
	node_t * median;	// Pointer to median
	
	if(dataPointer->value > inputData)		//delete value > insert value
	{
		//remove dataPointer node:
		dataPointer->next->last = dataPointer->last;
		dataPointer->last->next = dataPointer->next;
		
		//scan to &small
		scan = dataPointer->next;
		while(1)
		{
			if((scan->value > inputData) && (scan != &small))
			{
				scan = scan->last;
			}
			else
			{
				if(scan == &big) scan = scan->last;
				else break;
			}
		}
		//insert value and update pointers
		dataPointer->last = scan;
		dataPointer->next = scan->next;
		scan->next->last = dataPointer;
		scan->next = dataPointer;		
		dataPointer->value = inputData;
	}
	else
	{
		if(dataPointer->value < inputData)	//delete value < insert value 
		{
			//remove dataPointer node:
			dataPointer->next->last = dataPointer->last;
			dataPointer->last->next = dataPointer->next;
			
			//scan to &big
			scan = dataPointer->last;
			while(1)
			{
				if((scan->value < inputData) && (scan != &big))
				{
					scan = scan->next;
				}
				else
				{
					if(scan == &small) scan = scan->next;
					else break;
				}
			}
			//insert value and update pointers
			dataPointer->next = scan;
			dataPointer->last = scan->last;
			scan->last->next = dataPointer;
			scan->last = dataPointer;			
			dataPointer->value = inputData;
		}		
	}
	//find median position, start from big pointer
	scan = big.last;
	for(i = 0; i < dynamicWindowSize/2; i++) scan = scan->last;
	median = scan;
	IncrementDataPoiner();
	return CalcMedian(median->value, median->next->value);
}
