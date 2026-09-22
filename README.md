# Flashcards GUI

A desktop flashcard application developed in **C++ using Qt** as a course project at Tampere University.

The application provides a graphical interface for creating and managing flashcard decks and studying cards. The project focused on GUI development, object-oriented programming, inheritance, and separating user-interface code from application logic.

## Technologies

* C++
* Qt
* Qt Widgets
* STL
* Git

## Features

* View all flashcard decks
* View cards belonging to a selected deck
* Create new decks with customizable fields
* Add new flashcards
* Remove decks and individual cards
* Import decks and cards from files
* Interactive flashcard functionality
* Scrollable interface supporting decks with multiple cards and fields

## GUI Development

I implemented the graphical user interface using Qt and connected it to the existing flashcard application logic.

The GUI includes a `MainWindow` responsible for managing the main application interface and interactions between the user and the underlying flashcard data.

I also implemented a custom card component derived from a Qt widget, allowing flashcards to be displayed and interacted with dynamically.

## Application Structure

The application separates the graphical interface from the underlying program logic.

The main application components include:

* `MainWindow` - manages the main graphical interface
* `DeckManager` - manages decks and coordinates operations
* `Deck` - stores and manages collections of flashcards
* `Card` - represents individual flashcards
* Custom Qt card widget - provides the graphical representation and interaction for individual cards

This separation allowed the GUI to interact with the application through existing class interfaces rather than placing application logic directly inside the user interface.

## Flashcard Functionality

Decks may contain an arbitrary number of fields, allowing flashcards to represent information such as:

```text
English | Finnish | Spanish
one     | yksi    | uno
```

The interface dynamically handles cards according to the fields defined by their deck.

### Interactive Card Feature

[Replace this section with what you actually implemented.]

For example:

**If you implemented card flipping:**

The custom card widget can hide and reveal selected card fields, allowing the user to use the application like a traditional flashcard system.

**If you implemented studying:**

The application provides an interactive study mode where the user is shown selected card fields and can enter answers for the remaining fields.

**If you implemented editing:**

The custom card widget allows existing card definitions to be edited directly through the graphical interface.

## File Import

Flashcard decks can also be loaded from text files.

A file defines:

1. the deck name
2. the fields used by the deck
3. the cards belonging to the deck

Example:

```text
Numbers;3
EN;DE;ES
one;eins;uno
two;zwei;dos
three;drei;tres
```

The imported information is converted into decks and cards and displayed through the graphical interface.

## What I Learned

This project gave me practical experience with:

* Building desktop graphical interfaces with Qt
* Event-driven programming
* Qt signals and slots
* Object-oriented programming and inheritance
* Designing reusable GUI components
* Separating presentation and application logic
* Working with an existing codebase and predefined class interfaces
* Managing dynamically generated GUI elements
* C++ memory management

## Running the Application

The project can be opened and built using Qt Creator.

### Requirements

* C++ compiler
* Qt
* qmake / Qt Creator

Open the project `.pro` file in Qt Creator, configure the project, and build and run the application.

## Academic Context

This project was developed as part of **Programming 3: Interfaces** at Tampere University.

The course provided parts of the application structure and requirements. My work focused on implementing and integrating the graphical user interface and required flashcard functionality.
