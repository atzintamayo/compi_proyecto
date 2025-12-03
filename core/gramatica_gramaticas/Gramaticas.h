#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

//
// Representa los resultados del análisis de una regex
//
struct RegexResult {
    std::string tokenName;
    std::string regex;

    // Símbolos que pueden aparecer al inicio
    std::set<char> firstSymbols;

    // followpos: pos -> conjunto de posiciones
    std::map<int, std::set<int>> followpos;

    // mapa para saber qué símbolo representa una hoja
    std::map<int, char> posToSymbol;
};

//
// Analizador principal
//
class GrammarAnalyzer {
public:
    // Analiza un archivo TOKEN: regex
    std::vector<RegexResult> analyzeFile(const std::string &path);

private:
    // ---- Implementación interna ----
    struct Node {
        enum Type { LEAF, CONCAT, UNION, STAR } type;
        char symbol;           // solo si leaf
        int pos;               // posición única para leaf
        bool nullable;
        std::set<int> firstpos;
        std::set<int> lastpos;

        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        Node(Type t) : type(t), symbol(0), pos(-1), nullable(false) {}
    };

    std::string input;
    int index;
    int positionCounter;
    std::map<int, char> posToSymbol;
    std::map<int, std::set<int>> followpos;

    // Parser
    char peek();
    char get();
    std::shared_ptr<Node> parseRegex();
    std::shared_ptr<Node> parseUnion();
    std::shared_ptr<Node> parseConcat();
    std::shared_ptr<Node> parseStar();
    std::shared_ptr<Node> parseAtom();

    void computeNullableFirstLast(std::shared_ptr<Node> node);
    void computeFollowpos(std::shared_ptr<Node> node);

    RegexResult analyzeSingle(const std::string &token, const std::string &regex);
};

