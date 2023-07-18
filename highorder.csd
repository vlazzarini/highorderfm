<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>

0dbfs = 1
nchnls = 2

gisin ftgen 0,0,2^24,10,1


instr 1 ; cos modulator 
indx0 = 3
indx1 = 2
ifc = p5
iamp = p4
ifm1 = ifc
ifm0 = ifm1
// level 1 FM
aphm0 phasor ifm0
asin = sin(2*$M_PI*aphm0)
acos = cos(2*$M_PI*aphm0)
aphm1 phasor ifm1 + indx0*ifm0*acos
afm = cos(2*$M_PI*aphm1)
// level 1 PM
aphp phasor ifm1
apm = cos(2*$M_PI*aphp + indx0*asin)
aeps = afm - apm     
//athe = integ(2*$M_PI*(ifc + indx0*ifm0*acos)*indx1*aeps/sr) 
aphs phasor ifc + indx1*(ifm1 + indx0*ifm0*acos)*afm
a3 = cos(2*$M_PI*aphs) 
  outch 1, a3*iamp 
endin

instr 2; PM
indx0 = 3
indx1 = 2
ifc = p5
iamp = p4
ifm1 = ifc
ifm0 = ifm1
aphm0 phasor ifm0
asin = sin(2*$M_PI*aphm0)
aphm1 phasor ifm1
a2 = sin(2*$M_PI*aphm1 + indx0*asin)
aph  phasor ifc
a3 = cos(2*$M_PI*aph + indx1*a2) 
  outch 2, a3*iamp
endin


instr 3 ; eq 12
indx0 = 3
indx1 = 2
ifc = p5
iamp = p4
ifm1 = ifc
ifm0 = ifm1
// level 1 FM
aphm0 phasor ifm0
aphm1 phasor ifm1
aphs phasor ifc + indx1*(ifm1 + indx0*ifm0*cos(2*$M_PI*aphm0))*cos(2*$M_PI*aphm1 +    indx0*sin(2*$M_PI*aphm0))
a3 = cos(2*$M_PI*aphs) 
  outch 1, a3*iamp 
endin

opcode OpFM,aa,kkap
kam,kfm,afm,iph xin
asig oscili kam,kfm+afm,-1,0.25
  xout asig,asig*(kfm+afm)
endop

instr 4
indx0 = 3
indx1 = 2
ifc = p5
iamp = p4
ifm1 = ifc
ifm0 = ifm1
afm0 = 0
asig,afm1 OpFM indx0,ifm0,afm0
asig,afm2 OpFM indx1,ifm1,afm1
asig,afm  OpFM iamp,ifc,afm2
   outch 1,asig
endin

instr 5
afm init 0
kndx = 1; line 0,p3,0.9
ifm = p5
asig,afm OpFM 1,ifm,afm*kndx
  outch 1, asig*p4
endin

instr 6
asin init 0
indx = 1
ifm = p5
aphm0 phasor ifm
asin = sin(2*$M_PI*aphm0 + asin*indx) 
  outch  2, asin*p4 
endin

instr 7
indx0 = 3
indx1 = 2
ifc = p5
iamp = p4
ifm1 = ifc
ifm0 = ifm1
afm0 = 0
amod0  oscili indx0,ifm0,-1,0.25
amod1  oscili indx1, ifm1 + amod0*ifm0,-1,0.25
asig   oscili  p4, ifc + amod1*(ifm1 + amod0*ifm0),-1,0.25
   outch 1,asig
endin

</CsInstruments>
<CsScore>
f1 0 16385 7 0 8192 0.5 8192 1
f0 1
</CsScore>
</CsoundSynthesizer>
















<bsbPanel>
 <label>Widgets</label>
 <objectName/>
 <x>0</x>
 <y>0</y>
 <width>553</width>
 <height>519</height>
 <visible>true</visible>
 <uuid/>
 <bgcolor mode="nobackground">
  <r>255</r>
  <g>255</g>
  <b>255</b>
 </bgcolor>
 <bsbObject type="BSBScope" version="2">
  <objectName/>
  <x>39</x>
  <y>31</y>
  <width>514</width>
  <height>286</height>
  <uuid>{25efac97-2736-40fa-b371-ff4628d9a023}</uuid>
  <visible>true</visible>
  <midichan>0</midichan>
  <midicc>-3</midicc>
  <description/>
  <value>1.00000000</value>
  <type>scope</type>
  <zoomx>2.00000000</zoomx>
  <zoomy>1.00000000</zoomy>
  <dispx>1.00000000</dispx>
  <dispy>1.00000000</dispy>
  <mode>0.00000000</mode>
  <triggermode>NoTrigger</triggermode>
 </bsbObject>
 <bsbObject type="BSBScope" version="2">
  <objectName/>
  <x>42</x>
  <y>321</y>
  <width>511</width>
  <height>198</height>
  <uuid>{d896d4b0-8df1-41f2-99f6-aabe2c83e2ad}</uuid>
  <visible>true</visible>
  <midichan>0</midichan>
  <midicc>-3</midicc>
  <description/>
  <value>2.00000000</value>
  <type>scope</type>
  <zoomx>2.00000000</zoomx>
  <zoomy>1.00000000</zoomy>
  <dispx>1.00000000</dispx>
  <dispy>1.00000000</dispy>
  <mode>0.00000000</mode>
  <triggermode>NoTrigger</triggermode>
 </bsbObject>
</bsbPanel>
<bsbPresets>
</bsbPresets>
