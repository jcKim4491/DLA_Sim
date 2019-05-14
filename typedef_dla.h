#pragma once
typedef int _TICK;
typedef char _BITSIZE;
typedef int _ACCESS_CNT;
typedef __int32 _DATA_STORAGE;

#define NEG_MAX -32768
#define POS_MAX 32767

#define BUFFER_WIDTH 16		// normal buffer width
#define TREE_LENGTH 15		// Fixed Size adder tree
#define ADTREE_CNT 16
#define NFU3_WIDTH 16
#define NBOUT_WIDTH 16
#define BUFFER_SIZE 1024

// ADDRESS
#define BEGIN_ADDR 0x0
#define INPUT_ADDR 0x0
#define WEIGHT_ADDR 0x100000
#define OUTPUT_ADDR 0x300000

enum TIMINGOBJ_TYPE
{
	NONE = 0,
	REGISTER,
	MULT,
	ADD,
	WIRE,
	MUX,
	DEMUX,
	ALU
};

enum MEM_CONTROL
{
	NOCTRL = 0,
	NBIN_CTRL,
	SB_CTRL,
	NBOUT_CTRL,		//write back control
	NBOUT_READ_CTRL,
};

enum LAYER_PURPOSE
{
	CONV = 0,
	POOL,
	CLASSIFY
};

enum ACTFUNC
{
	SIGMOID = 0,
	RELU,
	TANH,
	NOACT,
};

struct Opcode
{
public:
	// current layer purpose
	LAYER_PURPOSE _cur_layer_purpose = LAYER_PURPOSE::CONV;
	/* =================== CONV ================= */
	int _cur_input_cnt = 0;		// current input feature counter
	int _max_input_cnt = 0;		// maximum input feature count
	int _input_width = 32;		// size of input feature map.
	int _input_size = 1024;

	int _cur_kernel_cnt = 0;	// current kernel counter
	int _max_kernel_cnt = 0;	// maximum kernel count
	int _kernel_width = 3;		// kernel size(width == height)
	int _kernel_size = 9;
	int _cur_kX = 0;			// current calculating position X in kernel map.
	int _cur_kY = 0;			// current calculating position Y in kernel map.

	int _cur_output_cnt = 0;	// current output feature counter
	int _max_output_cnt = 0;	// maximum output feature count.
	int _cur_X = 0;				// current calculating position X in output feature map.
	int _cur_Y = 0;				// current calculating position Y in output feature map.
	int _output_width = 32;		// size of output feature map.
	int _output_size = 1024;

	int _stride = 1;			// stride
	bool _zero_padding = true;	// 0 padding
	int _padding_num = 1;		// 0 padding number

	int _nfu2_setnum = 0;		// number of calculated set in NFU2
	int _nfu3_setnum = 0;		// number of calculated set in NFU3
	int _nbout_setnum = 0;

	ACTFUNC _actfunc = ACTFUNC::SIGMOID;
};