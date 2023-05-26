import ctcsound
import numpy as np
import matplotlib.pyplot as plt

def fft(x):
  x /= abs(max(x)) 
  s = abs(np.fft.rfft(x)/x.size)
  return 20*np.log10(s+0.000001)

cs = ctcsound.Csound()
cs.compile_(['','epsilon.csd'])
sr = cs.sr()
chan = cs.nchnls()
buffer = np.zeros(int(sr)*chan)
r = cs.performKsmps()
cnt = 0
ksmps = cs.ksmps()
while r == 0:
 if cnt < int(sr)*chan:
  buffer[cnt:cnt+ksmps*chan] = cs.spout()   
 r = cs.performKsmps()
 cnt += ksmps*chan

end = chan*round(sr/50)

freqs = np.arange(0,buffer.size/4+1)
time = np.arange(0,end//chan)/(sr)

fig,axs = plt.subplots(1,2,figsize=[8,3.5])

axs[0].plot(time*1000, buffer[0:end:chan], 'k')
axs[1].plot(time*1000,buffer[1:end+1:chan], 'k') 

axs[1].set_xlabel('time (ms)')
axs[0].set_xlabel('time (ms)')
axs[1].set_ylabel('amplitude')
axs[0].set_ylabel('amplitude')

axs[0].set_title('$\phi(t)$')
axs[1].set_title('$\epsilon(t)$')

for i in (0,1):
      axs[i].grid()
plt.tight_layout()
plt.savefig("epsilon.png")
plt.show()
