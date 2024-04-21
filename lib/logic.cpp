#include "logic.h"


bool WriteRecord::write( const std::vector<std::byte> &data)
{
	if (_write_req_func) {
        _data = data;
		return _write_req_func(data);
	}
	
	return false;
}

std::vector<std::byte> ReadRecord::read()
{
	std::vector<std::byte> data;

	if (_read_req_func) {
		data = _read_req_func();
	}

	return data;
}



Logic::Logic() :
    QObject(nullptr)
{

}

Logic::~Logic()
{

}

void Logic::doWorkInternal()
{
    // run logic
    doWork();

    // write outputs
    if (auto interface = _interface.lock()) {
        interface->writeOutputs();
    }
}
