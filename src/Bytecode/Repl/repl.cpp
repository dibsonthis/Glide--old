#include "repl.hpp"

void Repl::interpret_statement(std::string statement, std::shared_ptr<StackFrame>& frame)
{
    Lexer lexer;
    lexer.init(statement);
    lexer.tokenize();

    Parser parser(lexer.file_name, lexer.nodes);
    parser.parse();

    Bytecode_Generator generator(parser.file_name, parser.nodes);
    auto instructions = generator.generate();
    //generator.print_instructions();

    Bytecode_Evaluator evaluator(instructions);
    evaluator.repl = true;
    evaluator.file_name = parser.file_name;
    evaluator.evaluate(frame);

    frame->stack.clear();
}

void Repl::run()
{
    auto main_frame = std::make_shared<StackFrame>();

    while (true)
    {
        std::cout << "\n>> ";
        std::vector<std::string> multi_input;
        std::string input;
        while (std::getline(std::cin, input))
        {
            if (input.empty()) {
                break;
            }
            multi_input.push_back(input + "\n");
        }
        std::string final_input;
        for (auto& str : multi_input)
        {
            final_input = final_input + str;
        }

        interpret_statement(final_input, main_frame);

        main_frame->stack.clear();
    }
}