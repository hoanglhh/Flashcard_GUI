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

// ── Constructor / Destructor ─────────────────────────────────────────────────

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

// ── UI construction ──────────────────────────────────────────────────────────

void MainWindow::buildUi()
{
    // ── Central splitter ──────────────────────────────────────────────────
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    // ── Column 1: Deck panel ──────────────────────────────────────────────
    QGroupBox* deckGroup = new QGroupBox("Decks");
    QVBoxLayout* deckLayout = new QVBoxLayout(deckGroup);

    // File loading row
    QLabel* fileLabel = new QLabel("File name:");
    fileInput_ = new QLineEdit();
    fileInput_->setPlaceholderText("e.g. deck.txt");
    loadFileBtn_ = new QPushButton("Load File");

    QHBoxLayout* fileRow = new QHBoxLayout();
    fileRow->addWidget(fileLabel);
    fileRow->addWidget(fileInput_);
    fileRow->addWidget(loadFileBtn_);

    // Deck list
    deckList_ = new QListWidget();
    deckList_->setSelectionMode(QAbstractItemView::SingleSelection);

    // Deck buttons
    addDeckBtn_    = new QPushButton("+ Add Deck");
    removeDeckBtn_ = new QPushButton("✕ Remove Deck");

    QHBoxLayout* deckBtnRow = new QHBoxLayout();
    deckBtnRow->addWidget(addDeckBtn_);
    deckBtnRow->addWidget(removeDeckBtn_);

    deckLayout->addLayout(fileRow);
    deckLayout->addWidget(deckList_);
    deckLayout->addLayout(deckBtnRow);

    // ── Column 2: Card list panel ─────────────────────────────────────────
    QGroupBox* cardGroup = new QGroupBox("Cards in Deck");
    QVBoxLayout* cardListLayout = new QVBoxLayout(cardGroup);

    cardList_ = new QListWidget();
    cardList_->setSelectionMode(QAbstractItemView::SingleSelection);

    removeCardBtn_ = new QPushButton("✕ Remove Card");

    // Mode-select buttons for card panel
    QPushButton* addCardModeBtn   = new QPushButton("Add Card");
    QPushButton* flipModeBtn      = new QPushButton("Flip Card");
    QPushButton* studyModeBtn     = new QPushButton("Study Card");

    QHBoxLayout* modeBtnRow = new QHBoxLayout();
    modeBtnRow->addWidget(addCardModeBtn);
    modeBtnRow->addWidget(flipModeBtn);
    modeBtnRow->addWidget(studyModeBtn);

    cardListLayout->addWidget(cardList_);
    cardListLayout->addWidget(removeCardBtn_);
    cardListLayout->addLayout(modeBtnRow);

    // ── Column 3: Card widget panel ───────────────────────────────────────
    QScrollArea* cardScroll = new QScrollArea();
    cardScroll->setWidgetResizable(true);

    cardWidget_ = new CardWidget();
    cardScroll->setWidget(cardWidget_);

    // ── Assemble splitter ─────────────────────────────────────────────────
    splitter->addWidget(deckGroup);
    splitter->addWidget(cardGroup);
    splitter->addWidget(cardScroll);
    splitter->setSizes({280, 280, 440});

    // ── Status bar ────────────────────────────────────────────────────────
    statusBar()->showMessage("Ready");

    // ── Exit button in toolbar-style footer ───────────────────────────────
    QPushButton* exitBtn = new QPushButton("Exit");
    exitBtn->setFixedWidth(80);
    statusBar()->addPermanentWidget(exitBtn);

    // ── Signal / slot connections ─────────────────────────────────────────
    connect(loadFileBtn_,  &QPushButton::clicked,
            this, &MainWindow::onLoadFile);

    connect(addDeckBtn_,   &QPushButton::clicked,
            this, &MainWindow::onAddDeck);

    connect(removeDeckBtn_,&QPushButton::clicked,
            this, &MainWindow::onRemoveDeck);

    connect(deckList_, &QListWidget::itemClicked,
            this, &MainWindow::onDeckSelected);

    connect(removeCardBtn_,&QPushButton::clicked,
            this, &MainWindow::onRemoveCard);

    connect(cardList_, &QListWidget::itemClicked,
            this, &MainWindow::onCardSelected);

    connect(cardWidget_, &CardWidget::cardSubmitted,
            this, &MainWindow::onCardAdded);

    connect(exitBtn, &QPushButton::clicked,
            this, &MainWindow::onExit);

    // Switch card-panel mode buttons
    connect(addCardModeBtn, &QPushButton::clicked, this, [this]()
    {
        QString deckName = selectedDeckName();
        if ( deckName.isEmpty() )
        {
            showStatus("Please select a deck first.", true);
            return;
        }
        auto deck = manager_.get_deck(deckName.toStdString());
        if ( deck )
        {
            cardWidget_->setupForAdd(*deck->get_fields());
        }
    });

    connect(flipModeBtn, &QPushButton::clicked, this, [this]()
    {
        QString deckName = selectedDeckName();
        unsigned int id  = selectedCardId();
        if ( deckName.isEmpty() || id == 0 )
        {
            showStatus("Please select a card first.", true);
            return;
        }
        auto card = manager_.get_deck(deckName.toStdString())->get_card(id);
        auto deck = manager_.get_deck(deckName.toStdString());
        if ( card && deck )
        {
            cardWidget_->setupForFlip(card, *deck->get_fields());
        }
    });

    connect(studyModeBtn, &QPushButton::clicked, this, [this]()
    {
        QString deckName = selectedDeckName();
        unsigned int id  = selectedCardId();
        if ( deckName.isEmpty() || id == 0 )
        {
            showStatus("Please select a card first.", true);
            return;
        }
        auto deck = manager_.get_deck(deckName.toStdString());
        auto card = deck ? deck->get_card(id) : nullptr;
        if ( card && deck )
        {
            cardWidget_->setupForStudy(card, *deck->get_fields());
        }
    });
}

// ── Slots ────────────────────────────────────────────────────────────────────

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

void MainWindow::onRemoveCard()
{
    QString deckName = selectedDeckName();
    unsigned int id  = selectedCardId();

    if ( deckName.isEmpty() || id == 0 )
    {
        showStatus("Select a card to remove.", true);
        return;
    }

    bool ok = manager_.remove_card(deckName.toStdString(), id);
    if ( ok )
    {
        showStatus("Card removed.");
        refreshCardList(deckName);
    }
    else
    {
        showStatus("Failed to remove card.", true);
    }
}

void MainWindow::onCardSelected(QListWidgetItem* item)
{
    if ( !item )
    {
        return;
    }
    showStatus("Card selected – choose Add / Flip / Study below the card "
               "list.");
}

void MainWindow::onCardAdded(const Fields& fieldNames,
                             const Fields& definitions)
{
    QString deckName = selectedDeckName();
    if ( deckName.isEmpty() )
    {
        showStatus("No deck selected.", true);
        return;
    }

    auto deck = manager_.get_deck(deckName.toStdString());
    if ( !deck )
    {
        return;
    }

    bool ok = deck->add_card(fieldNames, definitions);
    if ( ok )
    {
        showStatus("Card added to deck: " + deckName);
        refreshCardList(deckName);
    }
    else
    {
        showStatus("Failed to add card (field mismatch).", true);
    }
}

void MainWindow::onExit()
{
    close();
}

// ── Private helpers ──────────────────────────────────────────────────────────

void MainWindow::refreshDeckList()
{
    deckList_->clear();
    for ( const string& name : manager_.get_deck_names() )
    {
        deckList_->addItem(QString::fromStdString(name));
    }
}

void MainWindow::refreshCardList(const QString& deckName)
{
    cardList_->clear();
    cardIds_.clear();

    auto deck = manager_.get_deck(deckName.toStdString());
    if ( !deck )
    {
        return;
    }

    auto cards = deck->get_cards();
    Fields deckFields = *deck->get_fields();

    for ( auto& card : cards )
    {
        Fields defs;
        card->get_definitions(deckFields, defs);

        // Build a display string from the first two fields (or fewer)
        QString display = QString("#%1").arg(card->get_id());
        for ( int i = 0 ;
              i < static_cast<int>(deckFields.size()) && i < 2 ;
              ++i )
        {
            display += " | ";
            display += QString::fromStdString(
                defs.size() > static_cast<Fields::size_type>(i)
                    ? defs.at(static_cast<Fields::size_type>(i))
                    : "");
        }

        cardList_->addItem(display);
        cardIds_.append(card->get_id());
    }
}

QString MainWindow::selectedDeckName() const
{
    QListWidgetItem* item = deckList_->currentItem();
    return item ? item->text() : QString();
}

unsigned int MainWindow::selectedCardId() const
{
    int row = cardList_->currentRow();
    if ( row < 0 || row >= cardIds_.size() )
    {
        return 0;
    }
    return cardIds_.at(row);
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
