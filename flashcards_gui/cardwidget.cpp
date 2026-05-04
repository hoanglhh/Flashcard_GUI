/*
#############################################################################
# COMP.CS.115 Ohjelmointi 3: Rajapinnat / Programming 3: Interfaces         #
# Project: Opettelukorttien paluu / Return of Flashcards                    #
# File: cardwidget.cpp                                                      #
# Description: Implements CardWidget – the custom card element.             #
#                                                                           #
# Notes: * Three modes: AddMode, FlipMode, StudyMode.                       #
#        * Dynamically creates one row per deck field.                      #
#                                                                           #
# Author information:                                                       #
#   - Name:           Huy Hoang Le                                          #
#   - Student number: 153144716                                             #
#   - Gitlab user:    txf523                                                #
#   - Tuni email:     huyhoang.le@tuni.fi                                   #
#                                                                           #
#   - Name:           Phuong Thao Nguyen                                    #
#   - Student number: 152270131                                             #
#   - Gitlab user:    dbphng                                                #
#   - Tuni email:     phuong.4.nguyen@tuni.fi                               #
#############################################################################
*/

#include "cardwidget.hh"
#include "card.hh"

#include <QFrame>

CardWidget::CardWidget(QWidget* parent) :
    QWidget(parent),
    mode_(CardMode::AddMode),
    flipRevealIndex_(0),
    mainLayout_(new QVBoxLayout(this)),
    titleLabel_(new QLabel(this)),
    resultLabel_(new QLabel(this)),
    rowsLayout_(new QVBoxLayout()),
    actionBtn_(new QPushButton(this))
{
    titleLabel_->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel_->font();
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);

    resultLabel_->setAlignment(Qt::AlignCenter);
    resultLabel_->setWordWrap(true);
    resultLabel_->hide();

    mainLayout_->addWidget(titleLabel_);
    mainLayout_->addLayout(rowsLayout_);
    mainLayout_->addWidget(resultLabel_);
    mainLayout_->addStretch();
    mainLayout_->addWidget(actionBtn_);

    connect(actionBtn_, &QPushButton::clicked,
            this, &CardWidget::onActionButton);

    titleLabel_->setText("Select a deck to add a card");
    actionBtn_->setEnabled(false);
}

CardWidget::~CardWidget() {}

void CardWidget::setupForAdd(const Fields& fields)
{
    mode_ = CardMode::AddMode;
    fieldNames_ = fields;
    currentCard_ = nullptr;
    flipRevealIndex_ = 0;

    clearRows();
    resultLabel_->hide();

    titleLabel_->setText("Add New Card");
    actionBtn_->setText("Add Card");
    actionBtn_->setEnabled(true);

    for ( const string& f : fields )
    {
        addRow(QString::fromStdString(f), false);
    }
}

void CardWidget::onActionButton()
{
    if ( mode_ == CardMode::AddMode )
        {
            // Collect values from all editable fields
            Fields defs;
            for ( QLineEdit* edit : fieldEdits_ )
            {
                defs.push_back(edit->text().trimmed().toStdString());
            }

            // Basic validation: no empty fields
            for ( const string& d : defs )
            {
                if ( d.empty() )
                {
                    resultLabel_->setText("Please fill in all fields.");
                    resultLabel_->setStyleSheet("color: red;");
                    resultLabel_->show();
                    return;
                }
            }

            emit cardSubmitted(fieldNames_, defs);

            // Clear inputs after successful add
            for ( QLineEdit* edit : fieldEdits_ )
            {
                edit->clear();
            }
            resultLabel_->setText("Card added!");
            resultLabel_->setStyleSheet("color: green;");
            resultLabel_->show();
        }
}

void CardWidget::clearRows()
{
    for ( QWidget* row : fieldRows_ )
    {
        rowsLayout_->removeWidget(row);
        delete row;
    }
    fieldRows_.clear();
    fieldLabels_.clear();
    fieldEdits_.clear();
}

void CardWidget::addRow(const QString& fieldName, bool readOnly)
{
    QWidget*     row = new QWidget(this);
    QHBoxLayout* lay = new QHBoxLayout(row);
    lay->setContentsMargins(0, 0, 0, 0);

    QLabel*    label = new QLabel(fieldName + ":", row);
    label->setMinimumWidth(80);
    QLineEdit* edit  = new QLineEdit(row);
    edit->setReadOnly(readOnly);

    if ( readOnly )
    {
        edit->setStyleSheet("background: #f0f0f0; color: #555;");
    }

    lay->addWidget(label);
    lay->addWidget(edit);
    rowsLayout_->addWidget(row);

    fieldRows_.append(row);
    fieldLabels_.append(label);
    fieldEdits_.append(edit);
}
