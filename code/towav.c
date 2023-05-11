#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#define SR 44100

int main(int argc, const char *argv[]){
  SNDFILE *fp;
  SF_INFO sfinfo;
  const char *fname = "out.wav";
  float f[BUFSIZ];
  size_t n,i;
  
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT; 
  sfinfo.samplerate = SR;
  sfinfo.channels = 1;
  
  switch(argc) {
   case 4:
      sfinfo.channels = atof(argv[3]);
   case 3:
      sfinfo.samplerate = atof(argv[2]);
   case 2:
      fname = argv[1];  
  }

  fp = sf_open(fname, SFM_WRITE, &sfinfo);
  do {
    for(n=0,i=0 ; n < BUFSIZ; n++){
      if(fscanf(stdin, "%f", &f[n]) > 0);
      else break;
    }
    sf_write_float(fp, f, n);
  }
  while(n);
    
  sf_close(fp);
  return 0;
}
