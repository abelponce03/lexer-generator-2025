#include "regex_parser_driver.hpp"
#include <iostream>
#include <sstream>

RegexParserDriver::RegexParserDriver()
    : trace_scanning(false), trace_parsing(false) {
}

RegexParserDriver::~RegexParserDriver() {
}

std::shared_ptr<Expression> RegexParserDriver::parse(const std::string& input) {
    // Reiniciar el estado y errores
    errors.clear();
    result = nullptr;
    
    // Si la entrada está vacía, devolver null directamente
    if (input.empty()) {
        errors.push_back("Error: expresión regular vacía");
        return nullptr;
    }
    
    // Inicializar ubicación
    location.initialize();
    
    try {
        std::cout << "Parseando: '" << input << "'" << std::endl;
        
        // Inicializar nuestro lexer manual
        lexer_init(input);
        
        // Crear el parser
        yy::parser parser(*this);
        
        // Realizar el análisis
        int res = parser.parse();
        
        // Comprobar errores
        if (res != 0 || !errors.empty()) {
            // Manejar errores
            for (const auto& err : errors) {
                std::cerr << err << std::endl;
            }
            return nullptr;
        }
        
        // Verificar que el resultado no sea nullptr antes de devolverlo
        if (!result) {
            std::cerr << "Error: El parser no generó un resultado válido" << std::endl;
            return nullptr;
        }
        
        return result;
    } catch (const std::exception& e) {
        errors.push_back(std::string("Excepción durante el análisis: ") + e.what());
        return nullptr;
    } catch (...) {
        errors.push_back("Error desconocido durante el análisis");
        return nullptr;
    }
}

void RegexParserDriver::error(const yy::location& loc, const std::string& message) {
    std::ostringstream oss;
    oss << "Error en " << loc << ": " << message;
    errors.push_back(oss.str());
}

void RegexParserDriver::error(const std::string& message) {
    errors.push_back(message);
}

void RegexParserDriver::increaseLocation(int count) {
    location.columns(count);
}

void RegexParserDriver::scan_begin() {
    // Preparar el scanner para el análisis
    if (trace_scanning) {
        std::cerr << "Escaneando entrada" << std::endl;
    }
}

void RegexParserDriver::scan_end() {
    // No es necesario hacer nada con nuestro lexer manual
    if (trace_scanning) {
        std::cerr << "Finalizando escaneo" << std::endl;
    }
}

void RegexParserDriver::parse_begin() {
    scan_begin();
}

void RegexParserDriver::parse_end() {
    scan_end();
}