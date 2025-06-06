#include "searchBar.h"

#include "customWidgets/smallBoxWidget.h"
#include "modelView/propertyModel.h"
#include "modelView/propertyProxy.h"
#include "modelView/propertyTreeView.h"

#include <QtGui/QAction>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

SearchBar::SearchBar(PropertyTreeView* treeView, QWidget* parent)
    : QWidget(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_legendHelpButton(new QToolButton(this))
    , m_treeView(treeView)
{
    m_lineEdit->setPlaceholderText("Search...");

    m_legendHelpButton->setToolTip("Show Legend");
    m_legendHelpButton->setIcon(QIcon(":/help_small.png"));
    m_legendHelpButton->setCheckable(true);
    m_legendHelpButton->setChecked(false);

    // add search and clear actions
    m_searchAction = m_lineEdit->addAction(QIcon(":/search.png"), QLineEdit::LeadingPosition);
    m_clearAction = m_lineEdit->addAction(QIcon(":/clear.png"), QLineEdit::LeadingPosition);
    m_clearAction->setVisible(false);

    // layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(m_lineEdit);
    searchLayout->addWidget(m_legendHelpButton);
    searchLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addLayout(searchLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    // connections
    connect(m_lineEdit, &QLineEdit::textChanged, this, &SearchBar::onTextChanged);
    connect(m_clearAction, &QAction::triggered, this, &SearchBar::onClearClicked);
    connect(m_legendHelpButton, &QToolButton::toggled, this, &SearchBar::onCheckboxToggled);
    connect(
        this,
        &SearchBar::specialFilterChanged,
        m_treeView->getProxyModel(),
        &PropertyProxy::setSpecialFilter);
}

void SearchBar::onTextChanged()
{
    QString searchText = m_lineEdit->text();
    m_treeView->getProxyModel()->setFilterRegularExpression(QRegularExpression(
        QRegularExpression::escape(searchText), QRegularExpression::CaseInsensitiveOption));

    auto modifiedValueKeyWord = toString(SpecialFilterKeyword::ModifiedValue).second;
    auto defaultValueKeyWord = toString(SpecialFilterKeyword::DefaultValue).second;

    if (searchText.contains(modifiedValueKeyWord, Qt::CaseInsensitive))
    {
        emit specialFilterChanged(SpecialFilterKeyword::ModifiedValue);
    }
    else if (searchText.contains(defaultValueKeyWord, Qt::CaseInsensitive))
    {
        emit specialFilterChanged(SpecialFilterKeyword::DefaultValue);
    }
    else
    {
        emit specialFilterChanged(SpecialFilterKeyword::None);
    }

    m_clearAction->setVisible(!searchText.isEmpty());
    m_searchAction->setVisible(searchText.isEmpty());

    m_treeView->reapplyPersistentEditors();

    emit textChanged(searchText);
}

void SearchBar::onClearClicked() { m_lineEdit->clear(); }

void SearchBar::onCheckboxToggled(bool checked)
{
    if (checked)
    {
        QHBoxLayout* legendLayout = new QHBoxLayout();
        legendLayout->setSpacing(4);
        legendLayout->setContentsMargins(0, 0, 0, 0);

        QMap<SpecialFilterKeyword, QColor> specialFilters
            = { { SpecialFilterKeyword::NoValue, QColor(0, 0, 255) },
                { SpecialFilterKeyword::DefaultValue, QColor(64, 64, 64) },
                { SpecialFilterKeyword::TimeSamples, QColor(255, 255, 0) },
                { SpecialFilterKeyword::ModifiedValue, QColor(144, 238, 144) } };

        for (auto specialFilter : specialFilters.keys())
        {
            legendLayout->addWidget(
                new SmallBoxWidget(QSize(10, 10), specialFilters[specialFilter], this));
            legendLayout->addWidget(new QLabel(toString(specialFilter).first, this));
        }

        QWidget* legendWidget = new QWidget(this);
        legendWidget->setObjectName("LegendWidget");
        legendWidget->setLayout(legendLayout);
        layout()->addWidget(legendWidget);
    }
    else
    {
        QLayoutItem* item;
        while ((item = layout()->takeAt(1)) != nullptr)
        {
            if (item->widget() && item->widget()->objectName() == "LegendWidget")
            {
                delete item->widget();
            }
            delete item;
        }
    }
}
} // namespace TINKERUSD_NS
