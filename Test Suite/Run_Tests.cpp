// The code for including and running the test suites goes here.
// This file HOPEFULLY shouldn't need to be modified.

#include "Driver/Tests.cpp"
#include "Code Generation/Tests.cpp"
#include "Lexer/Tests.cpp"
#include "Parser/Tests.cpp"
#include "Semantic Analysis/Tests.cpp"

void runTests(){
    testReturn testArray[2];

    //Runs each set of tests in execution order

    std::cout << "Running Lexer Tests." << std::endl;
    testArray[0] = lexerTests();
    std::cout << "Tests passed: " << testArray[0].passed << std::endl;
    std::cout << "Tests failed: " << testArray[0].failed << std::endl;
    std::cout << "Error messages:" << std::endl;
    for (const auto &message : testArray[0].errorMessages)
    {
        std::cout << message << std::endl;
    }
    std::cout << "______________________________" << std::endl;

    std::cout << "Running Parser Tests." << std::endl;
    testArray[1] = parserTests();
    std::cout << "Tests passed: " << testArray[1].passed << std::endl;
    std::cout << "Tests failed: " << testArray[1].failed << std::endl;
    std::cout << "Error messages:" << std::endl;
    for (const auto &message : testArray[1].errorMessages)
    {
        std::cout << message << std::endl;
    }
    std::cout << "______________________________" << std::endl;

    std::cout << "Running Semantic Analysis Tests." << std::endl;
    testArray[2] = semanticTests();
    std::cout << "Tests passed: " << testArray[2].passed << std::endl;
    std::cout << "Tests failed: " << testArray[2].failed << std::endl;
    std::cout << "Error messages:" << std::endl;
    for (const auto &message : testArray[2].errorMessages)
    {
        std::cout << message << std::endl;
    }
    std::cout << "______________________________" << std::endl;

    std::cout << "Running Code Generation Tests." << std::endl;
    testArray[3] = codeGenerationTests();
    std::cout << "Tests passed: " << testArray[3].passed << std::endl;
    std::cout << "Tests failed: " << testArray[3].failed << std::endl;
    std::cout << "Error messages:" << std::endl;
    for (const auto &message : testArray[3].errorMessages)
    {
        std::cout << message << std::endl;
    }
    std::cout << "______________________________" << std::endl;

    std::cout << "Running Driver Tests." << std::endl;
    testArray[4] = driverTests();
    std::cout << "Tests passed: " << testArray[4].passed << std::endl;
    std::cout << "Tests failed: " << testArray[4].failed << std::endl;
    std::cout << "Error messages:" << std::endl;
    for (const auto &message : testArray[4].errorMessages)
    {
        std::cout << message << std::endl;
    }
    std::cout << "______________________________" << std::endl;

    int totalPassed = 0;
    int totalFailed = 0;
        for (testReturn result : testArray){
        totalPassed += result.passed;

        if(result.failed > 0){
            totalFailed += result.failed;
        }
    }
    std::cout << "______________________________" << std::endl;
    std::cout << "Total tests passed: " << totalPassed << std::endl;
    std::cout << "Total tests failed: " << totalFailed << std::endl;
}

int main(){
    runTests();
}