folder::prj_dir is "/export/home/wllv/tmp/tangzhangwen_course_project4";
folder::abs_process_root is "/export/home/wllv/tmp/tangzhangwen_course_project4/process";

file::parameter is "opamp_param"      # here dcdc_param is the parameter file
{

    param cm      = (0.5~10)[ic:1.70384];
    param ival    = (0~3)[ic:1.23982];
    param l_fixed = (0.35~10)[ic:0.407896];
    param w10     = (0.4~20)[ic:9.92128];
    param w11     = (0.4~20)[ic:3.42564];
    param w12     = (0.4~20)[ic:17.1526];
    param w13     = (0.4~20)[ic:14.982];
    param w14     = (0.4~20)[ic:13.7501];
    param w15     = (0.4~20)[ic:6.17339];
    param w16_18  = (0.4~20)[ic:17.7731];
    param w17_19  = (0.4~20)[ic:12.3508];
    param w1_2    = (0.4~20)[ic:20];
    param w20     = (0.4~20)[ic:18.9364];
    param w21     = (0.4~20)[ic:19.0962];
    param w22_23  = (0.4~20)[ic:2.31726];
    param w24     = (0.4~20)[ic:9.4451];
    param w25     = (0.4~20)[ic:11.7396];
    param w26     = (0.4~20)[ic:16.9433];
    param w27     = (0.4~20)[ic:4.36916];
    param w28     = (0.4~20)[ic:14.2134];
    param w29     = (0.4~20)[ic:1.03834];
    param w30     = (0.4~20)[ic:16.1044];
    param w31     = (0.4~20)[ic:4.79339];
    param w32     = (0.4~20)[ic:6.16261];
    param w33     = (0.4~20)[ic:18.0503];
    param w34     = (0.4~20)[ic:1.68449];
    param w35     = (0.4~20)[ic:15.4551];
    param w3_4    = (0.4~20)[ic:2.73696];
    param w5      = (0.4~20)[ic:13.5429];
    param w6      = (0.4~20)[ic:12.9523];
    param w7      = (0.4~20)[ic:8.87272];
    param w8      = (0.4~20)[ic:8.10652];
    param w9      = (0.4~20)[ic:19.7317];
    param vin_cm  = (0 ~ 3.3)[ic:1.09407]; # dc input bias

    # param for testbench
    # we set vdd = 3.3v
    param vdd_v      = 3.3;
    param vin_cm_rtr = 3.0;
};

file::process is "opamp_process"
{
    corner { typical } # tsmc
    {
        lib "sm046005-1j.hspice" "$$";
    };
};
# define options for simulater
file::simulation is "opamp"
{
    netlist typical
    {
        sim_tool        = "hspice64";
        sim_corner      = {typical};
        net_proc_rel    = "MUL";

        net_source      = "Single_ended.sp";
        sim_depend      = "weixin_opamp.sp";

        extract { "gain", "pm", "ugf", "gain_rtr"} from "Single_ended.ma0";
    };
};

operation::specification is "opamp"
{
  var gain     = min(typical::gain);
  var pm       = min(( typical::pm > 0.0 ) ? ( typical::pm - 180 ) : ( typical::pm + 180 ));
  var ugf      = min(typical::ugf / 1e6); # MHz
  var gain_rtr = min(typical::gain_rtr);

  constraint gain_rtr > 70;

  var target = -1 * gain_rtr;
  fom target;
};

operation::optimization is "optimize"
{
  opt_max_iter   = 800000;
  opt_max_solver = 12;
  opt_strategy   = "ACT";
  opt_algo       = "SLSQP";
  opt_option     = "gi0";
  opt_stat       = "pf";
};
