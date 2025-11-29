#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants for maximum sizes
#define MAX_VAR_NAME 50
#define MAX_VARS 100
#define MAX_TOKEN_LEN 100
#define MAX_TOKENS 1000
#define MAX_LINE_LEN 1024

// Token types for the lexer
typedef enum {
    TOK_KEYWORD,
    TOK_IDENTIFIER,
    TOK_OPERATOR,
    TOK_INTCONST,
    TOK_STRINGCONST,
    TOK_OPENBLOCK,
    TOK_CLOSEBLOCK,
    TOK_ENDOFLINE,
    TOK_UNKNOWN
} TokenType;

// Token structure with line info
typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LEN];
    int line;
} Token;

// Token list
Token tokens[MAX_TOKENS];
int token_count = 0;

// Variable structure and list
typedef struct {
    char name[MAX_VAR_NAME];
    int value;
} Variable;

Variable vars[MAX_VARS];
int var_count = 0;

// Finds the index of a variable by name
int findVarIndex(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) return i;
    }
    return -1;
}

// Sets or creates a variable
void setVar(const char* name, int value) {
    int idx = findVarIndex(name);
    if (idx >= 0) vars[idx].value = value;
    else {
        if (var_count >= MAX_VARS) {
            printf("ERROR: Too many variables defined.\n");
            exit(1);
        }
        strcpy(vars[var_count].name, name);
        vars[var_count].value = value;
        var_count++;
    }
}

// Gets the value of a variable or throws error
int getVar(const char* name,int line_number) {
    int idx = findVarIndex(name);
    if (idx >= 0) return vars[idx].value;
    printf("ERROR: Undefined variable %s (position %d)\n", name,line_number);
    exit(1);
}

// Checks if a string is a keyword
int isKeyword(const char* word) {
    const char* keywords[] = {"number", "write", "repeat", "times", "newline", "and", NULL};
    for (int i = 0; keywords[i]; i++) {
        if (strcmp(keywords[i], word) == 0) return 1;
    }
    return 0;
}

// Checks if a string is a valid operator
int isOperator(const char* word) {
    const char* operators[] = {":=", "+=", "-=", NULL};
    for (int i = 0; operators[i]; i++) {
        if (strcmp(operators[i], word) == 0) return 1;
    }
    return 0;
}

// Tokenizes a single line of code
void tokenizeLine(const char* line,int linenum) {
    int i = 0;
    while (line[i]) {
        while (isspace(line[i])) i++;

        if (line[i] == '\0') break;

        // Handle special characters and tokens
        if (line[i] == ';') {
            tokens[token_count] = (Token){TOK_ENDOFLINE, "EndOfLine"};
            tokens[token_count++].line = linenum;
            i++;
            continue;
        }
        if (line[i] == '{') {
            tokens[token_count] = (Token){TOK_OPENBLOCK, "OpenBlock"};
            tokens[token_count++].line = linenum;
            i++;
            continue;
        }
        if (line[i] == '}') {
            tokens[token_count] = (Token){TOK_CLOSEBLOCK, "CloseBlock"};
            tokens[token_count++].line = linenum;
            i++;
            continue;
        }
        if (line[i] == '"') {
            i++;
            int start = i;
            while (line[i] && line[i] != '"') i++;
            if (line[i] != '"') {
                printf("ERROR: Unterminated string.\n");
                exit(1);
            }
            int len = i - start;
            if (len >= MAX_TOKEN_LEN) len = MAX_TOKEN_LEN - 1;
            strncpy(tokens[token_count].text, &line[start], len);
            tokens[token_count].text[len] = '\0';
            tokens[token_count].type = TOK_STRINGCONST;
            tokens[token_count++].line = linenum;
            i++;
            continue;
        }

        // Handle compound operators
        if (line[i] == ':' && line[i+1] == '=') {
            tokens[token_count++] = (Token){TOK_OPERATOR, ":="};
            i += 2;
            continue;
        }
        if (line[i] == '+' && line[i+1] == '=') {
            tokens[token_count] = (Token){TOK_OPERATOR, "+="};
            tokens[token_count++].line = linenum;
            i += 2;
            continue;
        }
        if (line[i] == '-' && line[i+1] == '=') {
            tokens[token_count] = (Token){TOK_OPERATOR, "-="};
            tokens[token_count++].line = linenum;
            i += 2;
            continue;
        }

        // Handle negative numbers
        if (line[i] == '-' && isdigit(line[i+1])) {
            int start = i++;
            while (isdigit(line[i])) i++;
            int len = i - start;
            strncpy(tokens[token_count].text, &line[start], len);
            tokens[token_count].text[len] = '\0';
            tokens[token_count].type = TOK_INTCONST;
            tokens[token_count++].line = linenum;
            continue;
        }

        // Handle integers
        if (isdigit(line[i])) {
            int start = i;
            while (isdigit(line[i])) i++;
            int len = i - start;
            strncpy(tokens[token_count].text, &line[start], len);
            tokens[token_count].text[len] = '\0';
            tokens[token_count].type = TOK_INTCONST;
            tokens[token_count++].line = linenum;
            continue;
        }

        // Handle identifiers and keywords
        if (isalpha(line[i])) {
            int start = i;
            while (isalnum(line[i]) || line[i] == '_') i++;
            int len = i - start;
            char word[MAX_TOKEN_LEN];
            strncpy(word, &line[start], len);
            word[len] = '\0';
            if (isKeyword(word)) {
                tokens[token_count] = (Token){TOK_KEYWORD, ""};
                tokens[token_count++].line = linenum;
                strcpy(tokens[token_count-1].text, word);
            } else {
                tokens[token_count] = (Token){TOK_IDENTIFIER, ""};
                tokens[token_count++].line = linenum;
                strcpy(tokens[token_count-1].text, word);
            }
            continue;
        }

        // Unknown characters
        printf("ERROR: Unknown character '%c' (position %d)\n", line[i], linenum);
        exit(1);
    }
}

// Function prototypes
void interpretCommands(int* pos, int end);

// Repeats a block of commands
void interpretBlock(int start, int end, int repeat_count, const char* counterVar) {
    for (int i = 0; i < repeat_count; i++) {
        int p = start;
        while (p < end) {
            interpretCommands(&p, end);
        }
        if (counterVar != NULL) {
            int idx = findVarIndex(counterVar);
            if (idx >= 0) {
                vars[idx].value--;
            }
        }
    }
}

// Expects an end-of-line token, else throws error
void expectEndOfLine(int* pos, int end, const char* context, int line) {
    if (*pos >= end || tokens[*pos].type != TOK_ENDOFLINE) {
        printf("ERROR: missing ';' at the end of %s (line %d)\n", context, line);
        exit(1);
    }
    (*pos)++;
}

// Main interpreter logic
void interpretCommands(int* pos, int end) {
    // If we have reached the end of tokens, we return immediately
    if (*pos >= end) return;
    // Get the current token and move forward
    Token *token = &tokens[(*pos)++];
    int line_num = token->line;

    // If we find an EndOfLine immediately, we ignore it
    if (token->type == TOK_ENDOFLINE) return;

    // Handle keywords first
    if (token->type == TOK_KEYWORD) {
        // If we have "number", it's a variable declaration
        if (strcmp(token->text, "number") == 0) {
            // The next must be an identifier
            if (*pos >= end || tokens[*pos].type != TOK_IDENTIFIER) {
                printf("ERROR: variable name expected after number (position %d)\n",line_num);
                exit(1);
            }
            // Declare variable with initial 0
            setVar(tokens[(*pos)++].text, 0);
            // Check for end of the command
            expectEndOfLine(pos, end, "number declaration",line_num);
            return;
        }
        // Handle "write" command
        if (strcmp(token->text, "write") == 0) {
            int newline_flag = 0;
            // Loop through arguments to write
            while (*pos < end) {
                Token *t = &tokens[*pos];
                if (t->type == TOK_ENDOFLINE) {
                    break;
                }
                if (t->type == TOK_KEYWORD && strcmp(t->text, "newline") == 0) {
                    // If "newline" is present, we will print a newline afterwards
                    newline_flag = 1;
                    (*pos)++;
                    continue;
                }
                if (t->type == TOK_KEYWORD && strcmp(t->text, "and") == 0) {
                    // "and" is a separator, we just ignore it
                    (*pos)++;
                    continue;
                }
                if (t->type == TOK_INTCONST || t->type == TOK_STRINGCONST || t->type == TOK_IDENTIFIER) {
                    // If it's a literal or a variable, we print its value
                    if (t->type == TOK_INTCONST || t->type == TOK_STRINGCONST)
                        printf("%s", t->text);
                    else
                        printf("%d", getVar(t->text,line_num)); // Look up variable's value
                    (*pos)++;
                    continue;
                }
                // If we find something else, it's an error
                printf("ERROR: invalid write argument. (position %d)\n",line_num);
                exit(1);
            }
            if (newline_flag) printf("\n"); // Print a newline if "newline" was present
            expectEndOfLine(pos, end, "write command",line_num);
            return;
        }
        // Handle "repeat" loop
        if (strcmp(token->text, "repeat") == 0) {
            if (*pos >= end) {
                printf("ERROR: missing expression after repeat. position(%d)\n",line_num);
                exit(1);
            }
            Token *countToken = &tokens[(*pos)++];
            int count = 0;
            // Determine the repeat count from literal or variable
            if (countToken->type == TOK_INTCONST)
                count = atoi(countToken->text);
            else if (countToken->type == TOK_IDENTIFIER)
                count = getVar(countToken->text,line_num);
            else {
                printf("ERROR: number or variable expected after repeat. position(%d)\n",line_num);
                exit(1);
            }
            if (*pos >= end || tokens[*pos].type != TOK_KEYWORD ||
                strcmp(tokens[*pos].text, "times") != 0) {
                printf("ERROR: 'times' expected after repeat. position(%d)\n",line_num);
                exit(1);
            }
            (*pos)++;
            if (*pos >= end) {
                printf("ERROR: missing command after repeat. position(%d)\n",line_num);
                exit(1);
            }
            if (tokens[*pos].type == TOK_OPENBLOCK) {
                // If we have a block of commands to repeat
                (*pos)++;
                int blockStart = *pos, blockEnd = blockStart, openBlocks = 1;

                // Find closing block
                while (blockEnd < end && openBlocks > 0) {
                    if (tokens[blockEnd].type == TOK_OPENBLOCK) openBlocks++;
                    else if (tokens[blockEnd].type == TOK_CLOSEBLOCK) openBlocks--;
                    blockEnd++;
                }

                if (openBlocks != 0) {
                    int err_line = (blockEnd < end) ? tokens[blockEnd].line : tokens[blockEnd - 1].line;
                    printf("ERROR: repeat block not closed position(%d)\n", err_line-1);
                    exit(1);
                }

                // If count comes from a variable, we remember its name
                const char* counterVar = NULL;
                if (countToken->type == TOK_IDENTIFIER)
                    counterVar = countToken->text;

                // Execute block `count` times
                interpretBlock(blockStart, blockEnd - 1, count, counterVar);

                *pos = blockEnd;
            } else {
                printf("ERROR: repeat block not opened position(%d)\n",line_num);
                exit(1);
            }
            return;
        }
    }
    // Handle assignment
    if (token->type == TOK_IDENTIFIER) {
        if (*pos >= end || tokens[*pos].type != TOK_OPERATOR) {
            printf("ERROR: assignment operator expected position(%d)\n",line_num);
            exit(1);
        }
        char* varName = token->text;
        Token* opToken = &tokens[(*pos)++];
        if (*pos >= end) {
            printf("ERROR: missing value after assignment position(%d)\n",line_num);
            exit(1);
        }
        Token* valToken = &tokens[(*pos)++];
        int val = (valToken->type == TOK_INTCONST) ? atoi(valToken->text) : getVar(valToken->text,line_num);
        // Handle :=, +=, -=
        if (strcmp(opToken->text, ":=") == 0) setVar(varName, val);
        else if (strcmp(opToken->text, "+=") == 0) setVar(varName, getVar(varName,line_num) + val);
        else if (strcmp(opToken->text, "-=") == 0) setVar(varName, getVar(varName,line_num) - val);
        else {
            printf("ERROR: unknown operator %s\n", opToken->text);
            exit(1);
        }
        expectEndOfLine(pos, end, "Assignment command",line_num);
        return;
    }
    // If we reach here, it's an unknown or invalid token
    printf("ERROR: unexpected token '%s' position(%d)\n", token->text,line_num);
    exit(1);
}
// Starts the interpretation process
void interpret(const char* filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("ERROR: FİLE DOES NOT EXIST: %s\n", filename);
        exit(1);
    }
    char line[MAX_LINE_LEN];
    int line_Number = 1;
    while (fgets(line, sizeof(line), f)) {
        tokenizeLine(line,line_Number);
        line_Number++;
    }
    fclose(f);
    int pos = 0;
    while (pos < token_count) interpretCommands(&pos, token_count);
}

// Entry point
int main(int argc, char* argv[]) {
    const char* filename = (argc >= 2) ? argv[1] : "test.ppp";
    if (argc < 2) printf("Warning: No file specified, defaulting to 'test.ppp'.\n");
    interpret(filename);
    return 0;
}
