#include "act_module.h"

_DATA_STORAGE tempd;
double tempf;

// Constructors
act_module::act_module()
{
	_act_access_cnt = 0;
}
act_module::act_module(const char* name)
	: m_reg(name)
{
	_act_access_cnt = 0;
}

// Destructor
act_module::~act_module()
{
}

// tick
void
act_module::tick(_TICK curTick)
{
	// stats
	if (_data != _data_buffer)
		ACTFUNC_ACCESS++;

	// propagate previous data
	_data = _data_buffer;

	// validation check
	if (_in_port == NULL)
		return;

	// apply activation functions
	switch (_cur_actfunc)
	{
	case ACTFUNC::SIGMOID:
		_data_buffer = _in_port->getData();
		tempf = (double)(_data_buffer) / 1024;			// convert to double
		tempf = 1 / (1 + std::exp(-tempf));				// sigmoid
		_data_buffer = (_DATA_STORAGE)(tempf * 1024.0);	// restore to int
		
		if (!_quite_actfunc)
			std::cout << "[NFU-3]" << this->getName() << ": SIGMOID(" << _in_port->getData() << " => "
			<< _data_buffer << ") applied." << std::endl;
		break;
	case ACTFUNC::TANH:
		_data_buffer = _in_port->getData();
		tempf = double(_data_buffer) / 1024;			// convert to double
		tempf = std::tanh(tempf);						// sigmoid
		_data_buffer = (_DATA_STORAGE)(tempf * 1024.0);	// restore to int

		if (!_quite_actfunc)
			std::cout << "[NFU-3]" << this->getName() << ": TANH(" << _in_port->getData() << " => "
			<< _data_buffer << ") applied." << std::endl;
		break;
	case ACTFUNC::RELU:
		tempd = _in_port->getData();
		if (tempd < 0)
			tempd = 0;

		_data_buffer = tempd;
		if (!_quite_actfunc)
			std::cout << "[NFU-3]" << std::hex << this->getName() << ": RELU(" << _in_port->getData() << " => "
			<< _data_buffer << ") applied." << std::endl;
		break;
	case ACTFUNC::NOACT:
		_data_buffer = _in_port->getData();
		if (!_quite_actfunc)
			std::cout << "[NFU-3]" << std::hex << this->getName() << ": NONE(" << _in_port->getData() << " => "
			<< _data_buffer << ") applied." << std::endl;
		break;
	default:
		break;
	}
}