#include "Gramaticas.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// ======================================================
//       UTILS
// ======================================================
char GrammarAnalyzer::peek() {
    if (index >= (int)input.size()) return '\0';
    return input[index];
}

char GrammarAnalyzer::get() {
    if (index >= (int)input.size()) return '\0';
    return input[index++];
}

// ======================================================
//       PARSER DE REGEX
// ======================================================
std::shared_ptr<GrammarAnalyzer::Node> GrammarAnalyzer::parseRegex() {
    return parseUnion();
}

std::shared_ptr<GrammarAnalyzer::Node> GrammarAnalyzer::parseUnion() {
    auto left = parseConcat();
    while (peek() == '|') {
        get();
        auto right = parseConcat();
        auto n = std::make_shared<Node>(Node::UNION);
        n->left = left;
        n->right = right;
        left = n;
    }
    return left;
}

std::shared_ptr<GrammarAnalyzer::Node> GrammarAnalyzer::parseConcat() {
    auto left = parseStar();
    while (true) {
        char c = peek();
        if (c == '\0' || c == ')' || c == '|') break;
        auto right = parseStar();
        auto n = std::make_shared<Node>(Node::CONCAT);
        n->left = left;
        n->right = right;
        left = n;
    }
    return left;
}

std::shared_ptr<GrammarAnalyzer::Node> GrammarAnalyzer::parseStar() {
    auto node = parseAtom();

    while (peek() == '*' || peek() == '+') {
        char op = get();
        if (op == '*') {
            auto n = std::make_shared<Node>(Node::STAR);
            n->left = node;
            node = n;
        } else if (op == '+') {
            // A+ = A A*
            auto concat = std::make_shared<Node>(Node::CONCAT);
            auto star = std::make_shared<Node>(Node::STAR);
            star->left = node;
            concat->left = node;
            concat->right = star;
            node = concat;
        }
    }
    return node;
}

std::shared_ptr<GrammarAnalyzer::Node> GrammarAnalyzer::parseAtom() {
    char c = peek();
    if (c == '(') {
        get();
        auto node = parseRegex();
        if (peek() == ')') get();
        return node;
    }

    if (c == '\\') {
        get();
        char lit = get();
        auto n = std::make_shared<Node>(Node::LEAF);
        n->symbol = lit;
        n->pos = ++positionCounter;
        posToSymbol[n->pos] = lit;
        return n;
    }

    // literal
    get();
    auto n = std::make_shared<Node>(Node::LEAF);
    n->symbol = c;
    n->pos = ++positionCounter;
    posToSymbol[n->pos] = c;
    return n;
}

// ======================================================
//       CÁLCULO DE NULLABLE / FIRSTPOS / LASTPOS
// ======================================================
void GrammarAnalyzer::computeNullableFirstLast(std::shared_ptr<Node> node) {
    if (!node) return;

    if (node->type == Node::LEAF) {
        node->nullable = false;
        node->firstpos = {node->pos};
        node->lastpos = {node->pos};
        return;
    }

    computeNullableFirstLast(node->left);
    computeNullableFirstLast(node->right);

    switch (node->type) {
        case Node::UNION:
            node->nullable = node->left->nullable || node->right->nullable;
            node->firstpos = node->left->firstpos;
            node->firstpos.insert(node->right->firstpos.begin(), node->right->firstpos.end());
            node->lastpos = node->left->lastpos;
            node->lastpos.insert(node->right->lastpos.begin(), node->right->lastpos.end());
            break;

        case Node::CONCAT:
            node->nullable = node->left->nullable && node->right->nullable;

            if (node->left->nullable) {
                node->firstpos = node->left->firstpos;
                node->firstpos.insert(node->right->firstpos.begin(), node->right->firstpos.end());
            } else {
                node->firstpos = node->left->firstpos;
            }

            if (node->right->nullable) {
                node->lastpos = node->left->lastpos;
                node->lastpos.insert(node->right->lastpos.begin(), node->right->lastpos.end());
            } else {
                node->lastpos = node->right->lastpos;
            }
            break;

        case Node::STAR:
            node->nullable = true;
            node->firstpos = node->left->firstpos;
            node->lastpos = node->left->lastpos;
            break;

        default: break;
    }
}

// ======================================================
//       CÁLCULO FOLLOWPOS
// ======================================================
void GrammarAnalyzer::computeFollowpos(std::shared_ptr<Node> node) {
    if (!node) return;

    if (node->type == Node::CONCAT) {
        for (int i : node->left->lastpos) {
            followpos[i].insert(node->right->firstpos.begin(), node->right->firstpos.end());
        }
    }

    if (node->type == Node::STAR) {
        for (int i : node->lastpos) {
            followpos[i].insert(node->firstpos.begin(), node->firstpos.end());
        }
    }

    computeFollowpos(node->left);
    computeFollowpos(node->right);
}

// ======================================================
//       PROCESA UNA SOLA EXPRESIÓN
// ======================================================
RegexResult GrammarAnalyzer::analyzeSingle(const std::string &token, const std::string &regex) {
    input = regex;
    index = 0;
    positionCounter = 0;
    posToSymbol.clear();
    followpos.clear();

    auto root = parseRegex();
    computeNullableFirstLast(root);
    computeFollowpos(root);

    RegexResult result;
    result.tokenName = token;
    result.regex = regex;
    result.firstSymbols.clear();

    for (int pos : root->firstpos)
        result.firstSymbols.insert(posToSymbol[pos]);

    result.followpos = followpos;
    result.posToSymbol = posToSymbol;

    return result;
}

// ======================================================
//       LEE EL ARCHIVO .TXT
// ======================================================
std::vector<RegexResult> GrammarAnalyzer::analyzeFile(const std::string &path) {
    std::vector<RegexResult> results;

    std::ifstream in(path);
    if (!in.is_open())
        throw std::runtime_error("No se pudo abrir: " + path);

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        size_t p = line.find(':');
        if (p == std::string::npos) continue;

        std::string token = line.substr(0, p);
        std::string regex = line.substr(p + 1);

        // trim espacios
        while (!regex.empty() && regex.front() == ' ') regex.erase(regex.begin());
        while (!regex.empty() && regex.back() == ' ') regex.pop_back();

        results.push_back(analyzeSingle(token, regex));
    }

    return results;
}

