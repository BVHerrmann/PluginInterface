#ifndef BOOST_ICL_SUPPORT_H
#define BOOST_ICL_SUPPORT_H

#include <QDataStream>

#ifndef Q_OS_WIN
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#endif
#ifndef Q_MOC_RUN
#include <boost/icl/interval_set.hpp>
#endif
#ifndef Q_OS_WIN
#pragma clang diagnostic pop
#endif

using namespace boost;

inline QDataStream &operator<<(QDataStream &stream, const boost::icl::interval_set<double> &set)
{
    stream << (uint64_t) set.iterative_size();
    for (boost::icl::interval_set<double>::iterator it = set.begin() ; it != set.end(); ++it) {
        icl::continuous_interval<double> is = *it;
        stream << is.lower();
        stream << is.upper();
    }

    return stream;
}

inline QDataStream &operator>>(QDataStream &stream, boost::icl::interval_set<double> &set)
{
    uint64_t size;
    stream >> size;

    for (uint64_t i = 0; i < size; i++) {
        double lower;
        double upper;
        stream >> lower;
        stream >> upper;
        set += icl::interval<double>::closed(lower, upper);
    }

    return stream;
}

#endif // BOOST_ICL_SUPPORT_H
