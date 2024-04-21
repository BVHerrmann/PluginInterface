#include "bbarcode.h"

BBarcode::BBarcode(BarcodeType type, const QString &data)
{
    _type = type;
    _data = data;
}
