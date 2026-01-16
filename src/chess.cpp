#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

// Simple types
enum class Color { WHITE, BLACK, NONE };

enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE };

struct Position {
    int row;
    int col;
    Position(int r = -1, int c = -1) : row(r), col(c) {}
    bool isValid() const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
};

Position fromAlgebraic(const string& s) {
    if (s.size() != 2) return Position();
    char file = s[0];
    char rank = s[1];
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') return Position();
    int col = file - 'a';
    int row = 8 - (rank - '0');
    return Position(row, col);
}

// Simple piece
struct Piece {
    Color color;
    PieceType type;
    Piece(Color c = Color::NONE, PieceType t = PieceType::NONE) : color(c), type(t) {}
    char symbol() const {
        if (type == PieceType::NONE) return ' ';
        char base = ' ';
        switch (type) {
            case PieceType::KING: base = 'K'; break;
            case PieceType::QUEEN: base = 'Q'; break;
            case PieceType::ROOK: base = 'R'; break;
            case PieceType::BISHOP: base = 'B'; break;
            case PieceType::KNIGHT: base = 'N'; break;
            case PieceType::PAWN: base = 'P'; break;
            default: base = ' '; break;
        }
        if (color == Color::WHITE) return base;
        if (color == Color::BLACK) {
            if (base >= 'A' && base <= 'Z') base = static_cast<char>(base - 'A' + 'a');
            return base;
        }
        return base;
    }
};

// Board
class Board {
public:
    Board() { setupInitial(); }

    const Piece& at(const Position& p) const { return cells_[p.row][p.col]; }
    Piece& at(const Position& p) { return cells_[p.row][p.col]; }

    bool isEmpty(const Position& p) const { return at(p).type == PieceType::NONE; }
    bool sameColor(const Position& p, Color c) const {
        const Piece& x = at(p);
        return x.type != PieceType::NONE && x.color == c;
    }
    bool differentColorOrEmpty(const Position& p, Color c) const {
        const Piece& x = at(p);
        return x.type == PieceType::NONE || x.color != c;
    }

    bool move(const Position& from, const Position& to, Color turn) {
        if (!from.isValid() || !to.isValid()) return false;
        if (isEmpty(from)) return false;
        const Piece& mover = at(from);
        if (mover.color != turn) return false;
        if (!differentColorOrEmpty(to, turn)) return false;
        if (!validPieceMove(from, to)) return false;

        // Simulate
        Board tmp = *this;
        tmp.cells_[to.row][to.col] = tmp.cells_[from.row][from.col];
        tmp.cells_[from.row][from.col] = Piece();
        if (tmp.inCheck(turn)) return false;

        // Apply
        cells_[to.row][to.col] = cells_[from.row][from.col];
        cells_[from.row][from.col] = Piece();
        return true;
    }

    string render() const {
        ostringstream out;
        out << "\n  +------------------------+\n";
        for (int r = 0; r < 8; ++r) {
            out << (8 - r) << " |";
            for (int c = 0; c < 8; ++c) {
                char ch = cells_[r][c].symbol();
                out << ' ' << ch << ' ';
            }
            out << "|\n";
        }
        out << "  +------------------------+\n";
        out << "    a  b  c  d  e  f  g  h\n";
        return out.str();
    }

private:
    Piece cells_[8][8];

    void setupInitial() {
        for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) cells_[r][c] = Piece();
        // Black
        cells_[0][0] = Piece(Color::BLACK, PieceType::ROOK);
        cells_[0][1] = Piece(Color::BLACK, PieceType::KNIGHT);
        cells_[0][2] = Piece(Color::BLACK, PieceType::BISHOP);
        cells_[0][3] = Piece(Color::BLACK, PieceType::QUEEN);
        cells_[0][4] = Piece(Color::BLACK, PieceType::KING);
        cells_[0][5] = Piece(Color::BLACK, PieceType::BISHOP);
        cells_[0][6] = Piece(Color::BLACK, PieceType::KNIGHT);
        cells_[0][7] = Piece(Color::BLACK, PieceType::ROOK);
        for (int c = 0; c < 8; ++c) cells_[1][c] = Piece(Color::BLACK, PieceType::PAWN);
        // White
        for (int c = 0; c < 8; ++c) cells_[6][c] = Piece(Color::WHITE, PieceType::PAWN);
        cells_[7][0] = Piece(Color::WHITE, PieceType::ROOK);
        cells_[7][1] = Piece(Color::WHITE, PieceType::KNIGHT);
        cells_[7][2] = Piece(Color::WHITE, PieceType::BISHOP);
        cells_[7][3] = Piece(Color::WHITE, PieceType::QUEEN);
        cells_[7][4] = Piece(Color::WHITE, PieceType::KING);
        cells_[7][5] = Piece(Color::WHITE, PieceType::BISHOP);
        cells_[7][6] = Piece(Color::WHITE, PieceType::KNIGHT);
        cells_[7][7] = Piece(Color::WHITE, PieceType::ROOK);
    }

    bool pathClearStraight(const Position& from, const Position& to) const {
        if (from.row == to.row) {
            int step = (to.col > from.col) ? 1 : -1;
            for (int c = from.col + step; c != to.col; c += step)
                if (!isEmpty(Position(from.row, c))) return false;
            return true;
        }
        if (from.col == to.col) {
            int step = (to.row > from.row) ? 1 : -1;
            for (int r = from.row + step; r != to.row; r += step)
                if (!isEmpty(Position(r, from.col))) return false;
            return true;
        }
        return false;
    }

    bool pathClearDiagonal(const Position& from, const Position& to) const {
        int dr = to.row - from.row;
        int dc = to.col - from.col;
        if (abs(dr) != abs(dc)) return false;
        int stepR = (dr > 0) ? 1 : -1;
        int stepC = (dc > 0) ? 1 : -1;
        int r = from.row + stepR;
        int c = from.col + stepC;
        while (r != to.row && c != to.col) {
            if (!isEmpty(Position(r, c))) return false;
            r += stepR;
            c += stepC;
        }
        return true;
    }

    bool validPieceMove(const Position& from, const Position& to) const {
        const Piece& p = at(from);
        int dr = to.row - from.row;
        int dc = to.col - from.col;
        switch (p.type) {
            case PieceType::KING:
                return abs(dr) <= 1 && abs(dc) <= 1;
            case PieceType::QUEEN:
                if (dr == 0 || dc == 0) return pathClearStraight(from, to);
                if (abs(dr) == abs(dc)) return pathClearDiagonal(from, to);
                return false;
            case PieceType::ROOK:
                if (dr == 0 || dc == 0) return pathClearStraight(from, to);
                return false;
            case PieceType::BISHOP:
                if (abs(dr) == abs(dc)) return pathClearDiagonal(from, to);
                return false;
            case PieceType::KNIGHT:
                return (abs(dr) * abs(dc)) == 2;
            case PieceType::PAWN: {
                int dir = (p.color == Color::WHITE) ? -1 : 1;
                if (dc == 0 && dr == dir && isEmpty(to)) return true;
                int startRow = (p.color == Color::WHITE) ? 6 : 1;
                if (dc == 0 && dr == 2*dir && from.row == startRow) {
                    Position mid(from.row + dir, from.col);
                    if (isEmpty(mid) && isEmpty(to)) return true;
                }
                if (abs(dc) == 1 && dr == dir && !isEmpty(to) && at(to).color != p.color) return true;
                return false;
            }
            default:
                return false;
        }
    }

    bool squareAttacked(const Position& sq, Color by) const {
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position from(r, c);
                const Piece& p = cells_[r][c];
                if (p.type == PieceType::NONE || p.color != by) continue;
                if (p.type == PieceType::PAWN) {
                    int dir = (by == Color::WHITE) ? -1 : 1;
                    if (sq.row == r + dir && abs(sq.col - c) == 1) return true;
                    continue;
                }
                if (validPieceMove(from, sq) && differentColorOrEmpty(sq, by)) return true;
            }
        }
        return false;
    }

    bool inCheck(Color c) const {
        Position kp;
        bool found = false;
        for (int r = 0; r < 8 && !found; ++r) {
            for (int co = 0; co < 8 && !found; ++co) {
                const Piece& p = cells_[r][co];
                if (p.type == PieceType::KING && p.color == c) { kp = Position(r, co); found = true; }
            }
        }
        if (!found) return false;
        Color opp = (c == Color::WHITE) ? Color::BLACK : Color::WHITE;
        return squareAttacked(kp, opp);
    }
};

static string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, b - a + 1);
}

int main() {
    Board board;
    Color turn = Color::WHITE;
    cout << "Welcome to Simple Terminal Chess" << endl;
    cout << "Enter moves like e2e4 or e2 e4. Type 'quit' to exit." << endl;

    while (true) {
        cout << board.render() << endl;
        cout << ((turn == Color::WHITE) ? "White" : "Black") << " to move > ";
        string line;
        if (!getline(cin, line)) break;
        string in = trim(line);
        if (in == "quit" || in == "exit") break;
        if (in.empty()) continue;

        string a, b;
        if (in.size() == 4) { a = in.substr(0,2); b = in.substr(2,2); }
        else { istringstream iss(in); iss >> a >> b; }
        Position from = fromAlgebraic(a);
        Position to = fromAlgebraic(b);
        if (!from.isValid() || !to.isValid()) { cout << "Invalid input." << endl; continue; }

        if (board.move(from, to, turn)) {
            turn = (turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        } else {
            cout << "Illegal move." << endl;
        }
    }
    cout << "Goodbye!" << endl;
    return 0;
}
