#include "mult_module.h"


// Constructor
m_mult::m_mult() : timingObject()
{
	_type = TIMINGOBJ_TYPE::MULT;
	_mul_bitsize = 16;
	_mul_access_cnt = 0;
	_in_port1 = 0;
	_in_port2 = 0;
	_out_port = 0;
	_data_buffer = 0;
}

m_mult::m_mult(const char* name, _BITSIZE mult_bitsize) : timingObject(name)
{
	_type = TIMINGOBJ_TYPE::MULT;
	_mul_bitsize = mult_bitsize;
	_mul_access_cnt = 0;
	_in_port1 = 0;
	_in_port2 = 0;
	_out_port = 0;
	_data_buffer = 0;
}

// Copy Constructor
m_mult::m_mult(const m_mult& p) : timingObject(p)
{
	_type = TIMINGOBJ_TYPE::MULT;
	_mul_bitsize = p._mul_bitsize;
	_mul_access_cnt = p._mul_access_cnt;
	_in_port1 = p._in_port1;
	_in_port2 = p._in_port2;
	_out_port = p._out_port;
	_data_buffer = p._data_buffer;
}

// Destructor
m_mult::~m_mult()
{

}

// Tick
void
m_mult::tick(_TICK curTick)
{
	if (calculation())
	{
		// none
	}
	else
	{
		if (!_quite_mult)
			std::cout << "[" << this->_name << "] calculation failed." << std::endl;
	}
}

// Tick
void
m_mult::tick(_TICK curTick, bool write)
{
	if (write == false)
	{
		// propagate previous result
		_data = _data_buffer;
	}
	else
	{
		tick(curTick);
	}
}

// Set ports
void
m_mult::setInPort1(timingObject* tobj)
{
	_in_port1 = tobj;
	if (!_quite_moduleconnection)
		std::cout << tobj->getName() << " is connected to " << this->getName() << std::endl;
}

void
m_mult::setInPort2(timingObject* tobj)
{
	_in_port2 = tobj;
	if (!_quite_moduleconnection)
		std::cout << tobj->getName() << " is connected to " << this->getName() << std::endl;
}

void
m_mult::setOutPort(timingObject* tobj)
{
	_out_port = tobj;
}

// Get ports
timingObject*
m_mult::getInPort1()
{
	return _in_port1;
}

timingObject*
m_mult::getInPort2()
{
	return _in_port2;
}

timingObject*
m_mult::getOutPort()
{
	return _out_port;
}

// Input feed
_DATA_STORAGE
m_mult::getData()
{
	return _data;
}

// Calculation
bool
m_mult::calculation()
{
	if (_data != _data_buffer)
		MULT_ACCESS++;

	// propagate previous result
	_data = _data_buffer;

	// all ports should be set.
	if (_in_port1 == NULL)
		return false;
	if (_in_port2 == NULL)
		return false;

	// all ports should be a register
	if (_in_port1->_type != TIMINGOBJ_TYPE::REGISTER)
		return false;
	if (_in_port2->_type != TIMINGOBJ_TYPE::REGISTER)
		return false;

	// fixed point multiply
	_data_buffer = ((m_reg*)_in_port1)->getData() * (((m_reg*)_in_port2)->getData());

	if (!_quite_mult)
		std::cout << "[" << this->_name << "] calculation done. result " << std::hex << ((m_reg*)_in_port1)->getData()
		<< " * " << (((m_reg*)_in_port2)->getData()) << " = " << _data_buffer << std::endl;
	// TODO: fit for the data set fixed-point representation type.

	// calc successfully
	return true;
}