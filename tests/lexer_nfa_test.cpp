#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include "../../regex/regex_parser_driver.hpp"
#include "../bob_construye_nfa.hpp"
#include "../lexer_nfa.hpp"

void imprimirEstadosTransiciones(const std::shared_ptr<NFA>& nfa) {
    std::cout << "Estados: " << nfa->getStateCount() << std::endl;
    std::cout << "Estado inicial: " << nfa->getStartState() << std::endl;
    std::cout << "Estados finales: ";
    for (auto s : nfa->getFinalStates()) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Transiciones:" << std::endl;
    for (const auto& t : nfa->getTransitions()) {
        std::cout << t.from << " --";
        if (t.symbol == NFA::EPSILON) {
            std::cout << "ε";
        } else {
            std::cout << "'" << t.symbol << "'";
        }
        std::cout << "--> " << t.to << std::endl;
    }
}

void probarToken(const std::shared_ptr<NFA>& nfa, const std::vector<std::string>& ejemplosPositivos, 
                 const std::vector<std::string>& ejemplosNegativos, const std::string& nombre) {
    std::cout << "\n=== Probando token: " << nombre << " ===" << std::endl;
    
    for (const auto& ejemplo : ejemplosPositivos) {
        bool acepta = nfa->simulate(ejemplo);
        std::cout << "Ejemplo '" << ejemplo << "': " << (acepta ? "ACEPTADO ✓" : "RECHAZADO ✗ (debería aceptar)") << std::endl;
    }
    
    for (const auto& ejemplo : ejemplosNegativos) {
        bool acepta = nfa->simulate(ejemplo);
        std::cout << "Ejemplo '" << ejemplo << "': " << (!acepta ? "RECHAZADO ✓" : "ACEPTADO ✗ (debería rechazar)") << std::endl;
    }
}

int main() {
    std::cout << "== Probando LexerNFA ==" << std::endl;
    
    // Definimos patrones para nuestro lexer
    std::vector<std::tuple<std::string, std::string, bool>> patterns = {
        std::make_tuple("NUMERO", "[0-9]+", false),
        std::make_tuple("ID", "[a-zA-Z][a-zA-Z0-9_]*", false),
        std::make_tuple("OPERADOR", "[\\+\\-\\*/]", false),
        std::make_tuple("ESPACIO", "[ \\t\\n]+", true)  // ignorable
    };
    
    // Creamos un RegexParserDriver para analizar expresiones regulares
    RegexParserDriver driver;
    
    // Creamos un constructor de NFAs
    NfaBuild builder;
    
    // Creamos un LexerNFA
    LexerNFA lexerNfa;
    
    // Para cada patrón, crear un NFA y añadirlo al lexer
    for (const auto& pattern : patterns) {
        std::string nombre = std::get<0>(pattern);
        std::string regex = std::get<1>(pattern);
        bool ignorable = std::get<2>(pattern);
        
        std::cout << "\nProcesando token: " << nombre << " con regex: " << regex << std::endl;
        
        // Parsear la expresión regular
        auto expr = driver.parse(regex);
        if (!expr) {
            std::cerr << "Error al parsear la expresión regular: " << regex << std::endl;
            continue;
        }
        
        // Construir el NFA para esta expresión
        auto nfa = builder.buildFromExpression(expr);
        
        // Probar este NFA individualmente
        if (nombre == "NUMERO") {
            probarToken(nfa, {"0", "123", "9999"}, {"", "a", "12a"}, nombre);
        } else if (nombre == "ID") {
            probarToken(nfa, {"x", "var1", "nombre_largo"}, {"", "1var", "_var"}, nombre);
        } else if (nombre == "OPERADOR") {
            probarToken(nfa, {"+", "-", "*", "/"}, {"", "++", "a"}, nombre);
        } else if (nombre == "ESPACIO") {
            probarToken(nfa, {" ", "\t", "\n", "  \t\n"}, {"", "a", "1"}, nombre);
        }
        
        // Añadir este NFA al lexer
        lexerNfa.addPattern(nombre, nfa, ignorable);
    }
    
    // Construir el NFA combinado
    lexerNfa.build();
    
    // Obtener el NFA combinado
    auto combinedNfa = lexerNfa.getCombinedNfa();
    
    // Imprimir información del NFA combinado
    std::cout << "\n=== NFA combinado ===" << std::endl;
    imprimirEstadosTransiciones(combinedNfa);
    
    std::cout << "\n=== Tokens asociados a estados finales ===" << std::endl;
    for (const auto& entry : lexerNfa.getFinalStateToToken()) {
        NFA::State estado = entry.first;
        auto tokenInfo = entry.second;
        
        std::cout << "Estado " << estado << ": Token '" << tokenInfo.first 
                  << "' (Ignorable: " << (tokenInfo.second ? "Sí" : "No") << ")" << std::endl;
    }
    
    std::cout << "\nPrueba finalizada." << std::endl;
    return 0;
}