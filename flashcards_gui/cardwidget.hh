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

class Card;   // forward declaration

/**
 * @brief Modes that CardWidget can operate in.
 *
 * AddMode   – all fields are editable; the user fills them in and presses
 *             "Add Card".  The widget emits cardSubmitted with the values.
 *
 * FlipMode  – shows the first field's value; the user presses "Flip" to
 *             reveal the remaining fields one-by-one.
 *
 * StudyMode – the first field is shown as a prompt; the remaining fields
 *             are editable; pressing "Check" compares the answers and shows
 *             the score.
 */
enum class CardMode { AddMode, FlipMode, StudyMode };

/**
 * @brief A widget that renders a single flashcard in different modes.
 *
 * CardWidget is the custom card element required by the assignment.  It
 * inherits from QWidget and adapts its appearance and behaviour based on the
 * active CardMode.  The widget is scalable: it creates one row per field
 * regardless of how many fields a deck defines.
 */
class CardWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an empty CardWidget in AddMode.
     * @param parent Optional Qt parent.
     */
    explicit CardWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructs the CardWidget.
     */
    ~CardWidget();

    /**
     * @brief Prepares the widget to add a new card to the given deck.
     *
     * Switches to AddMode, clears all inputs, and creates one row for every
     * field in @p fields.
     *
     * @param fields Ordered list of field-type names (e.g. {"EN","FI"}).
     */
    void setupForAdd(const Fields& fields);

    /**
     * @brief Loads an existing card for flipping (FlipMode).
     *
     * Shows the first field's value and hides the rest.  The user can press
     * "Flip" to reveal them one by one.
     *
     * @param card   Shared pointer to the card to display.
     * @param fields Ordered field names matching the card.
     */
    void setupForFlip(std::shared_ptr<Card> card, const Fields& fields);

    /**
     * @brief Loads an existing card for studying (StudyMode).
     *
     * Shows the first field as a read-only prompt; all other fields are
     * editable for the user's answers.
     *
     * @param card   Shared pointer to the card to study.
     * @param fields Ordered field names matching the card.
     */
    void setupForStudy(std::shared_ptr<Card> card, const Fields& fields);

signals:
    /**
     * @brief Emitted when the user submits a new card in AddMode.
     * @param fieldNames  Ordered list of field-type identifiers.
     * @param definitions Ordered list of definition strings.
     */
    void cardSubmitted(const Fields& fieldNames, const Fields& definitions);

private slots:
    /**
     * @brief Handles the primary action button press.
     *
     * In AddMode   → validates inputs and emits cardSubmitted.
     * In FlipMode  → reveals the next hidden field.
     * In StudyMode → checks answers and shows a score label.
     */
    void onActionButton();

private:
    // ── Layout helpers ───────────────────────────────────────────────────────
    /**
     * @brief Removes all dynamically-created field rows from the layout.
     */
    void clearRows();

    /**
     * @brief Creates a label-input row for a single field.
     * @param fieldName  Human-readable field label.
     * @param readOnly   If true, the QLineEdit is not editable.
     */
    void addRow(const QString& fieldName,
                bool readOnly = false);

    // ── State ────────────────────────────────────────────────────────────────
    CardMode mode_;
    Fields   fieldNames_;
    std::shared_ptr<Card> currentCard_;
    int      flipRevealIndex_;   ///< Next field index to reveal in FlipMode

    // ── Widget containers ────────────────────────────────────────────────────
    QVBoxLayout*          mainLayout_;
    QLabel*               titleLabel_;
    QLabel*               resultLabel_;
    QVBoxLayout*          rowsLayout_;   ///< Holds the dynamic field rows
    QPushButton*          actionBtn_;

    // ── Per-field widgets (parallel arrays, one entry per field) ─────────────
    QVector<QWidget*>    fieldRows_;     ///< Container widget for each row
    QVector<QLabel*>     fieldLabels_;
    QVector<QLineEdit*>  fieldEdits_;
};

#endif // CARDWIDGET_HH
