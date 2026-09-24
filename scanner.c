#include<stdio.h>
#include<string.h>
#include "common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* curr;
    int line;
} Scanner;

Scanner scn;

static bool isAtEnd(){
    return *scn.curr == '\0';
}

static char advance(){
    scn.curr++;
    return scn.curr[-1];
}

static bool match(char expected){
    if(isAtEnd()) return false;
    if(*scn.curr != expected) return false;
    scn.curr++;
    return true;
}

static char peek(){
    return *scn.curr;
}

static char peekNext(){
    if(isAtEnd()) return '\0';
    return scn.curr[1];
}

static void skipWhitespace(){
    for(;;){
        char ch = peek();
        switch(ch){
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scn.line++;
                advance();
                break;
            case '/':
                if(peekNext() == '/'){
                    while(peek() != '\n' && !isAtEnd()) advance();
                } 
                else return;
                break;  
            default:
                return;
        }
    }
}

static bool isDigit(char ch){
    return ch >= '0' && ch <= '9';
}

static bool isAlpha(char ch){
    return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static Token errorToken(const char* msg){
    Token token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.length = (int)strlen(msg);
    token.line = scn.line;
    return token;
}

static Token makeToken(TokenType type){
    Token token;
    token.type = type;
    token.start = scn.start;
    token.length = (int)(scn.curr - scn.start);
    token.line = scn.line;
    return token;
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type){
    if(scn.curr - scn.start == start + length && memcmp(scn.start + start, rest, length) == 0){
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type(){
    switch(scn.start[0]){
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
        case 'f': 
            if(scn.curr - scn.start > 1){
                switch(scn.start[1]){
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 't':
            if(scn.curr - scn.start > 1){
                switch(scn.start[1]){
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
    }
    return TOKEN_IDENTIFIER;
}

static Token string(){
    while(peek() != '"' && !isAtEnd()){
        if(peek() == '\n') scn.line++;
        advance();
    }
    if(isAtEnd()) return errorToken("Unterminated String.");
    advance();
    return makeToken(TOKEN_STRING);
}

static Token number(){
    while(isDigit(peek())) advance();
    if(peek() == '.' && isDigit(peekNext())){
        advance();
        while(isDigit(peek())) advance();
    }
    return makeToken(TOKEN_NUMBER);
}

static Token identifier(){
    while(isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifier_type());
}

void initScanner(const char* src){
    scn.start = src;
    scn.curr = src;
    scn.line = 1;  
}

Token scanToken(){
    skipWhitespace();
    scn.start = scn.curr;
    if(isAtEnd()) return makeToken(TOKEN_EOF);  
    char ch = advance();
    if(isAlpha(ch)) return identifier();
    if(isDigit(ch)) return number();
    switch(ch){
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!': return makeToken(match('=')? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return makeToken(match('=')? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '>': return makeToken(match('=')? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '<': return makeToken(match('=')? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '"': return string();
    }
    return errorToken("Unexpected character.");
}