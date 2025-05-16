#pragma once

#include "specialFilterKeyword.h"

#include <QtCore/QSortFilterProxyModel>

namespace TINKERUSD_NS
{

class PropertyProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    PropertyProxy(QObject* parent = nullptr);

public slots:
    void setSpecialFilter(SpecialFilterKeyword keyword);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    SpecialFilterKeyword m_specialKeyword;
};

} // namespace TINKERUSD_NS