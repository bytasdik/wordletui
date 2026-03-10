#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>

using namespace std;

// ANSI color codes
#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define BG_GREEN  "\033[42m\033[30m"
#define BG_YELLOW "\033[43m\033[30m"
#define BG_GRAY   "\033[100m\033[37m"
#define BG_DARK   "\033[40m\033[37m"
#define FG_GREEN  "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_GRAY   "\033[90m"
#define FG_WHITE  "\033[97m"

const vector<string> WORD_LIST = {
    "CRANE","SLATE","AUDIO","HOUSE","PLANT","SUGAR","TIGER","FLAME","BLOOD","STORM",
    "BRICK","LIGHT","NIGHT","CHESS","PIXEL","MOUSE","GLOBE","CROWN","DREAM","FROST",
    "GRIND","SHARK","BLAST","SWIFT","CRISP","FLUTE","GRAPE","KNEEL","MOURN","ORBIT",
    "PORCH","SCOUT","THUMP","VAPOR","WASTE","YACHT","BRIDE","CHOIR","DEPOT","ELBOW",
    "FEAST","GUAVA","CLAIM","BLANK","STOVE","TROUT","PLANK","CHANT","DRONE","SPIRE",
    "BLAZE","CRIMP","DWARF","ENVOY","FJORD","GLYPH","HELIX","INFER","JOUST","KARMA",
    "LAPEL","MAXIM","NYMPH","OXIDE","PROXY","QUAFF","RAVEN","STOMP","TRAWL","UMBRA",
    "VIXEN","WALTZ","BOXER","CYNIC","DEBUT","EXPEL","FUDGE","GENRE","HOTLY","IRATE"
};

struct Tile {
    char letter;
    int state; // 0=empty, 1=absent, 2=present, 3=correct
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printTitle() {
    cout << BOLD << FG_WHITE
         << "\n"
         << "  ██╗    ██╗ ██████╗ ██████╗ ██████╗ ██╗     ███████╗\n"
         << "  ██║    ██║██╔═══██╗██╔══██╗██╔══██╗██║     ██╔════╝\n"
         << "  ██║ █╗ ██║██║   ██║██████╔╝██║  ██║██║     █████╗  \n"
         << "  ██║███╗██║██║   ██║██╔══██╗██║  ██║██║     ██╔══╝  \n"
         << "  ╚███╔███╔╝╚██████╔╝██║  ██║██████╔╝███████╗███████╗\n"
         << "   ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝\n"
         << RESET
         << FG_GRAY  << "  Guess the 5-letter word in 6 tries.\n" << RESET
         << FG_GREEN << "  Green" << RESET << " = correct spot  "
         << FG_YELLOW << "Yellow" << RESET << " = wrong spot  "
         << FG_GRAY  << "Gray"   << RESET << " = not in word\n\n";
}

void printTile(char letter, int state) {
    string display = {' ', letter, ' '};
    switch (state) {
        case 3: cout << BG_GREEN  << BOLD << display << RESET; break;
        case 2: cout << BG_YELLOW << BOLD << display << RESET; break;
        case 1: cout << BG_GRAY          << display << RESET; break;
        default:
            if (letter == ' ') cout << BG_DARK << "   " << RESET;
            else               cout << BG_DARK << BOLD << display << RESET;
            break;
    }
}

void printBoard(const vector<vector<Tile>>& board, int currentRow, const string& currentGuess) {
    cout << "\n";
    for (int row = 0; row < 6; row++) {
        cout << "  ";
        for (int col = 0; col < 5; col++) {
            if (row < (int)board.size()) {
                printTile(board[row][col].letter, board[row][col].state);
            } else if (row == currentRow) {
                char c = col < (int)currentGuess.size() ? currentGuess[col] : ' ';
                printTile(c, 0);
            } else {
                printTile(' ', 0);
            }
            if (col < 4) cout << " ";
        }
        cout << "\n";
        if (row < 5) cout << "\n";
    }
    cout << "\n";
}

void printKeyboard(const map<char, int>& keyStates) {
    string rows[3] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    for (int r = 0; r < 3; r++) {
        cout << "  ";
        if (r == 1) cout << " ";
        if (r == 2) cout << "  ";
        for (char c : rows[r]) {
            auto it = keyStates.find(c);
            int state = (it != keyStates.end()) ? it->second : -1;
            switch (state) {
                case 3: cout << BG_GREEN  << BOLD << " " << c << " " << RESET; break;
                case 2: cout << BG_YELLOW << BOLD << " " << c << " " << RESET; break;
                case 1: cout << BG_GRAY          << " " << c << " " << RESET; break;
                default: cout << BG_DARK << FG_WHITE << " " << c << " " << RESET; break;
            }
            cout << " ";
        }
        cout << "\n\n";
    }
}

vector<Tile> evaluateGuess(const string& guess, const string& solution) {
    vector<Tile> result(5);
    string remaining = solution;

    for (int i = 0; i < 5; i++) {
        result[i].letter = guess[i];
        if (guess[i] == solution[i]) {
            result[i].state = 3;
            remaining[i] = '*';
        } else {
            result[i].state = 1;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (result[i].state == 3) continue;
        size_t pos = remaining.find(guess[i]);
        if (pos != string::npos) {
            result[i].state = 2;
            remaining[pos] = '*';
        }
    }
    return result;
}

void updateKeyStates(map<char, int>& keyStates, const vector<Tile>& tiles) {
    for (const auto& tile : tiles) {
        auto it = keyStates.find(tile.letter);
        if (it == keyStates.end() || it->second < tile.state)
            keyStates[tile.letter] = tile.state;
    }
}

string toUpper(string s) {
    for (char& c : s) c = toupper(c);
    return s;
}

bool isValidWord(const string& word) {
    if (word.size() != 5) return false;
    for (char c : word) if (!isalpha(c)) return false;
    return true;
}

int main() {
    srand((unsigned)time(nullptr));

    bool playAgain = true;
    while (playAgain) {
        string solution = WORD_LIST[rand() % WORD_LIST.size()];
        vector<vector<Tile>> board;
        map<char, int> keyStates;
        string currentGuess, message;
        bool gameOver = false, won = false;

        while (!gameOver) {
            clearScreen();
            printTitle();
            printBoard(board, (int)board.size(), currentGuess);
            printKeyboard(keyStates);

            if (!message.empty())
                cout << "  " << BOLD << FG_WHITE << message << RESET << "\n\n";

            cout << "  " << FG_WHITE << "Your guess: " << RESET;
            string input;
            getline(cin, input);
            input = toUpper(input);

            if (input == "QUIT" || input == "EXIT") {
                cout << "\n  " << FG_GRAY << "Thanks for playing!\n\n" << RESET;
                return 0;
            }

            if (!isValidWord(input)) {
                message = "Please enter a valid 5-letter word.";
                continue;
            }

            auto tiles = evaluateGuess(input, solution);
            board.push_back(tiles);
            updateKeyStates(keyStates, tiles);

            bool correct = true;
            for (auto& t : tiles) if (t.state != 3) { correct = false; break; }

            if (correct) {
                won = gameOver = true;
                vector<string> winMsgs = {
                    "Genius! First try!", "Magnificent!", "Impressive!",
                    "Splendid!", "Great!", "Phew, that was close!"
                };
                message = winMsgs[min((int)board.size() - 1, 5)];
            } else if ((int)board.size() >= 6) {
                gameOver = true;
                message = "The word was: " + solution;
            } else {
                message = "";
                currentGuess = "";
            }
        }

        clearScreen();
        printTitle();
        printBoard(board, 6, "");
        printKeyboard(keyStates);
        cout << "  " << BOLD << (won ? FG_GREEN : FG_YELLOW) << message << RESET << "\n\n";

        cout << "  Play again? (y/n): ";
        string again;
        getline(cin, again);
        playAgain = (!again.empty() && tolower(again[0]) == 'y');
    }

    cout << "\n  " << FG_GRAY << "Thanks for playing Wordle!\n\n" << RESET;
    return 0;
}