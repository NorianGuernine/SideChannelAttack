# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/norian/Documents/github/SideChannelAtt/pourgithub/microblaze_GPIO/AES/AES_GPIO_system/_ide/scripts/systemdebugger_aes_gpio_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/norian/Documents/github/SideChannelAtt/pourgithub/microblaze_GPIO/AES/AES_GPIO_system/_ide/scripts/systemdebugger_aes_gpio_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-35T 210319B0C416A" && level==0 && jtag_device_ctx=="jsn-Arty A7-35T-210319B0C416A-0362d093-0"}
fpga -file /home/norian/Documents/github/SideChannelAtt/pourgithub/microblaze_GPIO/AES/AES_GPIO/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw /home/norian/Documents/github/SideChannelAtt/pourgithub/microblaze_GPIO/AES/design_1_wrapper_2/export/design_1_wrapper_2/hw/design_1_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow /home/norian/Documents/github/SideChannelAtt/pourgithub/microblaze_GPIO/AES/AES_GPIO/Debug/AES_GPIO.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
