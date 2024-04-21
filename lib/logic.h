#ifndef LOGIC_H
#define LOGIC_H

#include <memory>
#include <variant>

#include <QtCore>

#include "iointerface.h"


class io_interface {
public:
    void set_valid(bool valid) { _valid = valid; }
    virtual bool valid() const { return _valid; }
    
    virtual QVariant displayValue() const = 0;
    
protected:
    bool _valid = false;
};

class input_interface : public io_interface {
};

class output_interface : public io_interface {
};


template <class T>
class input_base : public input_interface {
    friend class ADSInputValue;
    friend class OPCInputValue;
    friend class PNIOInputValue;
    
public:
    input_base(T initial_value) { _value = initial_value; }
    T get() const { return _value; }
    T operator()() const { return _value; }
    
    QVariant displayValue() const override { return _value; }
    
private:
    T _value;
    
    input_base<T>(const input_base<T> &) = delete;
    input_base<T> &operator=(const input_base<T> &) = delete;
    
    void set(const T value) { _value = value; }
    void set(const T value, const bool valid) { _value = value; _valid = valid; }
};

template<>
inline QVariant input_base<std::vector<double>>::displayValue() const {
    if (!_value.empty()) {
        return QString("%1,..%2..,%3").arg(_value.front()).arg(_value.size()).arg(_value.back());
    }
    else {
        return QString();
    }
}

template<>
inline QVariant input_base<std::vector<uint16_t>>::displayValue() const {
    if (!_value.empty()) {
        return QString("%1,..%2..,%3").arg(_value.front()).arg(_value.size()).arg(_value.back());
    }
    else {
        return QString();
    }
}

template<>
inline QVariant input_base<std::string>::displayValue() const {
    return QString::fromStdString(_value);
}

template <class T>
class input : public input_base<T> {
public:
    input(T initial_value = T()) : input_base<T>(initial_value) { }
};


template <class T>
class output_base : public output_interface {
public:
    output_base(T initial_value) { _value = _initial_value = initial_value; }
    T get() const { return _value; }
    void set(const T value) { _value = value; }
    
    QVariant displayValue() const override { return _value; }
    
private:
    T _value;
    T _initial_value;
    
    output_base<T>(const output_base<T> &) = delete;
    output_base<T> &operator=(const output_base<T> &) = delete;
    
    void reset() { _value = _initial_value; }
};

template <class T>
class output : public output_base<T> {
public:
    output(T initial_value = T()) : output_base<T>(initial_value) { }
};

template<>
inline QVariant output_base<std::string>::displayValue() const {
    return QString::fromStdString(_value);
}

/*
class record : public io_interface
{
	friend class PNIORecord;
public:
	record() {}

	QVariant displayValue() const override { return QVariant(QByteArray((char*)_data.data(), _data.size())); }

	// sync lesen/schreiben
	std::vector<std::byte> read();
	bool write(const std::vector<std::byte> &data); //TODO

private:
	std::vector<std::byte> _data;

	std::function<std::vector<std::byte>()> _read_req_func;
	std::function<bool(const std::vector<std::byte> &)> _write_req_func;
};
*/

class WriteRecord : public io_interface
{
	friend class PNIOWriteRecord;
public:
	WriteRecord() {}

	QVariant displayValue() const override { return QVariant(QByteArray((char*)_data.data(), _data.size()).toHex()); }
	bool write(const std::vector<std::byte> &data);

private:
	//last write Req
	std::vector<std::byte> _data;
	std::function<bool(const std::vector<std::byte> &)> _write_req_func;
};

class ReadRecord : public io_interface
{
	friend class PNIOReadRecord;
public:
	ReadRecord() {}

	QVariant displayValue() const override { return QVariant(QByteArray((char*)_data.data(), _data.size()).toHex()); } // string hex werte
	std::vector<std::byte> read();

private:
	//last received data
	std::vector<std::byte> _data;
	std::function<std::vector<std::byte>()> _read_req_func;
};

class Logic : public QObject
{
    Q_OBJECT
    
public:
    explicit Logic();
    virtual ~Logic();
    
    void setInterface(std::shared_ptr<IOInterface> value) { _interface = value; }
    void doWorkInternal();

protected:
	virtual void doWork() = 0;
	
private:
    Logic(const Logic &) = delete;
    Logic &operator=(const Logic &) = delete;
    
    std::weak_ptr<IOInterface> _interface;
};

#endif // LOGIC_H
