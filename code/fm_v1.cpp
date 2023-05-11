#include <vector>
#include <cmath>

const double twopi = 2*M_PI;
const unsigned int def_vsize = 64;
const double def_sr = 44100; 

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
  
  S synth(S a, S f) {
    S s = (S) (a*F(ph));
    ph += f/sr;
    while (ph < 0) ph += 1.; 
    while (ph >= 1.) ph -= 1.;
    return s;
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
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = synth(a, f);
    return sig; 
  }

  /**
     S a: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  operator()(S a, const std::vector<S> &fm) {
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = synth(a, fm[n]);
    return sig; 
  }

  /**
     const std::vector<S> am: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  operator()(const std::vector<S> &am,
        const std::vector<S> &fm) {
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = synth(am[n], fm[n]);
    return sig;
  }
};

/**
   Amplifier template class
   takes sample type
*/
template<typename S> class Amp {
  std::vector<S> sig;
public:
  /** 
      unsigned int vsize: signal vector size
  */
  Amp(unsigned int vsize = def_vsize) : sig(vsize) { };
  
  const std::vector<S> &
  operator()(S a,const std::vector<S> &s) {
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = a*s[n];
    return sig;
  }
};

/**
   Add/Offset template class
   takes sample type
*/
template<typename S> class Add {
  std::vector<S> sig;
public:
  /** 
      unsigned int vsize: signal vector size
  */
  Add(unsigned int vsize = def_vsize) : sig(vsize) { };
  
  const std::vector<S> &
  operator()(S a, const std::vector<S> &s) {
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = a + s[n];
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
  Amp<double> amp;
  Add<double> add;

public:
  /** 
      S fs: sampling rate
      unsigned int vsize: signal vector size
  */
  StackedFM(S fs = (S) def_sr, unsigned int vsize = def_vsize) :
    mod0(fs,vsize), mod1(fs,vsize), car(fs,vsize), amp(vsize),
    add(vsize) { };

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
