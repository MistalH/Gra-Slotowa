#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>

// Struktura symbolu z wygranymi
struct Symbol {
    int symbolId;
    std::string name;
    std::vector<int> payouts;
};

// Struktura walca
struct Reel {
    std::vector<int> symbols;
};

// Klasa reprezentująca grę slotową
class SlotGame {
private:
    int gamesCount;
    int startCredit;
    int currentCredit;
    std::vector<Reel> reels;
    std::vector<Symbol> symbols;
    std::ofstream creditOutFile;

    int calculatePayout(int lineBet, const std::vector<int>& results);
    void spin();
    void generateReels();
    void loadSymbols();
    void displayResult(const std::vector<int>& results, int payout);

public:
    SlotGame(int games, int startCredit, const std::string& outFile);
    ~SlotGame();

    void run();
};

// Konstruktor
SlotGame::SlotGame(int games, int startCredit, const std::string& outFile)
    : gamesCount(games), startCredit(startCredit), currentCredit(startCredit) {
    creditOutFile.open(outFile);
    generateReels();
    loadSymbols();
}

// Destruktor
SlotGame::~SlotGame() {
    creditOutFile.close();
}

// Metoda generująca walcę
void SlotGame::generateReels() {
    // Tutaj generujemy walcę z odpowiednimi symbolami
    // W tym przykładzie używamy trzech walców o długości 5
    Reel reel1, reel2, reel3;
    reel1.symbols = { 0, 1, 2, 3, 4 };
    reel2.symbols = { 5, 5, 5, 5, 5 };
    reel3.symbols = { 6, 6, 6, 6, 6 };

    reels = { reel1, reel2, reel3 };
}

// Metoda wczytująca symbole i ich wygrane
void SlotGame::loadSymbols() {
    // Tutaj wczytujemy symbole z ich wygranymi
    // W tym przykładzie używamy 7 symboli
    Symbol symbol1 = { 0, "scatter", {0, 0, 0, 0, 0} };
    Symbol symbol2 = { 1, "wiśnia", {5, 10, 50, 100, 500} };
    Symbol symbol3 = { 2, "cytryna", {0, 10, 25, 75, 200} };
    Symbol symbol4 = { 3, "pomarańcza", {0, 5, 20, 50, 150} };
    Symbol symbol5 = { 4, "śliwka", {0, 5, 20, 50, 150} };
    Symbol symbol6 = { 5, "winogrona", {0, 2, 10, 25, 100} };
    Symbol symbol7 = { 6, "arbuz", {0, 2, 10, 25, 100} };

    symbols = { symbol1, symbol2, symbol3, symbol4, symbol5, symbol6, symbol7 };
}

// Metoda symulująca pojedyncze losowanie
void SlotGame::spin() {
    // Losujemy symbole na każdym z walców
    std::vector<int> results;
    for (const auto& reel : reels) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(0, reel.symbols.size() - 1);
        int symbolId = reel.symbols[distribution(gen)];
        results.push_back(symbolId);
    }

    // Sprawdzamy, ile wygranych układów udało się uzyskać
    int lineBet = 1; // Zakład na linię, można zmienić w przypadku bardziej rozbudowanej logiki gry
    int payout = calculatePayout(lineBet, results);

    // Aktualizujemy stan kredytu gracza
    currentCredit -= lineBet;
    currentCredit += payout;

    // Zapisujemy stan kredytu do pliku
    creditOutFile << currentCredit << "\n";

    // Wyświetlamy wynik losowania
    displayResult(results, payout);
}

// Metoda obliczająca wygraną za dany układ symboli
int SlotGame::calculatePayout(int lineBet, const std::vector<int>& results) {
    int payout = 0;

    for (const auto& symbolId : results) {
        const Symbol& symbol = symbols[symbolId];
        payout += symbol.payouts[std::count(results.begin(), results.end(), symbolId)];
    }

    return payout * lineBet;
}

// Metoda wyświetlająca wynik pojedynczego losowania
void SlotGame::displayResult(const std::vector<int>& results, int payout) {
    std::cout << "Symbols: ";
    for (int symbolId : results) {
        std::cout << symbols[symbolId].name << " ";
    }
    std::cout << "\n";

    if (payout > 0) {
        std::cout << "Congratulations! You won: " << payout << "\n";
    }
    else {
        std::cout << "No win this time.\n";
    }

    std::cout << "Current credit: " << currentCredit << "\n\n";
}

// Metoda uruchamiająca symulację gier slotowych
void SlotGame::run() {
    for (int i = 0; i < gamesCount; ++i) {
        spin();
    }

    // Wyświetlamy raport końcowy na konsoli
    std::cout << "Simulation Summary:\n";
    std::cout << "Total Games: " << gamesCount << "\n";
    std::cout << "Start Credit: " << startCredit << "\n";
    std::cout << "End Credit: " << currentCredit << "\n";
    std::cout << "Net Win/Loss: " << currentCredit - startCredit << "\n";
    double rtp = static_cast<double>(currentCredit - startCredit) / (gamesCount * 1.0 * 1.0);
    double hf = static_cast<double>(std::count_if(creditOutFile, std::istream_iterator<std::string>(), [](const std::string& line) {
        return std::stoi(line) > 0;
        })) / (gamesCount * 1.0 * 1.0);
        std::cout << "RTP: " << rtp << "\n";
        std::cout << "HF: " << hf << "\n";
}

int main(int argc, char* argv[]) {
    int gamesCount = 0;
    int startCredit = 0;
    std::string creditOutFile;
    std::vector<int> customSymbols;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-gamesCount") {
            if (i + 1 < argc) {
                gamesCount = std::stoi(argv[++i]);
            }
        }
        else if (arg == "-startCredit") {
            if (i + 1 < argc) {
                startCredit = std::stoi(argv[++i]);
            }
        }
        else if (arg == "-creditOutFile") {
            if (i + 1 < argc) {
                creditOutFile = argv[++i];
            }
        }
        else if (arg == "-symbols") {
            if (i + 1 < argc) {
                std::string symbolsArg = argv[++i];
                std::stringstream ss(symbolsArg);
                std::string symbol;
                while (getline(ss, symbol, ',')) {
                    customSymbols.push_back(std::stoi(symbol));
                }
            }
        }
    }

    if (!creditOutFile.empty()) {
        SlotGame slotGame(1, startCredit, creditOutFile);
        int payout = slotGame.calculatePayout(1, customSymbols);
        std::cout << payout << "\n";
    }
    else {
        SlotGame slotGame(gamesCount, startCredit, "credit_log.txt");
        slotGame.run();
    }

    return 0;
}
