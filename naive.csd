<CsoundSynthesizer>
<CsOptions>
-o dac
</CsOptions>
<CsInstruments>
0dbfs = 1
nchnls = 1

instr 1

kenv linseg 0,p3,2
a1 oscili kenv*p5,p5
a2 oscili p5,p5+a1
a3 oscili p4,p5+a2

out a3

endin
  


</CsInstruments>
<CsScore>
i1 0 5 0.5 500
</CsScore>
</CsoundSynthesizer>






<bsbPanel>
 <label>Widgets</label>
 <objectName/>
 <x>0</x>
 <y>0</y>
 <width>0</width>
 <height>0</height>
 <visible>true</visible>
 <uuid/>
 <bgcolor mode="background">
  <r>240</r>
  <g>240</g>
  <b>240</b>
 </bgcolor>
</bsbPanel>
<bsbPresets>
</bsbPresets>
