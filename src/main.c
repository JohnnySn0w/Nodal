
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>



// Data structures and enumerations for representing grammar elements
typedef enum
{
    Label,
    Link,
    Filter,
    Modifier,
    ConfigItem,
    ListRelationsValue,
    Digit,
    Integer,
    StatementTerminator,
    ConfigTerminator,
    QueryTerminator
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

typedef struct
{
    int size;
    Token *tokens;
} TokenStream;

// Forward declarations for grammar parsing functions


// Forward declarations for grammar parsing cantrips
void parseTokenStream(TokenStream* tokenStream);
void parseChangeStatement(TokenStream* tokenStream);
void parseLabelPhrase(TokenStream* tokenStream);
void parseLinkPhrase(TokenStream* tokenStream);
void parseQueryStatement(TokenStream* tokenStream);
void parseFilterPhrase(TokenStream* tokenStream);
void parseConfigStatement(TokenStream* tokenStream);
void parseConfigItem(TokenStream* tokenStream);
void parseInteger(TokenStream* tokenStream);
void parseListRelationsValue(TokenStream* tokenStream);
void parseDigit(TokenStream* tokenStream);

char *readCommandLineInput(int argc, char *argv[]);
TokenStream tokenize(char *input);

// Tokenize the input string
TokenStream tokenize(char* input)
{
    TokenStream tokenStream;
    tokenStream.size = 0;

    char* tokenStr = strtok(input, " ");
    while (tokenStr != NULL)
    {
        Token token;

        if (isLabel(tokenStr))
        {
            token.type = Label;
        }
        else if (isLink(tokenStr))
        {
            token.type = Link;
        }
        else if (isModifier(tokenStr))
        {
            token.type = Modifier;
        }
        // ... Other token types ...

        strncpy(token.value, tokenStr, sizeof(token.value) - 1);
        token.value[sizeof(token.value) - 1] = '\0';

        tokenStream.tokens[tokenStream.size++] = token;

        tokenStr = strtok(NULL, " ");
    }

    return tokenStream;
}

// Read input from the command line and tokenize the input into a TokenStream
TokenStream readUserInput(int argc, char *argv[])
{
    char *input = readCommandLineInput(argc, argv);
    TokenStream tokenStream = tokenize(input);
    free(input);
    return tokenStream;
}

// Read input from the command line arguments
char *readCommandLineInput(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(1);
    }

    size_t inputSize = 0;
    for (int i = 1; i < argc; i++)
    {
        inputSize += strlen(argv[i]) + 1;
    }

    char *input = malloc(inputSize);
    if (!input)
    {
        fprintf(stderr, "Failed to allocate memory for input\n");
        exit(1);
    }

    strcpy(input, argv[1]);
    for (int i = 2; i < argc; i++)
    {
        strcat(input, " ");
        strcat(input, argv[i]);
    }

    return input;
}

// Check if the token stream represents a change statement
bool isChangeStatement(TokenStream* tokenStream)
{
    if (tokenStream->size > 0 && tokenStream->tokens[0].type == Label)
    {
        for (size_t i = 1; i < tokenStream->size; i++)
        {
            Token token = tokenStream->tokens[i];
            if (token.type == Link && (strcmp(token.value, "is") == 0 || strcmp(token.value, "are") == 0 || strcmp(token.value, "has") == 0))
            {
                return true;
            }
        }
    }

    return false;
}

// Check if the token stream represents a query statement
bool isQueryStatement(TokenStream* tokenStream)
{
    if (tokenStream->size > 0 && tokenStream->tokens[0].type == Label)
    {
        for (size_t i = 1; i < tokenStream->size; i++)
        {
            Token token = tokenStream->tokens[i];
            if (token.type == QueryTerminator)
            {
                return true;
            }
        }
    }

    return false;
}

// Check if the token stream represents a config statement
bool isConfigStatement(TokenStream* tokenStream)
{
    if (tokenStream->size > 0 && tokenStream->tokens[0].type == ConfigItem)
    {
        for (size_t i = 1; i < tokenStream->size; i++)
        {
            Token token = tokenStream->tokens[i];
            if (token.type == ConfigTerminator)
            {
                return true;
            }
        }
    }

    return false;
}

// Parse the input according to the grammar rules
void parseTokenStream(TokenStream* tokenStream)
{
    while (tokenStream->size > 0)
    {
        if (isChangeStatement(tokenStream))
        {
            parseChangeStatement(tokenStream);
        }
        else if (isQueryStatement(tokenStream))
        {
            parseQueryStatement(tokenStream);
        }
        else if (isConfigStatement(tokenStream))
        {
            parseConfigStatement(tokenStream);
        }
        else
        {
            fprintf(stderr, "Error: Invalid statement encountered.\n");
            exit(1);
        }
    }
}

// Parse a change statement from the token stream
void parseChangeStatement(TokenStream* tokenStream)
{
    // Check for a valid change statement
    if (!isChangeStatement(tokenStream))
    {
        fprintf(stderr, "Error: Invalid change statement encountered.\n");
        exit(1);
    }

    // TODO: Implement the parsing logic for change statements
    // Consume the tokens related to the change statement from the token stream
    // Perform the necessary changes in the internal data structure

    // For now, just remove the tokens related to the change statement
    while (tokenStream->size > 0 && tokenStream->tokens[0].type != StatementTerminator)
    {
        for (size_t i = 0; i < tokenStream->size - 1; i++)
        {
            tokenStream->tokens[i] = tokenStream->tokens[i + 1];
        }
        tokenStream->size--;
    }

    // Remove the statement terminator token
    if (tokenStream->size > 0 && tokenStream->tokens[0].type == StatementTerminator)
    {
        for (size_t i = 0; i < tokenStream->size - 1; i++)
        {
            tokenStream->tokens[i] = tokenStream->tokens[i + 1];
        }
        tokenStream->size--;
    }
}



// Parse a query statement
void parseQueryStatement(TokenStream* tokenStream)
{
    parseLabelPhrase(tokenStream);

    if (tokenStream->size > 0 && strcmp(tokenStream->tokens[0].value, "to") == 0)
    {
        // Consume the "to" token
        tokenStream->size--;
        memmove(tokenStream->tokens, tokenStream->tokens + 1, tokenStream->size * sizeof(Token));

        parseLabelPhrase(tokenStream);
    }
    else
    {
        parseFilterPhrase(tokenStream);

        if (tokenStream->size > 0)
        {
            parseLabelPhrase(tokenStream);
        }
    }

    if (tokenStream->size > 0 && tokenStream->tokens[0].type == QueryTerminator)
    {
        // Consume the query terminator token
        tokenStream->size--;
        memmove(tokenStream->tokens, tokenStream->tokens + 1, tokenStream->size * sizeof(Token));
    }
    else
    {
        fprintf(stderr, "Error: Missing query terminator.\n");
        exit(1);
    }
}


// Parse a label phrase
void parseLabelPhrase(TokenStream* tokenStream)
{
    if (tokenStream->size > 0 && tokenStream->tokens[0].type == Label)
    {
        // Consume the label token
        tokenStream->size--;
        memmove(tokenStream->tokens, tokenStream->tokens + 1, tokenStream->size * sizeof(Token));

        if (tokenStream->size > 0 && strcmp(tokenStream->tokens[0].value, "and") == 0)
        {
            // Consume the "and" token
            tokenStream->size--;
            memmove(tokenStream->tokens, tokenStream->tokens + 1, tokenStream->size * sizeof(Token));

            // Continue parsing the label phrase
            parseLabelPhrase(tokenStream);
        }
    }
    else
    {
        fprintf(stderr, "Error: Expected a label.\n");
        exit(1);
    }
}



// Main function
int main(int argc, char* argv[])
{
    // TODO: Read input from the user and tokenize the input into a TokenStream
    TokenStream tokenStream = readUserInput(argc, argv);

    // TODO: Parse the input according to the grammar rules
    parseTokenStream(&tokenStream);

    // TODO: Process the parsed input and perform the requested operations

    // TODO: Provide output based on the parsed input and performed operations

    return 0;
}

