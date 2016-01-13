folder::prj_dir is "/export/home/svnhome/workspace/lvwenlong/opamTsmc35";
folder::abs_process_root is "/export/home/svnhome/process";


file::parameter is "opam_param"
{
  param ib    = (1.70e+0 ~ 2.20e+0);#
  param wa2   = (5.00e+0 ~ 8.00e+0);#
  param wb1   = (5.00e+0 ~ 8.00e+0);#
  param wb2   = (9.00e+0 ~ 1.50e+1);#
  param w1    = (1.10e+2 ~ 1.60e+2);#
  param w3    = (9.00e+0 ~ 1.30e+1);#
  param w5    = (9.00e+0 ~ 1.30e+1);#
  param w7    = (1.00e+1 ~ 1.50e+1);#
  param w9    = (9.00e+0 ~ 1.40e+1);#
  param w11   = (2.00e+1 ~ 3.50e+1);#
  param w12   = (1.70e+1 ~ 2.70e+1);#
  param w16   = (1.10e+2 ~ 1.60e+2);#
  param w13   = (1.40e+1 ~ 2.20e+1);#
  param w14   = (1.20e+1 ~ 1.60e+1);#
  param w15   = (2.70e+1 ~ 4.20e+1);#
  param cmm   = (1.00e+0 ~ 2.00e+0);#
  param r1    = (1.40e+2 ~ 2.20e+2);#
  param cz    = (5.00e-1 ~ 1.00e+0);#
  param rz    = (2.60e+2 ~ 3.20e+2);#
  param l5    = (3.70e-1 ~ 4.70e-1);#
  param l7    = (3.70e-1 ~ 4.70e-1);#
  param l9    = (1.00e+0 ~ 1.20e+0);#
  param ln    = (6.00e-1 ~ 8.00e-1);#
  param lp    = (1.00e+0 ~ 1.30e+0);#

};
                                                                                                
file::process is "opamp_process"
{
  corner { TT, FF, SS }
  {
    lib "/smic.35/hspice/v1p1/cmos035.lib" "$$";
  };

  temperature temp_TT = "27";
  temperature temp_FF = "-40";
  temperature temp_SS = "125";

};

                                                                                        
                                                
file::simulation is "opamp"
{

  sim_tool   = "hspicerf64";

  netlist typical
  {

    net_source  = "Single_ended.sp";
    sim_corner = { TT};
    sim_temperature = { temp_TT};
    sim_depend  = {"Single_ended_opamp.txt"};

    sim_tool    = "hspicerf64";


    extract { "PM", "GM", "UGF"} from "Single_ended.ma0";

    extract "I1(VDD_AC)" denote_as "Iq" feature { "vec", "idx(0)" } from "Single_ended.printsw0";
    extract "V(VO_TR)"   denote_as "srr" feature { "vec", "slew_rate(1, 0.1, 0.9)" } trig_end "time > 30e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"   denote_as "srf" feature { "vec", "slew_rate(0, 0.1, 0.9)" } trig_start "time > 40e-6" trig_end "time > 75e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"   denote_as "tsr" feature { "vec", "settling_time( 1, -40 )" } trig_end "time > 30e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"   denote_as "tsf" feature { "vec", "settling_time( 0, -40 )" } trig_start "time > 50e-6" trig_end "time > 75e-6" from "Single_ended.printtr0";

  };

  netlist fast
  {

    net_source  = "Single_ended.sp";
    sim_corner = { FF};
    sim_temperature = { temp_FF};
    sim_depend  = {"Single_ended_opamp.txt"};

    sim_tool    = "hspicerf64";
    extract "PM"  denote_as "PM_FF" from "Single_ended.ma0";
    extract "GM"  denote_as "GM_FF" from "Single_ended.ma0";
    extract "UGF" denote_as "UGF_FF" from "Single_ended.ma0";

    extract "I1(VDD_AC)"  denote_as "Iq_FF"  feature { "vec", "idx(0)" } from "Single_ended.printsw0";
    extract "V(VO_TR)"    denote_as "srr_FF" feature { "vec", "slew_rate(1, 0.1, 0.9)" } trig_end "time > 30e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"    denote_as "srf_FF" feature { "vec", "slew_rate(0, 0.1, 0.9)" } trig_start "time > 40e-6" trig_end "time > 75e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"    denote_as "tsr_FF" feature { "vec", "settling_time( 1, -40 )" } trig_end "time > 30e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"    denote_as "tsf_FF" feature { "vec", "settling_time( 0, -40 )" } trig_start "time > 50e-6" trig_end "time > 75e-6" from "Single_ended.printtr0";


  };

 netlist slow
  {

    net_source  = "Single_ended.sp";
    sim_corner = { SS};
    sim_temperature = { temp_SS};
    sim_depend  = {"Single_ended_opamp.txt"};

    sim_tool    = "hspicerf64";

    extract "PM"  denote_as "PM_SS" from "Single_ended.ma0";
    extract "GM"  denote_as "GM_SS" from "Single_ended.ma0";
    extract "UGF" denote_as "UGF_SS" from "Single_ended.ma0";

    extract "I1(VDD_AC)"  denote_as "Iq_SS"  feature { "vec", "idx(0)" } from "Single_ended.printsw0";
    extract "V(VO_TR)"    denote_as "srr_SS" feature { "vec", "slew_rate(1, 0.1, 0.9)" } trig_end "time > 30e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"    denote_as "srf_SS" feature { "vec", "slew_rate(0, 0.1, 0.9)" } trig_start "time > 40e-6" trig_end "time > 75e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"    denote_as "tsr_SS" feature { "vec", "settling_time( 1, -40 )" } trig_end "time > 30e-6" from "Single_ended.printtr0";
    extract "V(VO_TR)"    denote_as "tsf_SS" feature { "vec", "settling_time( 0, -40 )" } trig_start "time > 50e-6" trig_end "time > 75e-6" from "Single_ended.printtr0";

  };


};                                            
operation::specification is "spec"
{
var UGF = typical::UGF / 1e6; 
var PM = ( typical::PM > 0.0 ) ? ( typical::PM - 180 ) : ( typical::PM + 180 ); 
var GM = abs( typical::GM ); 
var srr = typical::srr / 1e6; 
var srf = typical::srf / 1e6; 
var tsr = typical::tsr * 1e6; 
var tsf = typical::tsf * 1e6; 
var Iq = abs( typical::Iq ) * 1000; 
var UGF_SS = slow::UGF_SS / 1e6; 
var PM_SS = ( slow::PM_SS > 0.0 ) ? ( slow::PM_SS - 180 ) : ( slow::PM_SS + 180 ); 
var GM_SS = abs( slow::GM_SS ); 
var srr_SS = slow::srr_SS / 1e6; 
var srf_SS = slow::srf_SS / 1e6; 
var tsr_SS = slow::tsr_SS * 1e6; 
var tsf_SS = slow::tsf_SS * 1e6; 
var Iq_SS = abs( slow::Iq_SS ) * 1000; 
var UGF_FF = fast::UGF_FF / 1e6; 
var PM_FF = ( fast::PM_FF > 0.0 ) ? ( fast::PM_FF - 180 ) : ( fast::PM_FF + 180 ); 
var GM_FF = abs( fast::GM_FF ); 
var srr_FF = fast::srr_FF / 1e6; 
var srf_FF = fast::srf_FF / 1e6; 
var tsr_FF = fast::tsr_FF * 1e6; 
var tsf_FF = fast::tsf_FF * 1e6; 
var Iq_FF = abs( fast::Iq_FF ) * 1000; 
probe tsr;
probe PM;
probe GM;
  constraint tsr < 5.17;
  var fom_ = PM*1+GM*1;
  fom fom_;
};

operation::optimization is "optimize"
{
  opt_max_time = 1800;
  opt_max_solver = 1;
  opt_strategy = "PR";
  opt_algo = "SLSQP";
  opt_option = "gi0";
  opt_stat = "pf";
};
