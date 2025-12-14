#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#ifndef NULL
#define NULL (void*) 0
#endif

char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

  // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
        p++;

    if (strlen(p) > DIRSIZ) return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

void handleExec(char* args[], char* path) {
    int forkReturn = fork();

    if (forkReturn == 0) {
        // Child block
        void* temp = args[0];
        char* newArgs[MAXARG - 4]; // Max possible number of arguments
        int i = 0;
        while (temp != NULL) {
            newArgs[i] = temp;
            temp = args[++i];
        }
        if (i >= MAXARG - 5) {
            return;
        }
        newArgs[i] = path; // The found file is inserted at the end
        newArgs[i+1] = NULL; // NULL is placed after path to indicate the last element
        exec(args[0], newArgs); // Thumbsup if it works and return if it doesn't
    }
    else {
        // Parnet block
        wait(NULL);
    }
}

// My find function can now perform exec too!
void find(char* path, char* strFind, int hasExec, char* args[]) {
    // printf("I'm sure you'll find it yourself :)\n");

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        printf("cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        printf("cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_DEVICE:
    case T_FILE:
        if (strcmp(fmtname(path), strFind) == 0) {
            if (hasExec == 1)
                handleExec(args, path);
            else
                printf("%s\n", path);
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("path too long\n");
            break;
        }
    
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
        
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
        
            if(stat(buf, &st) < 0){
                printf("cannot stat %s\n", buf);
                continue;
            }
            
            if (strcmp(fmtname(buf), ".") == 0 || 
                strcmp(fmtname(buf), "..") == 0)
                continue;

            find(buf, strFind, hasExec, args);
        }

        break;
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Provide path argument!\n");
        exit(0);
    }
    if (argc < 3) {
        printf("Provide find argument!\n");
        exit(0);
    }

    // Please ignore all the conditional statements. I wrote this at 1am.
    if (argc > 3) {
        // If through some magic the user evades the actual 10 args check
        if (argc > MAXARG - 1) {
            printf("Too many arguments!\n");
            exit(0);
        }
        
        // -exec is present in the 3rd place just like we wanted!
        if (strcmp(argv[3], "-exec") == 0) {
            char* newArgs[MAXARG - 4];
            int i = 0;
            for (i = 4; i < argc; i++) {
                newArgs[i - 4] = argv[i];      
            }
            newArgs[i] = NULL;
            find(argv[1], argv[2], 1, newArgs);
        }
    }
    else {
        find(argv[1], argv[2], 0, NULL);
    }
}