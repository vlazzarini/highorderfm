#include <vector>
#include <cmath>
const double twopi = 2*M_PI;
const std::size_t def_vsize = 64;
const double def_sr = 44100.; 

/**
   Oscillator template class
   takes sample type and lookup function
*/
template<typename S> class Osc {
  double ph;
  S ts;
  std::vector<S> sig;
  
  S synth(S a,S f,double &phs) {
    S s = (S) (a*std::cos(phs*twopi));
    phs += f*ts;
    while (phs < 0) phs += 1.; 
    while (phs >= 1.) phs -= 1.;
    return s;
  }

public:
  /** 
      S fs: sampling rate
      unsigned int vsize: signal vector size
  */
  Osc(S fs = (S) def_sr,
      std::size_t vsize = def_vsize) :
    ph(0.), ts(1/fs), sig(vsize) { };

  /**
     returns signal vector size
  */
  unsigned int vsize() { return sig.size(); }

  /** 
      returns sampling rate
  */
  S fs() { return 1/ts; }

  /**
     S a: signal amplitude
     S f: signal frequency
  */
  const std::vector<S> &
  operator()(S a,S f) {
    double phs = ph;
    for (auto& s : sig) 
      s = synth(a, f, phs);
    ph = phs;
    return sig; 
  }

  /**
     S a: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  operator()(S a,
        const std::vector<S> &fm) {
    double phs = ph;
    int n = 0;
    for (auto& s : sig) 
      s = synth(a, fm[n++], phs);
    ph = phs;
    return sig; 
  }

  /**
     const std::vector<S> am: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  operator()(const std::vector<S> &am,
        const std::vector<S> &fm) {
    double phs = ph;
    int n = 0;
    for (auto& s : sig) { 
      s = synth(am[n], fm[n], phs);
      n++;
    }
    ph = phs;
    return sig;
  }
};


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
  StackedFM(S fs = (S) def_sr,
            std::size_t vsize = def_vsize) :
    mod0(fs,vsize),mod1(fs,vsize), car(fs,vsize),
    amp([](S a, S b)->S{ return a*b; },vsize),
    add([](S a, S b)->S{ return a+b; },vsize) { };

  /**
     returns signal vector size
  */
  unsigned int vsize() { return car.vsize(); }

  /**
     returns sampling rate
  */
  S fs() { return car.fs();}

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
    StackedFM<double> fm(sr);
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
