#include "opcvalue.h"


void OPCInputValue::set_valid(bool valid)
{
    _identifier->set_valid(valid);
}

bool OPCInputValue::valid() const
{
    return _identifier->valid();
}

QVariant OPCInputValue::displayValue() const
{
    return _identifier->displayValue();
}

QVariant::Type OPCInputValue::datatype() const
{
    return dynamic_cast<input<bool> *>(_identifier) != nullptr ? QVariant::Bool : QVariant::UserType;
}

void OPCInputValue::force(const QVariant &value) {
    _is_forced = true;
    if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
        identifier->set(value.toBool());
    } else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
        identifier->set(value.toULongLong());
    } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
        identifier->set(value.toFloat());
    } else {
        assert(false);
    }
}

bool OPCInputValue::is_forced() const {
    return _is_forced;
}

std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> OPCInputValue::value() const
{
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> v;
    if (auto identifier = dynamic_cast<input<bool> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<int8_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<uint8_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<int16_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<uint16_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<int32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<uint32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<int64_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<uint64_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<float> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<double> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<input<std::string> *>(_identifier)) {
        v = identifier->get();
    } else {
        v = std::monostate();
    }

    return v;
}

bool OPCInputValue::has_changed(const std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string> &data)
{
    return _data != data;
}

void OPCInputValue::update(const std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string> &data, bool valid)
{
    if (has_changed(data)) {
        // store new data
        _data = data;
        _is_forced = false;
        
        std::visit([this, valid](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            auto identifier = dynamic_cast<input<T> *>(_identifier);
            if (identifier) {
                identifier->set(arg, valid);
            } else {
                qWarning() << "OPC-UA Input type missmatch for" << _node_id << "got" << typeid(T).name();
            }
        }, data);
    } else {
        _identifier->set_valid(valid);
    }
}


void OPCOutputValue::set_valid(bool valid)
{
    _identifier->set_valid(valid);
}

bool OPCOutputValue::valid() const
{
    return _identifier->valid();
}

QVariant OPCOutputValue::displayValue() const
{
    return _identifier->displayValue();
}

QVariant::Type OPCOutputValue::datatype() const
{
    return dynamic_cast<output<bool> *>(_identifier) != nullptr ? QVariant::Bool : QVariant::UserType;
}
    
void OPCOutputValue::force(const QVariant &value)
{
    if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
        _forced_value = value.toBool();
    } else {
        assert(false);
    }
}

bool OPCOutputValue::is_forced() const
{
    return !std::holds_alternative<std::monostate>(_forced_value);
}
    
std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> OPCOutputValue::value() const
{
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> v;
    if (auto identifier = dynamic_cast<output<bool> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<int8_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint8_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<int16_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint16_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<int32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint32_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<int64_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<uint64_t> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<float> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<double> *>(_identifier)) {
        v = identifier->get();
    } else if (auto identifier = dynamic_cast<output<std::string> *>(_identifier)) {
        v = identifier->get();
    } else {
        v = std::monostate();
    }
    
    return v;
}
    
std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> OPCOutputValue::update()
{
    // determine output value
    auto output_value = value() != _value ? value() : _forced_value;
    
    if (!std::holds_alternative<std::monostate>(output_value)) {
        // reset forced value if necessary
        if (value() != _value) {
            _value = value();
            _forced_value = std::monostate();
        }
    } else if (!valid()) {
        output_value = _value;
    }
    
    return output_value;
}
