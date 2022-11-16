#pragma once
#include <chrono>
#include <sstream>
#include <fstream>
#include <cmath>
#include "generator.hpp"
#include "../Lexer.hpp"
#include "../Parser.hpp"

struct StackFrame
{
    std::shared_ptr<StackFrame> outer_frame;
    std::unordered_map<std::string, std::shared_ptr<StackObject>> locals;
    std::unordered_map<std::string, std::shared_ptr<StackObject>> globals = 
    {
        {"int", so_make_string("int")}, 
        {"float", so_make_string("float")}, 
        {"string", so_make_string("string")},
        {"object", so_make_string("object")},
        {"list", so_make_string("list")},
        {"list", so_make_string("list")},
        {"comma_list", so_make_string("comma_list")},
        {"empty_type", so_make_string("empty")},
        {"function", so_make_string("function")},
    };
    std::vector<std::shared_ptr<StackObject>> stack;
};

struct Bytecode_Evaluator
{
    int index;
    Bytecode current_op;
    std::vector<Bytecode> instructions;
    std::vector<std::string> errors;
    std::string file_name;
    std::unordered_map<std::string, std::shared_ptr<StackObject>> import_cache;
    bool repl = false;
    
    void forward();
    void backward();
    void evaluate(std::shared_ptr<StackFrame>& frame);
    void eval_instruction(std::shared_ptr<StackFrame>& frame);

    void eval_load(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_load_const(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_load_builtin(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_load_tos_or_empty(std::shared_ptr<StackFrame>& frame);
    void eval_store(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_store_at(std::shared_ptr<StackFrame>& frame);
    void eval_add(std::shared_ptr<StackFrame>& frame);
    void eval_sub(std::shared_ptr<StackFrame>& frame);
    void eval_mul(std::shared_ptr<StackFrame>& frame);
    void eval_div(std::shared_ptr<StackFrame>& frame);
    void eval_mod(std::shared_ptr<StackFrame>& frame);
    void eval_lt(std::shared_ptr<StackFrame>& frame);
    void eval_gt(std::shared_ptr<StackFrame>& frame);
    void eval_lte(std::shared_ptr<StackFrame>& frame);
    void eval_gte(std::shared_ptr<StackFrame>& frame);
    void eval_eq_eq(std::shared_ptr<StackFrame>& frame);
    void eval_not_eq(std::shared_ptr<StackFrame>& frame);
    void eval_and(std::shared_ptr<StackFrame>& frame);
    void eval_or(std::shared_ptr<StackFrame>& frame);
    void eval_not(std::shared_ptr<StackFrame>& frame);
    void eval_dot(std::shared_ptr<StackFrame>& frame);
    void eval_pos(std::shared_ptr<StackFrame>& frame);
    void eval_neg(std::shared_ptr<StackFrame>& frame);
    void eval_range(std::shared_ptr<StackFrame>& frame);
    void eval_arrow(std::shared_ptr<StackFrame>& frame);
    void eval_build_list(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_build_comma_list(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_build_object(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_build_iter(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_build_partial_op(std::shared_ptr<StackFrame>& frame);
    void eval_build_function(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_call_function(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_loop_start(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_while_start(std::shared_ptr<StackFrame>& frame);
    void eval_jump_to(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_frame_in(std::shared_ptr<StackFrame>& frame);
    void eval_frame_out(std::shared_ptr<StackFrame>& frame);
    void eval_remove_iter(std::shared_ptr<StackFrame>& frame);
    void eval_jump_if_false(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_break(Bytecode op, std::shared_ptr<StackFrame>& frame);
    void eval_return(std::shared_ptr<StackFrame>& frame);
    void eval_copy(std::shared_ptr<StackFrame>& frame);
    void eval_builtin(std::shared_ptr<StackObject> function, std::shared_ptr<StackFrame>& frame);
    std::shared_ptr<StackObject>& eval_lookup(std::string name, std::shared_ptr<StackFrame>& frame);

    void make_error(std::string message);
    void exit();

    Bytecode_Evaluator(std::vector<Bytecode> instructions) : instructions(instructions), index(0) 
    {
        if (instructions.size() > 0)
        {
            current_op = instructions[0];
        }
    }
};