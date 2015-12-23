.subckt opamp vin+ vin- vo vdd vss
.param lb=2
.param l1=2
.param l3=2
*.param l5=0.35
*.param l7=0.35
*.param l9=1
*.param ln=0.6
*.param lp=1

ib2 vb1 vb2 'ib*1u'
ma0 vb1 vb1 vdd vdd pch w='wb1*1u' l='lb*1u'
ma2 vb2 vb2 vss vss nch w='wa2*1u' l='lb*1u'

mb1 1 vb1 vdd vdd pch w='wb1*1u' l='lb*1u'
mb2 8 vb1 vdd vdd pch w='wb2*1u' l='lb*1u'
m1  2 vin- 1  vdd pch w='w1*1u'  l='l1*1u'
m2  3 vin+ 1  vdd pch w='w1*1u'  l='l1*1u'
m3  2 vb2 vss vss nch w='w3*1u'  l='l3*1u'
m4  3 vb2 vss vss nch w='w3*1u'  l='l3*1u'
m5  6 2   vss vss nch w='w5*1u'  l='l5*1u'
m6  7 3   vss vss nch w='w5*1u'  l='l5*1u'
m7  4 6   2   vss nch w='w7*1u'  l='l7*1u'
m8  5 7   3   vss nch w='w7*1u'  l='l7*1u'
m9  4 4   vdd vdd pch w='w9*1u'  l='l9*1u'
m10 5 4   vdd vdd pch w='w9*1u'  l='l9*1u'
m11 10 5  vdd vdd pch w='w11*1u' l='lp*1u'
m12 11 4  vdd vdd pch w='w12*1u' l='lp*1u'
m16 vo 5  vdd vdd pch w='w16*1u' l='lp*1u'
m13 10 9  vss vss nch w='w13*1u' l='ln*1u'
m14 11 10 vss vss nch w='w14*1u' l='ln*1u'
m15 vo 11 vss vss nch w='w15*1u' l='ln*1u'
cmm 3 vo 'cmm*1p'
rz  9 10 'rz*1k'
cz  9 vss 'cz*1p'
r1  8 6 'r1*1k'
r2  8 7 'r1*1k'
cl vo vss 15n
.ends


