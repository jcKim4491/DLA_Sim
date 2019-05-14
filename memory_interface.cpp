#include "memory_interface.h"
#include <stdio.h>



// Constructor
memory_interface::memory_interface()
{
	_cur_CONTROL = MEM_CONTROL::NOCTRL;
	_input_set = false;
	_inMem = NULL;

	_allowed_max_cycle = MAXIMUM_READ_NUM / BUFFER_WIDTH;

	cout << "Memory Interface is created." << endl;
}

memory_interface::memory_interface(Diannao_system *const dian_sys)
	: _dian_sys(dian_sys)
{
	_cur_CONTROL = MEM_CONTROL::NOCTRL;
	_input_set = false;
	_inMem = NULL;

	_allowed_max_cycle = MAXIMUM_READ_NUM / BUFFER_WIDTH;

	cout << "Memory Interface is created." << endl;
}

// Destructor
memory_interface::~memory_interface()
{
	free(_inMem);
}

// Fill memory
void
memory_interface::fillMemory(long src_addr, long dst_addr, long size, bool overflow_ignore)
{
	if (_inMem == NULL)
	{
		_inMem = (char*)malloc(INNER_MEM_SIZE);
	}

	if (!_input_set)
	{
		cout << "ERROR: No input file has been set." << endl;
		return;
	}

	// check validation
	if (dst_addr + size >= INNER_MEM_SIZE)
	{
		cout << "WARNING: destination memory will overflow." << endl;
		if (!overflow_ignore)
			return;
	}

	//set destination start address.
	char *buffer = &(_inMem[dst_addr]);

	FILE* fp = NULL;

	if (fp = fopen(_inputfile.c_str(), "rb"))
	{
		fseek(fp, src_addr, SEEK_SET);
		fread(buffer, size, 1, fp); // read
		fclose(fp);
		return;
	}
	else
	{
		// file open error
		cout << "ERROR: input file cannot be opened." << endl;
		return;
	}

}


// Read from memory
char*
memory_interface::readMemory(long addr, long size)
{
	// validation
	if (_inMem == NULL)
	{
		cout << "ERROR: No memory space initialized." << endl;
		return NULL;
	}
	if (addr < 0 || (addr + size >= INNER_MEM_SIZE))
	{
		cout << "ERROR: Invalid memory space." << endl;
		return NULL;
	}

	char *buffer = (char*)malloc(size + 1);
	memcpy(buffer, &(_inMem[addr]), size); // copy from _inMem
	buffer[size] = 0x0;
	return buffer;
}

// Set Read Area
void
memory_interface::setReadArea(long addr, long size)
{
	// validation check
	if (_inMem == NULL)
	{
		cout << "ERROR: No memory space initialized." << endl;
		return;
	}
	if (addr < 0 || (addr + size >= INNER_MEM_SIZE))
	{
		cout << "ERROR: Invalid memory space." << endl;
		return;
	}

	_cur_addr = addr;
}

// set nbin read area
void
memory_interface::set_read_nbin(long addr, long size)
{
	// validation check
	if (_inMem == NULL)
	{
		cout << "ERROR:[DMA] No memory space initialized." << endl;
		return;
	}
	if (addr < 0 || (addr + size >= INNER_MEM_SIZE))
	{
		cout << "ERROR:[DMA] Invalid memory space." << endl;
		return;
	}

	_nbin_saved_addr = addr;
	_nbin_saved_size = size;

	if (!_quite_dma)
	{
		cout << "[DMA]NBIN read " << dec << size << "bytes from address " << hex
			<< addr << endl;
	}
}

// set sb read area
void
memory_interface::set_read_sb(long addr, long size)
{
	// validation check
	if (_inMem == NULL)
	{
		cout << "ERROR:[DMA] No memory space initialized." << endl;
		return;
	}
	if (addr < 0 || (addr + size >= INNER_MEM_SIZE))
	{
		cout << "ERROR:[DMA] Invalid memory space." << endl;
		return;
	}

	_sb_saved_addr = addr;
	_sb_saved_size = size;
	if (!_quite_dma)
	{
		cout << "[DMA]SB read " << dec << size << "bytes from address " << hex
			<< addr << endl;
	}
}

// set nbout read area
void
memory_interface::set_read_nbout(long addr, long size)
{
	// validation check
	if (_inMem == NULL)
	{
		cout << "ERROR:[DMA] No memory space initialized." << endl;
		return;
	}
	if (addr < 0 || (addr + size >= INNER_MEM_SIZE))
	{
		cout << "ERROR:[DMA] Invalid memory space." << endl;
		return;
	}

	_nbout_saved_addr = addr;
	_nbout_saved_size = size;
	if (!_quite_dma)
	{
		cout << "[DMA]NBOUT read " << dec << size << "bytes from address " << hex
			<< addr << endl;
	}
}

// set feed parameters
void
memory_interface::set_feed_nbin()
{
	// validation check
	if (conn_nbin == NULL)
	{
		cout << "No NBin has connected." << endl;
		return;
	}

	if (_nbin_saved_size <= 0)
		return;

	_cur_read_length = _nbin_saved_size;

	// calculate needed cycles
	_cur_cnt = 0;
	_cur_max_cnt = ((_nbin_saved_size - 1) / BUFFER_WIDTH) + 1;

	// over-loaded
	if (_allowed_max_cycle < _cur_max_cnt)
	{
		_cur_max_cnt = _allowed_max_cycle;
		_over_loaded = true;
	}

	_cur_addr = _nbin_saved_addr;

	_init_feeding = true;
	_done_feeding = false;
}

// set feed parameters
void
memory_interface::set_feed_sb()
{
	// validation check
	if (conn_sb == NULL)
	{
		cout << "No SB has connected." << endl;
		return;
	}

	if (_sb_saved_size <= 0)
		return;

	_cur_read_length = _sb_saved_size;

	// calculate needed cycles
	_cur_cnt = 0;
	_cur_max_cnt = ((_sb_saved_size - 1) / BUFFER_WIDTH) + 1;

	// over-loaded
	if (_allowed_max_cycle < _cur_max_cnt)
	{
		_cur_max_cnt = _allowed_max_cycle;
		_over_loaded = true;
	}

	_cur_addr = _sb_saved_addr;

	_init_feeding = true;
	_done_feeding = false;
}

// set feed nbout parameters
void
memory_interface::set_feed_nbout()
{
	// validation check
	if (conn_nbout == NULL)
	{
		cout << "No NBOut has connected." << endl;
		return;
	}

	if (_nbout_saved_size <= 0)
		return;

	_cur_read_length = _nbout_saved_size;

	// calculate needed cycles
	_cur_cnt = 0;
	_cur_max_cnt = ((_nbout_saved_size - 1) / BUFFER_WIDTH) + 1;

	_cur_addr = _nbout_saved_addr;

	// setting nbout row to 0
	conn_nbout->setCurrentRow(0);

	_nbout_readdone = false;
	_init_feeding = true;
	_done_feeding = false;
}

// set be fed parameters
void
memory_interface::set_fed_nbout()
{
	// validation check
	if (_saveOp.saving == false)
		return;
	if (_saveOp.size < 0)
		return;
	if (_saveOp.start_index < 0)
		return;

	// calculate needed cycles
	_cur_cnt = 0;
	_cur_max_cnt = ((_saveOp.size - 1) / BUFFER_WIDTH) + 1;

	// no over-load.

	_cur_write_pos = _saveOp.start_index;
	_cur_write_length = _saveOp.size;
	_cur_addr = OUTPUT_ADDR + (_saveOp.output_size * _saveOp.output_num) * 2;

	_nbout_writedone = false;
	_init_feeding = true;
	_done_feeding = false;

	if (!_quite_dma)
	{
		cout << "[DMA]NBOUT writeback " << dec << _saveOp.size << "bytes from address " << hex
			<< _cur_addr << endl;
	}
}


// Feed to NBin
void
memory_interface::feed_nbin()
{
	// get row idx
	_nbin_cur_row_idx = conn_nbin->getCurrentRow();

	int i, loop_num = 0;
	// Is this the last cycle
	if (_cur_max_cnt - 1 == _cur_cnt)
	{
		loop_num = _cur_read_length % BUFFER_WIDTH;
	}
	// include else block and same with BUFFER_WIDTH
	if (loop_num == 0)
		loop_num = BUFFER_WIDTH;

	// feed each register in NBIN
	unsigned int bdata1 = 0, bdata2 = 0;
	_DATA_STORAGE rdata = 0;

	// Read (BUFFER_WIDTH * 2) bytes starting from _cur_addr
	for (i = 0; i < loop_num; ++i)
	{
		// read two bytes
		bdata1 = _inMem[_cur_addr + (2 * i)];
		bdata2 = _inMem[_cur_addr + (2 * i) + 1];
		rdata = (bdata1 << 8) | (bdata2 & 0x000000FF);

		// feed data to NBIN.
		conn_nbin->feed_from_mem((BUFFER_WIDTH * _nbin_cur_row_idx) + i, rdata);
		MEMREAD_ACCESS++;	//stats
	}

	//update params
	++_cur_cnt;
	_cur_addr = _cur_addr + (loop_num * 2);		// update address
	_cur_read_size += loop_num;
}

// Feed to SB
void
memory_interface::feed_sb()
{
	// get row idx
	_sb_cur_row_idx = conn_sb->getCurrentRow();

	int i, loop_num = 0;
	// Is this the last cycle
	if (_cur_max_cnt - 1 == _cur_cnt)
	{
		loop_num = _cur_read_length % BUFFER_WIDTH;
	}
	// include else block and same with BUFFER_WIDTH
	if (loop_num == 0)
		loop_num = BUFFER_WIDTH;

	// feed each register in SB
	unsigned int bdata1 = 0, bdata2 = 0;
	_DATA_STORAGE rdata = 0;

	// Read (BUFFER_WIDTH * 2) bytes starting from _cur_addr
	for (i = 0; i < loop_num; ++i)
	{
		// read two bytes
		bdata1 = _inMem[_cur_addr + (2 * i)];
		bdata2 = _inMem[_cur_addr + (2 * i) + 1];
		rdata = (bdata1 << 8) | (bdata2 & 0x000000FF);

		// feed data to NBIN.
		conn_sb->feed_from_mem((BUFFER_WIDTH * _sb_cur_row_idx) + i, rdata);
		MEMREAD_ACCESS++;	//stats
	}

	//update params
	++_cur_cnt;
	_cur_addr = _cur_addr + (loop_num * 2);		// update address
	_cur_read_size += loop_num;
}

// Feed to NBOut
void
memory_interface::feed_nbout()
{
	// get row idx
	_nbout_cur_row_idx = conn_nbout->getCurrentRow();

	int i, loop_num = 0;
	// Is this the last cycle
	if (_cur_max_cnt - 1 == _cur_cnt)
	{
		loop_num = _cur_read_length % BUFFER_WIDTH;
	}
	// include else block and same with BUFFER_WIDTH
	if (loop_num == 0)
		loop_num = BUFFER_WIDTH;

	// feed each register in SB
	unsigned int bdata1 = 0, bdata2 = 0;
	_DATA_STORAGE rdata = 0;

	// Read (BUFFER_WIDTH * 2) bytes starting from _cur_addr
	for (i = 0; i < loop_num; ++i)
	{
		// read two bytes
		bdata1 = _inMem[_cur_addr + (2 * i)];
		bdata2 = _inMem[_cur_addr + (2 * i) + 1];
		rdata = (bdata1 << 8) | (bdata2 & 0x000000FF);

		// feed data to NBIN.
		conn_nbout->feed_from_mem((BUFFER_WIDTH * _nbout_cur_row_idx) + i, rdata);
		MEMREAD_ACCESS++;	//stats
	}

	//update params
	++_cur_cnt;
	_cur_addr = _cur_addr + (loop_num * 2);		// update address
	_cur_read_size += loop_num;

	if (!_quite_nbout)
		cout << "[DMA] NBOut re-filling " << dec << loop_num << " registers. " << (_nbout_saved_size - _cur_read_size) << " left." << endl;
}

// feed from nbout
void
memory_interface::feed_from_nbout()
{
	int i, loop_num = 0;
	// Is this the last cycle
	if (_cur_max_cnt - 1 == _cur_cnt)
	{
		loop_num = _cur_write_length % BUFFER_WIDTH;
	}
	// include else block and same with BUFFER_WIDTH
	if (loop_num == 0)
		loop_num = BUFFER_WIDTH;

	// validation check
	if (_saveOp.nbout_p == NULL)
	{
		cout << "ERROR:[DMA] NBOut is not connected." << endl;
		return;
	}
	// write data into memory
	for (i = 0; i < loop_num; ++i)
	{
		// get data and save
		_inMem[_cur_addr + (2 * i)] = (_saveOp.nbout_p->getReg(_cur_write_pos)->getData() & 0xFF00) >> 8;
		_inMem[_cur_addr + (2 * i) + 1] = (_saveOp.nbout_p->getReg(_cur_write_pos)->getData() & 0x00FF);

		// update
		_cur_write_pos++;
		MEMWRITE_ACCESS++;	//stats
	}

	//update params
	++_cur_cnt;
	_cur_addr = _cur_addr + (loop_num * 2);		// update address
	_cur_write_length -= loop_num;

	if (!_quite_writeback)
		cout << "[DMA] NBOut wrieback " << dec << loop_num << " registers. " << _cur_write_length << " left." << endl;
	if (_cur_write_length < 0)
	{
		cout << "ERROR:[DMA] NBOut writing segmentation fault." << endl;
		return;
	}
}

// port connect
void
memory_interface::connect_to_nbin(dian_nbin* target_nbin)
{
	conn_nbin = target_nbin;
	_nbin_width = target_nbin->_NBIN_WIDTH;
	if (!_quite_moduleconnection)
		cout << "[DMA]Memory interface is connected to NBIn." << endl;
}

void
memory_interface::connect_to_sb(dian_sb* target_sb)
{
	conn_sb = target_sb;
	//TODO: set number of kernel weights
	_sb_reg_cnt = 9;
	if (!_quite_moduleconnection)
		cout << "[DMA]Memory interface is connected to SB." << endl;
}

void
memory_interface::connect_to_nbout(dian_nbout* target_nbout)
{
	conn_nbout = target_nbout;
	_nbout_width = target_nbout->_NBOUT_WIDHT;
	_nbout_size = target_nbout->_NBOUT_REG_CNT;
	if (!_quite_moduleconnection)
		cout << "[DMA]Memory interface is connected to NBOut." << endl;
}

// tick
void
memory_interface::tick(_TICK curTick, NBoutSaveOp saveOp)
{
	_saveOp = saveOp;

	if (_init_feeding == false)
	{
		// need to initiate
		switch (_cur_CONTROL)
		{
		case MEM_CONTROL::NBIN_CTRL:
			set_feed_nbin();
			break;
		case MEM_CONTROL::SB_CTRL:
			set_feed_sb();
			break;
		case MEM_CONTROL::NBOUT_READ_CTRL:
			set_feed_nbout();
			break;
		case MEM_CONTROL::NBOUT_CTRL:
			//cout << "(before save NBOut) ";
			//conn_nbout->print();
			set_fed_nbout();
			break;
		case MEM_CONTROL::NOCTRL:
			//re-scheduling
			if (saveOp.saving)
				_cur_CONTROL = MEM_CONTROL::NBOUT_CTRL;
			else if (_nbout_saved_size > 0)
				_cur_CONTROL = MEM_CONTROL::NBOUT_READ_CTRL;
			break;
		default:
			break;
		}

		_clk_s = 0;
	}
	else if (_init_feeding == true && _done_feeding == false)
	{
		++_clk_s;
		// need some time to feeding
		if (_clk_s == _clk_end)
		{
			_clk_s = 0;	//reset


			switch (_cur_CONTROL)
			{
				// feeding
			case MEM_CONTROL::NBIN_CTRL:
				feed_nbin();
				conn_nbin->incCurrentRow();
				break;
			case MEM_CONTROL::SB_CTRL:
				feed_sb();
				conn_sb->incCurrentRow();
				break;
			case MEM_CONTROL::NBOUT_READ_CTRL:
				feed_nbout();
				conn_nbout->incCurrentRow();
				break;
				// feed from
			case MEM_CONTROL::NBOUT_CTRL:
				feed_from_nbout();
				break;
			default:
				break;
			}

			// is this last cycle?
			if (_cur_cnt == _cur_max_cnt)
			{
				_done_feeding = true;
				_init_feeding = false;	//reset

				// then switch contorl.
				// TODO: scheduling
				switch (_cur_CONTROL)
				{
				case MEM_CONTROL::NBIN_CTRL:
					// context switch
					_nbin_saved_addr = _cur_addr;
					_nbin_saved_size -= _cur_read_size;

					if (saveOp.saving)
						_cur_CONTROL = MEM_CONTROL::NBOUT_CTRL;
					else if (_nbout_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBOUT_READ_CTRL;
					else if (_sb_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::SB_CTRL;
					else if (_nbin_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBIN_CTRL;
					else
						_cur_CONTROL = MEM_CONTROL::NOCTRL;
					break;
				case MEM_CONTROL::SB_CTRL:
					_sb_saved_addr = _cur_addr;
					_sb_saved_size -= _cur_read_size;

					if (saveOp.saving)
						_cur_CONTROL = MEM_CONTROL::NBOUT_CTRL;
					else if (_nbout_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBOUT_READ_CTRL;
					else if (_nbin_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBIN_CTRL;
					else if (_sb_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::SB_CTRL;
					else
						_cur_CONTROL = MEM_CONTROL::NOCTRL;
					break;
				case MEM_CONTROL::NBOUT_CTRL:
					// should not leave any data to be saved.
					_nbout_writedone = true;
					if (!_quite_writeback)
						cout << "[DMA] NBOut Writeback is done." << endl;

					if (_nbout_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBOUT_READ_CTRL;
					else if (_nbin_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBIN_CTRL;
					else if (_sb_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::SB_CTRL;
					else
						_cur_CONTROL = MEM_CONTROL::NOCTRL;
					break;
				case MEM_CONTROL::NBOUT_READ_CTRL:
					_nbout_saved_addr = _cur_addr;
					_nbout_saved_size -= _cur_read_size;

					//conn_nbout->print();

					// should not leave any data to be saved.
					_nbout_readdone = true;
					if (!_quite_writeback)
						cout << "[DMA] NBOut previous data loading is done." << endl;

					if (_nbout_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBOUT_READ_CTRL;
					else if (_nbin_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBIN_CTRL;
					else if (_sb_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::SB_CTRL;
					else
						_cur_CONTROL = MEM_CONTROL::NOCTRL;
					break;
				case MEM_CONTROL::NOCTRL:
					// just scheduling when needed.
					if (saveOp.saving)
						_cur_CONTROL = MEM_CONTROL::NBOUT_CTRL;
					else if (_nbout_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBOUT_READ_CTRL;
					else if (_nbin_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::NBIN_CTRL;
					else if (_sb_saved_size > 0)
						_cur_CONTROL = MEM_CONTROL::SB_CTRL;
					else
						_cur_CONTROL = MEM_CONTROL::NOCTRL;
					break;
				default:
					break;
				}

				// reset
				_cur_read_size = 0;
			}
		}
	}
}

// save output as a file
void
memory_interface::save_output(int size)
{
	std::ofstream ofs("conv_output.bin");	//make a file

	_DATA_STORAGE dnum = 0;
	unsigned int bdata1 = 0, bdata2 = 0;

	for (int i = 0; i < size; ++i)
	{
		bdata1 = _inMem[OUTPUT_ADDR + (i * 2)];
		bdata2 = _inMem[OUTPUT_ADDR + (i * 2) + 1];
		dnum = ((bdata1 & 0x000000FF) << 8) | (bdata2 & 0x000000FF);
		ofs << hex << dnum << "\t";

		if (size % 256)
			ofs << endl;

		if ((i + 1) % NBOUT_WIDTH == 0)
			ofs << endl;
	}

	ofs.close();	//close

	cout << "Output File Saved as \'conv_output.bin\'" << endl;
}