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

void MainWindow::onExit()
{
    close();
}
