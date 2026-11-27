#include "CodeGeneration.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <typeinfo>

std::string mapType(const std::string& type) {
    auto it = typeMap.find(type);
    if (it == typeMap.end()) //Something is up with this logic, should probably be looked at.
        exit(EXIT_FAILURE);
    return it->second;
}

// ---Expression Generation---
// Takes Expression*, figures out what concrete type it is, and returns a string representing the complete expression.
std::string generateExpression(const Expression* expr) {

    if (const LiteralExpression *literal = dynamic_cast<const LiteralExpression *>(expr))
        return literal->value;

    if (const IdentifierExpression* identifier = dynamic_cast<const IdentifierExpression*>(expr))
        return identifier->name;

    if (const BinaryExpression* binaryExpr = dynamic_cast<const BinaryExpression*>(expr)) {
        return "(" + generateExpression(binaryExpr->left) + " " + binaryExpr->op + " "  // recursively calls generateExpression; handles cases like 'x + (y * 2)'
            + generateExpression(binaryExpr->right) + ")";
    }
    if (const UnaryExpression* unaryExpr = dynamic_cast<const UnaryExpression*>(expr)) {
        return unaryExpr->op + "(" + generateExpression(unaryExpr->right) + ")";
    }
    if (const FunctionCallExpression *funcExpr = dynamic_cast<const FunctionCallExpression *>(expr)){
        std::string output = funcExpr->functionName + "(";
        for(size_t i = 0 ; i < funcExpr->arguments.size() ; i++){
            output += generateExpression(funcExpr->arguments[i]);
            if(i != funcExpr->arguments.size()-1){
                output += ", ";
            }
        }
        output += ");";
        return output;
    }
    if (const ArrayAccessExpression *arrExpr = dynamic_cast<const ArrayAccessExpression *>(expr))
    {
        return arrExpr->arrayName + "[" + generateExpression(arrExpr->index) + "]";
    }

    //Throw a runtime error on unhandled expression.
    throw std::runtime_error(std::string("generateExpression: unhandled expression type ") + typeid(*expr).name());
}

// ---Statement Generation---

std::string generateStatement(const Statement* statement, int indent) {
    if (const VariableDeclaration* declareVariable = dynamic_cast<const VariableDeclaration*>(statement)) {
        std::string output = std::string(indent * 4, ' ') + mapType(declareVariable->varType) + " " + declareVariable->identifier;
        if (declareVariable->initializer)
            output += " = " + generateExpression(declareVariable->initializer);
        return output + ";\n";
    }

    if (const AssignmentStatement* assignVariable = dynamic_cast<const AssignmentStatement*>(statement)) {
        return std::string(indent * 4, ' ') + assignVariable->identifier + " = "
            + generateExpression(assignVariable->value) + ";\n";
    }

    if (const IfStatement* ifState = dynamic_cast<const IfStatement*>(statement)) {
        return generateIf(ifState, indent);
    }

    if (const WhileLoop* whileState = dynamic_cast<const WhileLoop*>(statement)) {
    std::string pad(indent * 4, ' ');
    return pad + "while (" + generateCondition(whileState->condition) + ") {\n"
        + generateBlock(whileState->body, indent + 1)
        + pad + "}\n";
    }

    if (const ForLoop* forState = dynamic_cast<const ForLoop*>(statement)) {
        std::string pad(indent * 4, ' ');

        auto inlineStatement = [&](const Statement* statement) -> std::string {
            if (!statement) return "";
            std::string inlineStatement = generateStatement(statement, 0); 
            while (!inlineStatement.empty() && (inlineStatement.back() == '\n' || inlineStatement.back() == ' ' || inlineStatement.back() == ';'))
                inlineStatement.pop_back();
            return inlineStatement;
            };

        return pad + "for (" + inlineStatement(forState->initialization) + "; "
            + generateCondition(forState->condition) + "; "
            + inlineStatement(forState->update) + ") {\n"
            + generateBlock(forState->body, indent + 1)
            + pad + "}\n";
    }

    if (const ArrayDeclaration* arrState = dynamic_cast<const ArrayDeclaration*>(statement)) {
        std::string pad(indent * 4, ' ');
        std::string output = pad + "std::array<" + mapType(arrState->varType) + ", " + generateExpression(arrState->size) + "> " + arrState->identifier + "{";

        for (size_t i = 0; i < arrState->elements.size(); ++i)
        {
            if (i > 0) 
                output += ", ";
            output += generateExpression(arrState->elements[i]);
        }

        return output + "};\n";
    }
    
    if (const FunctionDeclaration* funcState = dynamic_cast<const FunctionDeclaration*>(statement)) {
        std::string pad(indent * 4, ' ');
        std::string output = pad + mapType(funcState->returnType) + " " + funcState->name + "(";

        for (size_t i = 0; i < funcState->parameters.size(); i++){
            const auto& param = funcState->parameters[i];
            output += mapType(param.first) + " " + param.second;
            if(i < funcState->parameters.size()-1){
                output += ", ";
            }
        }
        output += "){\n";
        
        for (size_t j = 0; j < funcState->body.size(); j++){
            generateStatement(funcState->body[j],indent+1);
        }
        output += pad + "}\n";
    }

    // unknown node: report it instead of silently dropping it. Runtime Error to make it visable in the console
    throw std::runtime_error(std::string("generateStatement: unhandled node ") + typeid(*statement).name());
}

// std::string generateSwineSpecific () { } 

// helper for ifs, loops, etc.
std::string generateBlock(const std::vector<Statement*>& body, int indent) {
    std::string output;
    for (const Statement* statement : body)
        output += generateStatement(statement, indent);   // not 0, and not a hardcoded value
    return output;
}

// removes extra parentheses from conditionals (while, if, etc.)
std::string generateCondition(const Expression* expr) {
    if (const BinaryExpression* binaryExpr = dynamic_cast<const BinaryExpression*>(expr))
        return generateExpression(binaryExpr->left) + " " + binaryExpr->op + " "
        + generateExpression(binaryExpr->right);
    return generateExpression(expr);
}

std::string generateIf(const IfStatement* node, int indent, bool isElseIf) {
    std::string pad(indent * 4, ' ');
    std::string output = isElseIf ? " else " : pad;
    output += "if (" + generateCondition(node->condition) + ") {\n";

    for (const Statement* statement : node->thenBranch)     // then branch handles boolean operations, no need for explicate calls here
        output += generateStatement(statement, indent + 1);
    output += pad + "}";
    
    if (node->elseBranch.empty())
        return output + "\n";

    if (node->elseBranch.size() == 1)
        if (IfStatement* nested = dynamic_cast<IfStatement*>(node->elseBranch[0]))
            return output + generateIf(nested, indent, true);   // recursion for else-if

    output += " else {\n";
    for (const Statement* statement : node->elseBranch)
        output += generateStatement(statement, indent + 1);
    return output + pad + "}\n";
}

void handleStatement(Statement* statement, std::ofstream* output, int indent = 1) {
    *output << generateStatement(statement, indent);
}

std::string GenerateCode(const Program* program) {
    std::string outputPath = "./GeneratedCode.cpp";
    std::ofstream outputFile(outputPath);

    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file: " << outputPath << "\n";
        exit(EXIT_FAILURE);
    }

    outputFile << "#include <iostream>\n\n"
        << "int main() {\n";

    for (auto* statement : program->declarations) {
        handleStatement(statement, &outputFile);
    }

    outputFile << "\n    return 0;\n"
        << "}\n";

    return outputPath;
}