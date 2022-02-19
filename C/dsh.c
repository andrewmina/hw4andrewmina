#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>


void exec_command(char* command) {
    char* program = strtok(command," ");
    char *args[16]={program};
    int i=1;
    while((args[i++]=strtok(NULL," ")));

    // TODO: search the path instead of running "program directly"
    char* path = getenv("PATH");
    
    char* token = strtok(path, ":");
    // aaa/aaa/dsfds/dsfgd/: asdfsd/sadf:
    char arr[1000];
    while (token != NULL) {
        snprintf(arr, 1000,"%s%s%s", token, "/", program);
        
        execv(arr, args);  // executing "command"
        
        token = strtok(NULL, ":"); // going to the next token

    }
    
}

void run(char*);
void run_pipeline(char* head, char* tail) { 
    fprintf(stderr,"Uh-oh, I don't know how to do pipes.");
}

void run_sequence(char* head, char* tail) { 
    fprintf(stderr,"Uh-oh, I don't know how to do sequences.");
}

void run_writeto(char* head, char* tail) {
    int fd = open(tail, O_CREAT|O_RDWR ,0755);  // tail would be 
    if (fd == -1) {
    	perror("File error");
    	exit(0);
    }
    dup2(fd, 1);  // makes it so output is written to the file
   
    
    //fprintf(stderr, "Uh-oh, I dont know how to <");
}

void run(char *line) {
    char *sep;
    
    if((sep=strstr(line,";"))) {
        *sep=0;        
        run_sequence(line,sep+1);
    }
    else if((sep=strstr(line,"|"))) {
        *sep=0;        
        run_pipeline(line,sep+1);
    } else if ((sep=strstr(line, ">"))) {
        if (!fork()) {
            *sep=0;
            run_writeto(line, sep+1);
            exec_command(line);
        } else {
            wait(0);
        }
        
        
        
    }
     else if ((sep=strstr(line, "<"))) {
        if (!fork()) {
            *sep=0;
            int fd = open(sep+1, O_CREAT|O_RDWR ,0755);  // tail would be 
            if (fd == -1) {
                perror("File error");
                exit(0);
            }
            dup2(fd, 0);
            exec_command(line);
        } else {
            wait(0);
        }    
        
        
    }
    else {
        if(!fork()) 
         exec_command(line);
        else wait(0);        
    }
}

int main(int argc, char** argv) {
    char *line=0;
    size_t size=0;

    char folder[1000];
    snprintf(folder,1000,"%s/.dsh",getenv("HOME"));
    mkdir(folder,0755);

    // TODO: need to create the appropriate session folder
    //       to put our <N>.stdout and <N>.stderr files in.
    //int counter = 0;

    

    printf("dsh> ");

    // handy copies of original file descriptors
    int origin=dup(0);
    int origout=dup(1);
    int origerr=dup(2);

    while(getline(&line,&size,stdin) > 0) {

        // TODO: temporarily redirect stdio fds to
        //       files. This will be inherited by children.
        //char* filename = argv[];
        //fd = open(folder, O_CREAT|O_RDWR);

        //dup2(fd, 1);


        line[strlen(line)-1]=0; // kill the newline
        run(line);

        // TODO: restore the stdio fds before interacting
        //       with the user again

        dup2(origout, 1);
        //close(origout);

        dup2(origin, 0);
        // close(origout);

        // dup2(origerr, 2);
        // close(origerr);


        printf("dsh> ");
   }
}