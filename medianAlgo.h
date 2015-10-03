#ifndef MEDIAN_ALGO_H
#define MEDIAN_ALGO_H

#include <stdlib.h>
#include <stdint.h>

struct node
{
	struct node * next;
	struct node * last;
	uint32_t value;
};

typedef struct node node_t;

void InitMedianFilter(node_t * circularBufferPtr, const size_t windowSizeVal);
void IncreaseWindowSize(const size_t dynamicWindowSizeVal);
uint32_t MedianFilter(const uint32_t inputData);

#endif
