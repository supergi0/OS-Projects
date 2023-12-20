#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define ARBITARY_NUM -1934567
//global variables
int registered=0;
int l=0;

int register_shmid;
int *register_shared_memory;

int input_shmid;
int *input_shared_memory;

int output_shmid;
int *output_shared_memory;

int running_shmid;
int *running_shared_memory;

int client_id(){
    int pid = getpid();
    return pid;
}

void client_request(int code,int int1,int int2){
    *input_shared_memory = code;
    *(input_shared_memory+1)=int1;
    *(input_shared_memory+2)=int2;
}

void client_result(){
    int result=ARBITARY_NUM;
    while(result==ARBITARY_NUM){
        result = *output_shared_memory;
    }
        printf("Answer: %d\n",result);
    
    *output_shared_memory  =ARBITARY_NUM;
    
}

void client_register(){
    if(registered==1){
        printf("already registered!\n");
        return;
    }
    int is_running = *running_shared_memory;
    if(is_running!=ARBITARY_NUM){
        printf("start server first!\n");
        return;
    }
    printf("request sent to server!\n");
    *register_shared_memory=client_id();
    printf("connection to server successful!\n");
    registered=1;
}

void client_unregister(){
    if(registered==0){
        printf("already unregistered\n");
        return;
    }
    printf("request sent to server!\n");
    client_request(9,0,0);
    printf("client successfully unregistered!\n");
    registered=0;
}

void client_add(int int1,int int2){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request sent to server to add numbers %d %d!\n",int1,int2);
    client_request(2,int1,int2);
    client_result();
}

void client_sub(int int1,int int2){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request sent to server to subtract numbers %d %d!\n",int1,int2);
    client_request(3,int1,int2);
    client_result();
}

void client_mul(int int1,int int2){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request sent to server to multiply numbers %d %d!\n",int1,int2);
    client_request(4,int1,int2);
    client_result();
}

void client_div(int int1,int int2){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request sent to server to divide numbers %d %d!\n",int1,int2);
    client_request(5,int1,int2);
    client_result();
}

void client_eo(int int1){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request sent to server to check evenodd %d!\n",int1);
    client_request(6,int1,0);
    client_result();
}

void client_pri(int int1){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request sent to server to check prime %d!\n",int1);
    client_request(7,int1,0);
    client_result();
}

void client_neg(int int1){
    if(registered==0){
        printf("register first\n");
        return;
    }
    printf("request denied! Checking negative numbers is not supported\n");
}

void print_info(){
    sleep(1);
    printf("%d. SOURCE:CLIENT | ID: %d | PID: %d | THREAD:MAIN \n\n",l,client_id(),client_id());l++;
    printf("1. Register | Format: 1\n");
    printf("2. Addition | Format: 2 <int1> <int>\n");
    printf("3. Subtraction | Format: 3 <int1> <int>\n");
    printf("4. Multiplication | Format: 4 <int1> <int>\n");
    printf("5. Division | Format: 5 <int1> <int>\n");
    printf("6. Even or Odd | Format: 6 <int1> \n");
    printf("7. Is Prime | Format: 7 <int1>\n");
    printf("8. Is Negative | Format: 8 <int1>\n");
    printf("9. Unregister | Format: 9\n");
    printf("10. Exit | Format: 10\n");
    printf("\nInput: ");
}

int main(){
//mapping the register deregister and inputs shared mem
register_shmid = shmget((key_t)123456789,1024,IPC_CREAT|0666);
register_shared_memory = shmat(register_shmid,NULL,0);

input_shmid = shmget((key_t)client_id(),1024,IPC_CREAT|0666);
input_shared_memory = shmat(input_shmid,NULL,0);

output_shmid = shmget((key_t)(-1)*client_id(),1024,IPC_CREAT|0666);
output_shared_memory = shmat(output_shmid,NULL,0);

running_shmid = shmget((key_t)-123456789,1024,IPC_CREAT|0666);
running_shared_memory = shmat(running_shmid,NULL,0);

*output_shared_memory = ARBITARY_NUM;

while(1){
    print_info();
    int request_num;
    scanf("%d",&request_num);
    if(request_num==10){
        shmctl(input_shmid,IPC_RMID,NULL);
        shmctl(output_shmid,IPC_RMID,NULL);
        break;
    }
    else if(request_num==1){
        client_register();
    }
    else if(request_num==9){
        client_unregister();
    }
    else if(request_num==2){
        int int1;
        int int2;
        scanf("%d",&int1);
        scanf("%d",&int2);
        client_add(int1,int2);
    }
    else if(request_num==3){
        int int1;
        int int2;
        scanf("%d",&int1);
        scanf("%d",&int2);
        client_sub(int1,int2);
    }
    else if(request_num==4){
        int int1;
        int int2;
        scanf("%d",&int1);
        scanf("%d",&int2);
        client_mul(int1,int2);
    }
    else if(request_num==5){
        int int1;
        int int2;
        scanf("%d",&int1);
        scanf("%d",&int2);
        client_div(int1,int2);
    }
    else if(request_num==6){
        int int1;
        scanf("%d",&int1);
        client_eo(int1);
    }
    else if(request_num==7){
        int int1;
        scanf("%d",&int1);
        client_pri(int1);
    }
    else if(request_num==8){
        int int1;
        scanf("%d",&int1);
        client_neg(int1);
    }
    printf("\n");
}

printf("\nclient program has ended\n");
return 0;
}