#include <vector>
#include <cmath>
const double twopi = 2*M_PI;
const std::size_t def_vsize = 64;
const double def_sr = 44100.; 

template<typename S> class Op {
  std::vector<S> sig;
  std::function<S(S,S)> op;
public:
  /** 
      unsigned int vsize: signal vector size
  */
  Op(std::function<S(S,S)> f,
     std::size_t vsize = def_vsize)
    : op(f), sig(vsize) { };
  
  const std::vector<S> &
  operator()(S a, const std::vector<S> &s) {
    int n = 0;
    for(auto &out:sig)
      out = op(a,s[n++]);
    return sig;
  }
};


// integer indexing oscillator (32bit)
template<typename S>
class Osc {
  static constexpr long maxlen = 0x100000000; 
  const std::vector<S> &tab;  
  std::vector<S> out; 
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
  Osc(const std::vector<S> &table, float sr, 
       std::size_t vsize) :
    tab(table), out(vsize), lobits(0), fs(sr), 
    fac(maxlen/sr), phs(0) {
    for(unsigned long t = tab.size()-1; 
      (t & maxlen) == 0; t <<= 1)
      lobits += 1;
    lomask = (1 << lobits) - 1;
    lofac = 1.f/(lomask + 1);
  }

  unsigned int vsize() { return out.size(); }
  S sr() { return fs; }

  const std::vector<S> &operator() (S amp, S fr) {
    int si = (unsigned int) (fr*fac);
    for(auto &s : out) s = process(amp, si);
    return out;
  }

  const std::vector<S> &operator() (
  		const std::vector<S> &amp,
  		const std::vector<S> &fr) {
    std::size_t n = 0;
    for(auto &s : out) {
      s = process(amp[n], (int) (fr[n]*fac));
      n++;
    }
    return out;
  }

  const std::vector<S> &operator() (S amp,
  			const std::vector<S> &fr) {
    std::size_t n = 0;
    for(auto &s : out)
      s = process(amp, (int) (fr[n++]*fac));
    return out;
  }
};


/**
   Stacked FM template class
   takes sample type
*/
template<typename S> class StackedFM {
  Osc<S> mod0; 
  Osc<S> mod1;
  Osc<S> car;
  Op<S> amp;
  Op<S> add;
  
public:
  /** 
      S fs: sampling rate
      unsigned int vsize: signal vector size
  */
  StackedFM(const std::vector<S> &table, S fs = (S) def_sr,
            std::size_t vsize = def_vsize) :
    mod0(table,fs,vsize),mod1(table,fs,vsize), car(table,fs,vsize),
    amp([](S a, S b)->S{ return a*b; },vsize),
    add([](S a, S b)->S{ return a+b; },vsize) { };

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
    auto s0 = add(fm1, mod0(z0*fm0,fm0));
    auto s1 = add(fc, mod1(amp(z1,s0),s0));
    return car(a,s1);
  }
};

#include <iostream>
#include <cstdlib>
int main(int argc, const char* argv[]) {
  if(argc > 3) {
    double sr =argc>4?std::atof(argv[4]):def_sr;
    auto dur = std::atof(argv[1]);
    auto amp = std::atof(argv[2]);
    auto fr = std::atof(argv[3]);
    std::vector<double> tab(1025);

    std::size_t n = 0;
    for(auto &s : tab)
      s = std::cos((n++) *  twopi/(tab.size()-1));
    StackedFM<double> fm(tab,sr);
    for(int n = 0; n < fm.fs()*dur; n += fm.vsize()) {
      auto out = fm(amp,fr,fr,fr,3,2);
      for(auto s : out)
        std::cout << s << std::endl;
    }
  } else
    std::cout << "usage: " << argv[0] <<
      " dur(s) amp freq(Hz) [sr]" << std::endl;
  return 0;
}
