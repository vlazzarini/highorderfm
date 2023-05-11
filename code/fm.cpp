#include <vector>
#include <cmath>

const double twopi = 2*M_PI;
const unsigned int def_vsize = 64;
const double def_sr = 44100.; 

template<typename S, S (*F)(S)>
inline S lookup2pi(S ph) {
  return F(twopi*ph);
}

/**
   Oscillator template class
   takes sample type and lookup function
*/
template<typename S, double (*F)(double)> class Osc {
  double ph;
  S sr;
  std::vector<S> sig;
  
  double synth(S& s, S a, S f, double phs) {
    s = (S) (a*F(phs));
    phs += f/sr;
    while (phs < 0) phs += 1.; 
    while (phs >= 1.) phs -= 1.;
    return phs;
  }

public:
  /** 
      S fs: sampling rate
      unsigned int vsize: signal vector size
  */
  Osc(S fs = (S) def_sr,
      unsigned int vsize = def_vsize) :
    ph(0.), sr(fs), sig(vsize) { };

  /**
     returns signal vector size
  */
  unsigned int vsize() { return sig.size(); }

  /** 
      returns sampling rate
  */
  S fs() { return sr; }

  /**
     S a: signal amplitude
     S f: signal frequency
  */
  const std::vector<S> &
  operator()(S a, S f) {
    double phs = ph;
    S* s = sig.data();
    for (int n = 0; n < sig.size(); n++) 
      phs = synth(s[n], a, f, phs);
    ph = phs;
    return sig; 
  }

  /**
     S a: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  operator()(S a, const std::vector<S> &fm) {
    double phs = ph;
    S* s = sig.data();
    for (int n = 0; n < sig.size(); n++) 
      phs = synth(s[n], a, fm[n], phs);
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
    S* s = sig.data();
    for (int n = 0; n < sig.size(); n++) 
      phs = synth(s[n], am[n], fm[n], phs);
    ph = phs;
    return sig;
  }
 
};



/**
   Stacked FM template class
   takes sample type
*/
template<typename S> class StackedFM {
  Osc<double,lookup2pi<double,std::cos>> mod0; 
  Osc<double,lookup2pi<double,std::cos>> mod1;
  Osc<double,lookup2pi<double,std::cos>> car;
  struct Buffer<S> {
    std::vector<S> sig;
    Buffer(std::vector<S> s) : sig(std::move(s)) { };
    Buffer(std::
   
public:
  /** 
      S fs: sampling rate
      unsigned int vsize: signal vector size
  */
  StackedFM(S fs = (S) def_sr, unsigned int vsize = def_vsize) :
    mod0(fs,vsize), mod1(fs,vsize), car(fs,vsize), sig(vsize) { };

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
    auto s0 = fm1 + mod0(z0*fm0,fm0);
    auto s1 = fc + mod1(z1*s0,s0);
    return car(a,s1);
  }
};

#include <iostream>
#include <cstdlib>
int main(int argc, const char* argv[]) {
  if(argc > 3) {
    auto dur = std::atof(argv[1]);
    auto amp = std::atof(argv[2]);
    auto fr = std::atof(argv[3]);
    StackedFM<double> fm;
    for(int n = 0; n < fm.fs()*dur; n += fm.vsize()) {
      auto out = fm(amp,fr,fr,fr,3,2);
      for(auto s : out)
        std::cout << s << std::endl;
    }
  } else
    std::cout << "usage: " << argv[0] <<
      " dur(s) amp freq(Hz)" << std::endl;
  return 0;
}
