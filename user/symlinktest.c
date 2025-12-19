#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define fail(msg) do { printf("FAILURE: " msg "\n"); exit(1); } while (0)

void
test_basic()
{
  int fd;
  char buf[16];

  printf("Start: test symlinks\n");

  mkdir("test_sym");
  
  // Create a target file
  fd = open("test_sym/target", O_CREATE | O_WRONLY);
  if(fd < 0) fail("create target");
  write(fd, "hello", 5);
  close(fd);

  // Create symlink: link -> target
  if(symlink("test_sym/target", "test_sym/link") < 0) fail("symlink");

  // Open the link (should open the target)
  fd = open("test_sym/link", O_RDONLY);
  if(fd < 0) fail("open link");

  read(fd, buf, 5);
  if(memcmp(buf, "hello", 5) != 0) fail("read link content");
  close(fd);

  // Test dangling link
  unlink("test_sym/target");
  if(open("test_sym/link", O_RDONLY) >= 0) fail("opened dangling link");

  printf("test symlinks: ok\n");
}

void
test_concurrent()
{
  int i, pid;
  int n = 4;
  int iterations = 20;

  printf("Start: test concurrent symlinks\n");

  for(i = 0; i < n; i++){
    pid = fork();
    if(pid == 0){
      for(int j = 0; j < iterations; j++){
        char sym[16], target[16];
        printf(sym, sizeof(sym), "sym%d_%d", i, j);
        printf(target, sizeof(target), "target%d_%d", i, j);
        
        if(symlink(target, sym) < 0) exit(1);
        if(unlink(sym) < 0) exit(1);
      }
      exit(0);
    }
  }

  for(i = 0; i < n; i++){
    wait(0);
  }

  printf("test concurrent symlinks: ok\n");
}

int
main(int argc, char *argv[])
{
  test_basic();
  test_concurrent();
  exit(0);
}