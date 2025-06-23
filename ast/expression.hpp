#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <memory>
#include <string>
#include "atoms/matchable_atom.hpp"
#include "bin_op.hpp"
#include "un_op.hpp"

// Forward declarations
class BinOp;
class UnOp;

/// Representa un nodo en el árbol de sintaxis abstracta (AST) para una expresión regular.
class Expression {
public:
    enum class Type {
        Atom,
        BinOp,
        UnOp
    };

private:
    Type type;
    
    // Unión para almacenar uno de los tres posibles valores
    union {
        MatchableAtom atom;
        std::shared_ptr<BinOp> binOp;
        std::shared_ptr<UnOp> unOp;
    };
    
    // Constructor privado para inicializar la unión como vacía
    void initEmpty() {
        // No hacer nada, solo para evitar inicialización de valores
    }

public:
    /// Constructor para átomo
    Expression(const MatchableAtom& a) : type(Type::Atom) {
        new (&atom) MatchableAtom(a); // Placement new
    }
    
    /// Constructor para operación binaria
    Expression(const std::shared_ptr<BinOp>& b) : type(Type::BinOp) {
        new (&binOp) std::shared_ptr<BinOp>(b); // Placement new
    }
    
    /// Constructor para operación unaria
    Expression(const std::shared_ptr<UnOp>& u) : type(Type::UnOp) {
        new (&unOp) std::shared_ptr<UnOp>(u); // Placement new
    }
    
    /// Destructor
    ~Expression() {
        switch (type) {
            case Type::Atom:
                atom.~MatchableAtom(); // Llamar al destructor explícitamente
                break;
            case Type::BinOp:
                binOp.~shared_ptr(); // Llamar al destructor explícitamente
                break;
            case Type::UnOp:
                unOp.~shared_ptr(); // Llamar al destructor explícitamente
                break;
        }
    }
    
    /// Constructor de copia
    Expression(const Expression& other) : type(other.type) {
        switch (type) {
            case Type::Atom:
                new (&atom) MatchableAtom(other.atom);
                break;
            case Type::BinOp:
                new (&binOp) std::shared_ptr<BinOp>(other.binOp);
                break;
            case Type::UnOp:
                new (&unOp) std::shared_ptr<UnOp>(other.unOp);
                break;
        }
    }
    
    /// Operador de asignación
    Expression& operator=(const Expression& other) {
        if (this != &other) {
            this->~Expression(); // Destruir el objeto actual
            new (this) Expression(other); // Reconstruir con el otro
        }
        return *this;
    }

    /// Devuelve el tipo de expresión
    Type getType() const {
        return type;
    }

    /// Devuelve un puntero al átomo si esta expresión es un átomo
    const MatchableAtom* getAtom() const {
        return (type == Type::Atom) ? &atom : nullptr;
    }

    /// Devuelve un puntero a la operación binaria si esta expresión es una operación binaria
    const BinOp* getBinOp() const {
        return (type == Type::BinOp) ? binOp.get() : nullptr;
    }

    /// Devuelve un puntero a la operación unaria si esta expresión es una operación unaria
    const UnOp* getUnOp() const {
        return (type == Type::UnOp) ? unOp.get() : nullptr;
    }

    /// Convierte la expresión a una representación de string
    std::string toString() const;
};

// Implementación de BinOp::toString() (requiere Expression)
inline std::string BinOp::toString() const {
    switch (op) {
        case BinaryOperator::Concat:
            return "(" + left->toString() + right->toString() + ")";
        case BinaryOperator::Union:
            return "(" + left->toString() + "|" + right->toString() + ")";
    }
    return ""; // Para evitar advertencias del compilador
}

// Implementación de UnOp::toString() (requiere Expression)
inline std::string UnOp::toString() const {
    switch (op) {
        case UnaryOperator::KleeneStar:
            return operand->toString() + "*";
        case UnaryOperator::Plus:
            return operand->toString() + "+";
        case UnaryOperator::QuestionMark:
            return operand->toString() + "?";
    }
    return ""; // Para evitar advertencias del compilador
}

// Implementación de Expression::toString()
inline std::string Expression::toString() const {
    switch (type) {
        case Type::Atom:
            return atom.toString();
        case Type::BinOp:
            return binOp->toString();
        case Type::UnOp:
            return unOp->toString();
    }
    return ""; // Para evitar advertencias del compilador
}

#endif // EXPRESSION_HPP