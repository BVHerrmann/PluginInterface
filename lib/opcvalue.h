#ifndef OPCVALUE_H
#define OPCVALUE_H

#include "logic.h"


class OPCValue {
public:
    OPCValue() = default;
    virtual ~OPCValue() = default;
    OPCValue(const OPCValue &) = default;
    OPCValue &operator=(const OPCValue &) = default;
    
    QString name() const { return _name; }
    QString description() const { return _description; }
    QString nodeId() const { return _node_id; }
    QString nsUri() const { return _namespace_uri; }
    
    virtual void set_valid(bool valid) = 0;
    virtual bool valid() const = 0;
    virtual QVariant displayValue() const = 0;
    virtual QVariant::Type datatype() const = 0;

    virtual void force(const QVariant &value) = 0;
    virtual bool is_forced() const = 0;
    
protected:
    QString _name;
    QString _description;
    QString _node_id;
    QString _namespace_uri;
};


class OPCInputValue : public OPCValue {
public:
    OPCInputValue() = default;
    ~OPCInputValue() = default;
    OPCInputValue(const OPCInputValue &) = default;
    OPCInputValue &operator=(const OPCInputValue &) = default;
    
    OPCInputValue(input<bool> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) { 
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description; 
    }
    OPCInputValue(input<int8_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<uint8_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<int16_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<uint16_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<int32_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<uint32_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<int64_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<uint64_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<float> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<double> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCInputValue(input<std::string> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }

    input_interface* identifier() { return _identifier; }

    void set_valid(bool valid) override;
    bool valid() const override;

    QVariant displayValue() const override;
    QVariant::Type datatype() const override;

    void force(const QVariant &value) override;
    bool is_forced() const override;

    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> value() const;

    bool has_changed(const std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string> &data);
    void update(const std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string> &data, bool valid);
    
private:
    input_interface *_identifier;
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string> _data;
    bool _is_forced = false;
};
Q_DECLARE_METATYPE(std::shared_ptr<OPCInputValue>)


class OPCOutputValue : public OPCValue {
public:
    OPCOutputValue() = default;
    ~OPCOutputValue() = default;
    OPCOutputValue(const OPCOutputValue &) = default;
    OPCOutputValue &operator=(const OPCOutputValue &) = default;
    
    OPCOutputValue(output<bool> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<int8_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<uint8_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<int16_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<uint16_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<int32_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<uint32_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<int64_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<uint64_t> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<float> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<double> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    OPCOutputValue(output<std::string> &identifier, QString name, QString node_id, QString namespace_uri = QCoreApplication::organizationDomain(), QString description = QString()) {
        _identifier = &identifier; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
    }
    
    output_interface* identifier() { return _identifier; }

    void set_valid(bool valid) override;
    bool valid() const override;

    QVariant displayValue() const override;
    QVariant::Type datatype() const override;

    void force(const QVariant &value) override;
    bool is_forced() const override;

    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> value() const;
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> update();
    
private:
    output_interface *_identifier;
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> _value; // from logic
    std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::monostate> _forced_value = std::monostate{}; // manually overwritten
};
Q_DECLARE_METATYPE(std::shared_ptr<OPCOutputValue>)

class OPCMethod {
public:
    OPCMethod() = default;
    virtual ~OPCMethod() = default;
    OPCMethod(const OPCMethod &) = default;
    OPCMethod &operator=(const OPCMethod &) = default;

    OPCMethod(QString name, QString node_id, std::function<void(void)> func, QString description = QString(), QString namespace_uri = QCoreApplication::organizationDomain()) {
        _v1 = func; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
        _type = 1;
    }
    OPCMethod(QString name, QString node_id, std::function<bool(void)> func, QString outputName, QString outputDescription = QString(), QString description = QString(), QString namespace_uri = QCoreApplication::organizationDomain()) {
        _v2 = func; _outputName = outputName; _outputDescription = outputDescription; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
        _type = 2;
    }
    OPCMethod(QString name, QString node_id, std::function<bool(bool)> func, QString outputName, QString inputName, QString outputDescription = QString(), QString inputDescription = QString(), QString description = QString(), QString namespace_uri = QCoreApplication::organizationDomain()) {
        _v3 = func; _outputName = outputName; _outputDescription = outputDescription; _inputName = inputName; _inputDescription = inputDescription; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
        _type = 3;
    }
    OPCMethod(QString name, QString node_id, std::function<bool(std::string)> func, QString outputName, QString inputName, QString outputDescription = QString(), QString inputDescription = QString(), QString description = QString(), QString namespace_uri = QCoreApplication::organizationDomain()) {
        _v4 = func; _outputName = outputName; _outputDescription = outputDescription; _inputName = inputName; _inputDescription = inputDescription; _name = name; _node_id = node_id; _namespace_uri = namespace_uri; _description = description;
        _type = 4;
    }

    QString name() const { return _name; }
    QString description() const { return _description; }
    QString nodeId() const { return _node_id; }
    QString nsUri() const { return _namespace_uri; }

    QString inputName() { return _inputName; }
    QString inputDescription() { return _inputDescription; }
    QString outputName() { return _outputName; }
    QString outputDescription() { return _outputDescription; }

    std::function<void()> v1() {return _v1; }
    std::function<bool()> v2() { return _v2; }
    std::function<bool(bool)> v3() { return _v3; }
    std::function<bool(std::string)> v4() { return _v4; }

    uint type() const { return _type; }

private:
    uint _type = 0;

    std::function<void()> _v1;
    std::function<bool()> _v2;
    std::function<bool(bool)> _v3;
    std::function<bool(std::string)> _v4;

    QString _inputName;
    QString _inputDescription;
    QString _outputName;
    QString _outputDescription;

    QString _name;
    QString _description;
    QString _node_id;
    QString _namespace_uri;
};
Q_DECLARE_METATYPE(std::shared_ptr<OPCMethod>)


#endif // OPCVALUE_H
