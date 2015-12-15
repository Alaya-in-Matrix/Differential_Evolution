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

** xrtr    vin_rtr+   vin_rtr-   vo_rtr     vdd_rtr   vss opamp
** vdd_rtr vdd_rtr    0          vdd_v
** e_rtr+  vin_rtr+   vcm_rtr    100rtr     0         0.5
** e_rtr-  vin_rtr-   vo_rtr     100rtr     0         0.5
** vcm_rtr vcm_rtr    0          dc=vin_cm_rtr
** vs_rtr  100rtr     0          dc=0       ac=1

vss         vss         0       0

.ac   dec 100 1 1g
.meas ac  gain max  vdb(vo_ac)   from=1 to=1g
** .meas ac  ugf  when vdb(vo_ac)=0
** .meas ac  pm   find vp(vo_ac)    at=ugf
** .meas ac  gain_rtr max vdb(vo_rtr) from =1 to=10g

.end
