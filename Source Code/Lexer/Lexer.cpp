//The tutorial I followed included these
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>
#include "Lexer.h"

std::string getTokenTypeName(TokenType type)
{
    switch (type) {
    case TokenType::Keyword:
        return "Keyword";
    case TokenType::VarType:
        return "VarType";
    case TokenType::VarName:
        return "VarName";
    case TokenType::Integer:
        return "Integer";
    case TokenType::Decimal:
        return "Decimal";
    case TokenType::Word:
        return "Word";
    case TokenType::BoolValue:
        return "BoolValue";
    case TokenType::Assign:
        return "Assign";
    case TokenType::Math:
        return "Math";
    case TokenType::Quote:
        return "Quote";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::Parentheses:
        return "Parentheses";
    case TokenType::Comparison:
        return "Comparison";
    case TokenType::UNKNOWN:
        return "UNKNOWN";
    case TokenType::EndFileMarker:
        return "End of file";
    default:
        return "ERROR: Unknown TokenType";
    }
}

std::vector<Token> RunLexerLine(std::string input, int line, int vecSize, bool commenting, std::vector<std::string> vars) {
    std::vector<Token> lineSet;

    LexicalAnalyzer lexer(input);
    lineSet = lexer.tokenize(line, vecSize, commenting, vars);

    return lineSet;
}

//assume the file type is correct
std::vector<Token> RunLexerProgram(const std::string& filePath) {
    std::vector<Token> tokenSet;
    int line = 1;
    std::string programLine;
    std::vector<std::string> vars;
    std::ifstream file(filePath);
    bool commenting = false;

    if (file.is_open()) {
        while (getline(file, programLine)) {
            std::vector<Token> newLine = RunLexerLine(programLine, line, tokenSet.size(), commenting, vars);
            if (newLine.size() > 0 && newLine.back().type == TokenType::UnclosedComment) {
                commenting = true;
                newLine.pop_back();
            }
            else {
                commenting = false;
            }

            for (int i = 0; i < newLine.size(); i++) {
                if (newLine[i].type == TokenType::VarName) {
                    vars.push_back(newLine[i].value);
                }
                tokenSet.push_back(newLine[i]);
            }

            line++;
        }
    }

    int finalOff = tokenSet.size() + 1;
    SourceLocation endFileLocation = { line, 0, finalOff };
    tokenSet.emplace_back(TokenType::EndFileMarker, "0", endFileLocation);

    //if there's an unkown token, return null
    //wasn't working, scrapped for now
    /*if (find(tokenSet.begin(), tokenSet.end(), TokenType::UNKNOWN) != tokenSet.end()) {
        std::vector<Token> nullVec;
        return nullVec;
    } else {
        return tokenSet;
    }*/
    return tokenSet;
}

//int main(int argc, char *argv[]){
    //std::vector<Token> output = RunLexerProgram("textProgram.txt");

   //for (int i = 0; i < output.size(); i++) {
        //std::cout << getTokenTypeName(output[i].type) << std::endl;
        //std::cout << output[i].value << std::endl;
        //std::cout << output[i].sourceLocal.line << " " << output[i].sourceLocal.column << " " << output[i].sourceLocal.offset << std::endl;
    //}
    //return 0;
//}