//===--------------------------------------------------------------------------------------------===
// parser.c - Implements the SAX JSON parser
// This source is part of JSOX
//
// Created on 2019-09-07 by Amy Parent <amy@amyparent.com>
// Copyright (c) 2019 Amy Parent
// Licensed under the MIT License
// =^•.•^=
//===--------------------------------------------------------------------------------------------===
#include <jsox/jsox.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct JSOX {
    const char* current;
    JSOXReceiver rec;
    const char* error;
    void* userData;
} JSOX;

static inline bool isWhitespace(char c) { return c == 0x20 || c == 0x0a || c == 0x0d || c == 0x09; }
static inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
static inline bool have(JSOX* js, char c) { return *js->current == c; }
static inline char peek(JSOX* js) { return js->current ? *js->current : '\0'; }

static char eat(JSOX* js) {
    if(js->error) return '\0';
    char c = peek(js);
    if(c) js->current += 1;
    return c;
}

static void eatWhitespace(JSOX* js) {
    if(js->error) return;
    while(*js->current && isWhitespace(*js->current))
        eat(js);
}

static bool match(JSOX* js, char c) {
    if(js->error) return false;
    if(have(js, c)) {
        eat(js);
        return true;
    }
    return false;
}

static bool expect(JSOX* js, char c) {
    if(js->error) return false;
    if(match(js, c)) return true;
    js->error = "unexpected character";
    return false;
}

// MARK: - recursive descent ahoy
static void value(JSOX* js);

static void string(JSOX* js, bool isKey) {
    if(js->error) return;
    if(!expect(js, '"')) return;
    // TODO: this is more complex and we must implement escape sequences
    const char* start = js->current;
    while(peek(js) != '"') {
        js->current += 1;
    }
    
    const char* end = js->current;
    if(!expect(js, '"')) return;
    if(isKey)
        js->rec.onKey(start, end - start, js->userData);
    else
        js->rec.onString(start, end - start, js->userData);
}

static void member(JSOX* js) {
    if(js->error) return;
    
    eatWhitespace(js);
    string(js, true);
    eatWhitespace(js);
    if(!expect(js, ':')) return;
    value(js);
    eatWhitespace(js);
}

static void object(JSOX* js) {
    if(!expect(js, '{')) return;
    js->rec.onOpenObject(js->userData);
    
    eatWhitespace(js);
    if(!have(js, '}')) {
        member(js);
        while(match(js, ',')) {
            member(js);
        }
    }
    eatWhitespace(js);
    if(!expect(js, '}')) return;
    js->rec.onCloseObject(js->userData);
}

static void array(JSOX* js) {
    if(!expect(js, '[')) return;
    js->rec.onOpenList(js->userData);
    
    eatWhitespace(js);
    if(!have(js, ']')) {
    value(js);
        while(match(js, ',')) {
            value(js);
        }
    }
    
    js->rec.onCloseList(js->userData);
    expect(js, ']');
}

static void boolean(JSOX* js) {
    if(js->error) return;
    
    if(strncmp(js->current, "true", 4) == 0) {
        js->rec.onBool(true, js->userData);
        js->current += 4;
    }
    else if(strncmp(js->current, "false", 5) == 0) {
        js->rec.onBool(false, js->userData);
        js->current += 5;
    }
    js->error = "invalid literal";
}

static void null(JSOX* js) {
    if(js->error) return;
    
    if(strncmp(js->current, "null", 4)) {
        js->error = "invalid literal";
    } else {
        js->rec.onNull(js->userData);
        js->current += 4;
    }
        
}

static void number(JSOX* js) {
    if(js->error) return;
    const char* start = js->current;
    // TODO: handle decimal, negatives, etc.
    while(isDigit(peek(js)))
        js->current += 1;
    // const char* end = js->current;
    
    js->rec.onInt(atoi(start), js->userData);
}

static void value(JSOX* js) {
    eatWhitespace(js);
    
    switch(peek(js)) {
    case '{':
        object(js);
        break;
        
    case '[':
        array(js);
        break;
        
    case '"':
        string(js, false);
        break;
        
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        number(js);
        break;
        
    case 't':
    case 'f':
        boolean(js);
        break;
        
    case 'n':
        null(js);
        break;
        
    default:
        printf("invalid character: %c\n", peek(js));
        break;
    }
    eatWhitespace(js);
}

void jsoxParse(const char* source, JSOXReceiver receiver, void* userData) {
    JSOX js = {.current = source, .rec = receiver, .error=NULL, .userData = userData};
    value(&js);
    if(js.error)
        fprintf(stderr, "error: %s\n", js.error);
}
