#include "automata/nfa.hpp"
#include "ast/expression.hpp"
#include <queue>
#include <set>
#include <algorithm>

// Construir un NFA simple para un único carácter
std::shared_ptr<NFA> NFA::fromChar(char c) {
    auto nfa = std::make_shared<NFA>();
    State start = nfa->addState();
    State end = nfa->addState();
    
    nfa->setStartState(start);
    nfa->addFinalState(end);
    nfa->addTransition(start, c, end);
    
    return nfa;
}

// Construir un NFA para un wildcard (.)
std::shared_ptr<NFA> NFA::fromWildcard() {
    auto nfa = std::make_shared<NFA>();
    State start = nfa->addState();
    State end = nfa->addState();
    
    nfa->setStartState(start);
    nfa->addFinalState(end);
    
    // Añadir transiciones para todos los caracteres ASCII imprimibles
    for (char c = 32; c <= 126; c++) {
        nfa->addTransition(start, c, end);
    }
    
    return nfa;
}

// Construir un NFA para una clase de caracteres
std::shared_ptr<NFA> NFA::fromCharSet(const CharSet& charset) {
    auto nfa = std::make_shared<NFA>();
    State start = nfa->addState();
    State end = nfa->addState();
    
    nfa->setStartState(start);
    nfa->addFinalState(end);
    
    // Si es un conjunto negado, primero añadimos todos los caracteres ASCII
    if (charset.isNegated()) {
        std::set<char> excludedChars;
        
        // Recolectar todos los caracteres excluidos
        for (const auto& range : charset.getRanges()) {
            for (char c = range.first; c <= range.second; c++) {
                excludedChars.insert(c);
            }
        }
        
        // Añadir transiciones para todos los caracteres no excluidos
        for (char c = 32; c <= 126; c++) {
            if (excludedChars.find(c) == excludedChars.end()) {
                nfa->addTransition(start, c, end);
            }
        }
    } else {
        // Para conjuntos normales, simplemente añadir transiciones para cada rango
        for (const auto& range : charset.getRanges()) {
            for (char c = range.first; c <= range.second; c++) {
                nfa->addTransition(start, c, end);
            }
        }
    }
    
    return nfa;
}

// Operación de concatenación: N(AB) = N(A)N(B)
std::shared_ptr<NFA> NFA::concatenate(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second) {
    if (!first || !second) return nullptr;
    
    auto result = std::make_shared<NFA>();
    
    // Mapear estados del primer NFA al resultado
    std::map<State, State> firstMap;
    for (State s : first->states) {
        State newState = result->addState();
        firstMap[s] = newState;
    }
    
    // Mapear estados del segundo NFA al resultado
    std::map<State, State> secondMap;
    for (State s : second->states) {
        State newState = result->addState();
        secondMap[s] = newState;
    }
    
    // Copiar transiciones del primer NFA
    for (const auto& trans : first->transitions) {
        State from = firstMap[trans.from];
        State to = firstMap[trans.to];
        result->addTransition(from, trans.symbol, to);
    }
    
    // Copiar transiciones del segundo NFA
    for (const auto& trans : second->transitions) {
        State from = secondMap[trans.from];
        State to = secondMap[trans.to];
        result->addTransition(from, trans.symbol, to);
    }
    
    // Añadir epsilon-transiciones desde estados finales del primer NFA
    // al estado inicial del segundo NFA
    for (State finalState : first->finalStates) {
        State mappedFinal = firstMap[finalState];
        State mappedStart = secondMap[second->startState];
        result->addEpsilonTransition(mappedFinal, mappedStart);
    }
    
    // Establecer estado inicial y estados finales
    result->setStartState(firstMap[first->startState]);
    for (State finalState : second->finalStates) {
        result->addFinalState(secondMap[finalState]);
    }
    
    return result;
}

// Operación de alternación: N(A|B)
std::shared_ptr<NFA> NFA::alternate(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second) {
    if (!first || !second) return nullptr;
    
    auto result = std::make_shared<NFA>();
    
    // Crear nuevo estado inicial y final
    State newStart = result->addState();
    State newEnd = result->addState();
    
    result->setStartState(newStart);
    result->addFinalState(newEnd);
    
    // Mapear estados del primer NFA al resultado
    std::map<State, State> firstMap;
    for (State s : first->states) {
        State newState = result->addState();
        firstMap[s] = newState;
    }
    
    // Mapear estados del segundo NFA al resultado
    std::map<State, State> secondMap;
    for (State s : second->states) {
        State newState = result->addState();
        secondMap[s] = newState;
    }
    
    // Copiar transiciones del primer NFA
    for (const auto& trans : first->transitions) {
        State from = firstMap[trans.from];
        State to = firstMap[trans.to];
        result->addTransition(from, trans.symbol, to);
    }
    
    // Copiar transiciones del segundo NFA
    for (const auto& trans : second->transitions) {
        State from = secondMap[trans.from];
        State to = secondMap[trans.to];
        result->addTransition(from, trans.symbol, to);
    }
    
    // Añadir epsilon-transiciones desde el nuevo estado inicial
    // a los estados iniciales de ambos NFAs
    result->addEpsilonTransition(newStart, firstMap[first->startState]);
    result->addEpsilonTransition(newStart, secondMap[second->startState]);
    
    // Añadir epsilon-transiciones desde los estados finales de ambos NFAs
    // al nuevo estado final
    for (State finalState : first->finalStates) {
        result->addEpsilonTransition(firstMap[finalState], newEnd);
    }
    
    for (State finalState : second->finalStates) {
        result->addEpsilonTransition(secondMap[finalState], newEnd);
    }
    
    return result;
}

// Operación de clausura de Kleene: N(A*)
std::shared_ptr<NFA> NFA::kleeneStar(std::shared_ptr<NFA> nfa) {
    if (!nfa) return nullptr;
    
    auto result = std::make_shared<NFA>();
    
    // Crear nuevo estado inicial y final
    State newStart = result->addState();
    State newEnd = result->addState();
    
    result->setStartState(newStart);
    result->addFinalState(newEnd);
    
    // Mapear estados del NFA original al resultado
    std::map<State, State> stateMap;
    for (State s : nfa->states) {
        State newState = result->addState();
        stateMap[s] = newState;
    }
    
    // Copiar transiciones del NFA original
    for (const auto& trans : nfa->transitions) {
        State from = stateMap[trans.from];
        State to = stateMap[trans.to];
        result->addTransition(from, trans.symbol, to);
    }
    
    // Añadir epsilon-transición desde el nuevo estado inicial al original
    result->addEpsilonTransition(newStart, stateMap[nfa->startState]);
    
    // Añadir epsilon-transición desde el nuevo estado inicial al nuevo final
    // para permitir cero ocurrencias
    result->addEpsilonTransition(newStart, newEnd);
    
    // Añadir epsilon-transiciones desde los estados finales originales
    // al estado inicial original (para permitir repeticiones)
    for (State finalState : nfa->finalStates) {
        result->addEpsilonTransition(stateMap[finalState], stateMap[nfa->startState]);
    }
    
    // Añadir epsilon-transiciones desde los estados finales originales
    // al nuevo estado final
    for (State finalState : nfa->finalStates) {
        result->addEpsilonTransition(stateMap[finalState], newEnd);
    }
    
    return result;
}

// Operación de clausura positiva: N(A+)
std::shared_ptr<NFA> NFA::plus(std::shared_ptr<NFA> nfa) {
    // A+ = AA*
    if (!nfa) return nullptr;
    
    // Crear copia para la primera A
    auto firstCopy = nfa->clone();
    
    // Crear el A* 
    auto starCopy = kleeneStar(nfa);
    
    // Concatenar A con A*
    return concatenate(firstCopy, starCopy);
}

// Operación de opcionalidad: N(A?)
std::shared_ptr<NFA> NFA::optional(std::shared_ptr<NFA> nfa) {
    if (!nfa) return nullptr;
    
    auto result = std::make_shared<NFA>();
    
    // Crear nuevo estado inicial y final
    State newStart = result->addState();
    State newEnd = result->addState();
    
    result->setStartState(newStart);
    result->addFinalState(newEnd);
    
    // Mapear estados del NFA original al resultado
    std::map<State, State> stateMap;
    for (State s : nfa->states) {
        State newState = result->addState();
        stateMap[s] = newState;
    }
    
    // Copiar transiciones del NFA original
    for (const auto& trans : nfa->transitions) {
        State from = stateMap[trans.from];
        State to = stateMap[trans.to];
        result->addTransition(from, trans.symbol, to);
    }
    
    // Añadir epsilon-transición desde el nuevo estado inicial al original
    result->addEpsilonTransition(newStart, stateMap[nfa->startState]);
    
    // Añadir epsilon-transición desde el nuevo estado inicial al nuevo final
    // para permitir cero ocurrencias
    result->addEpsilonTransition(newStart, newEnd);
    
    // Añadir epsilon-transiciones desde los estados finales originales
    // al nuevo estado final
    for (State finalState : nfa->finalStates) {
        result->addEpsilonTransition(stateMap[finalState], newEnd);
    }
    
    return result;
}

// Método utilitario para verificar si una cadena es aceptada por el NFA
bool NFA::accepts(const std::string& input) {
    std::set<State> currentStates = epsilonClosure({startState});
    
    for (char c : input) {
        currentStates = epsilonClosure(move(currentStates, c));
        if (currentStates.empty()) {
            return false; // No hay estados alcanzables con este carácter
        }
    }
    
    // Verificar si alguno de los estados actuales es un estado final
    for (State s : currentStates) {
        if (std::find(finalStates.begin(), finalStates.end(), s) != finalStates.end()) {
            return true;
        }
    }
    
    return false;
}

// Método para simular el NFA con una cadena de entrada
bool NFA::simulate(const std::string& input) {
    return accepts(input); // Simplemente redirigimos a la implementación existente
}

// Calcular el epsilon-closure de un conjunto de estados
std::set<NFA::State> NFA::epsilonClosure(const std::set<State>& states) {
    std::set<State> result = states;
    std::queue<State> queue;
    
    for (State s : states) {
        queue.push(s);
    }
    
    while (!queue.empty()) {
        State current = queue.front();
        queue.pop();
        
        for (const auto& trans : transitions) {
            if (trans.from == current && trans.symbol == EPSILON) {
                if (result.find(trans.to) == result.end()) {
                    result.insert(trans.to);
                    queue.push(trans.to);
                }
            }
        }
    }
    
    return result;
}

// Calcular el movimiento desde un conjunto de estados con un símbolo
std::set<NFA::State> NFA::move(const std::set<State>& states, char symbol) {
    std::set<State> result;
    
    for (State s : states) {
        for (const auto& trans : transitions) {
            if (trans.from == s && trans.symbol == symbol) {
                result.insert(trans.to);
            }
        }
    }
    
    return result;
}