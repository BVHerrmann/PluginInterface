#ifndef PNIOVALUE_H
#define PNIOVALUE_H

#include "logic.h"


class PNIOValue {
public:
    PNIOValue() = default;
    virtual ~PNIOValue() = default;
    PNIOValue(const PNIOValue &) = default;
    PNIOValue &operator=(const PNIOValue &) = default;
    
    QString name() const { return _name; }
    uint32_t slot() const { return _slot; }
    uint32_t subslot() const { return _subslot; }
    std::pair<uint32_t, uint32_t> slot_subslot() const { return std::make_pair(_slot, _subslot); }
    uint32_t offset() const { return _offset; }
    uint32_t bit() const { return _bit; }
    uint32_t size() const { return _size; }
    
    virtual void set_valid(bool valid) = 0;
    virtual bool valid() const = 0;
    virtual QVariant displayValue() const = 0;
    virtual QVariant::Type datatype() const = 0;
    
    virtual void force(const QVariant &value) = 0;
    virtual bool is_forced() const = 0;
    
protected:
    QString _name;
    uint32_t _slot = 0;
    uint32_t _subslot = 0;
    uint32_t _offset = 0;
    uint32_t _bit = 0;
    uint32_t _size = 1;
};


class PNIOInputValue : public PNIOValue {
public:
    PNIOInputValue() = default;
    ~PNIOInputValue() = default;
    PNIOInputValue(const PNIOInputValue &) = default;
    PNIOInputValue &operator=(const PNIOInputValue &) = default;
    
    PNIOInputValue(input<bool> &identifier, QString name, uint32_t slot, uint32_t offset, uint32_t bit) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _bit = bit; _size = sizeof(bool); assert(bit < 8); }
	PNIOInputValue(input<uint8_t> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(uint8_t); }
	PNIOInputValue(input<uint16_t> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(uint16_t); }
    PNIOInputValue(input<uint32_t> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(uint32_t); }
    PNIOInputValue(input<float> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(float); }
    PNIOInputValue(input<double> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(double); }
    
    void set_valid(bool valid) override;
    bool valid() const override;
    
    QVariant displayValue() const override;
    QVariant::Type datatype() const override;
    
    void force(const QVariant &value) override;
    bool is_forced() const override;
    
    bool has_changed(const std::vector<std::byte> &data);
    void update(const std::vector<std::byte> &data, bool valid);
    
private:
    input_interface *_identifier;
    std::vector<std::byte> _data;
    bool _is_forced = false;
};
Q_DECLARE_METATYPE(std::shared_ptr<PNIOInputValue>)


class PNIOOutputValue : public PNIOValue {
public:
    PNIOOutputValue() = default;
    ~PNIOOutputValue() = default;
    PNIOOutputValue(const PNIOOutputValue &) = default;
    PNIOOutputValue &operator=(const PNIOOutputValue &) = default;
    
    PNIOOutputValue(output<bool> &identifier, QString name, uint32_t slot, uint32_t offset, uint32_t bit) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _bit = bit; _size = sizeof(bool); assert(bit < 8); }
    PNIOOutputValue(output<uint8_t> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(uint8_t); }
    PNIOOutputValue(output<uint16_t> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(uint16_t); }
    PNIOOutputValue(output<uint32_t> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(uint32_t); }
    PNIOOutputValue(output<float> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(float); }
    PNIOOutputValue(output<double> &identifier, QString name, uint32_t slot, uint32_t offset) { _identifier = &identifier; _name = name; _slot = slot; _offset = offset; _size = sizeof(double); }
    
    void set_valid(bool valid) override;
    bool valid() const override;
    
    QVariant displayValue() const override;
    QVariant::Type datatype() const override;
    
    void force(const QVariant &value) override;
    bool is_forced() const override;
    
    std::variant<bool, uint8_t, uint16_t, uint32_t, float, double, std::monostate> value() const;
    
    void update(std::vector<std::byte> &data);
    
private:
    output_interface *_identifier;
    std::variant<bool, uint8_t, uint16_t, uint32_t, float, double, std::monostate> _value; // from logic
    std::variant<bool, uint8_t, uint16_t, uint32_t, float, double, std::monostate> _forced_value = std::monostate{}; // manually overwritten
};
Q_DECLARE_METATYPE(std::shared_ptr<PNIOOutputValue>)


class PNIORecord {
public:
	PNIORecord() = default;
	~PNIORecord() = default;
	PNIORecord(const PNIORecord &) = default;
	PNIORecord &operator=(const PNIORecord &) = default;

	PNIORecord(QString name, uint32_t slot, uint32_t record_index);
	
	QString name() const { return _name; }
	uint32_t slot() const { return _slot; }
	uint32_t recordIndex() const { return _record_index; }
	uint32_t reqRef() const { return _req_ref; }

	virtual void set_valid(bool valid) = 0;
	virtual bool valid() const = 0;
	virtual QVariant displayValue() const = 0;

	//void update(bool valid);
	//void update(const std::vector<std::byte> &data, bool valid);

protected:
	QString _name;
	uint32_t _slot = 0;
	uint32_t _record_index;
	uint32_t _req_ref = 0;

private:
	QSemaphore _sem;
};
Q_DECLARE_METATYPE(std::shared_ptr<PNIORecord>)

class PNIOWriteRecord : public PNIORecord{
public:
	PNIOWriteRecord() = default;
	~PNIOWriteRecord() = default;
	PNIOWriteRecord(const PNIOWriteRecord &) = default;
	PNIOWriteRecord &operator=(const PNIOWriteRecord &) = default;

	PNIOWriteRecord(WriteRecord &identifier, QString name, uint32_t slot, uint32_t record_index);

	void set_valid(bool valid) override;
	bool valid() const override;
	QVariant displayValue() const override;
	void update(bool valid);

	void set_write_req_func(std::function<bool(const uint32_t slot, const uint32_t record_index, const uint32_t req_ref, const std::vector<std::byte> &)> write_req_func) {
		_write_req_func = std::bind(write_req_func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}
private:
	WriteRecord* _identifier;
	QSemaphore _sem;

	bool writeReq(const std::vector<std::byte> &data);
	std::function<bool(const uint32_t slot, const uint32_t record_index, uint32_t req_ref, const std::vector<std::byte> &)> _write_req_func;

};
Q_DECLARE_METATYPE(std::shared_ptr<PNIOWriteRecord>)

class PNIOReadRecord : public PNIORecord {
public:
	PNIOReadRecord() = default;
	~PNIOReadRecord() = default;
	PNIOReadRecord(const PNIOReadRecord &) = default;
	PNIOReadRecord &operator=(const PNIOReadRecord &) = default;

	PNIOReadRecord(ReadRecord &identifier, QString name, uint32_t slot, uint32_t record_index);

	void set_valid(bool valid) override;
	bool valid() const override;
	QVariant displayValue() const override;
	void update(const std::vector<std::byte> &data, bool valid);

	void set_read_req_func(std::function<bool(const uint32_t slot, const uint32_t record_index, const uint32_t req_ref)> read_req_func) {
		_read_req_func = std::bind(read_req_func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}
private:
	ReadRecord* _identifier;
	QSemaphore _sem;

	std::vector<std::byte> readReq();
	std::function<bool(const uint32_t slot, const uint32_t record_index, const uint32_t req_ref)> _read_req_func;
};
Q_DECLARE_METATYPE(std::shared_ptr<PNIOReadRecord>)




#endif // PNIOVALUE_H
