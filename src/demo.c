#include <jsox/jsox.h>
#include <stdio.h>
#include <stdlib.h>

int depth = 0;
bool inMember = false;
bool needsComma = false;

void indent() {
    if(inMember) {
        inMember = false;
        needsComma = true;
        return;
    }
    putchar('\n');
    for(int i = 0; i < depth; ++i) {
        printf("  ");
    }
}

void openObject(void* data) {
    indent();
    printf("{");
    depth += 1;
}

void closeObject(void* data) {
    depth -= 1;
    indent();
    printf("}");
}

void openList(void* data) {
    indent();
    printf("[");
    depth += 1;
}

void closeList(void* data) {
    depth -= 1;
    indent();
    printf("]");
}

void key(const char* str, int length, void* data) {
    indent();
    printf("%.*s = ", length, str);
    inMember = true;
}

void string(const char* str, int length, void* data) {
    indent();
    printf("\"%.*s\"", length, str);
}

void null(void* data) {
    indent();
    printf("null");
}

void integer(int value, void* data) {
    indent();
    printf("%d", value);
}

void real(double value, void* data) {
    indent();
    printf("%f", value);
}

void boolean(bool value, void* data) {
    indent();
    printf("%s", value ? "true" : "false");
}

int main(int argc, const char** argv) {
    JSOXReceiver rec;
    rec.onOpenObject = openObject;
    rec.onCloseObject = closeObject;
    rec.onOpenList = openList;
    rec.onCloseList = closeList;
    
    rec.onKey = key;
    rec.onString = string;
    rec.onNull = null;
    rec.onInt = integer;
    rec.onDouble = real;
    rec.onBool = boolean;
    
    FILE* f = fopen("/Users/amy/Desktop/ast.json", "r");
    if(!f) {
        fprintf(stderr, "cannot open ast.json\n");
        return -1;
    }
    
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    
    fseek(f, 0, SEEK_SET);
    char* data = malloc(size + 1);
    fread(data, 1, size, f);
    data[size] = '\0';
    fclose(f);
    
    jsoxParse(data, rec, NULL);
    free(data);
    printf("\n\n");
}