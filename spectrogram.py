import ctcsound
import numpy as np
from matplotlib import pyplot as pl
import sys

cs = ctcsound.Csound()
cs.compileCsd(sys.argv[1])
sr = cs.sr()
ksmps = cs.ksmps()

print(ksmps)

dur = float(sys.argv[2])
end = int(dur*sr)
buffer = np.zeros(int(end)+ksmps)
cs.start()
time = 0
while time < end:
    err = cs.performKsmps()
    buffer[time:time+ksmps] = cs.spout()
    time += ksmps

pl.rcParams.update({'font.size': 16})
fig,axs = pl.subplots(1,1,figsize=[8,5])
#axs.grid(True)
axs.spines['top'].set_visible(False)
axs.spines['right'].set_visible(False)
axs.spines['bottom'].set_visible(False)
axs.spines['left'].set_visible(False)
axs.set_xlabel("time (s)")
axs.set_ylabel("frequency (kHz)")
axs.set_yticks(np.arange(1000,5001,1000))
axs.set_yticklabels(['1','2','3','4', '5'])


axs.specgram(buffer,16384,sr,cmap="binary")
axs.set_ylim(0, 5000)
pl.tight_layout()
pl.show()

