#pragma once

#include "specialFilterKeyword.h"

#include <QtCore/QSortFilterProxyModel>

namespace TINKERUSD_NS
{

/*
 * @class PropertyProxy
 * @brief This class provides support for sorting and filtering 
 *        data passed between another model and a view.
 */
class PropertyProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    PropertyProxy(QObject* parent = nullptr);

    virtual ~PropertyProxy() = default;

public slots:
    void setSpecialFilter(SpecialFilterKeyword keyword);

private:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    SpecialFilterKeyword m_specialKeyword;
};

} // namespace TINKERUSD_NS