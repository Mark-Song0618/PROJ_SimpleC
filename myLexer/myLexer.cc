#include <string>
#include <unistd.h>
#include <unordered_set>
#include "lexPub.hh"
#include "../utils/Msg.hh"


namespace LEX {
using namespace UTIL;

Token::Token()
: _type(TokenType::BAD), _linefile(0,0) {}

Token::Token(const TokenType type, const std::string& img, const Cursor& linefile)
: _type(type) ,_linefile(linefile)
{
    switch(type) {
    case TokenType::INTEGERLITERAL:
    {
        _value = std::atoi(img.c_str());
        break;
    }
    case TokenType::FLOATLITERAL:
    {
        _value = (float)std::atof(img.c_str());
        break;
    }
    case TokenType::ID:
    case TokenType::STRLITERAL:
    {
        _value = img;
    }
    default:
        break;
    } 
}

std::string
Token::dump() {
    std::string rt;
    rt += "Type: ";
    switch(_type) {
    case TokenType::VOID:
        rt += "VOID";
        break;
    case TokenType::INT:
        rt += "INT";
        break;
    case TokenType::LONG:
        rt += "LONG";
        break;
    case TokenType::FLOAT:
        rt += "FLOAT";
        break;
    case TokenType::DOUBLE:
        rt += "DOUBLE";
        break;
    case TokenType::BOOL:
        rt += "BOOl";
        break;
    case TokenType::STRUCT:
        rt += "STRUCT";
        break;
    case TokenType::CONST:
        rt += "CONST";
        break;
    case TokenType::ID:
        rt += "ID. Value:";
        rt += std::get<std::string>(_value);
        break;
    case TokenType::INTEGERLITERAL:
        {
            rt += "INTEGERLITERAL. Value:";
            rt += std::to_string(std::get<long long>(_value));
            break;
        }
    case TokenType::FLOATLITERAL:
        rt += "FLOATLITERAL. Value:";
        rt += std::to_string(std::get<float>(_value));
        break;
    case TokenType::STRLITERAL:
        rt += "STRLITERAL. Value:";
        rt += std::get<std::string>(_value);
        break;
    case TokenType::ADD:
        rt += "ADD";
        break;
    case TokenType::MINUS:
        rt += "MINUS";
        break;
    case TokenType::MULTI:
        rt += "MULTI";
        break;
    case TokenType::DIV:
        rt += "DIV";
        break;
    case TokenType::EQUAL:
        rt += "EQUAL";
        break;
    case TokenType::GREATER:
        rt += "GREATER";
        break;
    case TokenType::LESS:
        rt += "LESS";
        break;
    case TokenType::IF:
        rt += "IF";
        break;
    case TokenType::ELSE:
        rt += "ELSE";
        break;
    case TokenType::WHILE:
        rt += "WHILE";
        break;
    case TokenType::FOR:
        rt += "FOR";
        break;
    case TokenType::BREAK:
        rt += "BREAK";
        break;
    case TokenType::CONTINUE:
        rt += "CONTINUE";
        break;
    case TokenType::RETURN:
        rt += "RETURN";
        break;
    case TokenType::PARENTHESESL:
        rt += "PARENTHESESRL";
        break;
    case TokenType::PARENTHESESR:
        rt += "PARENTHESESL";
        break;
    case TokenType::BRACKETL:
        rt += "BRACEL";
        break;
    case TokenType::BRACKETR:
        rt += "BRACER";
        break;
    case TokenType::BRACEL:
        rt +="BRACEL";
        break;
    case TokenType::BRACER:
        rt += "BRACER";
        break;
    case TokenType::DOT:
        rt += "DOT";
        break;
    case TokenType::COMMA:
        rt += "COMMA";
        break;
    case TokenType::SEMICOLON:
        rt += "SEMICOLON";
        break;
    }
    rt += ". Line: ";
    rt += std::to_string(_linefile.line());
    rt += ". Col: ";
    rt += std::to_string(_linefile.column());
    return std::move(rt);
}

void
MyLexer::initFsmHandlers()
{
    addTransfunc(LexState::START, std::bind(&MyLexer::atStart, this, std::placeholders::_1));

    addTransfunc(LexState::SPACE, std::bind(&MyLexer::atSpace, this, std::placeholders::_1));

    addTransfunc(LexState::NUM_PHASE1, std::bind(&MyLexer::atNumPhase1, this, std::placeholders::_1));
    addTransfunc(LexState::NUM_PHASE2, std::bind(&MyLexer::atNumPhase2, this, std::placeholders::_1));
    addTransfunc(LexState::NUM_PHASE3, std::bind(&MyLexer::atNumPhase3, this, std::placeholders::_1));

    addTransfunc(LexState::STR_PHASE1, std::bind(&MyLexer::atStrPhase1, this, std::placeholders::_1));
    
    addTransfunc(LexState::ID_PHASE1, std::bind(&MyLexer::atIDPhase1, this, std::placeholders::_1));

    addTransfunc(LexState::COMM_PHASE1, std::bind(&MyLexer::atCommPhase1, this, std::placeholders::_1));
    addTransfunc(LexState::COMM_PHASE2, std::bind(&MyLexer::atCommPhase2, this, std::placeholders::_1));
    addTransfunc(LexState::COMM_PHASE3, std::bind(&MyLexer::atCommPhase3, this, std::placeholders::_1));
    addTransfunc(LexState::COMM_PHASE4, std::bind(&MyLexer::atCommPhase4, this, std::placeholders::_1));
    addTransfunc(LexState::INVALID, std::bind(&MyLexer::errorHandler, this));
}

void
MyLexer::initKeywords()
{
    _keywordTab = 
    {
        {"void",        TokenType::VOID},
        {"int",         TokenType::INT},
        {"long",        TokenType::LONG},
        {"float",       TokenType::FLOAT},
        {"bool",        TokenType::BOOL},
        {"struct",      TokenType::STRUCT},
        {"const",       TokenType::CONST},
        {"if",          TokenType::IF},
        {"else",        TokenType::ELSE},
        {"while",       TokenType::WHILE},
        {"for",         TokenType::FOR},
        {"break",       TokenType::BREAK},
        {"continue",    TokenType::DOUBLE},
        {"return",      TokenType::RETURN},
    };
}

int
MyLexer::scan(std::string filePath)
{
    // check if file exist
    if (access(filePath.c_str(), F_OK)) {
        Msg::instance().error(ERRTYPE::FILE_NOT_EXIST, "source file not exists.");
        return -1;
    }
    
    if (access(filePath.c_str(), R_OK)) {
        Msg::instance().error(ERRTYPE::FILE_NOT_READABLE, "source file not access to read");
        return -1;
    }

    if (!(_input = fopen(filePath.c_str(), "r"))) {
        Msg::instance().error(ERRTYPE::FILE_NOT_OPEN, "can not open source file");
        return -1;
    }

    _absorbed = true;
    _curr = {1,1};
    initState(LexState::START);
    return 0;
}

int
MyLexer::output(std::string outputFile = "")
{
    if (outputFile.empty()) {
        _output = stdout;
        return 0;
    } 

    if (access(outputFile.c_str(), F_OK)) {
        Msg::instance().warn(ERRTYPE::FILE_EXIST, "file exists. Might be overwritten.");
        return -1;
    }

    if (!(_output = fopen(outputFile.c_str(), "rw+"))) {
        Msg::instance().error(ERRTYPE::FILE_NOT_WRITABLE, "cannot open file to write");
        return -1;
    }
    return 0;
}

Token
MyLexer::nextToken()
{
    // fsm 
    static char c;
    static bool finished = false;
    Token tk;
    while (!finished) {
        if (_absorbed) {
            c = getNextInput();
            _absorbed = false;
            finished = (c == -1);
        }

        toNextState(c);
        if (stateAcceptable()) {
            tk = extractToken();
            reset();
            if (_output) {
                fprintf(_output, "%s\n", tk.dump().c_str());
            }
            return tk; 
        } else if (stateIgnore()) {
            reset();
        } else if (stateInvalid()) {
            errorHandler();
        }
    }
}

char 
MyLexer::getNextInput()
{
    char c = fgetc(_input);
    if (c == '\n') {
        _curr.carriageRetur();
    } else {
        _curr.moveLeft();
    }

    return c;
}

void
MyLexer::toNextState(char c)
{
    _transTab[_state](c);
    return;
}

bool
MyLexer::stateAcceptable()
{
    static std::unordered_set<LexState> acceptable = 
    {
        LexState::START,
        LexState::INT,
        LexState::FLOAT, 
        LexState::STR,
        LexState::ID,
        LexState::ADD,
        LexState::MINUS,
        LexState::MULTI,
        LexState::DIV,
        LexState::EQUAL,
        LexState::GREATER,
        LexState::LESS,
        LexState::PARENTHESESL,
        LexState::PARENTHESESR,
        LexState::BRACKETL,
        LexState::BRACKETR,
        LexState::BRACEL,
        LexState::BRACER,
        LexState::DOT,
        LexState::COMMA,
        LexState::SEMICOLON,
        LexState::REF,
    };
    return acceptable.find(_state) != acceptable.end();
}

bool
MyLexer::stateInvalid()
{
    return _state == LexState::INVALID;
}

bool
MyLexer::stateIgnore()
{
    return _state == LexState::SPACEEND || _state == LexState::COMMENT;
}

Token
MyLexer::extractToken()
{
    Token tk;
    if (stateAcceptable()) {
        if (isKeyword(_cache)) {
            tk = Token(_keywordTab[_cache], "", _tkStart);
        } else {
            switch (_state) {
            case LexState::INT:
                tk = Token(TokenType::INT, _cache, _tkStart);
                break;
            case LexState::FLOAT:
                tk = Token(TokenType::FLOAT, _cache, _tkStart);
                break;
            case LexState::STR:
                tk = Token(TokenType::STRLITERAL, _cache, _tkStart);
                break;
            case LexState::ID:
                tk = Token(TokenType::ID, _cache, _tkStart);
                break;
            case LEX::LexState::ADD:
                tk = Token(TokenType::ADD, "", _tkStart);
                break;
            case LEX::LexState::MINUS:
                tk = Token(TokenType::MINUS, "", _tkStart);
                break;
            case LEX::LexState::MULTI:
                tk = Token(TokenType::MULTI, "", _tkStart);
                break;
            case LEX::LexState::DIV:
                tk = Token(TokenType::DIV, "", _tkStart);
                break;
            case LEX::LexState::EQUAL:
                tk = Token(TokenType::EQUAL, "", _tkStart);
                break;
            case LEX::LexState::GREATER:
                tk = Token(TokenType::GREATER, "", _tkStart);
                break;
            case LEX::LexState::LESS:
                tk = Token(TokenType::LESS, "", _tkStart);
                break;
            case LEX::LexState::PARENTHESESL:
                tk = Token(TokenType::PARENTHESESL, "", _tkStart);
                break;
            case LEX::LexState::PARENTHESESR:
                tk = Token(TokenType::PARENTHESESR, "", _tkStart);
                break;
            case LEX::LexState::BRACKETL:
                tk = Token(TokenType::BRACKETL, "", _tkStart);
                break;
            case LEX::LexState::BRACKETR:
                tk = Token(TokenType::BRACKETR, "", _tkStart);
                break;
            case LEX::LexState::BRACEL:
                tk = Token(TokenType::BRACEL, "", _tkStart);
                break;
            case LEX::LexState::BRACER:
                tk = Token(TokenType::BRACER, "", _tkStart);
                break;
            case LEX::LexState::DOT:
                tk = Token(TokenType::DOT, "", _tkStart);
                break;
            case LEX::LexState::COMMA:
                tk = Token(TokenType::COMMA, "", _tkStart);
                break;
            case LEX::LexState::SEMICOLON:
                tk = Token(TokenType::SEMICOLON, "", _tkStart);
                break;
            case LEX::LexState::REF:
                tk = Token(TokenType::REF, "", _tkStart);
                break;
            defalse:
                break;
            }
        }
        
    } else {
        errorHandler();
    }
    return tk;
}

void
MyLexer::reset()
{
    _cache.clear();
    _state = LexState::START;
}
bool
MyLexer::isAlpha(char c)
{
    if (c >= 'a' && c<= 'z') {
        return true;
    }
    if (c >= 'A' && c<= 'Z') {
        return true;
    }
    return false;
}

bool 
MyLexer::isNum(char c)
{
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

bool
MyLexer::isSpace(char c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == EOF) {
        return true;
    }
    return false;
}

bool
MyLexer::isPunct(char c)
{
    static std::unordered_set<char> _puncts =
    {
        '+',
        '-',
        '*',
        '\\',
        ',',
        '.',
        ';',
        '&',
        '(',
        ')',
        '[',
        ']',
        '{',
        '}',
    };
    return _puncts.find(c) != _puncts.end();
}

bool
MyLexer::isSplitter(char c)
{
    if (isSpace(c) || isPunct(c)) {
        return true;
    }
    return false;
}

bool
MyLexer::isKeyword(const std::string& word)
{
    if (_keywordTab.find(word) != _keywordTab.end()) {
        return true;
    }
    return false;
}

TokenType
MyLexer::keyword(const std::string img)
{
    if (isKeyword(img)) {
        return _keywordTab[img];
    } else {
        return {};
    }
}

void
MyLexer::atStart(char c)
{
    if (isSpace(c)) {
        changeState(LexState::SPACE);
    } else if (isAlpha(c) || c == '_') {
        changeState(LexState::ID_PHASE1);
    } else if (isNum(c)) {
        if (c == '0') 
            changeState(LexState::NUM_PHASE1);
        else
            changeState(LexState::NUM_PHASE2);
    } else if (c == '/') {
        changeState(LexState::COMM_PHASE1);
    } else if (c=='"') {
        changeState(LexState::STR_PHASE1);
    } else if (c == '+') {
        changeState(LexState::ADD);
    } else if (c == '-') {
        changeState(LexState::MINUS);
    } else if (c == '*') {
        changeState(LexState::MULTI);
    } else if (c == '=') {
        changeState(LexState::EQUAL);
    } else if (c == '<') {
        changeState(LexState::LESS);
    } else if (c == '>') {
        changeState(LexState::GREATER);
    } else if (c == '(') {
        changeState(LexState::PARENTHESESL);
    } else if (c == ')') {
        changeState(LexState::PARENTHESESR);
    } else if (c == '[') {
        changeState(LexState::BRACKETL);
    } else if (c == ']') {
        changeState(LexState::BRACKETR);
    } else if (c == '{') {
        changeState(LexState::BRACEL);
    } else if (c == '}') {
        changeState(LexState::BRACER);
    } else if (c == '.') {
        changeState(LexState::DOT);
    } else if (c == ',') {
        changeState(LexState::COMMA);
    } else if (c == ';') {
        changeState(LexState::SEMICOLON);
    } else if (c == '&') {
        changeState(LexState::REF);
    }

    _tkStart = _curr;
    absorb(c);
    return;
}

void
MyLexer::atSpace(char c)
{
    if (isSpace(c)) {
        absorb(c);
    } else {
        changeState(LexState::SPACEEND);
    }
}

void
MyLexer::atIDPhase1(char c)
{
    if (isNum(c) || isAlpha(c)) {
        absorb(c);
    } else if (isSplitter(c)) {
        changeState(LexState::ID);
    } else {
        changeState(LexState::INVALID);
        absorb(c);
    }
}

void
MyLexer::atStrPhase1(char c) {
    if (c == '"') {
        changeState(LexState::STR);
    } else if (c == EOF) {
        changeState(LexState::INVALID);
    } 
    absorb(c);
    return;
}

void
MyLexer::atNumPhase1(char c) {
    if (c == '.') {
        changeState(LexState::NUM_PHASE3);
        absorb(c);
    } else if (isSplitter(c)) {
        changeState(LexState::INT);
    } else {
        changeState(LexState::INVALID);
        absorb(c);
    }
}

void
MyLexer::atNumPhase2(char c) {
    if (c == '.') {
        changeState(LexState::NUM_PHASE3);
        absorb(c);
    } else if (isSplitter(c)) {
        changeState(LexState::INT);
    } else {
        changeState(LexState::INVALID);
        absorb(c);
    }
}

void
MyLexer::atNumPhase3(char c) {
    if (isNum(c)) {
        absorb(c);
    } else if (isSplitter(c)) {
        changeState(LexState::FLOAT);
    } else {
        absorb(c);
        changeState(LexState::INVALID);
    }
}

void
MyLexer::atCommPhase1(char c)
{
    if (c == '/') {
        changeState(LexState::COMM_PHASE2);
        absorb(c);
    } else if (c == '*') {
        changeState(LexState::COMM_PHASE3);
        absorb(c);
    } else {
        changeState(LexState::DIV);
    }
}

void
MyLexer::atCommPhase2(char c)
{
    if (c == '\n' || c == EOF) {
        changeState(LexState::COMMENT);
    } else {
        absorb(c);
    }
}

void
MyLexer::atCommPhase3(char c)
{
    if (c == '*') {
        changeState(LexState::COMM_PHASE4);
    }
    absorb(c);
}

void
MyLexer::atCommPhase4(char c)
{
    if (c == '/') {
        changeState(LexState::COMMENT);
    } else if (c != '*') {
        changeState(LexState::COMM_PHASE3);
        absorb(c);
    }
}

void
MyLexer::errorHandler()
{

    std::string msg;
    msg += "Line: ";
    msg += std::to_string(_curr.line());
    msg += ", Col: ";
    msg += std::to_string(_curr.column());
    msg += "\n";
    Msg::instance().error(ERRTYPE::UNREC_TOKEN, msg);
}

void
MyLexer::absorb(char c)
{
    _cache += c;
    _absorbed = true;
}

}