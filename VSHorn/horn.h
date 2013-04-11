#pragma once
/*********************************************************************/
/*   HORN INCLUDEs and DEFINEs 						     */
/*********************************************************************/
#define _USE_MATH_DEFINES


#include  <fcntl.h>
#include <io.h>
#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "rasterfile.h"
#include <sys/stat.h>



// Constants
#define BORDER 2
#define HEAD 32
//#define PI  M_PI
#define PIC_X 675
#define PIC_Y 675
#define PIC_T 15
#define FIVE 5
#define PMODE 0644
#define TRUE 1
#define FALSE 0
#define HEAD 32
#define NO_VALUE 100.0
#define OUTPUT_SMOOTH 1
#define BIG_MAG 1000000.0


// Function prototypes
void read_and_smooth3D(char path[100],char stem[100],float sigma,float floatpic[FIVE][PIC_X][PIC_Y],unsigned char pic[FIVE][PIC_X][PIC_Y],unsigned char inpic[PIC_T][PIC_X][PIC_Y],int start,int middle,int end,unsigned char header[HEAD]);
void writefiles(char path[100],char s[100],unsigned char result[FIVE][PIC_X][PIC_Y],float sigma,int pic_t,int pic_x,int pic_y,int start,int end,unsigned char header[HEAD]);
void readfiles(char path[100],char s[100],unsigned char pic[PIC_T][PIC_X][PIC_Y],int pic_t,int *pic_x,int *pic_y,int start,int end,unsigned char header[HEAD]);
void calcIx(float Ex[PIC_X][PIC_Y],float floatpic[FIVE][PIC_X][PIC_Y],int t);
void calcIy(float Ey[PIC_X][PIC_Y],float floatpic[FIVE][PIC_X][PIC_Y],int t);
void calcIt(float Et[PIC_X][PIC_Y],float floatpic[FIVE][PIC_X][PIC_Y],int t);
void vels_avg(float vels[PIC_X][PIC_Y][2],float ave[PIC_X][PIC_Y][2]);
void calc_vels(float vels[PIC_X][PIC_Y][2],float vels1[PIC_X][PIC_Y][2],float Ex[PIC_X][PIC_Y],float Ey[PIC_X][PIC_Y],float Et[PIC_X][PIC_Y]);
void print_ders(float Ex[PIC_X][PIC_Y],float Ey[PIC_X][PIC_Y],float Et[PIC_X][PIC_Y]);
void compute_ders(float Ix[PIC_X][PIC_Y],float Iy[PIC_X][PIC_Y],float It[PIC_X][PIC_Y],float floatpic[PIC_T][PIC_X][PIC_Y],int pic_t,int pic_x,int pic_y,int n);
void calc_diff_kernel(float diff_kernel[5]);
float diff_x(float floatpic[PIC_T][PIC_X][PIC_Y],float kernel[5],int x,int y,int n);
float diff_y(float floatpic[PIC_T][PIC_X][PIC_Y],float kernel[5],int x,int y,int n);
float diff_t(float floatpic[PIC_T][PIC_X][PIC_Y],float kernel[5],int x,int y,int n);
float difference(float v1[PIC_X][PIC_Y][2],float v2[PIC_X][PIC_Y][2],int pic_x,int pic_y);
void rearrange(float v1[PIC_X][PIC_Y][2],float v2[PIC_X][PIC_Y][2]);
void threshold(float full_vels[PIC_X][PIC_Y][2],float Ix[PIC_X][PIC_Y],float Iy[PIC_X][PIC_Y],float tau,int pic_x,int pic_y);
void calc_statistics(float correct_vels[PIC_X][PIC_Y][2],int int_size_x,int int_size_y,float full_vels[PIC_X][PIC_Y][2],int pic_x,int pic_y,int n,float *ave_error,float *st_dev,float *density,float *min_angle,float *max_angle);
void output_velocities(int fdf,char s[100],float full_velocities[PIC_X][PIC_Y][2],int pic_x,int pic_y,int n);
void convolve_Gaussian(unsigned char inpic[PIC_T][PIC_X][PIC_Y],float floatpic[FIVE][PIC_X][PIC_Y],unsigned char pic[FIVE][PIC_X][PIC_Y],float sigma,int pic_t,int pic_x,int pic_y,int start,int frame,int time);
float PsiER(float ve[2],float va[2]);
float norm(float v[],int n);
float fmin(float x,float y);

//Utility functions
void print_times(double total_elapsed_time);
void print_plot_data();

// Global variables
extern unsigned char inpic[PIC_T][PIC_X][PIC_Y];
extern unsigned char pic[FIVE][PIC_X][PIC_Y];
extern float floatpic[FIVE][PIC_X][PIC_Y];
extern unsigned header[HEAD];
extern float alpha;
extern float Ix[PIC_X][PIC_Y],Iy[PIC_X][PIC_Y];
extern float It[PIC_X][PIC_Y],full_vels[PIC_X][PIC_Y][2];
extern float correct_vels[PIC_X][PIC_Y][2],full_vels1[PIC_X][PIC_Y][2]; 
extern float temp_vels[PIC_X][PIC_Y][2];
extern int pic_x,pic_y,pic_t,THRESHOLD,STANDARD,BINARY,int_size_x,int_size_y;
extern float actual_x,actual_y,size_x,size_y,offset_x,offset_y;
extern int startx,starty,endx,endy,step,WRITE_SMOOTH;
