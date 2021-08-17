# aclk {FREQ_HZ 83333333 CLK_DOMAIN design_1_mig_7series_0_0_ui_clk PHASE 0} aclk1 {FREQ_HZ 16000000 CLK_DOMAIN design_1_mig_7series_0_0_ui_clk PHASE 0}
# Clock Domain: design_1_mig_7series_0_0_ui_clk
create_clock -name aclk -period 12.000 [get_ports aclk]
# Generated clocks
create_generated_clock -name aclk1 -source [get_ports aclk] -multiply_by 16000000 -divide_by 83333333 [get_ports aclk1]
