// Nabin Shrestha 1001746226
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <wait.h> 
#include <sched.h>
#include <pthread.h>



typedef struct info 
{
	char detail[500]; //saving each line
	double latitude;  // getting only latitude
	double longitude; // getting only longitude
}info;

typedef struct thread_data{
  int first;
  int last;
  info *data;
}thread_data;

void psorted(info earthquake_data[],int begin,int end)
{
  int i;
  for (i = begin; i < end ; i++)
  {
        printf("latitude : %f \n", earthquake_data[i].latitude);
  }
}
void swap(info *xp, info *yp)  //passing pointer
{
  info temp=*xp;
  *xp=*yp;
  *yp=temp;
}
void *bubblesort(void *data) //geeksforgeeks
{
  thread_data my_data = *(thread_data *) data;
  info *earthquake_data= my_data.data;
  int first= my_data.first;
  int last = my_data.last;
  int i,j;
  for(i=first;i<last-1;i++)
    for(j=first;j<last-1;j++)
      if(earthquake_data[j].latitude>earthquake_data[j+1].latitude)
        swap(&earthquake_data[j],&earthquake_data[j+1]);
  

      pthread_exit(NULL);

}
void* create_shared_memory(size_t size) //stackoverflow
{
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

//geeksforgeeks
void merge(info arr[], int first, int middle, int last) // l = first , m = middle, last = r
{ 
    int i, j, k; 
    int n1 = middle - first; 
    int n2 = last - middle; 
 
    /* create temp arrays */
    info L[n1], R[n2]; 
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[first + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[middle + j]; 
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = 0; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i].latitude <= R[j].latitude) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else 
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
 
    /* Copy the remaining elements of L[], if there 
    are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
 
    /* Copy the remaining elements of R[], if there 
    are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
 
/* l is for left index and r is right index of the 
sub-array of arr to be sorted */

int main()
{
    //pthread_t thread[2];
	 int count,t=0;
   char *token;
   double num;
   FILE *fp = NULL;
   time_t begin,end;
   int i;
   fp =fopen("all_month_1.csv","r");// opening file named after all_month in read only mode.
   if (fp == NULL)
   {
	   printf("file cannot be open.");
	   exit(0);
   }
    char buff[500]; // to save line in buff
	 while(fgets(buff, sizeof(buff)-1, fp) != NULL) // read the file until the end of file
		count++;
   count = count-1;
  int sizes=sizeof(info)*count;
	info *earthquake_data = create_shared_memory(sizes);//calling create_shared_memory function which returns shared memory 
	fseek(fp, 0,0); //  taking file pointer to the beginign of the file
  fgets(buff, sizeof(buff)-1, fp);
  i=0;
  while (fgets(buff, sizeof(buff)-1, fp) != NULL)
  {
    strcpy(earthquake_data[i].detail, buff);
    token = strtok(buff, ","); //store value separated by comma in token
    token = strtok(NULL, ","); //doesnot read 1st line of the file
    num = atof(token); // convert string to floating number
    earthquake_data[i].latitude = num; //storing value of num at latitude of an array
    token = strtok(NULL, ","); //doesnot read 1st line of the file
    num = atof(token); // convert string to floating number
    earthquake_data[i].longitude = num; //storing value of num at longitude of an array
    i++;
  }
  begin=time(NULL);
  int input=0;    
  printf("how many threads you want to use?? \n ");
  scanf("%d", &input);
  pthread_t thread[input];

  int range[input+1];
  range[0]=0;
  for(int i=1;i<input+1;i++)
  {
    if((i+1)==input+1)
      range[i]=range[i-1]+count%input+count/input;
    else
      range[i]=range[i-1]+count/input;
  }
  int rc;
  int noprocess=input;
  input++;
 //loop first and last depends upon the range from upper looop
thread_data thread_data_array[input];

  for(i =0; i < input; i++)
  {
    thread_data_array[i].first= range[i];
    thread_data_array[i].last= range[i+1] ;
    thread_data_array[i].data = earthquake_data;
  }
   
  //loop no of threads
  for(i = 0; i < input; i++)
  {
    rc = pthread_create(&thread[i], NULL, bubblesort,(void*) &thread_data_array[i]);
    
  }
  //loop
  for(i = 0; i < input ; i++)
  {  
    void * status;
    pthread_join(thread[i], &status);
    pthread_join(thread[i], &status);
    merge(earthquake_data, 0 , count/2, count);
  } 
  
  for(i=0;i<noprocess-1;i++)
  {
    merge(earthquake_data,0,range[i+1],range[i+2]); //calling merge function
  }
  
  end=time(NULL);
  time_t diff= end -begin;
  printf("print after mering: \n");
  psorted(earthquake_data,0,count); // calling psorted function to print array
  printf("Begin time: %s\n",ctime(&begin) );
  printf("End time; %s\n", ctime(&end) );
  printf("The time taken is %ldsec\n",diff);
}


