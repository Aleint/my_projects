#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

#define BSIZE 1024  // block size

#define NDIRECT 11
#define NINDIRECT (BSIZE / sizeof(uint))


#define NDOUBLY_INDIRECT NINDIRECT*NINDIRECT
#define MAXFILE (NDIRECT + NINDIRECT + NDOUBLY_INDIRECT)

void printProgressBar(int t) {
    const int barWidth = 50;
    int percentage = (t * 100) / MAXFILE;
    int barLength = (t * barWidth) / MAXFILE;

    printf("[");
    for (int i = 0; i < barWidth; ++i) {
        if (i < barLength)
            printf("=");
        else
            printf(" ");
    }
    printf("] %d%%\r", percentage);
    
}


int main(int argc, char **argv)
{
  char buf[512];
  int fd, i, t;
  printf("\nMAXFILE: %d\n",MAXFILE);
  fd = open("max.file", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf("max: cannot open max.file for writing\n");
   return 0;
  }

  t = 0;
  
            
  while(t<MAXFILE){
    *(int*)buf = t;
    int cc = write(fd, buf, sizeof(buf));
    if(cc <= 0)
      break;
    t++;

   
  if (t % (MAXFILE / 100) == 0 || t == MAXFILE) {
            printProgressBar(t);
        }
  
  }

  printf("\nwrote: %d \n\n", t);

  close(fd);
  fd = open("max.file", O_RDONLY);
  if(fd < 0){
    printf("maxfile: cannot re-open max.file for reading\n");
     return 0;
  }
  for(i = 0; i < t; i++){
    int cc = read(fd, buf, sizeof(buf));
    if(cc <= 0){
      printf("maxfile: read error at sector %d\n", i);
      return 0;
    }
    if(*(int*)buf != i){
      printf("maxfile: read the wrong data (%d) for sector %d\n",
             *(int*)buf, i);
       return 0;
    }

  }
  unlink("max.file");
  exit(1);
}
