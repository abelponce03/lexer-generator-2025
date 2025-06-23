#include "lexer_dfa.hpp"
#include <queue>
#include <iostream>
#include <set>
#include <algorithm>

void LexerDFA::build(const LexerNFA& nfa) {
    // Inicializar estado inicial y estructuras de datos
    startState = 0;
    transitions.clear();
    finalStates.clear();
    
    // Mapeo de conjuntos de estados del NFA a estados del DFA
    std::map<std::set<NFA::State>, int> nfaSetToDfaState;
    
    // Estado inicial del DFA: epsilon-clausura del estado inicial del NFA
    std::set<NFA::State> initialSet;
    initialSet.insert(nfa.getCombinedNfa()->getStartState());
    std::set<NFA::State> initialClosure = nfa.getCombinedNfa()->epsilonClosure(initialSet);
    
    // Asignar el primer estado del DFA
    nfaSetToDfaState[initialClosure] = startState;
    
    // Cola de conjuntos de estados pendientes de procesar
    std::queue<std::set<NFA::State>> pendingStateSets;
    pendingStateSets.push(initialClosure);
    
    // Determinar si el estado inicial es final
    for (NFA::State state : initialClosure) {
        auto it = nfa.getFinalStateToToken().find(state);
        if (it != nfa.getFinalStateToToken().end()) {
            finalStates[startState] = it->second;
            break;  // Solo un token por estado DFA
        }
    }
    
    // Recolectar solo los caracteres realmente usados en el NFA
    std::set<char> usedChars;
    for (const auto& trans : nfa.getCombinedNfa()->getTransitions()) {
        if (trans.symbol != NFA::EPSILON) {
            usedChars.insert(trans.symbol);
        }
    }
    
    std::cout << "Construyendo DFA usando " << usedChars.size() << " caracteres distintos..." << std::endl;
    
    // Límite de seguridad para evitar procesar demasiados estados
    const int MAX_STATES = 1000;
    int processedStates = 0;
    
    // Procesar cada conjunto de estados pendientes
    while (!pendingStateSets.empty() && processedStates < MAX_STATES) {
        // Mostrar progreso cada 10 estados
        if (processedStates % 10 == 0) {
            std::cout << "Procesados " << processedStates << " estados, quedan " 
                      << pendingStateSets.size() << " en la cola..." << std::endl;
        }
        
        processedStates++;
        std::set<NFA::State> currentStateSet = pendingStateSets.front();
        pendingStateSets.pop();
        
        int currentDfaState = nfaSetToDfaState[currentStateSet];
        
        // Solo iterar por los caracteres realmente usados
        for (char c : usedChars) {
            if (c == NFA::EPSILON) continue;  // Ignorar transiciones epsilon
            
            // Calcular estados alcanzables con este carácter
            std::set<NFA::State> reachableStates = nfa.getCombinedNfa()->move(currentStateSet, c);
            
            if (reachableStates.empty()) continue;
            
            // Calcular epsilon-clausura de los estados alcanzables
            std::set<NFA::State> nextStateSet = nfa.getCombinedNfa()->epsilonClosure(reachableStates);
            
            if (nextStateSet.empty()) continue;
            
            // Si este conjunto no está mapeado aún, crear un nuevo estado DFA
            if (nfaSetToDfaState.find(nextStateSet) == nfaSetToDfaState.end()) {
                int newDfaState = nfaSetToDfaState.size();
                nfaSetToDfaState[nextStateSet] = newDfaState;
                
                // Verificar si este nuevo estado es final
                for (NFA::State state : nextStateSet) {
                    auto it = nfa.getFinalStateToToken().find(state);
                    if (it != nfa.getFinalStateToToken().end()) {
                        finalStates[newDfaState] = it->second;
                        break;  // Solo un token por estado DFA (resolución de conflictos)
                    }
                }
                
                // Añadir a la cola para procesar
                pendingStateSets.push(nextStateSet);
            }
            
            // Añadir transición al DFA
            transitions[currentDfaState][c] = nfaSetToDfaState[nextStateSet];
        }
    }
    
    if (processedStates >= MAX_STATES) {
        std::cout << "¡ADVERTENCIA! Se alcanzó el límite de estados (" << MAX_STATES 
                  << "). El DFA puede estar incompleto." << std::endl;
    }
    
    std::cout << "DFA construido con " << nfaSetToDfaState.size() << " estados y " 
              << finalStates.size() << " estados finales." << std::endl;
    
    // Minimizar el DFA
    minimizeDFA();
    
    std::cout << "DFA minimizado, quedaron " << transitions.size() << " estados y "
              << finalStates.size() << " estados finales." << std::endl;
}

std::vector<Lexeme> LexerDFA::scan(const std::string& input) {
    std::vector<Lexeme> tokens;
    size_t position = 0;
    
    while (position < input.length()) {
        Match match = findLongestMatch(input, position);
        
        if (match.length == 0) {
            // Comprobar si es un espacio en blanco y ignorarlo silenciosamente
            if (input[position] == ' ' || input[position] == '\t' || 
                input[position] == '\n' || input[position] == '\r') {
                position++; // Simplemente avanzar para espacios
                continue;
            }
            
            // Error: no se encontró ninguna coincidencia
            std::cerr << "Error léxico en posición " << position 
                      << ": carácter inesperado '" << input[position] << "'" << std::endl;
            position++; // Avanzar para evitar bucle infinito
            continue;
        }
        
        position += match.length;
        
        if (!match.isIgnorable) {
            Lexeme token;
            token.kind = match.tokenName;
            token.text = input.substr(position - match.length, match.length);
            token.position = position - match.length;
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

Match LexerDFA::findLongestMatch(const std::string& input, size_t startPos) {
    int state = startState;
    size_t longestMatchLength = 0;
    std::string longestMatchToken;
    bool longestMatchIsIgnorable = false;
    
    size_t currentPos = startPos;
    
    // Intentar hacer la coincidencia más larga posible
    while (currentPos < input.length()) {
        char c = input[currentPos];
        
        // Verificar si hay una transición para este carácter
        auto stateIt = transitions.find(state);
        if (stateIt != transitions.end()) {
            auto transIt = stateIt->second.find(c);
            if (transIt != stateIt->second.end()) {
                // Hay una transición, actualizar estado
                state = transIt->second;
                currentPos++;
                
                // Verificar si el nuevo estado es final
                auto finalIt = finalStates.find(state);
                if (finalIt != finalStates.end()) {
                    // Actualizar el match más largo
                    longestMatchLength = currentPos - startPos;
                    longestMatchToken = finalIt->second.first;
                    longestMatchIsIgnorable = finalIt->second.second;
                }
                
                continue;
            }
        }
        
        // No hay más transiciones posibles
        break;
    }
    
    Match match;
    match.tokenName = longestMatchToken;
    match.length = longestMatchLength;
    match.isIgnorable = longestMatchIsIgnorable;
    
    return match;
}

void LexerDFA::minimizeDFA() {
    // Recolectar todos los estados
    std::set<int> allStates;
    for (const auto& trans : transitions) {
        allStates.insert(trans.first);
        for (const auto& symTrans : trans.second) {
            allStates.insert(symTrans.second);
        }
    }
    
    // Crear la partición inicial: estados de aceptación vs. no aceptación
    // y subdividir por el token que aceptan
    std::map<std::pair<bool, std::string>, std::set<int>> tokenGroups;
    for (int state : allStates) {
        auto it = finalStates.find(state);
        if (it != finalStates.end()) {
            // Estado de aceptación
            std::string tokenName = it->second.first;
            bool isIgnorable = it->second.second;
            tokenGroups[{true, tokenName + (isIgnorable ? "_ignore" : "")}].insert(state);
        } else {
            // Estado de no aceptación
            tokenGroups[{false, ""}].insert(state);
        }
    }
    
    std::vector<std::set<int>> partition;
    for (const auto& group : tokenGroups) {
        partition.push_back(group.second);
    }
    
    // Refinamiento de la partición
    bool changed;
    do {
        changed = false;
        std::vector<std::set<int>> newPartition;
        
        for (const auto& group : partition) {
            if (group.size() <= 1) {
                newPartition.push_back(group);
                continue;
            }
            
            // Mapa para agrupar estados por su comportamiento
            std::map<std::map<char, int>, std::set<int>> behaviorGroups;
            
            for (int state : group) {
                // Mapear cada carácter de entrada al índice del grupo al que va el estado
                std::map<char, int> behavior;
                
                auto stateIt = transitions.find(state);
                if (stateIt != transitions.end()) {
                    for (const auto& symTrans : stateIt->second) {
                        char c = symTrans.first;
                        int targetState = symTrans.second;
                        
                        // Encontrar a qué grupo pertenece el estado destino
                        for (size_t i = 0; i < partition.size(); i++) {
                            if (partition[i].find(targetState) != partition[i].end()) {
                                behavior[c] = i;
                                break;
                            }
                        }
                    }
                }
                
                // Agrupar estados con el mismo comportamiento
                behaviorGroups[behavior].insert(state);
            }
            
            // Si se ha dividido el grupo, actualizar la partición
            if (behaviorGroups.size() > 1) {
                changed = true;
                for (const auto& subgroup : behaviorGroups) {
                    newPartition.push_back(subgroup.second);
                }
            } else {
                // El grupo no se pudo dividir
                newPartition.push_back(group);
            }
        }
        
        // Actualizar la partición
        partition = newPartition;
    } while (changed);
    
    // Construir el DFA minimizado
    // Mapear estados originales a nuevos estados
    std::map<int, int> stateMap;
    for (size_t i = 0; i < partition.size(); i++) {
        for (int state : partition[i]) {
            stateMap[state] = i;
        }
    }
    
    // Reconstruir transiciones y estados finales
    std::map<int, std::map<char, int>> newTransitions;
    std::map<int, std::pair<std::string, bool>> newFinalStates;
    
    for (const auto& trans : transitions) {
        int oldState = trans.first;
        if (stateMap.find(oldState) == stateMap.end()) continue;
        int newState = stateMap[oldState];
        
        for (const auto& symTrans : trans.second) {
            char c = symTrans.first;
            int oldTarget = symTrans.second;
            if (stateMap.find(oldTarget) == stateMap.end()) continue;
            int newTarget = stateMap[oldTarget];
            
            newTransitions[newState][c] = newTarget;
        }
    }
    
    for (const auto& final : finalStates) {
        int oldState = final.first;
        if (stateMap.find(oldState) == stateMap.end()) continue;
        int newState = stateMap[oldState];
        
        // Si ya hay un estado final con este nuevo ID, no importa cuál elegimos
        // ya que son equivalentes
        if (newFinalStates.find(newState) == newFinalStates.end()) {
            newFinalStates[newState] = final.second;
        }
    }
    
    // Actualizar estado inicial
    int newStartState = stateMap[startState];
    
    // Reemplazar el DFA original
    transitions = newTransitions;
    finalStates = newFinalStates;
    startState = newStartState;
}