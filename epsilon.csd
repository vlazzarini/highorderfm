<CsoundSynthesizer>
<CsOptions>
-n
</CsOptions>
<CsInstruments>

0dbfs = 1
nchnls = 2
instr 1
 i2pi = 2*$M_PI
 ifm = p5
 ifc = ifm
 iz = 3
 ia = p4
 aph phasor ifm
 afm = ia*cos(i2pi*integ((ifc + iz*ifm*cos(i2pi*aph))/sr))
 apm = ia*cos(i2pi*phasor(ifc) + iz*sin(i2pi*aph))
   outs apm, afm - apm
endin


</CsInstruments>
<CsScore>
i1 0 1 1 100
</CsScore>
</CsoundSynthesizer>
<bsbPanel>
 <label>Widgets</label>
 <objectName/>
 <x>100</x>
 <y>100</y>
 <width>320</width>
 <height>240</height>
 <visible>true</visible>
 <uuid/>
 <bgcolor mode="nobackground">
  <r>255</r>
  <g>255</g>
  <b>255</b>
 </bgcolor>
</bsbPanel>
<bsbPresets>
</bsbPresets>
