#include <vector>
#include <cmath>
const double twopi = 2*M_PI;
const std::size_t def_vsize = 64;
const double def_sr = 44100.; 

// integer indexing oscillator (32bit)
template<typename S>
class Op {
  static constexpr long maxlen = 0x100000000; 
  const std::vector<S> &tab;  
  std::vector<S> out;
  std::vector<S> mod; 
  int lobits; 
  int lomask; 
  S lofac; 
  S fs;  
  S fac; 
  unsigned int phs;
  
  S process(S amp, int si) {
   S frac = (phs & lomask)*lofac; 
   unsigned int ndx = phs >> lobits; 
   S s = amp*(tab[ndx] + frac*(tab[ndx+1] 
   			- tab[ndx])); 
   phs += si; 
   return s;
  }

  public:
  Op(const std::vector<S> &table, float sr, 
       std::size_t vsize) :
    tab(table), out(vsize), mod(vsize), lobits(0), fs(sr), 
    fac(maxlen/sr), phs(0) {
    for(unsigned long t = tab.size()-1; 
      (t & maxlen) == 0; t <<= 1)
      lobits += 1;
    lomask = (1 << lobits) - 1;
    lofac = 1.f/(lomask + 1);
  }

  unsigned int vsize() { return out.size(); }
  S sr() { return fs; }
  
  const std::vector<S> &operator()() { return mod; }

  const std::vector<S> &operator() (S amp, S fr) {
    int si = (unsigned int) (fr*fac);
    std::size_t n = 0;
    for(auto &s : out) {
      s = process(amp, si);
      mod[n++] = s*fr;
    }
    return out;
  }

  const std::vector<S> &operator() (S amp, S fr,
  			const std::vector<S> &fm) {
    std::size_t n = 0;
    S f;
    for(auto &s : out) {
      f = fr + fm[n];
      s = process(amp, (int) (f*fac));
      mod[n++] = s*f;
    }
    return out;
  }
};


/**
   Stacked FM template class
   takes sample type
*/
template<typename S> class StackedFM {
  Op<S> mod0; 
  Op<S> mod1;
  Op<S> car;
  
public:
  /** 
      S fs: sampling rate
      unsigned int vsize: signal vector size
  */
  StackedFM(const std::vector<S> &table, S fs = (S) def_sr,
            std::size_t vsize = def_vsize) :
    mod0(table,fs,vsize),mod1(table,fs,vsize),
    car(table,fs,vsize) { };

  /**
     returns signal vector size
  */
  unsigned int vsize() { return car.vsize(); }

  /**
     returns sampling rate
  */
  S fs() { return car.sr();}

  /**
     audio synthesis method
     S a: signal amplitude
     S fc: carrier freq
     S fm0: zero-level fm
     S fm1: first-level fm
     S z0: zero-level mod index
     S z1: first-level mod index

     returns the audio signal vector
  */
  const std::vector<S> &operator()(S a, S fc, S fm0, S fm1,
				   S z0, S z1) {
    mod0(z0,fm0);
    mod1(z1,fm1,mod0());
    return car(a,fc,mod1());
  }
};

#include <iostream>
#include <cstdlib>
#include <samplerate.h>

int main(int argc, const char* argv[]) {
  if(argc > 3) {
    int ovs = argc>5?std::atoi(argv[5]):8;
    float sr = argc>4?std::atof(argv[4]):def_sr;
    auto dur = std::atof(argv[1]);
    auto amp = std::atof(argv[2]);
    auto fr = std::atof(argv[3]);
    std::vector<float> tab(1025);
    std::vector<float> out(def_vsize);
    SRC_DATA cvt;
    cvt.src_ratio = 1./ovs;
    cvt.input_frames = def_vsize*ovs;
    cvt.output_frames = def_vsize;

    cvt.data_out = out.data();

    std::size_t n = 0;
    for(auto &s : tab)
      s = std::cos(twopi/(tab.size()-1)*n++);
    StackedFM<float> fm(tab,sr*ovs,cvt.input_frames);
    
    for(n = 0; n < fm.fs()*dur; n += fm.vsize()) {
      auto sig = fm(amp,fr,fr,fr,3,2);
      cvt.data_in = sig.data();
      src_simple(&cvt,SRC_SINC_FASTEST,1); 
      for(auto s : out)
        std::cout << s << std::endl;
    }
  } else
    std::cout << "usage: " << argv[0] <<
      " dur(s) amp freq(Hz) [sr] [osr]" << std::endl;
  return 0;
}
