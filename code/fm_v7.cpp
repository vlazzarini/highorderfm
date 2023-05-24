#include <vector>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <samplerate.h>
const double twopi = 2*M_PI;
const std::size_t def_vsize = 64;
const unsigned int def_sr = 44100; 

// integer indexing oscillator (32bit)
template<typename S>
class Op {
  static constexpr long maxlen = 0x100000000; 
  const std::vector<S> &tab;  
  std::vector<S> out;
  std::vector<S> mod;
  S fdb;
  unsigned int fs;  
  unsigned int phs;
  unsigned int lobits;
  unsigned int fac;
  unsigned int lomask;
  
  S lookup(S f){
    auto frac = (phs & lomask)/((S)lomask + 1); 
    unsigned int ndx = phs >> lobits; 
    auto s = tab[ndx] + frac*(tab[ndx+1] - tab[ndx]); 
    phs += (int)(f*fac); 
    return s;
  }

  const std::vector<S> &process(S a,S fr,const S* fm,S g){
    std::size_t n = 0;
    for(auto &o : out) {
      auto f = fr+fdb*g+(fm?fm[n]:0);
      auto s = lookup(f);
      mod[n++] = (fdb = s*f)*a;
      o = a*s;
    }
    return out;
  }

public:
  Op(const std::vector<S> &table, unsigned int sr, 
     std::size_t vsize) :
    tab(table),out(vsize),mod(vsize),fdb(0),fs(sr),
    phs(0),lobits(0),fac(maxlen/sr){
    for(unsigned long t = tab.size()-1; 
        (t & maxlen) == 0; t <<= 1) lobits += 1;
    lomask = (1 << lobits) - 1;
  }

  unsigned int vsize(){return out.size();}
  unsigned int sr(){return fs;}
  const S *data(){return out.data();}
  
  const std::vector<S> &operator()(){return mod;}
  const std::vector<S> &operator()(S a,S fr,S g=0){
    return process(a,fr,nullptr,0);
  }
  const std::vector<S> &operator()(S a, S fr,
                                   const std::vector<S> &fm,
                                   S g = 0) {
    return process(a,fr,fm.data(),0);
  }
};

/**
   Stacked FM template class
   takes sample type
*/
class StackedFM {
  Op<float> mod0; 
  Op<float> mod1;
  Op<float> car;
  std::vector<float> out;
  std::size_t ovs;
  SRC_STATE* stat;
  SRC_DATA cvt;
  
public:
  StackedFM(const std::vector<float> &table,unsigned int fs,
            std::size_t os,std::size_t vsize = def_vsize) :
    mod0(table,fs*os,vsize*os),mod1(table,fs*os,vsize*os),
    car(table,fs*os,vsize*os),out(vsize),ovs(os){
    int err;
    stat = src_new (SRC_SINC_FASTEST,1,&err);
    cvt.src_ratio = 1./ovs;
    cvt.input_frames = vsize*ovs;
    cvt.data_in = car.data();
    cvt.output_frames = vsize;
    cvt.data_out = out.data();
    cvt.end_of_input = 0;
  };

  ~StackedFM(){src_delete(stat);}

  unsigned int vsize(){return out.size();}
  unsigned int fs(){return car.sr()/ovs;}
  const float *data() {return out.data();}

  const std::vector<float> &operator()(float a,float fc,
                                       float fm0,float fm1,
				       float z0,float z1){
    mod0(z0,fm0);
    mod1(z1,fm1,mod0());
    car(a,fc,mod1());
    src_process(stat, &cvt);
    return out;
  }
};



int main(int argc, const char* argv[]) {
  if(argc > 3) {
    int ovs = argc>5?std::atoi(argv[5]):8;
    int sr = argc>4?std::atoi(argv[4]):def_sr;
    auto dur = std::atof(argv[1]);
    auto amp = std::atof(argv[2]);
    auto fr = std::atof(argv[3]);
    std::vector<float> tab(1025);
    std::size_t n = 0;
    for(auto &s : tab)
      s = std::cos(twopi/(tab.size()-1)*n++);
    StackedFM fm(tab,sr,ovs);
    for(std::size_t n = 0; n < fm.fs()*dur;
        n += fm.vsize()) {
      auto sig = fm(amp,fr,fr,fr,3,2);
      for(auto s : sig)
        std::cout << s << std::endl;
    }
  } else
    std::cout << "usage: " << argv[0] <<
      " dur(s) amp freq(Hz) [sr] [osr]" << std::endl;
  return 0;
}
