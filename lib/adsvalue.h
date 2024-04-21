#ifndef ADSVALUE_H
#define ADSVALUE_H

#include "logic.h"


class ADSValue {
public:
    ADSValue() = default;
    virtual ~ADSValue() = default;
    ADSValue(const ADSValue &) = default;
    ADSValue &operator=(const ADSValue &) = default;
    
    QString symbolName() const { return _symbol_name; }
    QString name() const { return _name; }
    
    virtual void set_valid(bool valid) = 0;
    virtual bool valid() const = 0;
    virtual QVariant displayValue() const = 0;
    virtual QVariant::Type datatype() const = 0;
    
    virtual void force(const QVariant &value) = 0;
    virtual bool is_forced() const = 0;
    
protected:
    QString _symbol_name;
    QString _name;
};


class ADSInputValue : public ADSValue {
public:
    ADSInputValue() = default;
    ~ADSInputValue() = default;
    ADSInputValue(const ADSInputValue &) = default;
    ADSInputValue &operator=(const ADSInputValue &) = default;
    
    ADSInputValue(input<bool> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<int32_t> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<uint16_t> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<uint32_t> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<float> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<double> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<std::vector<double>> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSInputValue(input<std::vector<uint16_t>> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    
    void set_valid(bool valid) override;
    bool valid() const override;
    
    QVariant displayValue() const override;
    QVariant::Type datatype() const override;
    
    void force(const QVariant &value) override;
    bool is_forced() const override;
    
    std::variant<bool, int32_t, uint16_t, uint32_t, float, double, std::vector<double>, std::vector<uint16_t>, std::monostate> value() const;
    
    bool has_changed(const std::vector<std::byte> &data);
    void update(const std::vector<std::byte> &data, bool valid);
    
private:
    input_interface *_identifier;
    std::vector<std::byte> _data;
    bool _is_forced = false;
};
Q_DECLARE_METATYPE(std::shared_ptr<ADSInputValue>)


class ADSOutputValue : public ADSValue {
public:
    ADSOutputValue() = default;
    ~ADSOutputValue() = default;
    ADSOutputValue(const ADSOutputValue &) = default;
    ADSOutputValue &operator=(const ADSOutputValue &) = default;
    
    ADSOutputValue(output<bool> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSOutputValue(output<int16_t> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSOutputValue(output<uint16_t> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSOutputValue(output<float> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    ADSOutputValue(output<double> &identifier, QString name, QString symbol_name) { _identifier = &identifier; _name = name; _symbol_name = symbol_name; }
    
    void set_valid(bool valid) override;
    bool valid() const override;
    
    QVariant displayValue() const override;
    QVariant::Type datatype() const override;
    
    void force(const QVariant &value) override;
    bool is_forced() const override;
    
    std::variant<bool, uint8_t, int16_t, uint16_t, uint32_t, float, double, std::monostate> value() const;
    
    std::vector<std::byte> update();
    
private:
    output_interface *_identifier;
    std::variant<bool, uint8_t, int16_t, uint16_t, uint32_t, float, double, std::monostate> _value; // from logic
    std::variant<bool, uint8_t, int16_t, uint16_t, uint32_t, float, double, std::monostate> _forced_value = std::monostate{}; // manually overwritten
};
Q_DECLARE_METATYPE(std::shared_ptr<ADSOutputValue>)


#endif // ADSVALUE_H
