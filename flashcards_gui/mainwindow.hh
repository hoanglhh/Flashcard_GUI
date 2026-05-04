/*
#############################################################################
# COMP.CS.115 Ohjelmointi 3: Rajapinnat / Programming 3: Interfaces         #
# Project: Opettelukorttien paluu / Return of Flashcards                    #
# File: mainwindow.hh                                                       #
# Description: Declares the MainWindow class – the primary GUI window.      #
#                                                                           #
# Notes: * Inherits QMainWindow.                                            #
#        * Manages DeckManager and orchestrates all user interactions.      #
#        * Provides deck/card listing, adding, removing, and card study.    #
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

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "deckmanager.hh"
#include "cardwidget.hh"

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QGroupBox>
#include <QStatusBar>

/**
 * @brief The MainWindow class is the top-level GUI window for the Flashcard
 *        application.
 *
 * It owns a DeckManager instance and exposes all deck/card operations
 * through a Qt-based interface.  The window is divided into three columns:
 *   1. Deck list  – add, select, remove decks and load from file.
 *   2. Card list  – shows cards belonging to the selected deck; remove card.
 *   3. Card panel – a CardWidget for adding cards or studying/flipping them.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the MainWindow and builds the entire UI.
     * @param parent Optional Qt parent widget.
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructs the MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Loads a deck from the file whose name is entered in the
     *        file-name input field.
     */
    void onLoadFile();

    /**
     * @brief Opens a small dialog to let the user create a new deck
     *        with a custom name and semicolon-separated field list.
     */
    void onAddDeck();

    /**
     * @brief Removes the deck that is currently selected in the deck list.
     */
    void onRemoveDeck();

    /**
     * @brief Populates the card list for the deck that was just selected.
     * @param item The list item that was clicked.
     */
    void onDeckSelected(QListWidgetItem* item);

    /**
     * @brief Removes the card that is currently selected in the card list.
     */
    void onRemoveCard();

    /**
     * @brief Highlights the card that was just selected in the card list
     *        and loads it into the CardWidget for study / flip.
     * @param item The list item that was clicked.
     */
    void onCardSelected(QListWidgetItem* item);

    /**
     * @brief Receives a new card submitted by the CardWidget and adds it
     *        to the currently selected deck.
     * @param fieldNames  Ordered list of field-type identifiers.
     * @param definitions Ordered list of definition strings.
     */
    void onCardAdded(const Fields& fieldNames, const Fields& definitions);

    /**
     * @brief Exits the application cleanly (without using exit()).
     */
    void onExit();

private:
    // ── Core logic ──────────────────────────────────────────────────────────
    DeckManager manager_;

    // ── UI helpers ───────────────────────────────────────────────────────────
    /**
     * @brief Builds and wires up all child widgets.  Called once from the
     *        constructor.
     */
    void buildUi();

    /**
     * @brief Refreshes the deck QListWidget from DeckManager.
     */
    void refreshDeckList();

    /**
     * @brief Populates the card QListWidget for the given deck.
     * @param deckName Name of the deck whose cards to display.
     */
    void refreshCardList(const QString& deckName);

    /**
     * @brief Returns the name of the currently selected deck, or "" if none.
     */
    QString selectedDeckName() const;

    /**
     * @brief Returns the card ID of the currently selected card list row,
     *        or 0 if none selected.
     */
    unsigned int selectedCardId() const;

    /**
     * @brief Shows a transient message in the status bar.
     * @param msg  Text to display.
     * @param isError  If true, text is shown in red.
     */
    void showStatus(const QString& msg, bool isError = false);

    // ── Deck panel ───────────────────────────────────────────────────────────
    QListWidget*  deckList_;
    QLineEdit*    fileInput_;
    QPushButton*  loadFileBtn_;
    QPushButton*  addDeckBtn_;
    QPushButton*  removeDeckBtn_;

    // ── Card panel ───────────────────────────────────────────────────────────
    QListWidget*  cardList_;
    QPushButton*  removeCardBtn_;

    // ── Card widget ──────────────────────────────────────────────────────────
    CardWidget*   cardWidget_;

    // ── Tracks which card ID each row in cardList_ corresponds to ───────────
    QVector<unsigned int> cardIds_;
};

#endif // MAINWINDOW_HH
