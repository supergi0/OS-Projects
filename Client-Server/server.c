#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define MAX_CONNECTIONS 10
#define ARBITARY_NUM -1934567

int conarray[MAX_CONNECTIONS];
int l=0;
pthread_mutex_t lock;

int register_shmid;
int *register_shared_memory;

int running_shmid;
int *running_shared_memory;

int isprime(int n)
{
	int i;
	for(i=2;i<=n/2;i++)
	{
		if(n%i!=0)
			continue;
		else
			return 0;
	}
	return 1;
}

int connection_exists(int id){
    for(int i=0;i<MAX_CONNECTIONS;i++){
        if(conarray[i]==id){
            return 1;
        }
    }
    return 0;
}

int server_calculation(int shared_request[3]){
    if(shared_request[0]==2){
        return shared_request[1]+shared_request[2];
    }
    if(shared_request[0]==3){
        return shared_request[1]-shared_request[2];
    }
    if(shared_request[0]==4){
        return shared_request[1]*shared_request[2];
    }
    if(shared_request[0]==5){
        return shared_request[1]/shared_request[2];
    }
    if(shared_request[0]==6){
        return shared_request[1]%2;
    }
    if(shared_request[0]==7){
        return isprime(shared_request[1]);
    }
    if(shared_request[0]==9){
        return ARBITARY_NUM;
    }
}

void print_request(int shared_request[3]){
    if(shared_request[0]==2){
        printf("adding %d and %d\n",shared_request[1],shared_request[2]);
    }
    if(shared_request[0]==3){
        printf("subtracting %d and %d\n",shared_request[1],shared_request[2]);
    }
    if(shared_request[0]==4){
        printf("multiplying %d and %d\n",shared_request[1],shared_request[2]);
    }
    if(shared_request[0]==5){
        printf("dividing %d and %d\n",shared_request[1],shared_request[2]);
    }
    if(shared_request[0]==6){
        printf("checking evenodd for %d\n",shared_request[1]);
    }
    if(shared_request[0]==7){
        printf("checking prime for %d\n",shared_request[1]);
    }
    if(shared_request[0]==9){
        printf("deregistering client\n");
    }
};

void *thread_routine(void *arguments){
    int id = *(int*) arguments;

    pthread_mutex_lock(&lock);
    printf("%d. Alloted thread to client %d!\n",l,id);
    l++;
    pthread_mutex_unlock(&lock);

    int input_shmid;
    int *input_shared_memory;

    int output_shmid;
    int *output_shared_memory;

    input_shmid = shmget((key_t)id,1024,IPC_CREAT|0666);
    input_shared_memory = shmat(input_shmid,NULL,0);
    
    output_shmid = shmget((key_t)(-1)*id,1024,IPC_CREAT|0666);
    output_shared_memory = shmat(output_shmid,NULL,0);

    *input_shared_memory = ARBITARY_NUM;
    *(input_shared_memory+1)=ARBITARY_NUM+1;
    *(input_shared_memory+2)=ARBITARY_NUM+2;

    pthread_mutex_lock(&lock);
    printf("%d. COMMS CHANNEL created ,alloted shared memory to client %d!\n",l,id);l++;
    pthread_mutex_unlock(&lock);

    int counter=0;
    while(1){
        if(connection_exists(id)){
            
            int request[3]={ARBITARY_NUM,ARBITARY_NUM+1,ARBITARY_NUM+2};
            while(request[0]==ARBITARY_NUM){                                                                                                                                                                                                                                                                                                   
            //printf("%d\n",request[0]);
            request[0]=*input_shared_memory;
            request[1]=*(input_shared_memory+1);
            request[2]=*(input_shared_memory+2);
            }

            pthread_mutex_lock(&lock);
            printf("%d. recieved request no.%d from client %d for ",l,counter,id); print_request(request);l++;
            pthread_mutex_unlock(&lock);

            counter++;
            int answer = server_calculation(request);
            // for closing the connection

            if(answer==ARBITARY_NUM){
            
                pthread_mutex_lock(&lock);
                for(int i=0;i<MAX_CONNECTIONS;i++){
                    if(conarray[i]==id){
                        conarray[i]=0;
                    }
                }
                shmdt(input_shared_memory);
                shmdt(output_shared_memory);
                printf("%d. Sucessfully deregistered client number: %d\n",l,id);l++;
                pthread_mutex_unlock(&lock);
                return 0;
            }

            *input_shared_memory = ARBITARY_NUM;
            *(input_shared_memory+1)= ARBITARY_NUM;
            *(input_shared_memory+2)=ARBITARY_NUM;
    
            *output_shared_memory = answer;

            pthread_mutex_lock(&lock);
            printf("%d. Answer %d sent to client %d for ",l,answer,id);print_request(request);l++;
            pthread_mutex_unlock(&lock);
        }
    }
}

void connections(){
    int alloted = 0;
    int client_id = ARBITARY_NUM;

    while(client_id==ARBITARY_NUM){
    client_id = *register_shared_memory;
    }

    printf("%d. Connection requested by client %d, on CONNECT CHANNEL!\n",l,client_id);l++;

    for(int i=0;i<MAX_CONNECTIONS;i++){
       // printf("working!\n");
        if(conarray[i]==0){
           // printf("this done!\n");
            conarray[i]=client_id;
            alloted = 1;
            printf("%d. Alloting thread %d to client %d!\n",l,i,client_id);l++;
            pthread_t thread;
            int *temp = malloc(sizeof(int));
            *temp = client_id;
            pthread_create(&thread,NULL,&thread_routine,temp);
            break;
        }
    }
    if(alloted!=1){
        printf("%d. Maximum server threads reached, connection request rejected!\n",l);l++;
    }

    *register_shared_memory  = ARBITARY_NUM;
}

int main(){

register_shmid = shmget((key_t)123456789,1024,IPC_CREAT|0666);
register_shared_memory = shmat(register_shmid,NULL,0);

running_shmid = shmget((key_t)-123456789,1024,IPC_CREAT|0666);
running_shared_memory = shmat(running_shmid,NULL,0);

*running_shared_memory = ARBITARY_NUM;
*register_shared_memory = ARBITARY_NUM;

for(int i=0;i<MAX_CONNECTIONS;i++){
    conarray[i]=0;
}

printf("%d. SERVER PROGRAM with max %d threads | SERVER PID: %d\n",l,MAX_CONNECTIONS,getpid());l++;
while(1){
    connections();
}

}