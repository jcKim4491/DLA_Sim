#include "diannao_system.h"

Diannao_system::Diannao_system() : DLA_System("DianNao")
{
	cout << "===== DianNao Components Configuration =====" << endl;

	// calculating setting initialization.
	_curOp._cur_input_cnt = 0;
	_curOp._max_input_cnt = 256;			////////	input cnt
	_curOp._input_width = 16;
	_curOp._input_size = 256;
	_curOp._cur_kernel_cnt = 0;
	_curOp._max_kernel_cnt = 64 * 64;
	_curOp._kernel_width = 3;
	_curOp._kernel_size = 9;
	_curOp._cur_kX = 0;
	_curOp._cur_kY = 0;
	_curOp._cur_output_cnt = 0;
	_curOp._max_output_cnt = 256;		/////////	output cnt
	_curOp._cur_X = 0;
	_curOp._cur_Y = 0;
	_curOp._output_width = 16;
	_curOp._output_size = 256;
	_curOp._stride = 1;
	_curOp._zero_padding = true;
	_curOp._padding_num = 1;

	// create objects
	_mem_inter = new memory_interface(this);
	_nbin = new	dian_nbin(this, 16 * 64);
	_sb = new dian_sb(this, 256 * 64);
	_nfu1 = new dian_nfu1(this, 256);
	_nfu2 = new dian_nfu2(this); //fixed size HW
	_nfu3 = new dian_nfu3(this);
	_nbout = new dian_nbout(this, 16 * 64);

	// Done.
	cout << "DianNao Deep Learning Accelerator is configured." << endl;
}

// Destructor
Diannao_system::~Diannao_system()
{
	delete _mem_inter;
	delete _nbin;
	delete _sb;
	delete _nfu1;
	delete _nfu2;
	delete _nfu3;
	delete _nbout;
}

// initialize
void
Diannao_system::initialize()
{
	// register for zero padding
	_zero_reg = new m_reg("REG_ZERO");
	_zero_reg->foreced_feed(0);

	// memory interface
	_mem_inter->connect_to_nbin(_nbin);
	_mem_inter->connect_to_sb(_sb);
	_mem_inter->connect_to_nbout(_nbout);

	// fill with input
	_mem_inter->setControl(MEM_CONTROL::NBIN_CTRL);
	_mem_inter->setInputFile("conv_input.bin");
	_mem_inter->fillMemory(BEGIN_ADDR, INPUT_ADDR, (_curOp._max_input_cnt * _curOp._input_size) * 2);
	// fill mem with weight
	_mem_inter->setControl(MEM_CONTROL::SB_CTRL);
	_mem_inter->setInputFile("conv_weight.bin");
	_mem_inter->fillMemory(BEGIN_ADDR, WEIGHT_ADDR, (_curOp._kernel_width * _curOp._kernel_width * _curOp._max_input_cnt * _curOp._max_output_cnt * 2));

	_mem_inter->setControl(MEM_CONTROL::NBIN_CTRL);

	// nbin
	_nbin->connect_to_nfu1(_nfu1);
	// sb
	_sb->connect_to_nfu1(_nfu1);
	// nfu1
	_nfu1->connect_to_nfu2(_nfu2);
	// nfu2
	_nfu2->connect_to_nfu3(_nfu3);
	_nfu2->connect_to_nbout(_nbout);
	// nfu3
	_nfu3->connection_to_nbout(_nbout);

	// read input file
	_mem_inter->set_read_nbin(INPUT_ADDR, _curOp._input_width * _curOp._input_width);
	// read weight file
	_mem_inter->set_read_sb(WEIGHT_ADDR, _curOp._kernel_size * _curOp._max_output_cnt);
}

// tick
void
Diannao_system::tick(_TICK curTick)
{
	// ticking for all modules
	_mem_inter->tick(curTick, _nbout->getSaveOp());
	_nbin->tick(curTick, _nfu1->get_calculating(), _nbout->getStalled(), _mem_inter->get_nbout_readdone(), _sb->get_data_avail());
	_sb->tick(curTick);
	_nfu1->tick(curTick, _nbin->get_calc_avail(), _nbin->get_calc_set(), _nbout->getStalled()
		, _mem_inter->get_nbout_reading(), _nbin->get_wait_nbout_filling());
	_nfu2->tick(curTick, _nfu1->get_calc_set(), _mem_inter->get_nbout_reading());
	_nfu3->tick(curTick, _nfu2->get_calc_set());
	_nbout->tick(curTick, _nfu3->get_calc_set(), _mem_inter->get_write_done());

	// update opcode
	if (_nbin->get_one_layer_end())
	{
		_curOp._cur_output_cnt++;	// move to next output
		_curOp._cur_kernel_cnt++;	// move to next kernel
		_nbin->move_to_next_kernel();
		_sb->move_to_next_kernel();

		// confirmation reset

		// move to next input feature map
		if (_curOp._cur_output_cnt == _curOp._max_output_cnt)
		{
			_curOp._cur_output_cnt = 0;
			_curOp._cur_input_cnt++;
			if (_curOp._cur_input_cnt == _curOp._max_input_cnt)
			{
				//END
				last_writeback = true;
				return;
			}
			else
			{
				// Data reload next input feature map.
				_mem_inter->set_read_nbin(
					INPUT_ADDR + (_curOp._cur_input_cnt *  _curOp._input_width * _curOp._input_width) * 2
					, _curOp._input_width * _curOp._input_width);
				// load next kernel feature map.
				_mem_inter->set_read_sb(
					WEIGHT_ADDR + (_curOp._cur_input_cnt * _curOp._kernel_size * _curOp._max_output_cnt) * 2
					, _curOp._kernel_size * _curOp._max_output_cnt);
			}
		}

		// progress
		cout << "[CLK:" << dec << curTick << "] In: " << _curOp._cur_input_cnt << ", Out: " << _curOp._cur_output_cnt << ", Kernel: " << _curOp._cur_kernel_cnt << endl;

		_nbin->set_one_layer_end(false);
		_nbin->set_wait_next_output(true);	// wait for writing back current output layer.
	}
	else if (_nbin->get_wait_next_output())
	{
		// when writeback is done.
		if (_mem_inter->get_write_done())
		{
			//_nbin->print();
			//_nbout->print();

			_mem_inter->set_write_done(false);
			// end process.
			if (last_writeback)
			{
				_process_end = true;
				cout << "<< PROCESS ENDS >>" << endl;
				return;
			}
			if (_curOp._cur_input_cnt >= 1)
				// Reload previous output feature map
				_mem_inter->set_read_nbout(OUTPUT_ADDR + (_curOp._cur_output_cnt *  _curOp._output_width * _curOp._output_width) * 2
					, _curOp._output_width * _curOp._output_width);

			_nbin->continue_process();
		}
	}
}

// print
void
Diannao_system::print_nbin()
{
	if (_nbin == NULL)
		return;

	_nbin->print();
}

void
Diannao_system::print_nbout()
{
	if (_nbout == NULL)
		return;

	_nbout->print();
}

// save output
void
Diannao_system::save_output(int size)
{
	_mem_inter->save_output(size);
}

// print statistics
void
Diannao_system::print_stats()
{
	cout << endl;
	cout << "<<<STATS>>>" << endl << endl;
	cout << "Register Access : \t\t" << dec << REGISTER_ACCESS << endl;
	cout << "Adder Access : \t\t" << ADDER_ACCESS << endl;
	cout << "Multiplier Access : \t" << MULT_ACCESS << endl;
	cout << "Activation Function Access : \t" << ACTFUNC_ACCESS << endl;
	cout << "Memory Read Access : \t" << MEMREAD_ACCESS << endl;
	cout << "Memory Write Access : \t" << MEMWRITE_ACCESS << endl;
}