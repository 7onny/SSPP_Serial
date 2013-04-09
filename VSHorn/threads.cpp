#include "stdafx.h"
#include "threads.h"

UINT calc_statistics_proc(LPVOID cs_data){
	calc_statistics_data *td=(calc_statistics_data*)cs_data;
	CSingleLock singleLock(&td->pcs);

	int id;
	singleLock.Lock();
	id=td->current_id;
	td->current_id++;
	singleLock.Unlock();

	int n=td->n;
	float temp, uva[2], uve[2], ave_error, sumX2, min_angle, max_angle;
	int full_count, no_full_count, total_count;
	ave_error=sumX2=0.0;
	min_angle=*(td->min_angle);
	max_angle=*(td->max_angle);
	full_count=no_full_count=total_count=0;

	int blocksize;
	if(id<(MAX_THREADS-1)){
		blocksize=(td->pic_x-n)/MAX_THREADS;
	}
	else {	//Last thread may have additional work
		blocksize=((td->pic_x-n)/MAX_THREADS)+((td->pic_x-n)%MAX_THREADS);
	}
	int start = (id*blocksize)+n;
	int end = (id+1)*blocksize;
	for(int i=start;i<end;i++)
	{
		for(int j=n;j<(td->pic_y-n);j++)
		{
			if(td->full_vels[i][j][0] != NO_VALUE && td->full_vels[i][j][1] != NO_VALUE)
			{
				full_count++;
				uve[0] = td->full_vels[i][j][0]; uve[1] = td->full_vels[i][j][1];
				uva[0] = td->correct_vels[i][j][0]; uva[1] = td->correct_vels[i][j][1];
				temp = PsiER(uve,uva);
				ave_error += temp;
				sumX2 += temp*temp;
				if(temp < min_angle) min_angle = temp;
				if(temp > max_angle) max_angle = temp;
			}
			else no_full_count++;
			total_count++;
		}
	}

	//Commiting to shared memory
	singleLock.Lock();
	*(td->ave_error)=ave_error;
	td->sumX2=sumX2;
	if(min_angle < *(td->min_angle)) *(td->min_angle) = min_angle;
	if(max_angle > *(td->max_angle)) *(td->max_angle) = max_angle;
	td->full_count=full_count;
	td->no_full_count=no_full_count;
	td->total_count=total_count;
	singleLock.Unlock();

	return 0;
}