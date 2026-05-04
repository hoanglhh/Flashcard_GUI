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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
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

    void onExit();

private:
    void buildUi();

    /**
     * @brief Refreshes the deck QListWidget from DeckManager.
     */
    void refreshDeckList();

    /**
     * @brief Shows a transient message in the status bar.
     * @param msg  Text to display.
     * @param isError  If true, text is shown in red.
     */
    void showStatus(const QString& msg, bool isError = false);

    DeckManager manager_;

    QListWidget*  deckList_;
    QLineEdit*    fileInput_;
    QPushButton*  loadFileBtn_;
    QPushButton*  addDeckBtn_;
    QPushButton*  removeDeckBtn_;

    QListWidget*  cardList_;
    QPushButton*  removeCardBtn_;

    CardWidget*   cardWidget_;

    QVector<unsigned int> cardIds_;
};

#endif // MAINWINDOW_HH
