#include "Simb.h"

Simb::Simb() : Simbolo(""), EsTerminal(false), IdToken(-1) {}

Simb::Simb(const std::string& simb, bool esTerminal, int idToken)
    : Simbolo(simb), EsTerminal(esTerminal), IdToken(idToken) {}