#include "CalenderPuzzle.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

int indexOf(const vector<int>& v, int value) {
    for (int i = 0; i < static_cast<int>(v.size()); ++i) {
        if (v[i] == value) return i;
    }
    return -1;
}

vector<Rotations> getPieces() {
    vector<Rotations> pieceRotations(10);
    pieceRotations[0] = getUniqueRotations(Piece{ {1, 1, 1, 0}, {0, 0, 1, 1} });                //S shape (long)
    pieceRotations[1] = getUniqueRotations(Piece{ {1, 1, 0}, {0, 1, 0}, {0, 1, 1} });           //Z shape
    pieceRotations[2] = getUniqueRotations(Piece{ {1, 1}, {1, 1}, {0, 1} });                    //P shape
    pieceRotations[3] = getUniqueRotations(Piece{ {1, 1}, {1, 0}, {1, 1} });                    //U shape 
    pieceRotations[4] = getUniqueRotations(Piece{ {0, 1}, {0, 1}, {1, 1} });                    //L shape (small)
    pieceRotations[5] = getUniqueRotations(Piece{ {0, 1, 1}, {1, 1, 0} });                      //S shape (small)
    pieceRotations[6] = getUniqueRotations(Piece{ {1, 0, 0, 0}, {1, 1, 1, 1} });                //L shape (long)
    pieceRotations[7] = getUniqueRotations(Piece{ {1, 0, 0}, {1, 0, 0}, {1, 1, 1} });           //V shape
    pieceRotations[8] = getUniqueRotations(Piece{ {0, 0, 1}, {1, 1, 1}, {0, 0, 1} });           //T shape
    pieceRotations[9] = getUniqueRotations(Piece{ {1, 1, 1, 1} });                              //I shape
    return pieceRotations;
}

Rotations getUniqueRotations(const Piece& piece) {
    vector<Piece> rotations;
    rotations.push_back(piece);
    // Add the three rotations:
    for (int i = 0; i < 3; ++i) {
        rotations.push_back(createRotation(rotations.back()));
    }
    // Add flips of the first four rotations:
    for (int i = 0; i < 4; ++i) {
        rotations.push_back(createFlip(rotations[i]));
    }

    // Removing duplicates:
    Rotations uniqueRotations;
    for (const Piece& p : rotations) {
        bool isUnique = true;
        for (const Piece& u : uniqueRotations) {
            if (isEqual(p, u)) {
                isUnique = false;
                break;
            }
        }
        if (isUnique) uniqueRotations.push_back(p);
    }
    return uniqueRotations;
}

Piece createRotation(const Piece& piece) {
    // rotate 90 degrees clockwise
    int h = static_cast<int>(piece.size());
    int w = static_cast<int>(piece[0].size());
    Piece rotation(w, vector<int>(h, 0));
    for (int x = 0; x < h; x++) {
        for (int y = 0; y < w; y++) {
            rotation[y][h - 1 - x] = piece[x][y];
        }
    }
    return rotation;
}

Piece createFlip(const Piece& piece) {
    int h = static_cast<int>(piece.size());
    int w = static_cast<int>(piece[0].size());
    Piece flipped(h, vector<int>(w, 0));
    for (int x = 0; x < h; ++x) {
        for (int y = 0; y < w; ++y) {
            flipped[h - 1 - x][y] = piece[x][y];
        }
    }
    return flipped;
}

bool isEqual(const Piece& a, const Piece& b) {
    if (a.size() != b.size()) return false;
    if (a[0].size() != b[0].size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

bool solveBoard(vector<int>& board, const vector<Rotations>& pieceRotations, int pieceToPlace, vector<int>& solution) {
    if (pieceToPlace == 10) {
        solution = board;
        return true;
    }

    int rows = ROWS ;
    int cols = COLS ;
    for (const Piece& rotation : pieceRotations[pieceToPlace]) {
        int rH = static_cast<int>(rotation.size());
        int rW = static_cast<int>(rotation[0].size());
        for (int position = 0; position < rows * cols; ++position) {
            int ox = position % cols;
            int oy = position / cols;

            // skip if we go outside the board
            if (ox + rW > cols) continue;
            if (oy + rH > rows) continue;

            vector<int> boardPositionsOfPiece;
            bool stillFits = true;
            for (int y = 0; stillFits && y < rH; ++y) {
                for (int x = 0; stillFits && x < rW; ++x) {
                    if (rotation[y][x] == 1) {
                        int pos = position + y * cols + x;
                        if (board[pos] == EMPTY) {
                            boardPositionsOfPiece.push_back(pos);
                        }
                        else {
                            stillFits = false;
                        }
                    }
                }
            }

            if (stillFits) {
                // Make the moves
                for (int i : boardPositionsOfPiece) board[i] = pieceToPlace;
                if (pieceToPlace >= 8 || getSmallestGroup(board) > 4) {
                    if (solveBoard(board, pieceRotations, pieceToPlace + 1, solution)) {
                        return true;
                    }
                }
                // Undo the moves
                for (int i : boardPositionsOfPiece) board[i] = EMPTY;
            }
        }
    }
    return false;
}

// Checks all empty regions on the board and returns the size of the smallest one
int getSmallestGroup(const vector<int>& board) {
    int total = static_cast<int>(board.size());
    vector<char> filledIn(total, 0);
    int smallestGroup = total;
    for (int i = 0; i < total; i++) {
        if (!filledIn[i] && board[i] == EMPTY) {
            int groupSize = floodfill(filledIn, board, i);
            if (groupSize < smallestGroup)
                smallestGroup = groupSize;
        }
    }
    return smallestGroup;
}

// Marks all connected EMPTY cells and returns the size of that connected group
int floodfill(vector<char>& visited, const vector<int>& board, int p) {
    int rows = ROWS;
    int cols = COLS;
    int total = static_cast<int>(board.size());
    if (p < 0 || p >= total) return 0;
    if (visited[p] || board[p] != EMPTY) return 0;
    int addedToGroup = 1;
    visited[p] = 1;
    int up = p - cols;
    int down = p + cols;
    int left = p - 1;
    int right = p + 1;
    if (up >= 0) addedToGroup += floodfill(visited, board, up);
    if (down < total) addedToGroup += floodfill(visited, board, down);
    if ((p % cols) > 0) addedToGroup += floodfill(visited, board, left);
    if ((p % cols) < cols - 1) addedToGroup += floodfill(visited, board, right);
    return addedToGroup;
}

//print the initial unsolved board so that the user can see what the puzzle looks like
void printInitialBoard() {               
    cout << "WELCOME TO THE CALENDAR PUZZLE SOLVER\n";
    cout << "Calendar puzzle grid:\n";

    const int rows = ROWS;
    const int cols = COLS;
    const int cellWidth = 5;

    const string horiz = "*" + string(cellWidth * cols + cols - 1, '-') + "*";

    const string grid[8][7] = {
        {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "X"},
        {"Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "X"},
        {"1", "2", "3", "4", "5", "6", "7"},
        {"8", "9", "10", "11", "12", "13", "14"},
        {"15", "16", "17", "18", "19", "20", "21"},
        {"22", "23", "24", "25", "26", "27", "28"},
        {"29", "30", "31", "Sun", "Mon", "Tue", "Wed"},
        {"X", "X", "X", "X", "Thu", "Fri", "Sat"}
    };

    cout << horiz << "\n";
    for (int y = 0; y < rows; y++) {
        cout << "|";
        for (int x = 0; x < cols; x++) {
            string cell = grid[y][x];
            int padding = cellWidth - cell.size();
            int left = padding / 2;
            int right = padding - left;
            cout << string(left, ' ') << cell << string(right, ' ') << "|";
        }
        cout << "\n" << horiz << "\n";
    }

    cout << "\n";
}

void displayBoardSFML(sf::RenderWindow& window, const vector<int>& board, int monthPos, int dayPos, int weekdayPos,
    int userMonth, int userDay, int userWeekday, bool solved, sf::Font& font) {
    const string monthNames[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
    const string weekdayNames[7] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };

    // Define colors for each piece
    const vector<sf::Color> pieceColors = {
        sf::Color(220, 50, 50),      // 0 - red
        sf::Color(50, 200, 50),      // 1 - green
        sf::Color(255, 200, 50),     // 2 - yellow
        sf::Color(50, 100, 220),     // 3 - blue
        sf::Color(200, 50, 200),     // 4 - magenta
        sf::Color(50, 200, 200),     // 5 - cyan
        sf::Color(255, 100, 100),    // 6 - bright red
        sf::Color(255, 100, 255),    // 7 - bright magenta
        sf::Color(255, 230, 100),    // 8 - light yellow
        sf::Color(180, 180, 180)     // 9 - gray
    };

    const sf::Color chosenColor(0, 200, 200);     // cyan for chosen date
    const sf::Color illegalColor(100, 100, 100);  // dark gray for illegal cells
    const sf::Color emptyColor(230, 230, 230);    // light gray
    const sf::Color borderColor(50, 50, 50);      // dark border

    int rows = ROWS, cols = COLS;
    float cellSize = 100.0f;
    float offsetX = 50.0f;
    float offsetY = 120.0f;
    float borderThickness = 3.0f;

    static const vector<int> monthPointers = { 0,1,2,3,4,5,7,8,9,10,11,12 };
    static const int dayStart = 14;
    static const vector<int> weekdayPointers = { 45,46,47,48,53,54,55 };

    // Draw title
    sf::Text title;
    title.setFont(font);
    title.setString("Calendar Puzzle Solution");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::Black);
    title.setPosition(offsetX, 20);
    window.draw(title);

    // Draw date info
    sf::Text dateInfo;
    dateInfo.setFont(font);
    string dateStr = "Solving for: " + monthNames[userMonth - 1] + " " + to_string(userDay) + ", " + weekdayNames[userWeekday - 1];
    dateInfo.setString(dateStr);
    dateInfo.setCharacterSize(24);
    dateInfo.setFillColor(sf::Color::Black);
    dateInfo.setPosition(offsetX, 70);
    window.draw(dateInfo);

    // Draw the board
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int idx = y * cols + x;
            float posX = offsetX + x * cellSize;
            float posY = offsetY + y * cellSize;

            // Create cell rectangle
            sf::RectangleShape cell(sf::Vector2f(cellSize - borderThickness, cellSize - borderThickness));
            cell.setPosition(posX, posY);
            cell.setOutlineThickness(borderThickness);
            cell.setOutlineColor(borderColor);

            // Determine cell color and text
            string cellText = "";
            sf::Color cellColor;
            bool isChosenCell = false;

            // Check if this is a chosen position
            if (idx == monthPos) {
                cellColor = sf::Color::White;
                cellText = monthNames[userMonth - 1];
                isChosenCell = true;
            }
            else if (idx == dayPos) {
                cellColor = sf::Color::White;
                cellText = to_string(userDay);
                isChosenCell = true;
            }
            else if (idx == weekdayPos) {
                cellColor = sf::Color::White;
                cellText = weekdayNames[userWeekday - 1];
                isChosenCell = true;
            }
            // Check if occupied by a piece
            else if (board[idx] >= 0 && board[idx] < 10) {
                cellColor = pieceColors[board[idx]];
                cellText = "";
            }
            // Empty cell - show label
            else if (board[idx] == EMPTY) {
                cellColor = emptyColor;
                if (find(monthPointers.begin(), monthPointers.end(), idx) != monthPointers.end()) {
                    int pos = static_cast<int>(distance(monthPointers.begin(), find(monthPointers.begin(), monthPointers.end(), idx)));
                    cellText = monthNames[pos];
                }
                else if (idx >= dayStart && idx <= 44) {
                    int dayNum = idx - dayStart + 1;
                    cellText = to_string(dayNum);
                }
                else if (find(weekdayPointers.begin(), weekdayPointers.end(), idx) != weekdayPointers.end()) {
                    int pos = static_cast<int>(distance(weekdayPointers.begin(), find(weekdayPointers.begin(), weekdayPointers.end(), idx)));
                    cellText = weekdayNames[pos];
                }
            }
            // Blocked/illegal cell
            else {
                cellColor = illegalColor;
                cellText = "X";
            }

            cell.setFillColor(cellColor);
            cell.setOutlineThickness(borderThickness);
            cell.setOutlineColor(borderColor);
            window.draw(cell);

            // Draw text if any
            if (!cellText.empty()) {
                sf::Text text;
                text.setFont(font);
                text.setString(cellText);
                text.setCharacterSize(cellText.length() > 2 ? 20 : 28);

                if (isChosenCell) {
                    text.setFillColor(sf::Color::Black);
                    text.setStyle(sf::Text::Bold);
                }
                else {
                    text.setFillColor(sf::Color::Black);
                }

                // Center the text in the cell
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setPosition(
                    posX + (cellSize - borderThickness - textBounds.width) / 2.0f - textBounds.left,
                    posY + (cellSize - borderThickness - textBounds.height) / 2.0f - textBounds.top
                );
                window.draw(text);
            }
        }
    }
}