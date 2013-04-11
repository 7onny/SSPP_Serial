#pragma once

#include "afxmt.h"
#include "horn.h"
#include "afxwin.h"
#include "threads.h"
#include <omp.h>
#include <iostream>
#include <fstream>
using namespace std;

#define MAX_THREADS 1 //Create one thread per processor

extern HANDLE events[MAX_THREADS];

//Declaration of global timers
extern double vels_avg_time;
extern double calc_vels_time;
extern double rearrange_time;
extern double calc_statistics_time;
//Declaration of global call counters
extern int vels_avg_count;
extern int calc_vels_count;
extern int rearrange_count;
extern int calc_statistics_count;

UINT thread_proc(LPVOID );

typedef struct{
	CCriticalSection pcs;
	int current_id;
	int numpass;
	int finish_counter; //Used to control all threads reaching a given point
	//Additional necessary variables
	int offset,full_count, no_full_count, total_count;
	float ave_error,min_angle,max_angle,sumX2,st_dev,density;
} threadData;

//Parallelized functions
void MT_vels_avg(float vels[PIC_X][PIC_Y][2],float ave[PIC_X][PIC_Y][2], int id);
void MT_calc_vels(float vels[PIC_X][PIC_Y][2],float vels1[PIC_X][PIC_Y][2],float Ex[PIC_X][PIC_Y],float Ey[PIC_X][PIC_Y],float Et[PIC_X][PIC_Y], int id);
void MT_rearrange(float v1[PIC_X][PIC_Y][2],float v2[PIC_X][PIC_Y][2], int id);
void MT_calc_statistics(float correct_vels[PIC_X][PIC_Y][2],int int_size_x,int int_size_y,float full_vels[PIC_X][PIC_Y][2],
						int pic_x,int pic_y,int n,float *ave_error,float *st_dev,float *density,float *min_angle,float *max_angle, int id, LPVOID data);

//Utility functions
void createEvents();
void closeEvents();
void resetThreadData(LPVOID data);
