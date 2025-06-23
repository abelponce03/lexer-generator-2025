#include "lexer_nfa.hpp"
#include "lexer_dfa.hpp" 
#include <iostream>

void LexerNFA::addPattern(const std::string& tokenName, std::shared_ptr<NFA> nfa, bool isIgnorable) {
    if (!nfa) return;
    
    TokenPattern pattern;
    pattern.name = tokenName;
    pattern.nfa = nfa;
    pattern.isIgnorable = isIgnorable;
    patterns.push_back(pattern);
}

void LexerNFA::build() {
    if (patterns.empty()) return;
    
    // Crear un nuevo NFA combinando todos los patrones
    combinedNfa = std::make_shared<NFA>();
    
    // Crear un nuevo estado inicial
    NFA::State newStart = combinedNfa->addState();
    combinedNfa->setStartState(newStart);
    
    // Para cada patrón, crear estados finales específicos del token
    std::map<std::string, std::vector<NFA::State>> tokenFinalStates;
    
    // Mapear estados de cada NFA individual al NFA combinado
    for (const auto& pattern : patterns) {
        const auto& nfa = pattern.nfa;
        const auto& name = pattern.name;
        
        // Mapear estados
        std::map<NFA::State, NFA::State> stateMap;
        for (int i = 0; i < nfa->getStateCount(); i++) {
            NFA::State newState = combinedNfa->addState();
            stateMap[i] = newState;
        }
        
        // Copiar transiciones
        for (const auto& trans : nfa->getTransitions()) {
            NFA::State from = stateMap[trans.from];
            NFA::State to = stateMap[trans.to];
            combinedNfa->addTransition(from, trans.symbol, to);
        }
        
        // Añadir epsilon-transición desde el nuevo estado inicial
        combinedNfa->addEpsilonTransition(newStart, stateMap[nfa->getStartState()]);
        
        // Registrar estados finales para este token
        for (NFA::State finalState : nfa->getFinalStates()) {
            NFA::State mappedFinal = stateMap[finalState];
            tokenFinalStates[name].push_back(mappedFinal);
            combinedNfa->addFinalState(mappedFinal);
            
            // Asociar estos estados finales con su token
            finalStateToToken[mappedFinal] = std::make_pair(name, pattern.isIgnorable);
        }
    }
}

std::shared_ptr<LexerDFA> LexerNFA::toDFA() {
    if (!combinedNfa) build();
    
    auto dfa = std::make_shared<LexerDFA>();
    dfa->build(*this);
    return dfa;
}