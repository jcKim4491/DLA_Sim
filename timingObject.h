#pragma once
#ifndef __TIMING_OBJECT_H__
#define __TIMING_OBJECT_H__
#include "typedef_dla.h"
#include "optionparser.h"
#include <iostream>
#include <string.h>

class timingObject
{
protected:
	_TICK _start_tick;
	_TICK _end_tick;
	_TICK _propagaion_tick = 1;
	char* _name;

	bool _init = false;
public:
	//type of object
	TIMINGOBJ_TYPE _type;

	// Constructor
	timingObject()
	{
		_start_tick = 0;
		_end_tick = 0;
		_propagaion_tick = 1;
		_name = (char*)("NO_NAME");
		_type = TIMINGOBJ_TYPE::NONE;

		if (!_quite_timingobj)
			std::cout << "\'" << _name << "\'" << " is created." << std::endl;
	}

	// Constructor
	timingObject(const char* name)
	{
		_start_tick = 0;
		_end_tick = 0;
		_propagaion_tick = 1;
		_name = (char*)malloc(sizeof(char) * strlen(name));
		strcpy_s(_name, strlen(name) + 1, name);

		if (!_quite_timingobj)
			std::cout << "\'" << _name << "\'" << " is created." << std::endl;
	}

	// Copy constructor
	timingObject(const timingObject &to)
	{
		//copy time status
		_start_tick = to._start_tick;
		_end_tick = to._end_tick;
		//copy name
		_name = (char*)malloc(sizeof(char) * strlen(to._name));
		strcpy_s(_name, strlen(to._name) + 1, to._name);
	}

	// Destructor
	~timingObject()
	{
		if (!_quite_timingobj)
			std::cout << "\'" << _name << "\'" << " is deleted." << std::endl;
	}

	// Get timing object name
	char* getName()
	{
		return _name;
	}

	void virtual tick(_TICK curTick)
	{

	}

	void virtual tick_update(_TICK curTick)
	{

	}

	_DATA_STORAGE virtual getData()
	{
		return 0;
	}
};

#endif //__TIMiNG_OBJECT_H__