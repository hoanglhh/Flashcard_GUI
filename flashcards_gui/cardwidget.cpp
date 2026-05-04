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

#include <QMessageBox>
#include <QFrame>

// ── Constructor / Destructor ─────────────────────────────────────────────────

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
    // Style the title label
    titleLabel_->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel_->font();
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);

    // Result label (shown after Study check or Flip reveal)
    resultLabel_->setAlignment(Qt::AlignCenter);
    resultLabel_->setWordWrap(true);
    resultLabel_->hide();

    // Build main layout: title → rows → result → button
    mainLayout_->addWidget(titleLabel_);
    mainLayout_->addLayout(rowsLayout_);
    mainLayout_->addWidget(resultLabel_);
    mainLayout_->addStretch();
    mainLayout_->addWidget(actionBtn_);

    connect(actionBtn_, &QPushButton::clicked,
            this, &CardWidget::onActionButton);

    // Start in a neutral state
    titleLabel_->setText("Select a deck to add a card");
    actionBtn_->setEnabled(false);
}

CardWidget::~CardWidget() {}

// ── Public setup methods ─────────────────────────────────────────────────────

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

void CardWidget::setupForFlip(shared_ptr<Card> card, const Fields& fields)
{
    mode_ = CardMode::FlipMode;
    fieldNames_ = fields;
    currentCard_ = card;
    flipRevealIndex_ = 1; // first field already shown

    clearRows();
    resultLabel_->hide();

    titleLabel_->setText("Flip Card");
    actionBtn_->setText("Flip ▶");
    actionBtn_->setEnabled(true);

    // Retrieve all definitions
    Fields defs;
    card->get_definitions(fields, defs);

    for ( int i = 0 ; i < static_cast<int>(fields.size()) ; ++i )
    {
        QString label = QString::fromStdString(fields.at(
                            static_cast<Fields::size_type>(i)));
        addRow(label, true);

        QString value = (i < static_cast<int>(defs.size()))
                            ? QString::fromStdString(
                                  defs.at(static_cast<Fields::size_type>(i)))
                            : "";
        fieldEdits_.at(i)->setText(value);

        // Hide all fields except the first
        if ( i > 0 )
        {
            fieldRows_.at(i)->hide();
        }
    }

    // If only one field exists, nothing to flip
    if ( fields.size() <= 1 )
    {
        actionBtn_->setEnabled(false);
    }
}

void CardWidget::setupForStudy(shared_ptr<Card> card, const Fields& fields)
{
    mode_ = CardMode::StudyMode;
    fieldNames_ = fields;
    currentCard_ = card;
    flipRevealIndex_ = 0;

    clearRows();
    resultLabel_->hide();

    titleLabel_->setText("Study Card");
    actionBtn_->setText("Check Answers");
    actionBtn_->setEnabled(true);

    // Retrieve all definitions for the prompt field
    Fields defs;
    card->get_definitions(fields, defs);

    for ( int i = 0 ; i < static_cast<int>(fields.size()) ; ++i )
    {
        QString label = QString::fromStdString(
            fields.at(static_cast<Fields::size_type>(i)));

        if ( i == 0 )
        {
            // Show first field as read-only prompt
            addRow(label, true);
            QString value = (!defs.empty())
                                ? QString::fromStdString(defs.at(0))
                                : "";
            fieldEdits_.at(0)->setText(value);
        }
        else
        {
            // Remaining fields are the user's answer inputs
            addRow(label, false);
            fieldEdits_.at(i)->setPlaceholderText("Your answer…");
        }
    }
}

// ── Private slot ─────────────────────────────────────────────────────────────

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
    else if ( mode_ == CardMode::FlipMode )
    {
        // Reveal the next hidden row
        int total = fieldRows_.size();
        if ( flipRevealIndex_ < total )
        {
            fieldRows_.at(flipRevealIndex_)->show();
            ++flipRevealIndex_;
        }

        if ( flipRevealIndex_ >= fieldRows_.size() )
        {
            actionBtn_->setText("All Revealed");
            actionBtn_->setEnabled(false);
        }
    }
    else if ( mode_ == CardMode::StudyMode )
    {
        if ( !currentCard_ || fieldNames_.empty() )
        {
            return;
        }

        // First field is the prompt; collect answers from the rest
        Fields answerFields(fieldNames_.begin() + 1, fieldNames_.end());
        Fields answers;
        for ( int i = 1 ; i < fieldEdits_.size() ; ++i )
        {
            answers.push_back(fieldEdits_.at(i)->text()
                                  .trimmed()
                                  .toStdString());
        }

        double score = currentCard_->check_answers(answerFields, answers);
        int pct = static_cast<int>(score * 100.0);

        QString msg = QString("Score: %1%").arg(pct);
        if ( pct == 100 )
        {
            msg += "  ✓ Perfect!";
            resultLabel_->setStyleSheet("color: green; font-weight: bold;");
        }
        else if ( pct >= 50 )
        {
            resultLabel_->setStyleSheet("color: orange; font-weight: bold;");
        }
        else
        {
            resultLabel_->setStyleSheet("color: red; font-weight: bold;");
        }

        // Show correct answers after checking
        Fields defs;
        currentCard_->get_definitions(fieldNames_, defs);
        QString details = "";
        for ( int i = 1 ; i < static_cast<int>(fieldNames_.size()) ; ++i )
        {
            QString field = QString::fromStdString(
                fieldNames_.at(static_cast<Fields::size_type>(i)));
            QString correct = (i < static_cast<int>(defs.size()))
                                  ? QString::fromStdString(
                                        defs.at(
                                            static_cast<Fields::size_type>(i)))
                                  : "?";
            details += QString("\n%1: %2").arg(field, correct);
        }

        resultLabel_->setText(msg + "\nCorrect answers:" + details);
        resultLabel_->show();
    }
}

// ── Private helpers ──────────────────────────────────────────────────────────

void CardWidget::clearRows()
{
    // Delete each row container; Qt cleans up its label and edit children
    for ( QWidget* row : fieldRows_ )
    {
        rowsLayout_->removeWidget(row);
        delete row;
    }
    fieldRows_.clear();
    fieldLabels_.clear();
    fieldEdits_.clear();
}

void CardWidget::addRow(const QString& fieldName,
                        bool readOnly)
{
    // Each row is a container widget so clearRows() can delete it safely,
    // which automatically destroys the child label and edit too.
    QWidget*    row   = new QWidget(this);
    QHBoxLayout* lay  = new QHBoxLayout(row);
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
