#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include "src/Lexer.hpp"
#include "src/Parser.hpp"
#include "src/Bytecode/generator.hpp"
#include "src/Bytecode/evaluator.hpp"
#include "src/Bytecode/Repl/repl.hpp"

int main(int argc, char** argv)
{
    enum Type
    {
        DEV,
        INTERP,
        REPL
    };

    Type type = Type::REPL;

    if (type == Type::DEV)
    {
        std::string path = "../../../dev_env/source.gl";
        std::string file_name = std::filesystem::path(path).stem().string();

        Lexer lexer(file_name, path, true);
        lexer.tokenize();

        Parser parser(lexer.file_name, lexer.nodes);
        parser.parse();

        Bytecode_Generator generator(parser.file_name, parser.nodes);
        auto instructions = generator.generate();
        generator.print_instructions();

        generator.nodes.clear();

        auto frame = std::make_shared<StackFrame>();

        Bytecode_Evaluator evaluator(instructions);
        evaluator.file_name = parser.file_name;
        evaluator.evaluate(frame);

        std::cout << "\n\n[ ";
        for (auto object : frame->stack)
        {
            std::cout << object->repr() << " ";
        }
        std::cout << "]" << std::flush;

        return 0;
    }

    if (type == Type::REPL)
    {
        Repl repl;
        repl.run();
        return 0;
    }

    if (argc == 1)
    {
        std::cout << "You must enter a source path e.g: glide \"main.gl\"\n";
        return 1;
    }

    if (argc > 2)
    {
        std::cout << "Compiler only accepts 1 argument: source path";
        return 1;
    }

    std::string path = argv[1];
    std::string file_name = std::filesystem::path(path).stem().string();

    Lexer lexer(file_name, path, true);
    lexer.tokenize();

    Parser parser(lexer.file_name, lexer.nodes);
    parser.parse();

    Bytecode_Generator generator(parser.file_name, parser.nodes);
    auto instructions = generator.generate();

    generator.nodes.clear();

    auto frame = std::make_shared<StackFrame>();

    Bytecode_Evaluator evaluator(instructions);
    evaluator.file_name = parser.file_name;
    evaluator.evaluate(frame);

    return 0;
}