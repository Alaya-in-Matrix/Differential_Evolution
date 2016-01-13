.option post
.option ingold=2
.option numdgt=10
.lib 'sm046005-1j.hspice' typical

*.option seed='random' randgen='moa'

.param vdd_v = 3.3
.inc param.sp
.inc weixin_opamp.sp
.param vin_cm=0
xac     vin_ac+     vin_ac-     vo_ac   vdd_ac      vss opamp
vdd_ac  vdd_ac      0       vdd_v
e+      vin_ac+     101     100         0 0.5
e-      vin_ac-     101     100         0 -0.5
vcm     101         0       dc=vin_cm 
vs      100         0       dc=0        ac=1

vss         vss         0       0

.ac   dec 100 1 1g sweep vin_cm 0 3.3 0.825
.meas ac  gain    find  vdb(vo_ac) at=1
.meas ac  ugf_actual     when vdb(vo_ac)=0
.meas ac  phase          min  vp(vo_ac) from=1 to=ugf_actual
.meas ac  ugf=param('ugf_actual*1e-6')
.meas ac  pm=param('phase+180')

** .meas ac  pm=param('phase+180')
** .meas ac  ugf=param('ugf_actual*1e-6')
** .meas ac  gain    max  vdb(vo_ac_rr) 

.end
