#ifndef NFA_HPP
#define NFA_HPP

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>
#include "../ast/atoms/charset.hpp"

class NFA {
public:
    using State = int;
    static constexpr char EPSILON = '\0';  // Representación para transiciones epsilon
    
    struct Transition {
        State from;
        char symbol;
        State to;
        
        Transition(State f, char s, State t) : from(f), symbol(s), to(t) {}
    };
    
    // Constructor por defecto
    NFA() : nextState(0), startState(0) {}
    
    // Métodos para construcción del NFA
    State addState() {
        State s = nextState++;
        states.push_back(s);
        return s;
    }
    
    void setStartState(State state) {
        startState = state;
    }
    
    void addFinalState(State state) {
        finalStates.push_back(state);
    }
    
    void addTransition(State from, char symbol, State to) {
        transitions.push_back(Transition(from, symbol, to));
    }
    
    void addEpsilonTransition(State from, State to) {
        transitions.push_back(Transition(from, EPSILON, to));
    }
    
    // Métodos para operaciones de Thompson
    static std::shared_ptr<NFA> fromChar(char c);
    static std::shared_ptr<NFA> fromWildcard();
    static std::shared_ptr<NFA> fromCharSet(const CharSet& charset);
    static std::shared_ptr<NFA> concatenate(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second);
    static std::shared_ptr<NFA> alternate(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second);
    static std::shared_ptr<NFA> kleeneStar(std::shared_ptr<NFA> nfa);
    static std::shared_ptr<NFA> plus(std::shared_ptr<NFA> nfa);
    static std::shared_ptr<NFA> optional(std::shared_ptr<NFA> nfa);
    
    // Métodos para simular el NFA
    bool accepts(const std::string& input);
    bool simulate(const std::string& input); // Agregado para compatibilidad
    std::set<State> epsilonClosure(const std::set<State>& states);
    std::set<State> move(const std::set<State>& states, char symbol);
    
    // Método para clonar el NFA
    std::shared_ptr<NFA> clone() const {
        auto copy = std::make_shared<NFA>();
        copy->nextState = nextState;
        copy->startState = startState;
        copy->states = states;
        copy->finalStates = finalStates;
        copy->transitions = transitions;
        return copy;
    }
    
    // Método para depuración
    void print() const;
    
    // Métodos de acceso para LexerNFA
    int getStateCount() const { return states.size(); }
    State getStartState() const { return startState; }
    const std::vector<State>& getFinalStates() const { return finalStates; }
    const std::vector<Transition>& getTransitions() const { return transitions; }
    
private:
    int nextState;
    State startState;
    std::vector<State> states;
    std::vector<State> finalStates;
    std::vector<Transition> transitions;
};

#endif // NFA_HPP