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
  const std::vector<double> &tab;  
  std::vector<S> out;
  std::vector<S> mod;
  S fdb;
  unsigned int fs;  
  unsigned int phs;
  unsigned int lobits;
  unsigned int fac;
  unsigned int lomask;
  
  double lookup(S f){
    unsigned int ndx = phs >> lobits; 
    auto s = tab[ndx] + (phs & lomask)*(tab[ndx+1]
                                        - tab[ndx])/(lomask + 1); 
    phs += (int)(f*fac); 
    return s;
  }

  const std::vector<S> &process(S a,S fr,
                                const S* fm,S g){
    std::size_t n = 0;
    for(auto &o : out) {
      auto f = fr+fdb*g+(fm?fm[n]:0);
      auto s = lookup(f);
      mod[n++] = (S) ((fdb = s*f)*a);
      o = (S) (a*s);
    }
    return out;
  }

public:
  Op(const std::vector<double> &table, unsigned int sr, 
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
    return process(a,fr,nullptr,g);
  }
  const std::vector<S> &operator()(S a, S fr,
                                   const std::vector<S> &fm,
                                   S g = 0) {
    return process(a,fr,fm.data(),g);
  }
};


int main(int argc, const char* argv[]) {
  if(argc > 3) {
    int sr = argc>4?std::atoi(argv[4]):def_sr;
    auto dur = std::atof(argv[1]);
    auto amp = std::atof(argv[2]);
    auto fr = std::atof(argv[3]);
    std::vector<double> table(1025);
    std::size_t n = 0;
    for(auto &s : table)
      s = std::cos(twopi/(table.size()-1)*n++);
    
    Op<float> fm(table,sr,ovs);
    for(std::size_t n = 0; n < fm.sr()*dur;
        n += fm.vsize()) {
      auto &sig = fm(amp,fr,1);
      for(auto s : sig)
        std::cout << s << std::endl;
    }
  } else
    std::cout << "usage: " << argv[0] <<
      " dur(s) amp freq(Hz) [sr] [osr]" << std::endl;
  return 0;
}
