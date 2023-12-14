#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>

// global declaration of variables
int arr[100][100],p,n,a,b;

// handler to handle events when a child process ends
void handler (int sig) {
   // printf("A process just ended!\n");
}

int wrong_input(int x,int a,int b){
    if(a<=x && x<=b){
        return 0;
    }
    printf("Value %d must lie between %d and %d **************ERROR\n",x,a,b);
    return 1;
}

// to check if a number is prime or not
int isPrime(int x)
{
    int c=0;
    for(int i=2;i<x;i++)
    {
        if(x%i==0)
        return 0;
    }
return 1;
}

// to find the prime numbers before x
void beforePrime (int x, int p,int sum[2])
{  
    sum[0]=0;
    sum[1]=0;
    int n=p;
        for(int j= x-1; j>0; j--)
        {
            if(isPrime(j))
                {
                    sum[0] = sum[0] + j;
                    sum[1] = sum[1] + 1;
                    --p;
                }
            if(p==0)
                break;
        }
    
}

//to find the prime numbers after x
void afterPrime(int x, int p,int sum[2])
{
    sum[0]=0;
    sum[1]=0;
    int n=p;
        for(int j=x+1; p>0; j++)
        {
            if(isPrime(j))
                {
                  sum[0] = sum[0] + j;
                  sum[1] = sum[1] + 1;
                    p--;
                }
            if(p==0)
                break;
        }
}

// our main thread routine which will execute n times
 void *thread_routine(void *arguments)
 {
	 int x = *(int*) arguments;
     if(wrong_input(x,a,b)){
        int *return_val2 = malloc(sizeof(int));
        *return_val2 = -1;
        return (void*) return_val2;
     }
     printf("Thread executing for value %d\n",x);
     int sum1[2];
     int sum2[2];
	 beforePrime(x,p,sum1);
     afterPrime(x,p,sum2);
     int total_sum = 0;
     int total_count = 0;
     // to check if x should be included or not in the thapx
           if(isPrime(x))
                {
                    printf("Prime number %d found!\n",x);
                    total_sum = total_sum + x;
                    total_count++;
                }
           total_sum = total_sum + sum1[0] + sum2[0];
           total_count = total_count + sum1[1] + sum2[1];
           int *return_val = malloc(sizeof(int));
           printf("px sum calculated: %d corresponding thapx: %d\n",total_sum,total_sum/total_count);
           *return_val = total_sum/total_count;
           return (void*) return_val;
 }

// main function
int main(int argc,char *argv[])
{
    n=atoi(argv[1]);
    a=atoi(argv[2]);
    b=atoi(argv[3]);
    p=atoi(argv[4]);
    int k = 5;

    for(int i=0;i<n;i++){
	    for(int j=0;j<n;j++)
    	{arr[i][j] = atoi(argv[k]);
	    k++;}
    }
printf("Inputs read!\n");
for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
        printf("%d ",arr[i][j]);
    }
    printf("\n");
}
printf("\n");
// creating n pipes for n processes. 0-read end 1-write end
int pipes[n][2];
int process_ids[n];

for(int i = 0;i<n+1;i++){
    if(pipe(pipes[i])==-1){
        printf("Error in creating pipe*****************ERROR\n\n");
    }
}

// signal executes when a child terminates
signal(SIGCHLD,handler);
printf("pipes created!\n");
// forking into n processes
for(int i=0;i<n;i++){
    if(!(process_ids[i]=fork())){
        printf("process %d created and assigned %d row!\n",i,i);
        close(pipes[i][0]);
        pthread_t threads[n];
        int *thapx[n];
        for(int j = 0;j<n;j++){
           int *temp = malloc(sizeof(int));
           *temp = arr[i][j];
            if(pthread_create(&threads[j], NULL, &thread_routine,temp) != 0){
                printf("Error in thread creation!*********************ERROR\n\n");
            }
        printf("A thread was just created and assigned (%d,%d) cell!\n",i,j);
        }
        for(int j=0;j<n;j++){
            int** val=malloc(sizeof(int));
            pthread_join(threads[j],(void**)&thapx[j]);
            printf("thread assigned to (%d,%d) just joined!\n",i,j);
            if(*thapx[j]==-1){
                int wpapx_avg = -1;
                write(pipes[i][1],&wpapx_avg,sizeof(int));
                printf("Bad value recieved from thread.******************ERROR\n");
                close(pipes[i][1]);
                return 0;
            }
        }
        int wpapx_sum = 0;
        for(int j=00;j<n;j++){
            wpapx_sum = wpapx_sum + *thapx[j];
        }
// sending data to pipes
        int wpapx_avg = wpapx_sum/n;
        printf("wpapx calculated: %d corresponding thapx values: ",wpapx_avg);
        for(int k=0;k<n;k++){
            printf("%d ",*thapx[k]);
        }
        printf("\n");
        write(pipes[i][1],&wpapx_avg,sizeof(int));
        printf("wpapx %d written to controller via pipe %d\n",wpapx_avg,i);
        close(pipes[i][1]);
        return 0;
    }
}

// waiting for n processes
for(int i=0;i<n;i++){
    wait(NULL);
}

// recieving data from pipes
int wpapx[n];
for(int i=0;i<n;i++){
    close(pipes[i][1]);
    read(pipes[i][0],&wpapx[i],sizeof(int));
    printf("wpapx %d captured via pipe %d\n",wpapx[i],i);
    if(wpapx[i]==-1){
        printf("Bad value recieved from process.****************ERROR\n");
        printf("\nTerminating program! fapx wasn't calculated!\n");
        return 0;
    }
    close(pipes[i][0]);
}

// fapx calculation
int fapx_sum = 0;
for(int i=0;i<n;i++){
    fapx_sum = fapx_sum + wpapx[i];
}

int fapx_avg = fapx_sum/n;

printf("\nfapx value: %d\n\n",fapx_avg);

return 0;
}
