//===--------------------------------------------------------------------------------------------===
// jsox.h - JSON SAX-style parser
// This source is part of JSOX
//
// Created on 2019-09-07 by Amy Parent <amy@amyparent.com>
// Copyright (c) 2019 Amy Parent
// Licensed under the MIT License
// =^•.•^=
//===--------------------------------------------------------------------------------------------===
#ifndef jsox_jsox_h
#define jsox_jsox_h

#include <stdbool.h>

typedef struct JSOXReceiver {
    void (*onOpenObject)(void*);
    void (*onCloseObject)(void*);
    
    void (*onOpenList)(void*);
    void (*onCloseList)(void*);
    
    void (*onKey)(const char* string, int length, void*);
    void (*onBool)(bool, void*);
    void (*onInt)(int, void*);
    void (*onDouble)(double, void*);
    
    void (*onNull)(void*);
    void (*onString)(const char* string, int length, void*);
} JSOXReceiver;

// TODO: provide a better way to stream data into this
void jsoxParse(const char* source, JSOXReceiver rec, void* userData);

#endif