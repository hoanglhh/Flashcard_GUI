/*
#############################################################################
# COMP.CS.115 Ohjelmointi 3: Rajapinnat / Programming 3: Interfaces         #
# Project: Opettelukorttien paluu / Return of Flashcards                    #
# File: cardwidget.hh                                                       #
# Description: Declares the CardWidget class.                               #
#                                                                           #
# Notes: * Inherits QWidget.                                                #
#        * Supports three modes: Add, Flip, Study.                          #
#        * Dynamically creates one row of QLabel + QLineEdit per field.     #
#        * Emits cardSubmitted when the user confirms a new card.           #
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

#ifndef CARDWIDGET_HH
#define CARDWIDGET_HH

#include "utils.hh"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <QString>
#include <memory>

class Card;

enum class CardMode { AddMode, FlipMode, StudyMode };

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardWidget(QWidget* parent = nullptr);
    ~CardWidget();

    void setupForAdd(const Fields& fields);

    void setupForFlip(std::shared_ptr<Card> card, const Fields& fields);

signals:
    void cardSubmitted(const Fields& fieldNames,
                       const Fields& definitions);

private slots:
    void onActionButton();

private:
    void clearRows();
    void addRow(const QString& fieldName, bool readOnly = false);

    CardMode mode_;
    Fields   fieldNames_;
    std::shared_ptr<Card> currentCard_;
    int      flipRevealIndex_;

    QVBoxLayout*         mainLayout_;
    QLabel*              titleLabel_;
    QLabel*              resultLabel_;
    QVBoxLayout*         rowsLayout_;
    QPushButton*         actionBtn_;

    QVector<QWidget*>    fieldRows_;
    QVector<QLabel*>     fieldLabels_;
    QVector<QLineEdit*>  fieldEdits_;
};

#endif // CARDWIDGET_HH
