#pragma once

#include <cstdint>
#include <stdio.h>
#include <string>
#include <map>
#include <variant>
#include "../utils/Fsm.hh"
namespace LEX {
/*
 * VOID
 *
 * ID           := [_a-zA-Z][_a-zA-Z0-9]*
 *
 * STRLITERAL   := \"[:]\"
 *
 * NUMLITERAL   := INTEGER(.[0-9]+)? 
 *
 * INTEGER      := [1-9][0-9]*
 *
 * COMMENT1     := \/\/[:]*"\n"
 *
 * COMMENT2     := \/\*[:]*\*\/
 */
enum class TokenType {
    // type
    VOID,       // void
    INT,        // int
    LONG,
    FLOAT,
    DOUBLE,
    BOOL,
    STRUCT,
    CONST,

    // OPERAND
    ID,
    INTEGERLITERAL,
    FLOATLITERAL,
    STRLITERAL,

    // operators
    ADD,
    MINUS,
    MULTI,
    DIV,
    EQUAL,
    GREATER,
    LESS,

    // keywords
    IF,
    ELSE,
    WHILE,
    FOR,
    BREAK,
    CONTINUE,
    RETURN,

    // punctuation
    PARENTHESESL,
    PARENTHESESR,
    BRACKETL,
    BRACKETR,
    BRACEL,
    BRACER,
    DOT,
    COMMA,
    SEMICOLON,
    REF,

    // special
    FEOF,
    BAD,
};

enum class LexState {
    START,

    SPACE,
    SPACEEND,

    NUM_PHASE1,
    NUM_PHASE2,
    NUM_PHASE3,
    INT,
    FLOAT, 

    STR_PHASE1,
    STR,

    ID_PHASE1,
    ID,

    COMM_PHASE1,
    COMM_PHASE2,
    COMM_PHASE3,
    COMM_PHASE4,
    COMMENT,

    ADD,
    MINUS,
    MULTI,
    DIV,
    EQUAL,
    GREATER,
    LESS,
    PARENTHESESL,
    PARENTHESESR,
    BRACKETL,
    BRACKETR,
    BRACEL,
    BRACER,
    DOT,
    COMMA,
    SEMICOLON,
    REF,

    INVALID
};

class Cursor {
public:
    Cursor() : _line(0), _col(0) {}

    Cursor(uint64_t line, uint64_t col) :_line(line), _col(col) {} 

    void        moveLeft() {++_col;}

    void        carriageRetur() {_line++; _col = 0;}

    uint64_t    line() { return _line; }

    uint64_t    column() { return _col; }

private:
    uint64_t    _line;

    uint64_t    _col;
};

class Token {
public:
    explicit Token(); 

    explicit Token(const TokenType, const std::string&, const Cursor&);

    TokenType           getType() {return _type;}

    std::string         dump();

private:
    TokenType           _type;

    std::variant<std::string, float, long long> _value;

    Cursor              _linefile;
};

class MyLexer final : public UTIL::FsmBase<LexState, char> {
public:
    MyLexer() { initFsmHandlers(); initKeywords(); }

    ~MyLexer() {
        if (_output) {
            if (_output != stdout && _output != stderr) {
                fclose(_output);
            }
        }

        if (_input) {
            if (_input != stdout && _input != stderr) {
                fflush(_input);
                fclose(_input);
            }
        }
    }

    void    initFsmHandlers();

    void    initKeywords();

    int     scan(std::string filePath); 

    int     output(std::string outputFile);

    Token   nextToken();
    
private:
    void    toNextState(char c) override;

    char    getNextInput() override;
   
    Token   extractToken();
    
    void    reset();

    bool    stateAcceptable() override;

    bool    stateInvalid() override;

    bool    stateIgnore() override;

    bool    isAlpha(char);

    bool    isNum(char);

    bool    isSpace(char);

    bool    isPunct(char);

    bool    isSplitter(char);

    bool    isKeyword(const std::string&);

    TokenType
            keyword(const std::string img);

    // FSM functions
    void    changeState(LexState state) { _state = state; }

    void    atStart(char);

    void    atSpace(char);

    void    atIDPhase1(char);

    void    atStrPhase1(char);

    void    atNumPhase1(char);

    void    atNumPhase2(char);

    void    atNumPhase3(char);

    void    atCommPhase1(char);

    void    atCommPhase2(char);
    
    void    atCommPhase3(char);
    
    void    atCommPhase4(char);

    void    errorHandler();

    void    absorb(char c);

private:
    FILE*    _input;

    FILE*    _output;  // verbose
                       //
    Cursor   _curr;

    Cursor   _tkStart;

    bool     _absorbed;

    std::string _cache;

    std::map<std::string, TokenType> _keywordTab;
};

}
