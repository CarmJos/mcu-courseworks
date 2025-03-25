// This file contains the prototypes of the functions defined in base.c
#ifndef BASE_H
#define BASE_H

#include <reg51.h>
#include <intrins.h>
#define TRUE 1
#define FALSE 0

#define uchar unsigned char
#define uint unsigned int

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

typedef signed char int8;
typedef signed int int16;
typedef signed long int32;

typedef enum { false, true } bool;

void delay(uint16 i);

#endif // BASE_H

