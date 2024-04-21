#ifndef FEATURESET_H
#define FEATURESET_H

class FeatureSet
{
public:
    FeatureSet();
    virtual ~FeatureSet() { }

    virtual bool isValid() const = 0;
};

#endif // FEATURESET_H
