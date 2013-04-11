#include "stdafx.h"
#include "threads.h"

UINT thread_proc(LPVOID data){
	threadData *td=(threadData*)data;
	CSingleLock singleLock(&td->pcs);
	int id=td->current_id; //IDs will range from 0 to MAX_THREADS-1
	InterlockedIncrement((LPLONG)&(td->current_id));
	createEvents();
	for(int i=0;i<td->numpass;i+=2) 
	{
		printf("%3dth iteration\n",i);
		fflush(stdout);
		
		MT_calc_vels(full_vels,full_vels1,Ix,Iy,It,id);
		WaitForSingleObject(events[id],INFINITE);

		printf("The improvement: %f\n",difference(full_vels,full_vels1,pic_x,pic_y));
		fflush(stdout);
		
		MT_rearrange(full_vels1,temp_vels,id);
		WaitForSingleObject(events[id],INFINITE);

		MT_calc_statistics(correct_vels,int_size_x,int_size_y,temp_vels,
			pic_x,pic_y,2*(td->offset),&(td->ave_error),&(td->st_dev),&(td->density),&(td->min_angle),&(td->max_angle),id,td);
		WaitForSingleObject(events[id],INFINITE);

		printf("Error: %f St Dev: %f Density: %f\n",td->ave_error,td->st_dev,td->density);
		fflush(stdout);
		printf("%3dth iteration\n",i+1);
		
		MT_calc_vels(full_vels1,full_vels,Ix,Iy,It,id);
		WaitForSingleObject(events[id],INFINITE);

		printf("The improvement: %f\n",difference(full_vels,full_vels1,pic_x,pic_y));
		
		MT_rearrange(full_vels,temp_vels,id);
		WaitForSingleObject(events[id],INFINITE);

		MT_calc_statistics(correct_vels,int_size_x,int_size_y,temp_vels,
			pic_x,pic_y,2*(td->offset),&(td->ave_error),&(td->st_dev),&(td->density),&(td->min_angle),&(td->max_angle),id,td);
		WaitForSingleObject(events[id],INFINITE);

		printf("Error: %f St Dev: %f Density: %f\n",td->ave_error,td->st_dev,td->density);
		fflush(stdout);
	}
	closeEvents();
	return 0;
}

//----------------------------------Parallelized functions---------------------------------------
void MT_vels_avg(float vels[PIC_X][PIC_Y][2],float ave[PIC_X][PIC_Y][2], int id)
	//float vels[PIC_X][PIC_Y][2],ave[PIC_X][PIC_Y][2];
{
	double launch_time=omp_get_wtime();
	InterlockedIncrement((LPLONG)&vels_avg_count);

	int i,j;
	for(i=startx;i<endx;i+=MAX_THREADS)
		for(j=starty;j<endy;j++)
		{
			ave[i][j][0] = (vels[i-1][j][0]+vels[i][j+1][0]+
				vels[i+1][j][0]+vels[i][j-1][0])/6.0 +
				(vels[i-1][j-1][0]+vels[i-1][j+1][0]+
				vels[i+1][j+1][0]+vels[i+1][j-1][0])/12.0;
			ave[i][j][1] = (vels[i-1][j][1]+vels[i][j+1][1]+
				vels[i+1][j][1]+vels[i][j-1][1])/6.0 +
				(vels[i-1][j-1][1]+vels[i-1][j+1][1]+
				vels[i+1][j+1][1]+vels[i+1][j-1][1])/12.0;
		}
	double end_time=omp_get_wtime();
	vels_avg_time+=end_time-launch_time;
	SetEvent(events[id]);
}

void MT_calc_vels(float vels[PIC_X][PIC_Y][2],float vels1[PIC_X][PIC_Y][2],float Ex[PIC_X][PIC_Y],float Ey[PIC_X][PIC_Y],float Et[PIC_X][PIC_Y], int id)
	//float vels[PIC_X][PIC_Y][2],vels1[PIC_X][PIC_Y][2];
	//float Ex[PIC_X][PIC_Y],Ey[PIC_X][PIC_Y],Et[PIC_X][PIC_Y];
{
	double launch_time=omp_get_wtime();
	InterlockedIncrement((LPLONG)&calc_vels_count);

	int i,j,k;
	float mag,ave[PIC_X][PIC_Y][2];

	printf("****** Computing Velocity ******\n"); 
	fflush(stdout);
	vels_avg(vels1,ave);
	WaitForSingleObject(events[id],INFINITE);

	for(i=startx;i<=endx;i+=(MAX_THREADS*step))
		for(j=starty;j<=endy;j+=step) 
		{
			vels[i][j][0] = ave[i][j][0]-Ex[i][j]*
				(Ex[i][j]*ave[i][j][0]+Ey[i][j]*ave[i][j][1]+Et[i][j])
				/(alpha*alpha+Ex[i][j]*Ex[i][j]+Ey[i][j]*Ey[i][j]);
			vels[i][j][1] = ave[i][j][1]-Ey[i][j]*
				(Ex[i][j]*ave[i][j][0]+Ey[i][j]*ave[i][j][1]+Et[i][j])
				/(alpha*alpha+Ex[i][j]*Ex[i][j]+Ey[i][j]*Ey[i][j]);
			mag = sqrt(vels[i][j][0]*vels[i][j][0]+vels[i][j][1]*vels[i][j][1]);
			if(mag > 5.0 && FALSE) 
			{
				printf("Velocity magnitude of %f at %d %d is over 5.0\n",mag,i,j) ;
			}
		}
	double end_time=omp_get_wtime();
	calc_vels_time+=end_time-launch_time;
	SetEvent(events[id]);
}

void MT_rearrange(float v1[PIC_X][PIC_Y][2],float v2[PIC_X][PIC_Y][2], int id)
	//float v1[PIC_X][PIC_Y][2],v2[PIC_X][PIC_Y][2];
{
	double launch_time=omp_get_wtime();
	InterlockedIncrement((LPLONG)&rearrange_count);

	int i,j;
	for(i=0;i<PIC_X;i+=MAX_THREADS)
		for(j=0;j<PIC_Y;j++)
		{
			if(v1[i][j][0] != NO_VALUE && v1[i][j][1] != NO_VALUE)
			{
				v2[i][j][0] =  v1[i][j][1];
				v2[i][j][1] = -v1[i][j][0];
			}
			else
			{
				v2[i][j][0] = NO_VALUE;
				v2[i][j][1] = NO_VALUE;
			}
		}
	double end_time=omp_get_wtime();
	rearrange_time+=end_time-launch_time;
	SetEvent(events[id]);
}

void MT_calc_statistics(float correct_vels[PIC_X][PIC_Y][2],int int_size_x,int int_size_y,float full_vels[PIC_X][PIC_Y][2],
					 int pic_x,int pic_y,int n,float *ave_error,float *st_dev,float *density,float *min_angle,float *max_angle, int id, LPVOID data)
					 //float full_vels[PIC_X][PIC_Y][2],*ave_error,*density,*st_dev;
					 //float correct_vels[PIC_X][PIC_Y][2],*min_angle,*max_angle;
					 //int n,pic_x,pic_y,int_size_x,int_size_y;
{
	double launch_time=omp_get_wtime();
	InterlockedIncrement((LPLONG)&calc_statistics_count);
	
	threadData *td=(threadData*)data;
	CSingleLock lock(&(td->pcs));

	int full_count,no_full_count,total_count;
	float sumX2,temp,uva[2],uve[2],minA,maxA,av_error;
	
	(*min_angle) = minA = HUGE;
	(*max_angle) = maxA =-HUGE;
	(*ave_error) = (*st_dev) = (*density) = av_error = 0.0;

	for(int i=n;i<pic_x-n;i++)
	{
		for(int j=n;j<pic_y-n;j++)
		{
			if(full_vels[i][j][0] != NO_VALUE && full_vels[i][j][1] != NO_VALUE)
			{
				full_count++;
				uve[0] = full_vels[i][j][0]; uve[1] = full_vels[i][j][1];
				uva[0] = correct_vels[i][j][0]; uva[1] = correct_vels[i][j][1];
				temp = PsiER(uve,uva);
				av_error += temp;
				sumX2 += temp*temp;
				if(temp < minA) minA = temp;
				if(temp > maxA) maxA = temp;
			}
			else no_full_count++;
			total_count++;
		}
	}

	//Reduce variables
	lock.Lock();
	if(lock.IsLocked()){
		td->full_count+=full_count;
		td->no_full_count+=no_full_count;
		td->total_count+=total_count;
		td->sumX2+=sumX2;
		(*ave_error)+=av_error;
		if(minA < (*min_angle)) (*min_angle) = minA;
		if(maxA > (*max_angle)) (*max_angle) = maxA;
		lock.Unlock();
	}
	int finished_threads=InterlockedIncrement((LPLONG)&(td->finish_counter));

	//Final
	if(finished_threads==MAX_THREADS){ //When all threads are done working proceed to print results
		if(td->full_count != 0) (*ave_error) = (*ave_error)/td->full_count;
		else (*ave_error) = 0.0;
		if(td->full_count > 1) 
		{
			temp = fabs((td->sumX2 - td->full_count*(*ave_error)*(*ave_error))/(td->full_count-1));
			(*st_dev) = sqrt(temp);
		}
		else (*st_dev) = 0.0;
		(*density) = td->full_count*100.0/(td->total_count*1.0);

		if((*ave_error) == 0.0) { (*min_angle) = (*max_angle) = 0.0; }

		if(FALSE)
		{
			printf("\nIn calc_statistics\n");
			printf("%d full velocities\n",td->full_count);
			printf("%d positons without full velocity\n",td->no_full_count);
			printf("%d positions in total\n",td->total_count);
			fflush(stdout);
		}
	}

	double end_time=omp_get_wtime();
	calc_statistics_time+=end_time-launch_time;
	SetEvent(events[id]);
}

//------------------------------------Utility functions------------------------------------------
void createEvents(){
	for(int i=0; i<MAX_THREADS; ++i){
		//Create a default event with auto-reset (i.e. once a wait call has been unlocked the event is unsignaled)
		events[i]=CreateEvent(NULL,FALSE,FALSE,NULL);
	}
}

void closeEvents(){
	for(int i=0; i<MAX_THREADS; ++i){
		CloseHandle(events[i]);
	}
}

void resetThreadData(LPVOID data){
	threadData *td=(threadData*)data;
	td->min_angle=HUGE;
	td->max_angle=-HUGE;
	td->full_count=td->no_full_count=td->total_count=td->finish_counter=0;
	td->sumX2=td->st_dev=td->density=td->ave_error=0.0;
}