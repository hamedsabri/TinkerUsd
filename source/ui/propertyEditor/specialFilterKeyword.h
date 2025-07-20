#pragma once

#include <QtCore/QString>
#include <QtCore/QPair>

namespace TINKERUSD_NS
{

enum class SpecialFilterKeyword
{
    None,
    NoValue,
    DefaultValue,
    TimeSamples,
    ModifiedValue
};

inline QPair<QString, QString> convert(SpecialFilterKeyword keyword)
{
    switch (keyword)
    {
    case SpecialFilterKeyword::NoValue:
        return qMakePair(QString("No Value"), QString(":nv"));
    case SpecialFilterKeyword::DefaultValue:
        return qMakePair(QString("Default Value"), QString(":dv"));
    case SpecialFilterKeyword::TimeSamples:
        return qMakePair(QString("Time Samples"), QString(":ts"));
    case SpecialFilterKeyword::ModifiedValue:
        return qMakePair(QString("Modified Value"), QString(":mv"));
    default:
        return qMakePair(QString(""), QString(""));
    }
}

} // namespace TINKERUSD_NS
