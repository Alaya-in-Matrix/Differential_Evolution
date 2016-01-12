.option post
.option ingold=2
.option numdgt=10
.lib 'sm046005-1j.hspice' typical

*.option seed='random' randgen='moa'

.param vdd_v = 3.3
.inc param.sp
.inc weixin_opamp.sp

xac     vin_ac+     vin_ac-     vo_ac   vdd_ac      vss opamp
vdd_ac  vdd_ac      0       vdd_v
e+      vin_ac+     101     100         0 0.5
e-      vin_ac-     101     100         0 -0.5
vcm     101         0       dc=vin_cm 
vs      100         0       dc=0        ac=1


xac_rr     vin_ac_rr+     vin_ac_rr-     vo_ac_rr   vdd_ac_rr      vss opamp
vdd_ac_rr  vdd_ac_rr      0       vdd_v
e_rr+      vin_ac_rr+     rr_101     rr_100         0 0.5
e_rr-      vin_ac_rr-     rr_101     rr_100         0 -0.5
vcm_rr     rr_101         0       dc=vin_cm_rr
vs_rr      rr_100         0       dc=0        ac=1


vss         vss         0       0

.ac   dec 100 1 1g 
.meas ac  gain    max  vdb(vo_ac)   
.meas ac  ugf     when vdb(vo_ac)=0
.meas ac  pm      find vp(vo_ac)    at=ugf
.meas ac  gain_rr max  vdb(vo_ac_rr) 

.end
