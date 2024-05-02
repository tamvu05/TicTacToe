#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <ctime>

#define LIGHT_RED "\033[91m";
#define BLUE "\033[36m";
#define RESET_COLOR "\033[0m";
const int SZ = 3;

using namespace std;

class Board {
	vector<char> b;
public:
	Board() : b(SZ*SZ, ' ') {}
	~Board() {}
	Board(const Board& board) : b(board.b) {}

	void display(int cell1 = -1, int cell2 = -1, int cell3 = -1) {
		cout << "-------------\n";
		for (int i = 0; i < SZ; i++) {
			cout << "| ";
			for (int j = 0; j < SZ; j++) {
				if (cell1 == -9) cout << BLUE;
				if (i * SZ + j == cell1 - 1 || i * SZ + j == cell2 - 1 || i * SZ + j == cell3 - 1) cout << LIGHT_RED;
				cout << b[i * SZ + j] << RESET_COLOR;
				cout << " | ";
			}
			cout << "\n-------------\n";
		}
	}
	bool cellEmpty(int cell) {
		if (b[cell - 1] == ' ') return true;
		return false;
	}
	bool checkFull() {
		for (int i = 0; i < 9; i++) if (b[i] == ' ') return false;
		return true;
	}
	bool checkWin(char symbol) {
		for (int i = 0; i < SZ; i++) {
			if (b[i] == symbol && b[i] == b[i + 3] && b[i] == b[i + 6]) return true;
			if (b[i * 3] == symbol && b[i * 3] == b[i * 3 + 1] && b[i * 3] == b[i * 3 + 2]) return true;
		}
		if (b[0] == symbol && b[0] == b[4] && b[0] == b[8]) return true;
		if (b[2] == symbol && b[2] == b[4] && b[2] == b[6]) return true;
		return false;
	}
	void makeMove(int cell, char symbol) {
		b[cell - 1] = symbol;
	}
	vector<int> availableMoves() {
		vector<int> v;
		for (int i = 1; i < 10; i++) if (cellEmpty(i)) v.push_back(i);
		return v;
	}
	void displayWin(char symbol) {
		int c1, c2, c3;
		if (b[0] == symbol && b[0] == b[4] && b[0] == b[8]) { c1 = 0; c2 = 4; c3 = 8; }
		else if (b[2] == symbol && b[2] == b[4] && b[2] == b[6]) { c1 = 2; c2 = 4; c3 = 6; }
		else 
			for (int i = 0; i < SZ; i++) {
				if (b[i] == symbol && b[i] == b[i + 3] && b[i] == b[i + 6]) { c1 = i; c2 = i + 3; c3 = i + 6; }
				if (b[i * 3] == symbol && b[i * 3] == b[i * 3 + 1] && b[i * 3] == b[i * 3 + 2]) { c1 = i * 3; c2 = i * 3 + 1; c3 = i * 3 + 2; }
			}
		c1++; c2++; c3++;
		display(c1, c2, c3);
	}
};

class Player {
	char symbol;
public:
	Player(char s) : symbol(s) {}
	virtual ~Player(){}
	Player(const Player& p) :symbol(p.symbol) {}
	virtual Player* clone() const = 0;

	char getSymbol() { return symbol; }
	virtual void move(Board &b) = 0;
};

class Human : public Player {
public:
	Human(char s) : Player(s) {}
	~Human() {}
	Human(const Human& h) :Player(h) {}
	Player* clone() const { return new Human(*this); }

	void move(Board& b) {
		cout << "\nNguoi choi " << getSymbol() << " chon vi tri danh (1-9): ";
		string s; cin >> s;
		stringstream ss(s);
		int cell; ss >> cell;
		try {
			if (cell < 1 || cell>9) throw out_of_range("-O khong hop le!\n-Vui long chon lai!\n\n");
			else if (!b.cellEmpty(cell)) throw invalid_argument("-O da duoc chon!\n-Vui long chon lai!\n\n");
			else {
				b.makeMove(cell, getSymbol());
				b.display(cell);
			}
		}
		catch (out_of_range& e) {
			cerr << e.what();
			move(b);
		}
		catch (invalid_argument& e) {
			cerr << e.what();
			move(b);
		}
	}
};

class Computer : public Player {
	char OSymbol;

	int minimax(Board b, int d, bool mp, char self, char opponent) {
		if (b.checkWin(self)) return 1;
		if (b.checkWin(opponent)) return -1;
		if (b.checkFull() || d == 0) return 0;

		vector<int> moves = b.availableMoves();

		if (mp) {
			int score = -10;
			for (int move : moves) {
				b.makeMove(move, self);
				score = max(score, minimax(b, d - 1, false, self, opponent));
				b.makeMove(move, ' ');
			}
			return score;
		}
		else {
			int score = 10;
			for (int move : moves) {
				b.makeMove(move, opponent);
				score = min(score, minimax(b, d - 1, true, self, opponent));
				b.makeMove(move, ' ');
			}
			return score;
		}
	}
public:
	Computer(char s) :Player(s), OSymbol(' ') {}
	~Computer() {}
	Computer(const Computer& c) :Player(c), OSymbol(c.OSymbol) {}
	Player* clone() const { return new Computer(*this); }

	void setOSymbol(char s) { OSymbol = s; }
	int getBestMove(Board b, char self, char opponent) {
		int bestScore = -10, bestMove = -10;

		vector<int> moves = b.availableMoves();
		for (int move : moves) {
			b.makeMove(move, self);
			int score = minimax(b, 5, false, self, opponent);
			b.makeMove(move, ' ');
			if (score > bestScore) {
				bestScore = score;
				bestMove = move;
			}
		}
		return bestMove;
	}
	void move(Board& b) {
		int cell = getBestMove(b, getSymbol(), OSymbol);
		b.makeMove(cell, getSymbol());
		cout << "\nNguoi may " << getSymbol() << ": " << cell << endl;
		b.display(cell);
	}
};

class Game {
	Board b;
	Player* p1, * p2;
public:
	Game() :p1(NULL), p2(NULL) {}
	~Game() { delete p1; delete p2; }
	Game(const Game& g) : b(g.b), p1(NULL), p2(NULL) {
		if (g.p1) p1 = g.p1->clone();
		if (g.p2) p2 = g.p2->clone();
	}

	void start() {
		cout << "Chao mung den voi Tro choi Tic Tac Toe!\n";
		cout << "Ban co the nhap vi tri cua o ma ban muon danh.\n";
		cout << "Vi tri tu 1 den 9 duoc danh dau tu trai sang phai va tu tren xuong duoi\n";
		cout << "Chuc ban may man!\n\n";
		cout << "Nhap Y de tiep tuc: ";
		char y; cin >> y;
		if (y != 'y' && y != 'Y') {
			cout << "\nTro choi da ket thuc!\n";
			return;
		}

		system("cls");

		cout << "---------MENU---------\n";
		cout << "| 1. Nguoi vs Nguoi  |\n";
		cout << "| 2. Nguoi vs May    |\n";
		cout << "| 3. May vs May      |\n";
		cout << "----------------------\n";
		cout << "Chon che do (1-3): ";

		string s; cin >> s;
		stringstream ss(s);
		int x; ss >> x;
		try {
			if (x < 1 || x>3) throw out_of_range("-Che do khong hop le!\n-Vui long chon lai!\n\n");
		}
		catch (out_of_range& e) {
			cerr << e.what();
			start();
		}
		string _s1, _s2;
		char s1, s2;
		cout << "Chon ki hieu cho nguoi choi 1 (Goi y: X-O): "; cin >> _s1;
		cout << "Chon ki hieu cho nguoi choi 2 (Goi y: X-O): "; cin >> _s2;
		
		while(_s1[0] == _s2[0]) {
			cout << "-Vui long chon ki hieu khac: ";
			cin >> _s2;
		}
		s1 = toupper(_s1[0]); s2 = toupper(_s2[0]);

		system("cls");

		if (x == 1) {
			Human* _p1 = new Human(s1);
			Human* _p2 = new Human(s2);
			p1 = _p1; p2 = _p2;
		}
		else if (x == 2) {
			Human* _p1 = new Human(s1);
			Computer* _p2 = new Computer(s2);
			_p2->setOSymbol(s1);
			p1 = _p1; p2 = _p2;
		}
		else {
			Computer* _p1 = new Computer(s1);
			Computer* _p2 = new Computer(s2);			
			_p1->setOSymbol(s2);
			_p2->setOSymbol(s1);
			p1 = _p2; p2 = _p1;

			srand((int)time(NULL));
			int cell = rand() % 9;
			b.makeMove(cell, s1);
			cout << "Nguoi may " << s1 << ": " << cell << endl;
			b.display(cell, -1, -1);
		}

		if (x != 3) b.display();
		while (!b.checkFull()) {
			p1->move(b);
			if (b.checkWin(p1->getSymbol())) { 
				cout << "\n-Ket qua: Nguoi choi " << p1->getSymbol() << " da thang\n"; 
				b.displayWin(p1->getSymbol()); return; }
			else if (b.checkFull()) { 
				cout << "\n-Ket qua: Hoa\n"; b.display(-9); 
				return; }
			p2->move(b);
			if (b.checkWin(p2->getSymbol())) { 
				cout << "\n-Ket qua: Nguoi choi " << p2->getSymbol() << " da thang\n"; 
				b.displayWin(p2->getSymbol()); return; }
			else if (b.checkFull()) { 
				cout << "\n-Ket qua: Hoa\n"; 
				b.display(-9); return; }
		}

	}
};

int main() {
	Game game;
	game.start();
}