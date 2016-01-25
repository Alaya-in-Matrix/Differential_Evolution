*OMAMP OPTIMIZATION
*
*Netlist
*Operational Amplifier with Single-Ended Output
*Author: Minghua Li
*University of Texas at Dallas
*Email: mxl095420@utdallas.edu
*9/23/2012
.option post
.option ingold=2
.option numdgt=10
.lib 'cmos035.lib' TT
.inc param.sp
.inc Single_ended_opamp.txt
.temp 27

.param vdd_v=2
.param vin_cm='0.5*vdd_v'
.param vin_low='0.375*vdd_v'
.param vin_high='0.625*vdd_v'
.param period=100e-6
.param risetime=1e-9
.param falltime=1e-9

xac vin_ac+ vin_ac- vo_ac vdd_ac vss opamp
vdd_ac vdd_ac 0 vdd_v
e+ vin_ac+ 101 100 0 0.5
e- vin_ac- 101 100 0 -0.5
vcm 101 0 dc=vin_cm
vs 100 0 dc=0 ac=1

xtr vin_tr+ vo_tr   vo_tr vdd_tr vss opamp
vpulse vin_tr+ 0 pulse (vin_low vin_high 0 risetime falltime 'period/2' period)
vdd_tr vdd_tr 0 vdd_v

xsw vin_sw+ vin_sw- vo_sw vdd_sw vss opamp
vdd_sw vdd_sw 0 vdd_v
rsw10 vin_sw- vo_sw 10meg
rsw1 vin_sw vin_sw- 1meg
vin_sw vin_sw 0 vin_cm
vin_sw+ vin_sw+ 0 vin_cm

xpsr vin_psr+ vo_psr vo_psr vdd_psrac vss opamp
vin_psr+ vin_psr+ 0 vin_cm
vdd_psr vdd_psr 0 vdd_v
vdd_psrac vdd_psrac vdd_psr dc=0 ac=1

xcmrr vin_cmrr+ vin_cmrr- vo_cmrr vdd_cmrr vss opamp
e_cmrr+ vin_cmrr+ vcm_cmrr 100cmrr 0 0.5
e_cmrr- vin_cmrr- vo_cmrr 100cmrr 0 0.5
vcm_cmrr vcm_cmrr 0 dc=vin_cm
vs_cmrr 100cmrr 0 dc=0 ac=1
vdd_cmrr vdd_cmrr 0 vdd_v

vss vss 0 0

** .dc vcm 0 vdd_v 0.1
** .meas dc Iq max 'abs(i(vdd_ac))' from=0 to=vdd_v

.dc vdd_ac vdd_v vdd_v vdd_v
.meas dc iq find 'abs(i(vdd_ac))' at vdd_v

** .param vin_incre='vdd_v*0.005'
** .dc vin_sw 0 vdd_v vin_incre 
** .print v(vo_sw)

.ac dec 100 1 50Meg
.meas ac gain max  vdb(vo_ac)   from=1 to=50Meg
.meas ac ugf  when vdb(vo_ac)=0
.meas ac pm   find vp(vo_ac)    at=ugf
.meas ac gm   find vdb(vo_ac)   when   vp(vo_ac)=-178
.meas ac psr  min  vdb(vo_psr)  from=1 to=50Meg
.meas ac cmrr min  vdb(vo_cmrr) from=1 to=50Meg


.trans 'period/10000' 'period*1'
** .print v(vin_tr+) v(vo_tr)
.meas tran vomax max v(vo_tr)
.meas tran vomin min v(vo_tr)
.meas tran srr deriv v(vo_tr) when v(vo_tr)='vomin + 0.5 * (vomax - vomin)' rise=1
.meas tran srf deriv v(vo_tr) when v(vo_tr)='vomin + 0.5 * (vomax - vomin)' fall=1
** .meas tran trise trig v(vo_tr) val = 'vomin + 0.1 * (vomax - vomin)' rise = 1 targ v(vo_tr) val = 'vomin + 0.9 * (vomax - vomin)' rise = 1
** .meas tran tfall trig v(vo_tr) val = 'vomin + 0.9 * (vomax - vomin)' fall = 1 targ v(vo_tr) val = 'vomin + 0.1 * (vomax - vomin)' fall = 1
.end
