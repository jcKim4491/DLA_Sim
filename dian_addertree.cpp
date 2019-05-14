#include "dian_addertree.h"


// Constructor
dian_addertree::dian_addertree()
{
}

dian_addertree::dian_addertree(int tree_id) : _tree_id(tree_id)
{
	// initialize
	_tree_regs = new m_add*[TREE_LENGTH];
	// Lv1
	_tree_regs[0] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_0")).c_str());
	_tree_regs[1] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_1")).c_str());
	_tree_regs[2] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_2")).c_str());
	_tree_regs[3] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_3")).c_str());
	_tree_regs[4] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_4")).c_str());
	_tree_regs[5] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_5")).c_str());
	_tree_regs[6] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_6")).c_str());
	_tree_regs[7] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV1_7")).c_str());
	// Lv2
	_tree_regs[8] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV2_0")).c_str());
	_tree_regs[9] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV2_1")).c_str());
	_tree_regs[10] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV2_2")).c_str());
	_tree_regs[11] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV2_3")).c_str());
	// Lv3
	_tree_regs[12] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV3_0")).c_str());
	_tree_regs[13] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV3_1")).c_str());
	// Lv4 (final)
	_tree_regs[14] = new m_add((string("AT_") + (to_string(tree_id)) + string("_LV4_0")).c_str());
	_tree_regs[14]->setOverChecking(true);	//overflow check

	// connection
	_tree_regs[0]->setOutPort(_tree_regs[8]);
	_tree_regs[1]->setOutPort(_tree_regs[8]);
	_tree_regs[2]->setOutPort(_tree_regs[9]);
	_tree_regs[3]->setOutPort(_tree_regs[9]);
	_tree_regs[4]->setOutPort(_tree_regs[10]);
	_tree_regs[5]->setOutPort(_tree_regs[10]);
	_tree_regs[6]->setOutPort(_tree_regs[11]);
	_tree_regs[7]->setOutPort(_tree_regs[11]);
	_tree_regs[8]->setInPort1(_tree_regs[0]);
	_tree_regs[8]->setInPort2(_tree_regs[1]);
	_tree_regs[9]->setInPort1(_tree_regs[2]);
	_tree_regs[9]->setInPort2(_tree_regs[3]);
	_tree_regs[10]->setInPort1(_tree_regs[4]);
	_tree_regs[10]->setInPort2(_tree_regs[5]);
	_tree_regs[11]->setInPort1(_tree_regs[6]);
	_tree_regs[11]->setInPort2(_tree_regs[7]);

	_tree_regs[8]->setOutPort(_tree_regs[12]);
	_tree_regs[9]->setOutPort(_tree_regs[12]);
	_tree_regs[10]->setOutPort(_tree_regs[13]);
	_tree_regs[11]->setOutPort(_tree_regs[13]);
	_tree_regs[12]->setInPort1(_tree_regs[8]);
	_tree_regs[12]->setInPort2(_tree_regs[9]);
	_tree_regs[13]->setInPort1(_tree_regs[10]);
	_tree_regs[13]->setInPort2(_tree_regs[11]);

	_tree_regs[12]->setOutPort(_tree_regs[14]);
	_tree_regs[13]->setOutPort(_tree_regs[14]);
	_tree_regs[14]->setInPort1(_tree_regs[12]);
	_tree_regs[14]->setInPort2(_tree_regs[13]);

	if (!_quite_timingobj)
		cout << "NFU-2's Adder tree ID " << tree_id << "is created." << endl;
}

dian_addertree::~dian_addertree()
{
	int i;
	for (i = 0; i < TREE_LENGTH; ++i)
	{
		delete _tree_regs[i];
	}

	delete[] _tree_regs;

	if (!_quite_timingobj)
		cout << "NFU-2's Adder tree ID " << _tree_id << "is deleted." << endl;
}

// tick
void
dian_addertree::tick(_TICK curTick)
{
	int i;
	for (i = 0; i < TREE_LENGTH; ++i)
	{
		_tree_regs[i]->tick(curTick);
	}
}

// set input ports
void
dian_addertree::setInPort(timingObject *src, int target_idx)
{
	// validation check
	if (target_idx < 0 || target_idx >= ADTREE_CNT)
		return;

	if (target_idx % 2 == 0)
	{
		// even number goto port1
		_tree_regs[target_idx / 2]->setInPort1(src);
		
	}
	else
	{
		// odd number goto port2
		_tree_regs[target_idx / 2]->setInPort2(src);
	}
}

// get results
_DATA_STORAGE
dian_addertree::getResult()
{
	if (_tree_regs[14] != NULL)
		return _tree_regs[14]->getData();
	else
		return 0;
}