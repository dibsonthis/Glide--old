#include "evaluator.hpp"

void Bytecode_Evaluator::forward()
{
    index++;
    current_op = instructions[index];
}

void Bytecode_Evaluator::backward()
{
    index--;
    current_op = instructions[index];
}

void Bytecode_Evaluator::evaluate(std::shared_ptr<StackFrame>& frame)
{
    if (instructions.size() == 0)
    {
        return;
    }

    while (current_op.type != OpType::EXIT)
    {
        eval_instruction(frame);

        // on Exit event
        if (current_op.type == OpType::EXIT)
        {
            for (auto error : errors)
            {
                std::cout << error << std::flush;
            }

            if (repl)
            {
                return;
            }

            frame->stack.clear();
            frame->locals.clear();

            auto outer_frame = frame->outer_frame;

            while (outer_frame != nullptr)
            {
                outer_frame->stack.clear();
                outer_frame->locals.clear();
                outer_frame = outer_frame->outer_frame;
            }

            return;
        }

        forward();
    }

    if (repl && frame->stack.size() > 0)
    {
        if (frame->stack.back()->type == SO_Type::EMPTY 
            || frame->stack.back()->type == SO_Type::ITERATOR)
        {
            return;
        }
        
        std::cout << frame->stack.back()->repr() << "\n";
    }
}

void Bytecode_Evaluator::eval_instruction(std::shared_ptr<StackFrame>& frame)
{
    if (current_op.type == OpType::LOAD)
    {
        eval_load(current_op, frame);
        return;
    }
    if (current_op.type == OpType::LOAD_CONST)
    {
        eval_load_const(current_op, frame);
        return;
    }
    if (current_op.type == OpType::LOAD_BUILTIN)
    {
        eval_load_builtin(current_op, frame);
        return;
    }
    if (current_op.type == OpType::LOAD_TOS_OR_EMPTY)
    {
        eval_load_tos_or_empty(frame);
        return;
    }
    if (current_op.type == OpType::STORE)
    {
        eval_store(current_op, frame);
        return;
    }
    if (current_op.type == OpType::STORE_AT)
    {
        eval_store_at(frame);
        return;
    }
    if (current_op.type == OpType::ADD)
    {
        eval_add(frame);
        return;
    }
    if (current_op.type == OpType::SUB)
    {
        eval_sub(frame);
        return;
    }
    if (current_op.type == OpType::MUL)
    {
        eval_mul(frame);
        return;
    }
    if (current_op.type == OpType::DIV)
    {
        eval_div(frame);
        return;
    }
    if (current_op.type == OpType::MOD)
    {
        eval_mod(frame);
        return;
    }
    if (current_op.type == OpType::LT)
    {
        eval_lt(frame);
        return;
    }
    if (current_op.type == OpType::GT)
    {
        eval_gt(frame);
        return;
    }
    if (current_op.type == OpType::LTE)
    {
        eval_lte(frame);
        return;
    }
    if (current_op.type == OpType::GTE)
    {
        eval_gte(frame);
        return;
    }
    if (current_op.type == OpType::EQ_EQ)
    {
        eval_eq_eq(frame);
        return;
    }
    if (current_op.type == OpType::NOT_EQ)
    {
        eval_not_eq(frame);
        return;
    }
    if (current_op.type == OpType::AND)
    {
        eval_and(frame);
        return;
    }
    if (current_op.type == OpType::OR)
    {
        eval_or(frame);
        return;
    }
    if (current_op.type == OpType::NOT)
    {
        eval_not(frame);
        return;
    }
    if (current_op.type == OpType::POS)
    {
        eval_pos(frame);
        return;
    }
    if (current_op.type == OpType::NEG)
    {
        eval_neg(frame);
        return;
    }
    if (current_op.type == OpType::RANGE)
    {
        eval_range(frame);
        return;
    }
    if (current_op.type == OpType::DOT)
    {
        eval_dot(frame);
        return;
    }
    if (current_op.type == OpType::COPY)
    {
        eval_copy(frame);
        return;
    }
    if (current_op.type == OpType::BUILD_LIST)
    {
        eval_build_list(current_op, frame);
        return;
    }
    if (current_op.type == OpType::BUILD_COMMA_LIST)
    {
        eval_build_comma_list(current_op, frame);
        return;
    }
    if (current_op.type == OpType::BUILD_OBJECT)
    {
        eval_build_object(current_op, frame);
        return;
    }
    if (current_op.type == OpType::ARROW)
    {
        eval_arrow(frame);
        return;
    }
    if (current_op.type == OpType::FRAME_IN)
    {
        eval_frame_in(frame);
        return;
    }
    if (current_op.type == OpType::FRAME_OUT)
    {
        eval_frame_out(frame);
        return;
    }
    if (current_op.type == OpType::BUILD_ITER)
    {
        eval_build_iter(current_op, frame);
        return;
    }
    if (current_op.type == OpType::BUILD_PARTIAL_OP)
    {
        eval_build_partial_op(frame);
        return;
    }
    if (current_op.type == OpType::LOOP_START)
    {
        eval_loop_start(current_op, frame);
        return;
    }
    if (current_op.type == OpType::WHILE_START)
    {
        eval_while_start(frame);
        return;
    }
    if (current_op.type == OpType::BREAK)
    {
        eval_break(current_op, frame);
        return;
    }
    if (current_op.type == OpType::JUMP_IF_FALSE)
    {
        eval_jump_if_false(current_op, frame);
        return;
    }
    if (current_op.type == OpType::JUMP_TO)
    {
        eval_jump_to(current_op, frame);
        return;
    }
    if (current_op.type == OpType::REMOVE_ITER)
    {
        eval_remove_iter(frame);
        return;
    }
    if (current_op.type == OpType::BUILD_FUNCTION)
    {
        eval_build_function(current_op, frame);
        return;
    }
    if (current_op.type == OpType::CALL_FUNCTION)
    {
        eval_call_function(current_op, frame);
        return;
    }
    if (current_op.type == OpType::RETURN)
    {
        eval_return(frame);
        return;
    }
}

void Bytecode_Evaluator::eval_load(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    auto name = op.data->STRING.value;
    auto& var = eval_lookup(name, frame);

    if (var->type == SO_Type::STRING && var->STRING.value == "__undefined_var__")
    {
        make_error("Variable '" + name + "' is undefined");
        exit();
        return;
    }

    if (var->type == SO_Type::OP || var->type == SO_Type::FUNCTION)
    {
        auto copy = std::make_shared<StackObject>(*var);
        frame->stack.push_back(copy);
        return;
    }

    frame->stack.push_back(var);
    return;
}

void Bytecode_Evaluator::eval_load_const(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    frame->stack.push_back(op.data);
}

void Bytecode_Evaluator::eval_load_builtin(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    auto name = op.data->STRING.value;

    auto function = so_make_function();
    function->FUNCTION.name = name;
    function->FUNCTION.builtin = true;
    frame->stack.push_back(function);
    return;

    return;
}

void Bytecode_Evaluator::eval_load_tos_or_empty(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        auto empty = so_make_empty();
        frame->stack.push_back(empty);
        return;
    }
}


std::shared_ptr<StackObject>& Bytecode_Evaluator::eval_lookup(std::string name, std::shared_ptr<StackFrame>& frame)
{
    // Check locals
    if (frame->locals.find(name) != frame->locals.end())
    {
        return frame->locals[name];
    }

    // Check globals
    if (frame->globals.find(name) != frame->globals.end())
    {
        return frame->globals[name];
    }

    // Check outer frame
    auto outer_frame = frame->outer_frame;
    while (outer_frame != nullptr)
    {
        if (outer_frame->locals.find(name) != outer_frame->locals.end())
        {
            return (outer_frame->locals)[name];
        }

        outer_frame = outer_frame->outer_frame;
    }

    // TODO: change to an undefined type, SO_Type::UNDEFINED instead of this hack
    auto undefined = so_make_string("__undefined_var__");

    return undefined;
}

void Bytecode_Evaluator::eval_store(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Right hand side of '=' must have a value");
        exit();
        return;
    }

    auto value = frame->stack.back();
    frame->stack.pop_back();

    std::string name = op.data->STRING.value;
    frame->locals[name] = value;

    if (value->type == SO_Type::FUNCTION)
    {
        value->FUNCTION.name = name;
    }
}

void Bytecode_Evaluator::eval_store_at(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Right hand side of '=' must have a value");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto& left = frame->stack.back();
    frame->stack.pop_back();
    auto value = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>(*value);

    if (left->type == SO_Type::LIST && right->type == SO_Type::INT)
    {
        if (right->INT.value < 0)
        {
            left->LIST.objects.insert(left->LIST.objects.begin(), value);
            return;
        }

        if (right->INT.value >= left->LIST.objects.size())
        {
            left->LIST.objects.push_back(value);
            return;
        }

        left->LIST.objects[right->INT.value] = value;
        return;
    }

    if (left->type == SO_Type::OBJECT && right->type == SO_Type::STRING)
    {
        left->OBJECT.properties[right->STRING.value] = value;

        if (value->type == SO_Type::FUNCTION)
        {
            value->FUNCTION.name = right->STRING.value;
        }

        return;
    }
}

void Bytecode_Evaluator::eval_add(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '+'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::INT;
        res->INT.value = left->INT.value + right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->INT.value + right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value + right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value + right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = left->STRING.value + right->STRING.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::INT)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = left->STRING.value + std::to_string(right->INT.value);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = std::to_string(left->INT.value) + right->STRING.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = left->STRING.value + std::to_string(right->FLOAT.value);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = std::to_string(left->FLOAT.value) + right->STRING.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::BOOL)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = left->STRING.value + (right->BOOL.value ? "true" : "false");
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::BOOL && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::STRING;
        res->STRING.value = (left->BOOL.value ? "true" : "false") + right->STRING.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::LIST && right->type == SO_Type::LIST)
    {
        res->type = SO_Type::LIST;
        res->LIST.objects = left->LIST.objects;
        for (auto element : right->LIST.objects)
        {
            res->LIST.objects.push_back(element);
        }
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::LIST)
    {
        res->type = SO_Type::LIST;
        res->LIST.objects = left->LIST.objects;
        res->LIST.objects.push_back(right);
        frame->stack.push_back(res);
        return;
    }
    
    if (right->type == SO_Type::LIST)
    {
        res->type = SO_Type::LIST;
        res->LIST.objects = right->LIST.objects;
        res->LIST.objects.insert(res->LIST.objects.begin(), left);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::OBJECT && right->type == SO_Type::OBJECT)
    {
        res = left;
        for (auto prop : right->OBJECT.properties)
        {
            res->OBJECT.properties[prop.first] = prop.second;
        }
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '+' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_sub(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '-'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::INT;
        res->INT.value = left->INT.value - right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->INT.value - right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value - right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value - right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '-' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_mul(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '*'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::INT;
        res->INT.value = left->INT.value * right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->INT.value * right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value * right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value * right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::STRING;
        std::string value;
        for (int i = 0; i < left->INT.value; i++)
        {
            value += right->STRING.value;
        }
        res->STRING.value = value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::INT)
    {
        res->type = SO_Type::STRING;
        std::string value;
        for (int i = 0; i < right->INT.value; i++)
        {
            value += left->STRING.value;
        }
        res->STRING.value = value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::LIST && right->type == SO_Type::INT)
    {
        res->type = SO_Type::LIST;
        for (int i = 0; i < right->INT.value; i++)
        {
            res->LIST.objects.push_back(std::make_shared<StackObject>(*left));
        }
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::LIST)
    {
        res->type = SO_Type::LIST;
        for (int i = 0; i < left->INT.value; i++)
        {
            res->LIST.objects.push_back(std::make_shared<StackObject>(*right));
        }
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '*' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_div(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '/'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = (double)left->INT.value / (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = (double)left->INT.value / right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value / (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = left->FLOAT.value / right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '/' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_mod(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '%'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::INT;
        res->INT.value = left->INT.value % right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = fmod(left->INT.value, right->FLOAT.value);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = fmod(left->FLOAT.value, right->INT.value);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = fmod(left->FLOAT.value, right->FLOAT.value);
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '%' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_lt(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '<'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value < (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value < right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value < (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value < right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    res->type = SO_Type::BOOL;
    res->BOOL.value = false;
    frame->stack.push_back(res);
}

void Bytecode_Evaluator::eval_gt(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '>'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value > (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value > right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value > (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value > right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    res->type = SO_Type::BOOL;
    res->BOOL.value = false;
    frame->stack.push_back(res);
}

void Bytecode_Evaluator::eval_lte(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '<='");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value <= (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value <= right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value <= (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value <= right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    res->type = SO_Type::BOOL;
    res->BOOL.value = false;
    frame->stack.push_back(res);
}

void Bytecode_Evaluator::eval_gte(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '>='");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value >= (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value >= right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value >= (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value >= right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    res->type = SO_Type::BOOL;
    res->BOOL.value = false;
    frame->stack.push_back(res);
}

void Bytecode_Evaluator::eval_eq_eq(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '=='");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::EMPTY && right->type == SO_Type::EMPTY)
    {
        res = so_make_bool(true);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::BOOL && right->type == SO_Type::BOOL)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->BOOL.value == right->BOOL.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value == (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value == right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value == (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value == right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->STRING.value == right->STRING.value;
        frame->stack.push_back(res);
        return;
    }

    res->type = SO_Type::BOOL;
    res->BOOL.value = false;
    frame->stack.push_back(res);
}

void Bytecode_Evaluator::eval_not_eq(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '!='");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::EMPTY && right->type == SO_Type::EMPTY)
    {
        res = so_make_bool(false);
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::BOOL && right->type == SO_Type::BOOL)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->BOOL.value != right->BOOL.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value != (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::INT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = (double)left->INT.value != right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value != (double)right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::FLOAT && right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->FLOAT.value != right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::STRING && right->type == SO_Type::STRING)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->STRING.value != right->STRING.value;
        frame->stack.push_back(res);
        return;
    }

    res->type = SO_Type::BOOL;
    res->BOOL.value = true;
    frame->stack.push_back(res);
}

void Bytecode_Evaluator::eval_and(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '&&'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::BOOL && !left->BOOL.value)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = false;
        frame->stack.push_back(res);
        return;
    }

    if (left->type == SO_Type::BOOL && right->type == SO_Type::BOOL)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->BOOL.value && right->BOOL.value;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '&&' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_or(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '||'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::BOOL && right->type == SO_Type::BOOL)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = left->BOOL.value || right->BOOL.value;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '||' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_not(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 1)
    {
        make_error("Malfunctioned operation '!'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (right->type == SO_Type::BOOL)
    {
        res->type = SO_Type::BOOL;
        res->BOOL.value = !right->BOOL.value;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '!' operation on " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_pos(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 1)
    {
        make_error("Malfunctioned operation unary '+'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (right->type == SO_Type::INT)
    {
        res->type = SO_Type::INT;
        res->INT.value = +right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = +right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '+' unary operation on " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_neg(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 1)
    {
        make_error("Malfunctioned operation unary '-'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (right->type == SO_Type::INT)
    {
        res->type = SO_Type::INT;
        res->INT.value = -right->INT.value;
        frame->stack.push_back(res);
        return;
    }

    if (right->type == SO_Type::FLOAT)
    {
        res->type = SO_Type::FLOAT;
        res->FLOAT.value = -right->FLOAT.value;
        frame->stack.push_back(res);
        return;
    }

    if (right->type == SO_Type::STRING)
    {
        res->type = SO_Type::STRING;
        std::string copy(right->STRING.value);
        std::reverse(copy.begin(), copy.end());
        res->STRING.value = copy;
        frame->stack.push_back(res);
        return;
    }

    if (right->type == SO_Type::LIST)
    {
        res->type = SO_Type::LIST;
        std::vector<std::shared_ptr<StackObject>> copy(right->LIST.objects);
        std::reverse(copy.begin(), copy.end());
        res->LIST.objects = copy;
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '-' unary operation on " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_dot(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '.'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::STRING && right->type == SO_Type::STRING)
    {
        if (right->STRING.value == "to_chars")
        {
            res->type = SO_Type::LIST;
            for (char c : left->STRING.value)
            {
                auto c_object = so_make_string(std::string(1, c));
                res->LIST.objects.push_back(c_object);
            }
            frame->stack.push_back(res);
            return;
        }

        if (right->STRING.value == "length")
        {
            res->type = SO_Type::INT;
            res->INT.value = left->STRING.value.length();
            frame->stack.push_back(res);
            return;
        }

        make_error("Property '" + right->STRING.value + "' does not exist on type 'string'");
        exit();
        return;
    }

    if (left->type == SO_Type::LIST && right->type == SO_Type::INT)
    {
        int list_length = left->LIST.objects.size();
        int index = right->INT.value;
        if (index >= list_length || index < 0)
        {
            res = so_make_empty();
            frame->stack.push_back(res);
            return;
        }
        else
        {
            auto& val = left->LIST.objects[index];
            frame->stack.push_back(val);
            return;
        }

    }

    if (left->type == SO_Type::LIST && right->type == SO_Type::STRING)
    {
        // accessing list built-in methods
        if (right->STRING.value == "length")
        {
            int length = left->LIST.objects.size();
            res->type = SO_Type::INT;
            res->INT.value = length;
            frame->stack.push_back(res);
            return;
        }
        
        make_error("Property '" + right->STRING.value + "' does not exist on type 'list'");
        exit();
        return;
    }

    if (left->type == SO_Type::OBJECT && right->type == SO_Type::STRING)
    {
        auto name = right->STRING.value;

        if (name == "_keys")
        {
            res->type = SO_Type::LIST;
            for (auto props : left->OBJECT.properties)
            {
                auto key = so_make_string(props.first);
                res->LIST.objects.push_back(key);
            }

            frame->stack.push_back(res);
            return;
        }

        if (name == "_values")
        {
            res->type = SO_Type::LIST;
            for (auto props : left->OBJECT.properties)
            {
                res->LIST.objects.push_back(props.second);
            }

            frame->stack.push_back(res);
            return;
        }

        if (left->OBJECT.properties.find(name) == left->OBJECT.properties.end())
        {
            res = so_make_empty();
            frame->stack.push_back(res);
            return;
        }

        res = left->OBJECT.properties[name];

        if (res->type == SO_Type::OP || res->type == SO_Type::FUNCTION)
        {
            res = std::make_shared<StackObject>(*res);
            frame->stack.push_back(res);
        }
        else
        {
            auto& res_ref = left->OBJECT.properties[name];
            frame->stack.push_back(res_ref);
        }
        return;
    }

    make_error("Cannot perform '.' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_range(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '..'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();
    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto res = std::make_shared<StackObject>();
    res->line = right->line;
    res->column = right->column;

    if (left->type == SO_Type::INT && right->type == SO_Type::INT)
    {
        res->type = SO_Type::LIST;
        for (int i = left->INT.value; i <= right->INT.value; i++)
        {
            auto element = std::make_shared<StackObject>(SO_Type::INT);
            element->INT.value = i;
            res->LIST.objects.push_back(element);
        }
        frame->stack.push_back(res);
        return;
    }

    make_error("Cannot perform '..' operation on " + left->repr() + " and " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_build_list(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    auto list = std::make_shared<StackObject>(SO_Type::LIST);
    for (int i = 0; i < op.index; i++)
    {
        auto element = frame->stack.back();
        frame->stack.pop_back();

        list->LIST.objects.insert(list->LIST.objects.begin(), element);
    }

    frame->stack.push_back(list);
}

void Bytecode_Evaluator::eval_build_comma_list(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    auto list = std::make_shared<StackObject>(SO_Type::COMMA_LIST);
    list->column = op.column;
    list->line = op.line;

    for (int i = 0; i < op.index; i++)
    {
        auto element = frame->stack.back();
        frame->stack.pop_back();

        list->COMMA_LIST.objects.insert(list->COMMA_LIST.objects.begin(), element);
    }

    frame->stack.push_back(list);
}

void Bytecode_Evaluator::eval_build_object(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    auto object = std::make_shared<StackObject>(SO_Type::OBJECT);
    auto& obj_ref = object;

    for (int i = 0; i < op.index; i++)
    {
        auto value = frame->stack.back();
        frame->stack.pop_back();

        auto key = frame->stack.back();
        frame->stack.pop_back();

        if (key->type != SO_Type::STRING)
        {
            make_error("Object key must be either an id or a string");
            exit();
            return;
        }

        auto name = key->STRING.value;
        object->OBJECT.properties[name] = value;

        frame->locals[name] = value;

        if (value->type == SO_Type::FUNCTION)
        {
            value->FUNCTION.name = name;
            value->FUNCTION.closure = frame->locals;
            value->FUNCTION.closure["this"] = obj_ref;
        }
    }

    frame->outer_frame->stack.push_back(object);
}

void Bytecode_Evaluator::eval_build_iter(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    auto num_args = op.index;
    std::string index_name = "_i";
    std::string item_name = "_it";

    if (num_args == 2)
    {
        auto index_name_node = frame->stack.back();
        frame->stack.pop_back();
        index_name = index_name_node->STRING.value;
    }

    if (num_args == 3)
    {
        auto item_name_node = frame->stack.back();
        frame->stack.pop_back();
        item_name = item_name_node->STRING.value;

        auto index_name_node = frame->stack.back();
        frame->stack.pop_back();
        index_name = index_name_node->STRING.value;
    }

    auto iterator = frame->stack.back();
    frame->stack.pop_back();

    if (iterator->type != SO_Type::LIST)
    {
        make_error("Iterator must be of type 'list'");
        exit();
        return;
    }

    if (iterator->LIST.objects.size() == 0)
    {
        // forward to loop_start so we get the end loop index
        forward();
        // add 1 to the end loop index to bypass remove_iter, because there is no iter
        int end_index = current_op.index + 1;
        // jump
        index = end_index;
        current_op = instructions[index];
        return;
    }

    auto iter = std::make_shared<StackObject>(SO_Type::ITERATOR);
    iter->ITER.iterator = iterator;
    iter->ITER.current_index = 0;
    iter->ITER.current_element = iterator->LIST.objects[0];
    iter->ITER.index_name = index_name;
    iter->ITER.item_name = item_name;

    frame->stack.push_back(iter);
}

void Bytecode_Evaluator::eval_build_partial_op(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 3)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    auto op = frame->stack.back();
    frame->stack.pop_back();

    auto right = frame->stack.back();
    frame->stack.pop_back();

    auto left = frame->stack.back();
    frame->stack.pop_back();

    auto partial_op = std::make_shared<StackObject>(*op);
    partial_op->OP.left = left;
    partial_op->OP.right = right;

    frame->stack.push_back(partial_op);
}

void Bytecode_Evaluator::eval_loop_start(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    // remove any empty objects left on the stack
    while (frame->stack.back()->type != SO_Type::ITERATOR)
    {
        frame->stack.pop_back();
    }

    auto loop_basis = frame->stack.back();

    if (loop_basis->type == SO_Type::ITERATOR)
    {
        // We're in a for loop

        if (loop_basis->ITER.iter_count > 0)
        {
            // Increment iterator
            loop_basis->ITER.current_index += 1;
            if (loop_basis->ITER.current_index == loop_basis->ITER.iterator->LIST.objects.size())
            {
                index = op.index;
                current_op = instructions[index];

                return;
            }
            loop_basis->ITER.current_element = 
                loop_basis->ITER.iterator->LIST.objects[loop_basis->ITER.current_index];
        }

        auto index = std::make_shared<StackObject>(SO_Type::INT);
        index->INT.value = loop_basis->ITER.current_index;
        
        frame->locals[loop_basis->ITER.index_name] = index;
        frame->locals[loop_basis->ITER.item_name] = loop_basis->ITER.current_element;

        loop_basis->ITER.iter_count += 1;

        return;
    }
}

void Bytecode_Evaluator::eval_while_start(std::shared_ptr<StackFrame>& frame)
{
    // Not sure if we need this in the future, for now will leave it blank
    return;
}

void Bytecode_Evaluator::eval_jump_to(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    // -1 because when we return, we forward
    index = op.index - 1;
    current_op = instructions[index];

    return;
}

void Bytecode_Evaluator::eval_break(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    if (op.data->type == SO_Type::EMPTY)
    {
        make_error("'break' cannot be used outside of a loop");
        exit();
        return;
    }

    int jump_to_index = op.index;
    index = jump_to_index;
    current_op = instructions[index];

    return;
}

void Bytecode_Evaluator::eval_return(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    auto return_value = frame->stack.back();
    frame->stack.pop_back();

    if (frame->outer_frame == nullptr)
    {
        make_error("Cannot return out of main scope");
        exit();
        return;
    }

    frame->outer_frame->stack.push_back(return_value);
    exit();
    backward();
    return;
}

void Bytecode_Evaluator::eval_copy(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    auto value = frame->stack.back();
    frame->stack.pop_back();

    auto copy = std::make_shared<StackObject>(*value);
    frame->stack.push_back(copy);
    return;
}

void Bytecode_Evaluator::eval_frame_in(std::shared_ptr<StackFrame>& frame)
{
    auto inner_frame = std::make_shared<StackFrame>();
    inner_frame->outer_frame = frame;
    frame = inner_frame;
}

void Bytecode_Evaluator::eval_frame_out(std::shared_ptr<StackFrame>& frame)
{
    frame->locals.clear();
    frame->stack.clear();
    
    auto outer_frame = frame->outer_frame;
    frame = outer_frame;
}

void Bytecode_Evaluator::eval_remove_iter(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    auto iter = frame->stack.back();

    while (iter->type != SO_Type::ITERATOR)
    {
        iter = frame->stack.back();
    }

    frame->stack.pop_back();

    frame->locals.erase(iter->ITER.index_name);
    frame->locals.erase(iter->ITER.item_name);

    return;
}

void Bytecode_Evaluator::eval_builtin(std::shared_ptr<StackObject> function, std::shared_ptr<StackFrame>& frame)
{
    if (function->FUNCTION.name == "print")
    {
        for (auto arg : function->FUNCTION.arguments)
        {
            std::cout << arg->repr();
        }

        frame->stack.push_back(so_make_empty());
        return;
    }

    if (function->FUNCTION.name == "read")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'read' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        std::ifstream stream(args[0]->STRING.value);
        std::stringstream buffer;
        buffer << stream.rdbuf();

        frame->stack.push_back(so_make_string(buffer.str()));
        return;
    }

    if (function->FUNCTION.name == "write")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 2)
        {
            make_error("Builtin function 'write' expects 2 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto file_path = args[0]->STRING.value;
        auto value = args[1]->STRING.value;

        std::ofstream new_file(file_path);
        new_file << value;
        new_file.close();

        frame->stack.push_back(so_make_empty());
        return;
    }

    if (function->FUNCTION.name == "append")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 2)
        {
            make_error("Builtin function 'append' expects 2 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto file_path = args[0]->STRING.value;
        auto value = args[1]->STRING.value;

        std::ofstream new_file(file_path, std::ios_base::app);
        new_file << value;
        new_file.close();

        frame->stack.push_back(so_make_empty());
        return;
    }

    if (function->FUNCTION.name == "frame")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 0)
        {
            make_error("Builtin function 'frame' expects 0 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto frame_obj = so_make_object();
        auto locals = so_make_object();
        auto globals = so_make_object();
        auto stack = so_make_list();

        for (auto& var : frame->locals) 
        {
            locals->OBJECT.properties[var.first] = var.second;
        }

        frame_obj->OBJECT.properties["locals"] = locals;

        for (auto& var : frame->globals) 
        {
            globals->OBJECT.properties[var.first] = var.second;
        }

        frame_obj->OBJECT.properties["globals"] = globals;

        for (auto& obj : frame->stack)
        {
            stack->LIST.objects.push_back(obj);
        }

        frame_obj->OBJECT.properties["stack"] = stack;

        frame->stack.push_back(frame_obj);
        return;
    }

    if (function->FUNCTION.name == "to_string")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'to_string' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        frame->stack.push_back(so_make_string(args[0]->repr()));
        return;
    }

    if (function->FUNCTION.name == "to_int")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'to_int' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto result = so_make_int(0);
        result->column = args[0]->column;
        result->line = args[0]->line;

        if (args[0]->type == SO_Type::BOOL)
        {
            result->INT.value = (long long)args[0]->BOOL.value;
        }
        else if (args[0]->type == SO_Type::INT)
        {
            result = args[0];
        }
        else if (args[0]->type == SO_Type::FLOAT)
        {
            result->INT.value = (long long)args[0]->FLOAT.value;
        }
        else if (args[0]->type == SO_Type::STRING)
        {
            try {
                if (args[0]->STRING.value.size() == 1 && !isdigit(args[0]->STRING.value[0]))
                {
                    char c = args[0]->STRING.value[0];
                    result->INT.value = int(c);
                }
                else
                {
                    result->INT.value = std::stol(args[0]->STRING.value);
                }
            } catch(...) {
                make_error("Unable to cast '" + args[0]->repr() + "' to an integer");
                exit();
                return;
            }
        }
        else
        {
            make_error("Unable to cast '" + args[0]->repr() + "' to an integer");
            exit();
            return;
        }

        frame->stack.push_back(result);
        return;
    }

    if (function->FUNCTION.name == "to_float")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'to_float' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto result = so_make_float(0);
        result->column = args[0]->column;
        result->line = args[0]->line;

        if (args[0]->type == SO_Type::BOOL)
        {
            result->INT.value = (double)args[0]->BOOL.value;
        }
        else if (args[0]->type == SO_Type::FLOAT)
        {
            result = args[0];
        }
        else if (args[0]->type == SO_Type::INT)
        {
            result->INT.value = (double)args[0]->INT.value;
        }
        else if (args[0]->type == SO_Type::STRING)
        {
            try {
                result->INT.value = std::stof(args[0]->STRING.value);
            } catch(...) {
                make_error("Unable to cast '" + args[0]->repr() + "' to a float");
                exit();
                return;
            }
        }
        else
        {
            make_error("Unable to cast '" + args[0]->repr() + "' to a float");
            exit();
            return;
        }

        frame->stack.push_back(result);
        return;
    }

    if (function->FUNCTION.name == "type")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'type' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        std::shared_ptr<StackObject> return_value;

        switch (args[0]->type)
        {
            case SO_Type::STRING: return_value = so_make_string("string"); break;
            case SO_Type::INT: return_value = so_make_string("int"); break;
            case SO_Type::FLOAT: return_value = so_make_string("float"); break;
            case SO_Type::BOOL: return_value = so_make_string("bool"); break;
            case SO_Type::LIST: return_value = so_make_string("list"); break;
            case SO_Type::COMMA_LIST: return_value = so_make_string("comma_list"); break;
            case SO_Type::OBJECT: return_value = so_make_string("object"); break;
            case SO_Type::FUNCTION: return_value = so_make_string("function"); break;
            case SO_Type::OP: return_value = so_make_string("op<" + args[0]->OP.repr + ">"); break;
            case SO_Type::EMPTY: return_value = so_make_string("empty"); break;
        }

        frame->stack.push_back(return_value);
        return;
    }

    if (function->FUNCTION.name == "address")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'address' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto address = so_make_int((long)args[0].get());
        frame->stack.push_back(address);
        return;
    }

    if (function->FUNCTION.name == "delete")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'delete' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        if (args[0]->type != SO_Type::STRING)
        {
            make_error("Builtin function 'delete' expects a variable name");
            exit();
            return;
        }

        frame->locals.erase(args[0]->STRING.value);
        return;
    }

    if (function->FUNCTION.name == "delete_prop")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 2)
        {
            make_error("Builtin function 'delete_prop' expects 2 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        if (args[0]->type != SO_Type::OBJECT && args[1]->type != SO_Type::STRING)
        {
            make_error("Builtin function 'delete_prop' expects an object and a property name");
            exit();
            return;
        }

        args[0]->OBJECT.properties.erase(args[1]->STRING.value);
        return;
    }

    if (function->FUNCTION.name == "shape")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'shape' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        if (args[0]->type != SO_Type::OBJECT)
        {
            make_error("Builtin function 'shape' expects argument of type 'object'");
            exit();
            return;
        }

        auto shape = so_make_object();
        shape->OBJECT.properties["length"] = so_make_int(args[0]->OBJECT.properties.size());
        shape->OBJECT.properties["keys"] = so_make_list();
        for (auto prop : args[0]->OBJECT.properties)
        {
            auto key = so_make_string(prop.first);
            shape->OBJECT.properties["keys"]->LIST.objects.push_back(key);
        }

        auto& keys = shape->OBJECT.properties["keys"]->LIST.objects;
        
        sort( keys.begin( ), keys.end( ), [ ]( const auto& lhs, const auto& rhs )
        {
            return lhs->STRING.value < rhs->STRING.value;
        });

        frame->stack.push_back(shape);
        return;
    }

    if (function->FUNCTION.name == "out")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'out' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        if (frame->outer_frame == nullptr)
        {
            make_error("Cannot push out a value when in the main scope");
            exit();
            return;
        }

        frame->outer_frame->stack.push_back(args[0]);
        return;
    }

    if (function->FUNCTION.name == "time")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 0)
        {
            make_error("Builtin function 'out' expects 0 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        auto value = so_make_int(time);

        frame->stack.push_back(value);
        return;
    }

    if (function->FUNCTION.name == "import")
    {
        auto args = function->FUNCTION.arguments;
        if (args.size() != 1)
        {
            make_error("Builtin function 'import' expects 1 argument but " 
            + std::to_string(args.size()) + " were provided");
            exit();
            return;
        }

        Lexer lexer(args[0]->STRING.value);
        lexer.tokenize();

        Parser parser(lexer.file_name, lexer.nodes);
        parser.parse();

        Bytecode_Generator generator(parser.file_name, parser.nodes);
        auto instructions = generator.generate();
        //generator.print_instructions();

        parser.nodes.clear();

        auto import_frame = std::make_shared<StackFrame>();

        Bytecode_Evaluator evaluator(instructions);
        evaluator.repl = repl;
        evaluator.file_name = parser.file_name;
        evaluator.evaluate(import_frame);

        auto import_object = so_make_object();

        for (auto symbol : import_frame->locals)
        {
            // if symbol is a lambda, we include itself in its own closure
            // so that in the case it's recursive, it can be referenced
            // from a different scope

            if (symbol.second->type == SO_Type::FUNCTION)
            {
                symbol.second->FUNCTION.closure[symbol.first] = symbol.second;
            }

            import_object->OBJECT.properties[symbol.first] = symbol.second;
        }

        frame->stack.push_back(import_object);
    }

    return;
}

void Bytecode_Evaluator::eval_build_function(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    // We capture local scope upon definition
    auto function = std::make_shared<StackObject>(*op.data);
    for (auto element : frame->locals)
    {
        function->FUNCTION.closure[element.first] = element.second;
    }

    frame->stack.push_back(function);
}

void Bytecode_Evaluator::eval_call_function(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    auto function = frame->stack.back();
    frame->stack.pop_back();

    if (!(function->type == SO_Type::FUNCTION))
    {
        make_error("Malformed function call - no function found");
        exit();
        return;
    }

    auto name = function->FUNCTION.name;
    auto& arguments = function->FUNCTION.arguments;
    auto parameters = function->FUNCTION.parameters;

    int preloded_args_size = arguments.size();
    bool has_preloaded_args = preloded_args_size > 0;

    for (int i = 0; i < op.index; i++)
    {
        if (frame->stack.size() == 0)
        {
            make_error("Malformed expression <Empty stack>");
            exit();
            return;
        }

        auto arg = frame->stack.back();
        frame->stack.pop_back();
        // If we have preloaded args, we want any new args to be inserted AFTER the initial args
        // otherwise, we want those args to be inserted at the beginning
        // because they are pulled off the stack in reverse order
        if (has_preloaded_args)
        {
            arguments.insert(arguments.begin() + preloded_args_size, arg);
        }
        else
        {
            arguments.insert(arguments.begin(), arg);
        }
    }

    // if built-in, defer evaluation
    if (function->FUNCTION.builtin)
    {
        eval_builtin(function, frame);
        return;
    }

    if (arguments.size() > parameters.size())
    {
        make_error("Function '" + name + "' expects " + std::to_string(parameters.size()) 
            + " argument(s) but " + std::to_string(arguments.size()) 
            + " were provided.");

        exit();
        return;
    }

    // Create curried function and push back on stack

    if (arguments.size() < parameters.size())
    {
        frame->stack.push_back(function);
        return;
    }

    auto function_frame = std::make_shared<StackFrame>();
    function_frame->outer_frame = frame;

    for (auto elem : function->FUNCTION.closure)
    {
        if (elem.second->type == SO_Type::OP || elem.second->type == SO_Type::FUNCTION)
        {
            function_frame->locals[elem.first] = std::make_shared<StackObject>(*elem.second);
        }
        else
        {
            function_frame->locals[elem.first] = elem.second;
        }
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        function_frame->locals[parameters[i]] = arguments[i];
    }

    Bytecode_Evaluator evaluator(function->FUNCTION.instructions);
    evaluator.repl = repl;
    evaluator.file_name = file_name;
    evaluator.evaluate(function_frame);

    function_frame->locals.clear();
}

void Bytecode_Evaluator::eval_arrow(std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() < 2)
    {
        make_error("Malfunctioned operation '->'");
        exit();
        return;
    }

    auto right = frame->stack.back();
    frame->stack.pop_back();

    if (right->type == SO_Type::OP)
    {
        auto left = frame->stack.back();
        frame->stack.pop_back();

        if (left->type == SO_Type::COMMA_LIST)
        {
            if (right->OP.is_unary)
            {
                make_error("Unary operator '" + right->repr() + "' can only be injected with one value");
            }

            if (left->COMMA_LIST.objects.size() != 2)
            {
                make_error("Cannot inject more than 2 values into the partial operator " + right->repr());
                exit();
                return;
            }

            if (right->OP.left->type != SO_Type::EMPTY && right->OP.right->type != SO_Type::EMPTY)
            {
                make_error("Operator " + right->repr() + " is not a fully partial operator. It can only be injected with a single value");
                exit();
                return;
            }

            right->OP.left = left->COMMA_LIST.objects[0];
            right->OP.right = left->COMMA_LIST.objects[1];
        }
        else 
        {
            // Inject into the empty operand

            if (right->OP.is_unary)
            {
                right->OP.right = left;            
            }
            else
            {
                if (right->OP.left->type == SO_Type::EMPTY)
                {
                    right->OP.left = left;
                }
                else if (right->OP.right->type == SO_Type::EMPTY)
                {
                    right->OP.right = left;
                }
            }
        }

        if (right->OP.is_unary)
        {
            // we've injected it fully, so we just execute it now
            frame->stack.push_back(right->OP.right);

            switch(right->OP.type)
            {
                case Operation_Type::POS:
                {
                    eval_pos(frame);
                    break;
                }
                case Operation_Type::NEG:
                {
                    eval_neg(frame);
                    break;
                }
                case Operation_Type::NOT:
                {
                    eval_not(frame);
                    break;
                }
            }

            return;
        }

        // If right op still has empty operands, we leave it on the stack as a right op

        if (right->OP.left->type == SO_Type::EMPTY || right->OP.right->type == SO_Type::EMPTY)
        {
            frame->stack.push_back(right);
            return;
        }

        frame->stack.push_back(right->OP.left);
        frame->stack.push_back(right->OP.right);

        switch(right->OP.type)
        {
            case Operation_Type::ADD:
            {
                eval_add(frame);
                break;
            }
            case Operation_Type::SUB:
            {
                eval_sub(frame);
                break;
            }
            case Operation_Type::MUL:
            {
                eval_mul(frame);
                break;
            }
            case Operation_Type::DIV:
            {
                eval_div(frame);
                break;
            }
            case Operation_Type::RANGE:
            {
                eval_range(frame);
                break;
            }
            case Operation_Type::ARROW:
            {
                eval_arrow(frame);
                break;
            }
            case Operation_Type::LT:
            {
                eval_lt(frame);
                break;
            }
            case Operation_Type::LTE:
            {
                eval_lte(frame);
                break;
            }
            case Operation_Type::GT:
            {
                eval_gt(frame);
                break;
            }
            case Operation_Type::GTE:
            {
                eval_gte(frame);
                break;
            }
            case Operation_Type::EQ_EQ:
            {
                eval_eq_eq(frame);
                break;
            }
            case Operation_Type::NOT_EQ:
            {
                eval_not_eq(frame);
                break;
            }
            case Operation_Type::AND:
            {
                eval_and(frame);
                break;
            }
            case Operation_Type::OR:
            {
                eval_or(frame);
                break;
            }
        }

        return;
    }

    if (right->type == SO_Type::FUNCTION)
    {
        auto left = frame->stack.back();
        frame->stack.pop_back();

        if (left->type == SO_Type::COMMA_LIST)
        {
            for (auto value : left->COMMA_LIST.objects)
            {
                right->FUNCTION.arguments.push_back(value);
            }
        }
        else
        {
            right->FUNCTION.arguments.push_back(left);
        }

        // if builtin, defer evaluation
        if (right->FUNCTION.builtin)
        {
            eval_builtin(right, frame);
            return;
        }

        // check if too many args

        auto arg_count = right->FUNCTION.arguments.size();
        auto param_count = right->FUNCTION.parameters.size();

        if (arg_count > param_count)
        {
            make_error("Function '" + right->FUNCTION.name + "' expects " 
            + std::to_string(param_count) + " arguments but " + std::to_string(arg_count) 
            + " were provided");
            exit();
            return;
        }

        // check if not enough arguments to call

        if (arg_count < param_count)
        {
            frame->stack.push_back(right);
            return;
        }

        // otherwise, we have all the arguments so we call the function

        auto function_frame = std::make_shared<StackFrame>();
        function_frame->outer_frame = frame;

        for (auto elem : right->FUNCTION.closure)
        {
            function_frame->locals[elem.first] = std::make_shared<StackObject>(*elem.second);
        }

        auto parameters = right->FUNCTION.parameters;
        auto arguments = right->FUNCTION.arguments;

        for (int i = 0; i < parameters.size(); i++)
        {
            function_frame->locals[parameters[i]] = arguments[i];
        }

        Bytecode_Evaluator evaluator(right->FUNCTION.instructions);
        evaluator.file_name = file_name;
        evaluator.repl = repl;
        evaluator.evaluate(function_frame);

        function_frame->locals.clear();
        return;
    }

    make_error("Cannot perform '->' operation on " + right->repr());
    exit();
}

void Bytecode_Evaluator::eval_jump_if_false(Bytecode op, std::shared_ptr<StackFrame>& frame)
{
    if (frame->stack.size() == 0)
    {
        make_error("Malformed expression <Empty stack>");
        exit();
        return;
    }

    auto value = frame->stack.back();
    frame->stack.pop_back();

    if (!value->BOOL.value)
    {
        // -1 because when we return, we forward
        index = op.index - 1;
        current_op = instructions[index];

        return;
    }

    return;
}

void Bytecode_Evaluator::make_error(std::string message)
{
    auto error = "\nError in '" + file_name + "' [" + std::to_string(current_op.line) + ":" + std::to_string(current_op.column) + "] - " + message + "\n";
    errors.push_back(error);
}

void Bytecode_Evaluator::exit()
{
    while (current_op.type != OpType::EXIT)
    {
        forward();
    }
}