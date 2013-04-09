#pragma once

#include "afxmt.h"
#include "horn.h"
#include "afxwin.h"
#include "threads.h"
#include <omp.h>
#include <iostream>
#include <fstream>
using namespace std;

#define MAX_THREADS 4 //Create one thread per processor

UINT calc_statistics_proc(LPVOID );

typedef struct{
	CCriticalSection pcs;
	int *threadID;
	int current_id;
	int n;
	int pic_x;
	int pic_y;
	int size_x;
	int size_y;
	float (*correct_vels)[PIC_Y][2]; //Assign first dimension pointer to existing data
	float *min_angle;
	float *max_angle;
	float (*full_vels)[PIC_Y][2];
	float *ave_error;
	float *density;
	float *st_dev;
	//local variables
	int full_count;
	int no_full_count;
	int total_count;
	float sumX2;

} calc_statistics_data;

