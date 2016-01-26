.inc 'm_params.sp'
.subckt opamp vinp vinn vout vdd gnd
m1      d1    vinp  s1  vdd pmos_3p3 L='l_fixed*1u' W='w1_2*1u'   M=m1_2
m2      d2    vinn  s1  vdd pmos_3p3 L='l_fixed*1u' W='w1_2*1u'   M=m1_2 
m3      d3    vinp  s3  gnd nmos_3p3 L='l_fixed*1u' W='w3_4*1u'   M=m3_4 
m4      d4    vinn  s3  gnd nmos_3p3 L='l_fixed*1u' W='w3_4*1u'   M=m3_4 
m5      s1    g5    vdd vdd pmos_3p3 L='l_fixed*1u' w='w5*1u'     M=m5 
m6      s3    ibias gnd gnd nmos_3p3 L='l_fixed*1u' w='w6*1u'     M=m6 
m7      ibias ibias gnd gnd nmos_3p3 L='l_fixed*1u' w='w7*1u'     M=m7 
m8      g5    ibias gnd gnd nmos_3p3 L='l_fixed*1u' w='w8*1u'     M=m8 
m9      g5    g5    vdd vdd pmos_3p3 L='l_fixed*1u' w='w9*1u'     M=m9 
m10     d10   ibias gnd gnd nmos_3p3 L='l_fixed*1u' w='w10*1u'    M=m10
m11     d10   d10   s11 vdd pmos_3p3 L='l_fixed*1u' w='w11*1u'    M=m11
m12     s11   d10   vdd vdd pmos_3p3 L='l_fixed*1u' w='w12*1u'    M=m12
m13     d13   g5    vdd vdd pmos_3p3 L='l_fixed*1u' w='w13*1u'    M=m13
m14     d13   d13   s14 gnd nmos_3p3 L='l_fixed*1u' w='w14*1u'    M=m14
m15     s14   d13   gnd gnd nmos_3p3 L='l_fixed*1u' w='w15*1u'    M=m15
m16     d16   d13   d2  gnd nmos_3p3 L='l_fixed*1u' W='w16_18*1u' M=m16_18
m17     d2    d16   gnd gnd nmos_3p3 L='l_fixed*1u' W='w17_19*1u' M=m17_19
m18     d18   d13   d1  gnd nmos_3p3 L='l_fixed*1u' W='w16_18*1u' M=m16_18
m19     d1    d16   gnd gnd nmos_3p3 L='l_fixed*1u' W='w17_19*1u' M=m17_19
m20     d16   d10   d4  vdd pmos_3p3 L='l_fixed*1u' W='w20*1u'    M=m20
m21     d21   d10   d3  vdd pmos_3p3 L='l_fixed*1u' W='w21*1u'    M=m21
m22     d4    g5    vdd vdd pmos_3p3 L='l_fixed*1u' W='w22_23*1u' M=m22_23
m23     d3    g5    vdd vdd pmos_3p3 L='l_fixed*1u' W='w22_23*1u' M=m22_23
m24     d24   g5    vdd vdd pmos_3p3 L='l_fixed*1u' W='w24*1u'    M=m24
m25     d25   d10   d24 vdd pmos_3p3 L='l_fixed*1u' W='w25*1u'    M=m25
m26     d25   d25   s26 gnd nmos_3p3 L='l_fixed*1u' W='w26*1u'    M=m26
m27     s26   s26   gnd gnd nmos_3p3 L='l_fixed*1u' W='w27*1u'    M=m27
m28     d28   ibias gnd gnd nmos_3p3 L='l_fixed*1u' W='w28*1u'    M=m28
m29     d29   d13   d28 gnd nmos_3p3 L='l_fixed*1u' W='w29*1u'    M=m29
m30     d29   d29   s30 vdd pmos_3p3 L='l_fixed*1u' W='w30*1u'    M=m30
m31     s30   s30   vdd vdd pmos_3p3 L='l_fixed*1u' W='w31*1u'    M=m31
m32     vout  d21   vdd vdd pmos_3p3 L='l_fixed*1u' W='w32*1u'    M=m32
m33     vout  d18   gnd gnd nmos_3p3 L='l_fixed*1u' W='w33*1u'    M=m33
m34     d21   d25   d18 gnd nmos_3p3 L='l_fixed*1u' W='w34*1u'    M=m34
m35     d18   d29   d21 vdd pmos_3p3 L='l_fixed*1u' w='w35*1u'    M=m35
ibias vdd ibias dc='ival*1e-3'
cm d1 vout 'cm*1p'
cload vout gnd 1p 
.ENDS
