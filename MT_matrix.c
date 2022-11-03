#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int **result;
int **matrix1;
int **matrix2;

void *threadRun(int *arg){
    for(int k=arg[2];k<arg[3];k++){
        for(int i=0;i<arg[0];i++){
            int total=0;
            for(int j=0;j<arg[1];j++){
                total+=matrix1[i][j]*matrix2[j][k];
            }
            result[i][k]=total;
            //printf("k: %d,%d\n",k,total);
        }
    }
    pid_t tid=gettid();
    //printf(" %d\n",tid);
    char data[BUFSIZ];
    sprintf(data,"%d",tid);
    int fd = open("/proc/thread_info", O_RDWR);
    if(fd == -1)
    {
      printf("/proc/thread_info does not exist\n");
      return 1;
    }
    write(fd, data, strlen(data));
    //read(fd, readbuffer, 100);
    //printf("\t%s\n",readbuffer);
    close(fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    int row1;
    int column1;
    int row2;
    int column2;
    int threadnum = strtol(argv[1],NULL,10);
    //printf("%d\n",threadnum);
    
    //printf("%d\n",argc);
    for(int i=0;i<argc;i++){
        //printf("%s\n",argv[i]);
    }
    FILE* input_file1=fopen(argv[2],"r");
    FILE* input_file2=fopen(argv[3],"r");
    if(!input_file1) 
        exit(EXIT_FAILURE);
    if(!input_file2) 
        exit(EXIT_FAILURE);

    fscanf(input_file1, "%d %d\n", &row1, &column1);
    fscanf(input_file2, "%d %d\n", &row2, &column2);
    //int matrix1[row1][column1];
    //int matrix2[row2][column2];

    matrix1=(int**)malloc(sizeof(int*)*row1);
    for(int i=0;i<row1;i++){
        matrix1[i]=(int*)malloc(sizeof(int)*column1);
    }

    matrix2=(int**)malloc(sizeof(int*)*row2);
    for(int i=0;i<row2;i++){
        matrix2[i]=(int*)malloc(sizeof(int)*column2);
    }


    for(int i=0;i<row1;i++){
        for(int j=0;j<column1;j++){
            fscanf(input_file1, "%d " ,&matrix1[i][j]);
        }
    }

    for(int i=0;i<row2;i++){
        for(int j=0;j<column2;j++){
            fscanf(input_file2, "%d " ,&matrix2[i][j]);
        }
    }
    
    //printf("%d %d\n",row1,column1);
    //printf("%d %d\n",row2,column2);
    /*
    for(int i=0;i<row1;i++){
        for(int j=0;j<column1;j++){
            printf("%d",matrix1[i][j]);
        }
        printf("\n");
    }
    
    for(int i=0;i<row2;i++){
        for(int j=0;j<column2;j++){
            printf("%d",matrix2[i][j]);
        }
        printf("\n");
    }
    */
    

    // malloc output matrix size

    result=(int**)malloc(sizeof(int*)*row1);
    for(int i=0;i<row1;i++){
        result[i]=(int*)malloc(sizeof(int)*column2);
    }


    //printf("PID:%d\n",getpid());

    pthread_t t[threadnum];

    int **childthreadwork=(int**)malloc(sizeof(int*)*threadnum);

    for(int a=0;a<threadnum;a++){
        childthreadwork[a]=(int*)malloc(sizeof(int)*4);
        childthreadwork[a][0]=row1;
        childthreadwork[a][1]=column1;
        //childthreadwork[a][2]=(column2*(a+1))/(threadnum+1);
        //childthreadwork[a][3]=((column2*(a+2))/(threadnum+1));
        childthreadwork[a][2]=(column2*(a))/(threadnum);
        childthreadwork[a][3]=((column2*(a+1))/(threadnum));
        //printf("left:%d, right:%d thread: %d\n",childthreadwork[a][2], childthreadwork[a][3], a);
        pthread_create(&t[a], NULL, threadRun, childthreadwork[a]);
    }

    /*
    for(int k=0;k<(column2/(threadnum+1));k++){
        for(int i=0;i<row1;i++){
            int total=0;
            for(int j=0;j<column1;j++){
                total+=matrix1[i][j]*matrix2[j][k];
            }
            result[i][k]=total;
        }
    }
    */

    for(int b=0;b<threadnum;b++){
        //write(fd, data, 32768);
        pthread_join(t[b], NULL);
    }

    int fd = open("/proc/thread_info", O_RDWR);
    if(fd == -1)
    {
      printf("/proc/thread_info does not exist\n");
      return 1;
    }
    char readbuffer[BUFSIZ]={"\0"};
    read(fd, readbuffer, BUFSIZ);
    printf("%s",readbuffer);

    /*
    for(int i=0;i<row1;i++){
        for(int j=0;j<column2;j++){
            printf("%d ",result[i][j]);
        }
        printf("\n");
    }
    */

    free(result);
    free(matrix1);
    free(matrix2);
    fclose(input_file1);
    fclose(input_file2);
    close(fd);
    return 0;
}
