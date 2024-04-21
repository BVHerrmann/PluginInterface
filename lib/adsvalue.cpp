#include "adsvalue.h"


void ADSInputValue::set_valid(bool valid)
{
    _identifier->set_valid(valid);
}

bool ADSInputValue::valid() const
{
    return _identifier->valid();
}

QVariant ADSInputValue::displayValue() const
{
    return _identifier->displayValue();
}

QVariant::Type ADSInputValue::datatype() const
{
    return dynamic_cast<input<bool> *>(_identifier) != nullptr ? QVariant::Bool : QVariant::UserType;
}

void ADSInputValue::force(const QVariant &value) {
    _is_forced = true;
    if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
        identifier->set(value.toBool());
    } else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
        identifier->set(value.toULongLong());
    } else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
        identifier->set(value.toULongLong());
    } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
        identifier->set(value.toFloat());
    } else if (auto identifier = dynamic_cast<input<double> *>(_identifier)) {
        identifier->set(value.toDouble());
    } else {
        assert(false);
    }
}

bool ADSInputValue::is_forced() const {
    return _is_forced;
}

std::variant<bool, int32_t, uint16_t, uint32_t, float, double, std::vector<double>, std::vector<uint16_t>, std::monostate> ADSInputValue::value() const
{
    std::variant<bool, int32_t, uint16_t, uint32_t, float, double, std::vector<double>, std::vector<uint16_t>, std::monostate> v;
    if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<int32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<double> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<std::vector<double>> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<std::vector<uint16_t>> *>(_identifier)) {
        v = identifier->get();
    } else {
        v = std::monostate();
    }

    return v;
}

bool ADSInputValue::has_changed(const std::vector<std::byte> &data)
{
    return _data != data;
}

void ADSInputValue::update(const std::vector<std::byte> &data, bool valid)
{
    if (has_changed(data)) {
        // store new data
        _data = data;
        _is_forced = false;
        
        if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
            uint8_t value = 0;
            memcpy(&value, data.data(), sizeof(value));
            
            identifier->set((bool)value, valid);

        } else if (auto identifier = dynamic_cast<input<int32_t> *>(_identifier)) {
            int32_t value = 0;
            memcpy(&value, data.data(), sizeof(value));
            
            identifier->set(qFromLittleEndian(value), valid);
            
        } else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
            uint16_t value = 0;
            memcpy(&value, data.data(), sizeof(value));
            
            identifier->set(qFromLittleEndian(value), valid);
            
        } else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
            uint32_t value = 0;
            memcpy(&value, data.data(), sizeof(value));
            
            identifier->set(qFromLittleEndian(value), valid);
            
        } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
            float value = 0.0;
            memcpy(&value, data.data(), sizeof(value));
            
            identifier->set(qFromLittleEndian(value), valid);
        
        } else if (auto identifier = dynamic_cast<input<double> *>(_identifier)) {
            double value = 0.0;
            memcpy(&value, data.data(), sizeof(value));
            
            identifier->set(qFromLittleEndian(value), valid);

        } else if (auto identifier = dynamic_cast<input<std::vector<double>> *>(_identifier)) {
            std::vector<double> value(data.size() / sizeof(double));
            memcpy(value.data(), data.data(), data.size());

            identifier->set(qFromLittleEndian(value), valid);

        }
        else if (auto identifier = dynamic_cast<input<std::vector<uint16_t>> *>(_identifier)) {
            std::vector<uint16_t> value(data.size() / sizeof(uint16_t));
            memcpy(value.data(), data.data(), data.size());

            identifier->set(qFromLittleEndian(value), valid);
        }

    } else {
        _identifier->set_valid(valid);
    }
}


void ADSOutputValue::set_valid(bool valid)
{
    _identifier->set_valid(valid);
}

bool ADSOutputValue::valid() const
{
    return _identifier->valid();
}

QVariant ADSOutputValue::displayValue() const
{
    return _identifier->displayValue();
}

QVariant::Type ADSOutputValue::datatype() const
{
    return dynamic_cast<output<bool> *>(_identifier) != nullptr ? QVariant::Bool : QVariant::UserType;
}
    
void ADSOutputValue::force(const QVariant &value)
{
    if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
        _forced_value = value.toBool();
    } else {
        assert(false);
    }
}

bool ADSOutputValue::is_forced() const
{
    return !std::holds_alternative<std::monostate>(_forced_value);
}

std::variant<bool, uint8_t, int16_t, uint16_t, uint32_t, float, double, std::monostate> ADSOutputValue::value() const
{
    std::variant<bool, uint8_t, int16_t, uint16_t, uint32_t, float, double, std::monostate> v;
    if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint8_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<int16_t> *>(_identifier)) {
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

std::vector<std::byte> ADSOutputValue::update()
{
    std::vector<std::byte> data;
    
    // determine output value
    auto output_value = std::holds_alternative<std::monostate>(_forced_value) || value() != _value ? value() : _forced_value;
    
    if (!std::holds_alternative<std::monostate>(output_value)) {
        // reset forced value if necessary
        if (value() != _value) {
            _value = value();
            _forced_value = std::monostate();
        }
        
        if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
            data.resize(sizeof(bool));
            bool value = std::get<bool>(output_value);
            qToLittleEndian(value, data.data());
            
        } else if (auto identifier = dynamic_cast<output<uint8_t> *>(_identifier)) {
            data.resize(sizeof(uint8_t));
            uint8_t output = std::get<uint8_t>(output_value);
            qToLittleEndian(output, data.data());

        } else if (auto identifier = dynamic_cast<output<int16_t> *>(_identifier)) {
            data.resize(sizeof(int16_t));
            int16_t output = std::get<int16_t>(output_value);
            qToLittleEndian(output, data.data());
            
        } else if (auto identifier = dynamic_cast<output<uint16_t> *>(_identifier)) {
            data.resize(sizeof(uint16_t));
            uint16_t output = std::get<uint16_t>(output_value);
            qToLittleEndian(output, data.data());
            
        } else if (auto identifier = dynamic_cast<output<uint32_t> *>(_identifier)) {
            data.resize(sizeof(uint32_t));
            uint32_t output = std::get<uint32_t>(output_value);
            qToLittleEndian(output, data.data());
            
        } else if (auto identifier = dynamic_cast<output<float> *>(_identifier)) {
            data.resize(sizeof(float));
            float output = std::get<float>(output_value);
            qToLittleEndian(output, data.data());
            
        } else if (auto identifier = dynamic_cast<output<double> *>(_identifier)) {
            data.resize(sizeof(double));
            double output = std::get<double>(output_value);
            qToLittleEndian(output, data.data());
            
        } else {
            assert(false);
        }
    }
    
    return data;
}
