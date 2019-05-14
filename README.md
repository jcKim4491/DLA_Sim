# DLA_Sim
Deep Learning Accelerator Simulation
(Only for Convolutional Layer)

dla_driver.h
-> dla_system.h
-> diannao_system.h : DianNao Architecture & Control Unit as well.

\<BaseCode\>
  1. timingObject.h     : basic class for timing modules.
  2. add_module.h       : adder.
  3. mult_module.h      : multiplier.
  4. register_module.h  : register.
  5. act_module.h       : activation function applier. (eg. ReLu, tanh, sigmoid.)

\<DianNao\>
  1. memory_interface.h : 4MB Cache and 3 DMAs for NBIn, SB, and NBOut.
  2. dian_nbin.h        : buffer for input feature map. size 16(width)*64(entry). circular buffer.
  3. dian_sb.h          : buffer for kernel weights. size 256(width)*64(entry). circular buffer.
  4. dian_nfu1.h        : Multiply module. 256 multipliers.
  5. dian_nfu2.h        : Add module. 16 Adder trees(each 15 adder modules).
  6. dian_nfu3.h        : Activation function module. Turn on only for last input feature map.
  7. dian_nbout.h       : buffer for output feature map. size 16(width)*64(entry). circular buffer.
  
  a. NBOutSaveOp.h      : Opcode struct for writing back from NBOut.
  b. optionparser.h     : Verbose flags for debugging.
  
