#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

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

void find(char* path, char* strFind) {
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
            
            if (strcmp(fmtname(buf), ".") == 0 || strcmp(fmtname(buf), "..") == 0)
                continue;

            find(buf, strFind);
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

    find(argv[1], argv[2]);
}