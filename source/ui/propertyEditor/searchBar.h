#pragma once

#include "specialFilterKeyword.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>

namespace TINKERUSD_NS
{

class PropertyTreeView;

/*
 * @class SearchBar
 * @brief This widget is used for searching and filtering items in a PropertyTreeView.
*/
class SearchBar : public QWidget
{
    Q_OBJECT
public:
    SearchBar(PropertyTreeView* treeView, QWidget* parent);
    virtual ~SearchBar() = default;

signals:
    /*
    * signal emitted when the text in the search bar changes.
    */
    void textChanged(const QString &text);

    /*
    * signal emitted when a special filter keyword is detected in the search bar.
    */
    void specialFilterChanged(SpecialFilterKeyword keyword);

private slots:
    void onTextChanged();
    void onClearClicked();
    void onHelpLegendToggled(bool checked);

private:
    QLineEdit*          m_lineEdit;
    QToolButton*        m_legendHelpButton;
    QAction*            m_clearAction;
    QAction*            m_searchAction;
    PropertyTreeView*   m_treeView;
};

} // namespace TINKERUSD_NS
