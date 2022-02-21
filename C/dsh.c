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


    int j = 0;
    while (args[j] != NULL) {
        if (strcmp(args[j], ">") == 0) {
            char* filename = args[j+1];
            args[j] = 0;
            int fd = open(filename, O_CREAT|O_RDWR ,0755);  // tail would be 
            if (fd == -1) {
                perror("File error");
                exit(0);
            }
            dup2(fd, 1);
            //close(fd);
        } else if (strcmp(args[j], "<") == 0) {
        
            char* filename = args[j+1];
            args[j] = 0;
            int fd = open(filename, O_CREAT|O_RDWR ,0755);  // tail would be 
            if (fd == -1) {
                perror("File error");
                exit(0);
            }
            dup2(fd, 0);
            //close(fd);
           
        
        
        } else if (strcmp(args[j], "2>") == 0) {
            char* filename = args[j+1];
            args[j] = 0;
            int fd = open(filename, O_CREAT|O_RDWR ,0755);  // tail would be 
            if (fd == -1) {
                perror("File error");
                exit(0);
            }
            dup2(fd, 2);  // makes it so output is written to the file
            //close(fd);
        }
        j++;
    }
    


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
    fprintf(stderr,"dsh: command not found: %s\n",program);
    exit(0);
    
}

void run(char*);
void run_pipeline(char* head, char* tail) { 
    //fprintf(stderr,"Uh-oh, I don't know how to do pipes.");

    int fds[2] = {-1, -1};

    pipe(fds);
    // printf("Made a Pipe: %d, %d \n", fds[0], fds[1]);
    
 
    int pid = fork();
    if (pid == 0) {
        
        dup2(fds[1], 1);
        run(head);
    }

    pid = fork();
    if(pid==0) {
       
        dup2(fds[0], 0);
        run(tail);

    }

}

void run_sequence(char* head, char* tail) { 
    //fprintf(stderr,"Uh-oh, I don't know how to do sequences.");
    run(head);
    run(tail);
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
    int counter = 0;
    int counter2 = 0;
    

    
    char folde[1000];
    char fold[1000];
    char fold2[1000];
    snprintf(folde, 1000, "%s/.dsh/%d",getenv("HOME"), counter);
    while (mkdir(folde, 0755) < 0) {
        counter++;
        snprintf(folde, 1000, "%s/.dsh/%d",getenv("HOME"), counter);
    }

    
    snprintf(fold, 1000, "%s/.dsh/%d/%d.stdout",getenv("HOME"), counter, counter2);
    snprintf(fold2, 1000, "%s/.dsh/%d/%d.stderr",getenv("HOME"), counter, counter2);

    printf("dsh> ");

    // handy copies of original file descriptors
    int origin=dup(0);
    int origout=dup(1);
    int origerr=dup(2);

    while(getline(&line,&size,stdin) > 0) {
        
        // TODO: temporarily redirect stdio fds to
        //       files. This will be inherited by children.
        int fd = open(fold, O_CREAT|O_RDWR ,0755);
        int fd2 = open(fold2, O_CREAT|O_RDWR ,0755);
        dup2(fd, 1);
        dup2(fd2, 2);
        


        line[strlen(line)-1]=0; // kill the newline
        run(line);
        
        counter2++;
        snprintf(fold, 1000, "%s/.dsh/%d/%d.stdout",getenv("HOME"), counter, counter2);
        snprintf(fold2, 1000, "%s/.dsh/%d/%d.stderr",getenv("HOME"), counter, counter2);

        // TODO: restore the stdio fds before interacting
        //       with the user again

        dup2(origout, 1);
        //close(origout);

        dup2(origin, 0);
        // close(origout);

        dup2(origerr, 2);
        // close(origerr);


        printf("dsh> ");
   }
}