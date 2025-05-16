#pragma once

#include "specialFilterKeyword.h"

#include <QSortFilterProxyModel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

namespace TINKERUSD_NS
{

class PropertyTreeView;

class SearchBar : public QWidget
{
    Q_OBJECT
public:
    SearchBar(PropertyTreeView* treeView, QWidget* parent);
    virtual ~SearchBar() = default;

signals:
    void textChanged(const QString& text);
    void specialFilterChanged(SpecialFilterKeyword keyword);

private slots:
    void onTextChanged();
    void onClearClicked();
    void onCheckboxToggled(bool checked);

private:
    QLineEdit*        m_lineEdit;
    QToolButton*      m_legendHelpButton;
    QAction*          m_clearAction;
    QAction*          m_searchAction;
    PropertyTreeView* m_treeView;
};

} // namespace TINKERUSD_NS
