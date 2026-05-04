/*
#############################################################################
# COMP.CS.115 Ohjelmointi 3: Rajapinnat / Programming 3: Interfaces         #
# Project: Opettelukorttien paluu / Return of Flashcards                    #
# File: mainwindow.cpp                                                      #
# Description: Implements the MainWindow class.                             #
#                                                                           #
# Notes: * Three-column layout: deck list | card list | card panel.         #
#        * Delegates core logic to DeckManager.                             #
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

#include "mainwindow.hh"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSplitter>
#include <QGroupBox>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    deckList_(nullptr),
    fileInput_(nullptr),
    loadFileBtn_(nullptr),
    addDeckBtn_(nullptr),
    removeDeckBtn_(nullptr),
    cardList_(nullptr),
    removeCardBtn_(nullptr),
    cardWidget_(nullptr)
{
    setWindowTitle("Flashcard Manager");
    resize(1000, 650);
    buildUi();
}

MainWindow::~MainWindow() {}

void MainWindow::buildUi()
{
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    // Column 1: Deck panel
    QGroupBox* deckGroup = new QGroupBox("Decks");
    QVBoxLayout* deckLayout = new QVBoxLayout(deckGroup);

    QLabel* fileLabel = new QLabel("File name:");
    fileInput_ = new QLineEdit();
    fileInput_->setPlaceholderText("e.g. deck.txt");
    loadFileBtn_ = new QPushButton("Load File");

    QHBoxLayout* fileRow = new QHBoxLayout();
    fileRow->addWidget(fileLabel);
    fileRow->addWidget(fileInput_);
    fileRow->addWidget(loadFileBtn_);

    deckList_ = new QListWidget();
    deckList_->setSelectionMode(QAbstractItemView::SingleSelection);

    addDeckBtn_    = new QPushButton("+ Add Deck");
    removeDeckBtn_ = new QPushButton("✕ Remove Deck");

    QHBoxLayout* deckBtnRow = new QHBoxLayout();
    deckBtnRow->addWidget(addDeckBtn_);
    deckBtnRow->addWidget(removeDeckBtn_);

    deckLayout->addLayout(fileRow);
    deckLayout->addWidget(deckList_);
    deckLayout->addLayout(deckBtnRow);

    // Column 2: Card list panel
    QGroupBox* cardGroup = new QGroupBox("Cards in Deck");
    QVBoxLayout* cardListLayout = new QVBoxLayout(cardGroup);

    cardList_ = new QListWidget();
    cardList_->setSelectionMode(QAbstractItemView::SingleSelection);

    removeCardBtn_ = new QPushButton("✕ Remove Card");

    QPushButton* addCardModeBtn = new QPushButton("Add Card");
    QPushButton* flipModeBtn    = new QPushButton("Flip Card");
    QPushButton* studyModeBtn   = new QPushButton("Study Card");

    QHBoxLayout* modeBtnRow = new QHBoxLayout();
    modeBtnRow->addWidget(addCardModeBtn);
    modeBtnRow->addWidget(flipModeBtn);
    modeBtnRow->addWidget(studyModeBtn);

    cardListLayout->addWidget(cardList_);
    cardListLayout->addWidget(removeCardBtn_);
    cardListLayout->addLayout(modeBtnRow);

    // Column 3: Card widget panel
    QScrollArea* cardScroll = new QScrollArea();
    cardScroll->setWidgetResizable(true);
    cardWidget_ = new CardWidget();
    cardScroll->setWidget(cardWidget_);

    splitter->addWidget(deckGroup);
    splitter->addWidget(cardGroup);
    splitter->addWidget(cardScroll);
    splitter->setSizes({280, 280, 440});

    statusBar()->showMessage("Ready");

    QPushButton* exitBtn = new QPushButton("Exit");
    exitBtn->setFixedWidth(80);
    statusBar()->addPermanentWidget(exitBtn);

    connect(exitBtn, &QPushButton::clicked,
            this, &MainWindow::onExit);
}

void MainWindow::onLoadFile()
{
    QString fileName = fileInput_->text().trimmed();
    if ( fileName.isEmpty() )
    {
        showStatus("Enter a file name first.", true);
        return;
    }

    bool ok = manager_.read_file(fileName.toStdString());
    if ( ok )
    {
        showStatus("File loaded: " + fileName);
        refreshDeckList();
    }
    else
    {
        showStatus("Failed to load file: " + fileName, true);
    }
}

void MainWindow::onAddDeck()
{
    // Small dialog: deck name + fields
    QDialog dlg(this);
    dlg.setWindowTitle("Add Deck");

    QFormLayout* form = new QFormLayout(&dlg);

    QLineEdit* nameEdit   = new QLineEdit();
    nameEdit->setPlaceholderText("e.g. Finnish Vocabulary");

    QLineEdit* fieldsEdit = new QLineEdit();
    fieldsEdit->setPlaceholderText("e.g. EN;FI  (semicolon-separated)");

    form->addRow("Deck name:", nameEdit);
    form->addRow("Fields:",    fieldsEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(btns);

    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if ( dlg.exec() != QDialog::Accepted )
    {
        return;
    }

    QString name   = nameEdit->text().trimmed();
    QString fStr   = fieldsEdit->text().trimmed();

    if ( name.isEmpty() || fStr.isEmpty() )
    {
        showStatus("Deck name and at least one field are required.", true);
        return;
    }

    // Parse semicolon-separated fields
    Fields fields;
    for ( const QString& f : fStr.split(';') )
    {
        QString trimmed = f.trimmed();
        if ( !trimmed.isEmpty() )
        {
            fields.push_back(trimmed.toStdString());
        }
    }

    auto deck = manager_.add_deck(name.toStdString(), fields);
    if ( deck )
    {
        showStatus("Deck added: " + name);
        refreshDeckList();
    }
    else
    {
        showStatus("A deck with that name already exists.", true);
    }
}

void MainWindow::onRemoveDeck()
{
    QString name = selectedDeckName();
    if ( name.isEmpty() )
    {
        showStatus("Select a deck to remove.", true);
        return;
    }

    QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Remove Deck",
                              "Remove deck \"" + name + "\"?",
                              QMessageBox::Yes | QMessageBox::No);
    if ( reply != QMessageBox::Yes )
    {
        return;
    }

    manager_.remove_deck(name.toStdString());
    showStatus("Deck removed: " + name);
    cardList_->clear();
    cardIds_.clear();
    refreshDeckList();
}

void MainWindow::onDeckSelected(QListWidgetItem* item)
{
    if ( !item )
    {
        return;
    }
    QString deckName = item->text();
    refreshCardList(deckName);

    // Switch card widget to Add mode for the selected deck
    auto deck = manager_.get_deck(deckName.toStdString());
    if ( deck )
    {
        cardWidget_->setupForAdd(*deck->get_fields());
    }
    showStatus("Deck selected: " + deckName);
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::refreshDeckList()
{
    deckList_->clear();
    for ( const string& name : manager_.get_deck_names() )
    {
        deckList_->addItem(QString::fromStdString(name));
    }
}

void MainWindow::showStatus(const QString& msg, bool isError)
{
    if ( isError )
    {
        statusBar()->setStyleSheet("color: red;");
    }
    else
    {
        statusBar()->setStyleSheet("");
    }
    statusBar()->showMessage(msg, 4000);
}
