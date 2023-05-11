#include <vector>
#include <cmath>

const double twopi = 2*M_PI;

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
  Osc(S fs = (S) 44100., unsigned int vsize = 64) :
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
  audio(S a, S f) {
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = synth(a, f);
    return sig; 
  }

  /**
     S a: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  audio(S a, const std::vector<S> &fm) {
    for (int n = 0; n < sig.size(); n++) 
      sig[n] = synth(a, fm[n]);
    return sig; 
  }

  /**
     const std::vector<S> am: signal amplitude
     const std::vector<S> fm: signal frequency
  */
  const std::vector<S> &
  audio(const std::vector<S> &am,
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
  Amp(unsigned int vsize = 64) : sig(vsize) { };
  
  const std::vector<S> &
  audio(S a,const std::vector<S> &s) {
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
  Add(unsigned int vsize = 64) : sig(vsize) { };
  
  const std::vector<S> &
  audio(S a, const std::vector<S> &s) {
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
  StackedFM(S fs = (S) 44100., unsigned int vsize = 64) :
    mod0(fs,vsize), mod1(fs,vsize), car(fs,vsize) { };

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
  const std::vector<S> &audio(S a, S fc, S fm0, S fm1,
                              S z0, S z1) {
    const std::vector<S> &s0 =
      add.audio(fm1, mod0.audio(z0*fm0,fm0));
    const std::vector<S> &s1 =
      add.audio(fc,mod1.audio(amp.audio(z1,s0),s0));
    return car.audio(a,s1);
  }
};

#include <iostream>
#include <cstdlib>
int main(int argc, const char* argv[]) {
  if(argc > 3) {
    double dur = std::atof(argv[1]);
    double amp = std::atof(argv[2]);
    double fr = std::atof(argv[3]);
    StackedFM<double> fm;
    for(int n = 0; n < fm.fs()*dur; n += fm.vsize()) {
      const std::vector<double> &out =
        fm.audio(amp,fr,fr,fr,3,2);
      for(int i = 0; i < fm.vsize(); i++)
        std::cout << out[i] << std::endl;
    }
  } else
    std::cout << "usage: " << argv[0] <<
      " dur(s) amp freq(Hz)" << std::endl;
  return 0;
}
