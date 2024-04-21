#include "pniovalue.h"


void PNIOInputValue::set_valid(bool valid)
{
    _identifier->set_valid(valid);
}

bool PNIOInputValue::valid() const
{
    return _identifier->valid();
}

QVariant PNIOInputValue::displayValue() const
{
    return _identifier->displayValue();
}

QVariant::Type PNIOInputValue::datatype() const
{
    return dynamic_cast<input<bool> *>(_identifier) != nullptr ? QVariant::Bool : QVariant::UserType;
}

void PNIOInputValue::force(const QVariant &value) {
    _is_forced = true;
    if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
        identifier->set(value.toBool());
    } else if (auto identifier = dynamic_cast<input<uint8_t> *>(_identifier)) {
		identifier->set(value.toUInt());
	} else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
		identifier->set(value.toUInt());
	} else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
        identifier->set(value.toULongLong());
    } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
        identifier->set(value.toFloat());
    } else {
        assert(false);
    }
}

bool PNIOInputValue::is_forced() const {
    return _is_forced;
}

bool PNIOInputValue::has_changed(const std::vector<std::byte> &data)
{
    if (data.size() != _data.size()) {
        return true;
    } else if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
        // only check one bit
        std::byte mask = std::byte(1 << _bit);
        return (data[_offset] & mask) != (_data[_offset] & mask);
    } else {
        // check full range
        return !std::equal(_data.begin() + _offset, _data.begin() + _offset + _size, data.begin() + _offset);
    }
}

void PNIOInputValue::update(const std::vector<std::byte> &data, bool valid)
{
    if (has_changed(data)) {
        // store new data
        _data = data;
        _is_forced = false;
        
        if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
            uint8_t value = 0;
            memcpy(&value, &data.data()[_offset], sizeof(value));
            
            identifier->set((value >> _bit) & 0x01, valid);

        } else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
            uint16_t value = 0;
            memcpy(&value, &data.data()[_offset], sizeof(value));
            
            identifier->set(qFromBigEndian(value), valid);
            
        } else if (auto identifier = dynamic_cast<input<uint8_t> *>(_identifier)) {
			uint8_t value = 0;
			memcpy(&value, &data.data()[_offset], sizeof(value));

			identifier->set(qFromBigEndian(value), valid);

		} else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
			uint16_t value = 0;
			memcpy(&value, &data.data()[_offset], sizeof(value));

			identifier->set(qFromBigEndian(value), valid);

		} else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
            uint32_t value = 0;
            memcpy(&value, &data.data()[_offset], sizeof(value));
            
            identifier->set(qFromBigEndian(value), valid);
            
        } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
            float value = 0.0;
            uint32_t tmp_value = 0;
            memcpy(&tmp_value, &data.data()[_offset], sizeof(tmp_value));
            tmp_value = qFromBigEndian(tmp_value);
            value = *reinterpret_cast<float*>(&tmp_value);
            
            identifier->set(value, valid);
        
        } else if (auto identifier = dynamic_cast<input<double> *>(_identifier)) {
            double value = 0.0;
            uint64_t tmp_value = 0;
            memcpy(&tmp_value, &data.data()[_offset], sizeof(tmp_value));
            tmp_value = qFromBigEndian(tmp_value);
            value = *reinterpret_cast<float*>(&tmp_value);
            
            identifier->set(value, valid);
        }
        
    } else {
        _identifier->set_valid(valid);
    }
}


void PNIOOutputValue::set_valid(bool valid)
{
    _identifier->set_valid(valid);
}

bool PNIOOutputValue::valid() const
{
    return _identifier->valid();
}

QVariant PNIOOutputValue::displayValue() const
{
    return _identifier->displayValue();
}

QVariant::Type PNIOOutputValue::datatype() const
{
    return dynamic_cast<output<bool> *>(_identifier) != nullptr ? QVariant::Bool : QVariant::UserType;
}
    
void PNIOOutputValue::force(const QVariant &value)
{
    if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
        _forced_value = value.toBool();
    } else {
        assert(false);
    }
}

bool PNIOOutputValue::is_forced() const
{
    return !std::holds_alternative<std::monostate>(_forced_value);
}
    
std::variant<bool, uint8_t, uint16_t, uint32_t, float, double, std::monostate> PNIOOutputValue::value() const
{
    std::variant<bool, uint8_t, uint16_t, uint32_t, float, double, std::monostate> v;
    if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint8_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint16_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<float> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<double> *>(_identifier)) {
        v = identifier->get();
    } else {
        v = std::monostate();
    }
    
    return v;
}
    
void PNIOOutputValue::update(std::vector<std::byte> &data)
{
    // determine output value
    auto output_value = value() != _value ? value() : _forced_value;
    
    if (!std::holds_alternative<std::monostate>(output_value)) {
        // check bounds
        assert(data.size() >= _offset + _size);
        
        // reset forced value if necessary
        if (value() != _value) {
            _value = value();
            _forced_value = std::monostate();
        }
        
        if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
            bool value = std::get<bool>(output_value);
            if (value) {
                std::byte mask = std::byte(1 << _bit);
                data[_offset] = data[_offset] | mask;
            } else {
                std::byte mask = std::byte(255 - (1 << _bit));
                data[_offset] = data[_offset] & mask;
            }
            
        } else if (auto identifier = dynamic_cast<output<uint8_t> *>(_identifier)) {
            uint8_t value = std::get<uint8_t>(output_value);
            qToBigEndian(value, (uchar*) &data.data()[_offset]);
        
        } else if (auto identifier = dynamic_cast<output<uint16_t> *>(_identifier)) {
            uint16_t value = std::get<uint16_t>(output_value);
            qToBigEndian(value, (uchar*) &data.data()[_offset]);
            
        } else if (auto identifier = dynamic_cast<output<uint32_t> *>(_identifier)) {
            uint32_t value = std::get<uint32_t>(output_value);
            qToBigEndian(value, (uchar*) &data.data()[_offset]);
            
        } else if (auto identifier = dynamic_cast<output<float> *>(_identifier)) {
            float value = std::get<float>(output_value);
            uint32_t value_bytes = 0;
            memcpy(&value_bytes, &value, sizeof(value_bytes));
            qToBigEndian(value_bytes, (uchar*) &data.data()[_offset]);
            
        } else if (auto identifier = dynamic_cast<output<double> *>(_identifier)) {
            double value = std::get<double>(output_value);
            uint64_t value_bytes = 0;
            memcpy(&value_bytes, &value, sizeof(value_bytes));
            qToBigEndian(value_bytes, (uchar*) &data.data()[_offset]);
            
        } else {
            assert(false);
        }
    }
}


PNIORecord::PNIORecord(QString name, uint32_t slot, uint32_t record_index)
{ 
	_name = name; 
	_slot = slot; 
	_record_index = record_index;
}

PNIOWriteRecord::PNIOWriteRecord(WriteRecord &identifier, QString name, uint32_t slot, uint32_t record_index) 
	: PNIORecord(name, slot, record_index)
{
	_identifier = &identifier;

	_identifier->_write_req_func = std::bind(&PNIOWriteRecord::writeReq, this, std::placeholders::_1);
}

void PNIOWriteRecord::set_valid(bool valid)
{
	_identifier->set_valid(valid);
}

bool PNIOWriteRecord::valid() const
{
	return _identifier->valid();
}

QVariant PNIOWriteRecord::displayValue() const
{
	return _identifier->displayValue();
}

void PNIOWriteRecord::update(bool valid)
{
	set_valid(valid);
	_sem.release();
}

bool PNIOWriteRecord::writeReq(const std::vector<std::byte>& data)
{
	_write_req_func(_slot, _record_index, _req_ref, data);

	_sem.acquire();

	return _identifier->valid();
}

PNIOReadRecord::PNIOReadRecord(ReadRecord & identifier, QString name, uint32_t slot, uint32_t record_index)
	: PNIORecord(name, slot, record_index)
{
	_identifier = &identifier;

	_identifier->_read_req_func = std::bind(&PNIOReadRecord::readReq, this);
}

void PNIOReadRecord::set_valid(bool valid)
{
	_identifier->set_valid(valid);
}

bool PNIOReadRecord::valid() const
{
	return _identifier->valid();
}

QVariant PNIOReadRecord::displayValue() const
{
	return _identifier->displayValue();
}

void PNIOReadRecord::update(const std::vector<std::byte>& data, bool valid)
{
	_identifier->_data = data;
	set_valid(valid);
	_sem.release();
}

std::vector<std::byte> PNIOReadRecord::readReq()
{
	_read_req_func(_slot, _record_index, _req_ref);

	_sem.acquire();

	return _identifier->_data;
}
