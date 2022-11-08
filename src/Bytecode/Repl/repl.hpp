#include "../generator.hpp"
#include "../evaluator.hpp"

struct Repl
{
    void run();
    void interpret_statement(std::string statement, std::shared_ptr<StackFrame>& frame);
};