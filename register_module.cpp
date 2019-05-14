#include "register_module.h"

//tick
void
m_reg::tick(_TICK curTick)
{
	// propagate previous data
	if (this->_data_buffer != this->_data)
		REGISTER_ACCESS++;
	this->_data = this->_data_buffer;

	// validate check
	if (_in_port == NULL)
		return;

	switch (_in_port->_type)
	{
	case TIMINGOBJ_TYPE::REGISTER:
		//forward input
		this->_data_buffer = ((m_reg*)_in_port)->_data;
		break;
	default:
		break;
	}

	// apply propagation delay
	_start_tick = curTick;
}

void
m_reg::tick(_TICK curTick, bool write)
{
	// propagate previous data
	if (this->_data_buffer != this->_data)
		REGISTER_ACCESS++;
	this->_data = this->_data_buffer;

	// validate check
	if (_in_port == NULL)
		return;

	if (write)
	{
		switch (_in_port->_type)
		{
		case TIMINGOBJ_TYPE::REGISTER:
			//forward input
			this->_data_buffer = ((m_reg*)_in_port)->_data;
			break;
		default:
			break;
		}
	}

	// apply propagation delay
	_start_tick = curTick;
}

// Set Ports
void
m_reg::setInPort(timingObject * tobj)
{
	_in_port = tobj;
}

void
m_reg::setOutPort(timingObject* tobj)
{
	_out_port = tobj;
}

// Get Ports 
timingObject*
m_reg::getInPort()
{
	return _in_port;
}

timingObject*
m_reg::getOutPort()
{
	return _out_port;
}

// Set Data
void 
m_reg::setData(_DATA_STORAGE data)
{
	this->_data = (data);
}

// Get Data
_DATA_STORAGE
m_reg::getData()
{
	// Truncate by bitsize
	return (_data);

}

// Input forced feed
void
m_reg::foreced_feed(_DATA_STORAGE feed_data)
{
	_data_buffer = feed_data;
	_data = feed_data;
}

// Input feed
void
m_reg::input_feed()
{
	// validate check
	if (_in_port == NULL)
		return;

	switch (_in_port->_type)
	{
	case TIMINGOBJ_TYPE::REGISTER:
		//forward input
		this->_data = ((m_reg*)_in_port)->_data;
		break;
	default:
		break;
	}

}