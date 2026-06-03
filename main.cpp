#include "CalenderPuzzle.h"
#include <iostream>

using namespace std;

int main() {
    // positions outside the board
    const vector<int> illegal = { 6, 13, 49, 50, 51, 52 };

    // Pointers to a date/day on the board (these are board indices):
    vector<int> months = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12 }; // months 1..12 map to these indices
    vector<int> days;
    for (int i = 14; i < 45; ++i) days.push_back(i); // days 1..31 map to 14..44
    vector<int> weekdays = { 45, 46, 47, 48, 53, 54, 55 }; // weekdays 1..7 map to these indices

    //printing initial board and shapes
    printInitialBoard();

    //user input
    int userMonth = 0, userDay = 0, userWeekday = 0;
    cout << "Enter month (1-12): ";
    if (!(cin >> userMonth) || userMonth < 1 || userMonth > 12) {
        cout << "Invalid month. Must be 1-12.\n";
        return 1;
    }
    cout << "Enter day (1-31): ";
    if (!(cin >> userDay) || userDay < 1 || userDay > 31) {
        cout << "Invalid day. Must be 1-31.\n";
        return 1;
    }
    cout << "Enter weekday (1-7) (1:Sun, 2:Mon, ... 7:Sat) : ";
    if (!(cin >> userWeekday) || userWeekday < 1 || userWeekday > 7) {
        cout << "Invalid weekday. Must be 1-7.\n";
        return 1;
    }

    // Mapping user inputs to board positions:
    int monthPos = months[userMonth - 1];
    int dayPos = days[userDay - 1];
    int weekdayPos = weekdays[userWeekday - 1];

    vector<Rotations> pieceRotations = getPieces();

    vector<int> filledBoard(7 * 8, EMPTY);
    for (int j = 0; j < illegal.size(); ++j) {
        filledBoard[illegal[j]] = BLOCKED;
    }

    // Set the chosen date/day as blocked positions so that we dont cover it with any piece
    filledBoard[monthPos] = BLOCKED;
    filledBoard[dayPos] = BLOCKED;
    filledBoard[weekdayPos] = BLOCKED;

    vector<int> solution;
    cout << "\nSolving for: " << userDay << "/" << userMonth << " [weekday " << userWeekday << "]..." << endl;
    bool found = solveBoard(filledBoard, pieceRotations, 0, solution);

    if (found) {
        cout << "Solution found! Opening graphical display..." << endl;
    }
    else {
        cout << "No solution found. Showing board with the chosen date markers..." << endl;
    }

    // FONT LOADING - Try to load a font
    sf::Font font;
    bool fontLoaded = false;

    // Try Windows system fonts in order
    vector<string> fontPaths = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/consola.ttf"
    };

    for (const string& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            cout << "Font loaded successfully from: " << path << endl;
            break;
        }
    }

    if (!fontLoaded) {
        cerr << "WARNING: Could not load font. Text may not display properly." << endl;
        cerr << "Make sure Windows fonts are available at C:/Windows/Fonts/" << endl;
    }

    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(900, 1000), "Calendar Puzzle Solver");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        window.clear(sf::Color(240, 240, 240));

        if (found) {
            displayBoardSFML(window, solution, monthPos, dayPos, weekdayPos, userMonth, userDay, userWeekday, true, font);
        }
        else {
            displayBoardSFML(window, filledBoard, monthPos, dayPos, weekdayPos, userMonth, userDay, userWeekday, false, font);
        }

        window.display();
    }

    return 0;
}