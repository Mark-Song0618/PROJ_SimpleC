#pragma once

#include <cstdint>
#include <stdio.h>
#include <string>
#include <map>
#include <queue>
#include <unordered_set>
#include <variant>
#include "../utils/Fsm.hh"

namespace LEX {
/*
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
    // keywords
    IF,
    ELSE,
    WHILE,
    FOR,
    BREAK,
    CONTINUE,
    RETURN,
    CONST,
    TYPEDEF,
    INCLUDE,
    VOID,       
    BOOL,
    CHAR,
    SHORT,
    INT,        
    LONG,
    FLOAT,
    DOUBLE,
    STRUCT,

    // operands 
    ID,
    INTEGERLITERAL,
    FLOATLITERAL,
    STRLITERAL,
    TRUE,
    FALSE,

    // operators
    ADD,
    MINUS,
    MULTI,
    DIV,
    MOD,
    EQUAL,
    GREATER,
    LESS,
    DOT,
    POINTER,

    LOGAND,
    REF,    // BITAND,

    LOGOR,
    BITOR,
    
    LOGNOT,
    BITNOT,

    XOR,

    // punctuation
    PARENTHESESL,
    PARENTHESESR,
    BRACKETL,
    BRACKETR,
    BRACEL,
    BRACER,
    COMMA,
    SEMICOLON,
    SHARP,

    // special
    BAD,
    FEND,
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

    MINUS_PHASE1,
    MINUS,
    POINTER,

    MULTI,
    DIV,
    MOD,
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
    SHARP,

    AND,
    LOGAND,
    REF,    // BITAND,

    OR,
    LOGOR,
    BITOR,
    XOR,
    
    LOGNOT,
    BITNOT,

    INVALID
};

class Cursor {
public:
    Cursor() : _line(0), _col(0) {}

    Cursor(uint64_t line, uint64_t col) :_line(line), _col(col) {} 

    void        moveLeft() {++_col;}

    void        carriageReturn() {_line++; _col = 0;}

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

    template<typename T>
    T                   getValue() { return std::get<T>(_value); }

    int                 currLine() { return _linefile.line(); }

    int                 currColumn() { return _linefile.column(); }

    std::string         dump();

private:
    TokenType           _type;

    std::variant<std::string, float, long long> _value;

    Cursor              _linefile;
};

class MyLexer final : public UTIL::FsmBase<LexState, char> {
public:
    MyLexer() { initFsmHandlers(); }

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

    int     scan(std::string filePath); 

    int     setOutput(std::string outputFile);

    Token   nextToken(bool noPeek = false);

    Token   peekToken(bool reset = false);
    
    TokenType
            keyword(const std::string img);

    std::string
            keywordStr(TokenType);
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

    // FSM functions
    void    changeState(LexState state) { _state = state; }

    void    atStart(char);

    void    atSpace(char);

    void    atIDPhase1(char);

    void    atMinusPhase1(char);

    void    atStrPhase1(char);

    void    atNumPhase1(char);

    void    atNumPhase2(char);

    void    atNumPhase3(char);

    void    atCommPhase1(char);

    void    atCommPhase2(char);
    
    void    atCommPhase3(char);
    
    void    atCommPhase4(char);

    void    atAnd(char);

    void    atOr(char);

    void    errorHandler();

    void    absorb(char c);

private:
    FILE*    _input;

    FILE*    _output;
                       
    Cursor   _curr;

    Cursor   _tkStart;

    bool     _absorbed;

    std::string _cache;

    std::vector<Token> _peek;

    size_t   _peekPos; // the pos after the first token

    static std::map<std::string, TokenType> _kwStr2Type;

    static std::map<TokenType, std::string> _kwType2Str;

    static std::map<LexState, TokenType>    _Lex2Token;

    static std::unordered_set<LexState>     _acceptable;
};

}
