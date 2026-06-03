#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

static const int EMPTY = -1;
static const int BLOCKED = -2;
static const int ROWS = 8 ;
static const int COLS = 7 ;

// Custom types for piece representation
typedef std::vector<std::vector<int>> Piece;
typedef std::vector<Piece> Rotations;

// Function prototypes

// Piece generation and manipulation
std::vector<Rotations> getPieces();
Rotations getUniqueRotations(const Piece& piece);
Piece createRotation(const Piece& piece);
Piece createFlip(const Piece& piece);
bool isEqual(const Piece& a, const Piece& b);

// Board solving and validation
bool solveBoard(std::vector<int>& board, const std::vector<Rotations>& pieceRotations, int pieceToPlace, std::vector<int>& solution);
int getSmallestGroup(const std::vector<int>& board);
int floodfill(std::vector<char>& visited, const std::vector<int>& board, int p);

// Utility functions
int indexOf(const std::vector<int>& v, int value);
void printInitialBoard();
void displayBoardSFML(sf::RenderWindow& window, const std::vector<int>& board, int monthPos, int dayPos, int weekdayPos,
    int userMonth, int userDay, int userWeekday, bool solved, sf::Font& font);


