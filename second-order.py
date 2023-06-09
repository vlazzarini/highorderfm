import ctcsound
import numpy as np
import matplotlib.pyplot as plt

def fft(x):
  s = abs(np.fft.rfft(x)/(x.size/2))
  s[0] *= 0.5 # DC has a factor of 2 in rfft()
  s *= 1/max(s)
  return 20*np.log10(s+0.000001)

cs = ctcsound.Csound()
cs.compile_(['','highorder.csd','-r5000000'])
cs.readScore('''
i1 0 1 1 500
i2 0 1 1 500
''')
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

end = chan*3*round(sr/500)

freqs = np.arange(0,buffer.size/4+1)
time = np.arange(0,end//chan)/(sr)

fig,axs = plt.subplots(2,2)

axs[0,0].plot(time*1000, buffer[0:end:chan], 'k')
axs[0,1].plot(time*1000,buffer[1:end+1:chan], 'k') 
axs[1,0].plot(freqs/1000, fft(buffer[0::chan]), 'k')
axs[1,1].plot(freqs/1000, fft(buffer[1::chan]),  'k')

axs[0,1].set_xlim(0,2.01)
axs[0,0].set_xlim(0,2.01)

axs[0,1].set_xlabel('time (ms)')
axs[0,0].set_xlabel('time (ms)')
axs[0,1].set_ylabel('amplitude')
axs[0,0].set_ylabel('amplitude')


axs[1,0].set_ylim(-90,0)
axs[1,1].set_ylim(-90,0)
axs[1,1].set_xlim(-1,20)
axs[1,0].set_xlim(-1,20)

axs[1,1].set_xlabel('frequency (KHz)')
axs[1,0].set_xlabel('frequency (KHz)')

axs[1,1].set_ylabel('magnitude (dB)')
axs[1,0].set_ylabel('magnitude (dB)')

axs[0,0].set_title('FM')
axs[0,1].set_title('PM')

for i in (0,1):
  for j in (0,1):
      axs[i,j].grid()
plt.tight_layout()
plt.savefig("fm-pm.png")
plt.show()
