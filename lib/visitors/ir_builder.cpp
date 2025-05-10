#include "ir_builder.h"

#include <iostream>
#include <sstream>

LLVMIRBuilder::LLVMIRBuilder(const SymbolTable<DataType>& table)
    : m_table(table),
      m_context(),
      m_builder(m_context),
      m_module("file", m_context) {
    auto main_function_type =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(m_context), false);

    m_main = llvm::Function::Create(
        main_function_type, llvm::Function::ExternalLinkage, "main", m_module);

    m_current_block = llvm::BasicBlock::Create(m_context, "entry", m_main);

    m_builder.SetInsertPoint(m_current_block);
}

void LLVMIRBuilder::visit(Sequence& node) {
    for (Action* action : node.actions) {
        visitVariant(*action);
    }
}

void LLVMIRBuilder::visit(Assignment& node) {
    visitVariant(*node.expr);

    static auto int_type = llvm::Type::getInt32Ty(m_context);
    static auto bool_type = llvm::Type::getInt8Ty(m_context);

    SymbolId id = node.var->entry;

    if (m_variables.find(id) == m_variables.end()) {
        DataType type = m_table[id];
        switch (type) {
            case DataType::Int: {
                m_variables[id] = m_builder.CreateAlloca(int_type);
            } break;
            case DataType::Bool: {
                m_variables[id] = m_builder.CreateAlloca(bool_type);
            } break;
        }
    }

    m_builder.CreateStore(m_value, m_variables[node.var->entry]);
}

template <class T>
llvm::Value* MakeConst(llvm::LLVMContext& context_, T x);

template <>
llvm::Value* MakeConst<int>(llvm::LLVMContext& context_, int x) {
    return llvm::ConstantInt::get(context_, llvm::APInt(32, x, true));
}

template <>
llvm::Value* MakeConst<bool>(llvm::LLVMContext& context_, bool x) {
    return llvm::ConstantInt::get(context_, llvm::APInt(1, x, true));
}

static llvm::Value* CreateLoad(llvm::IRBuilder<>* builder,
                               llvm::AllocaInst* alloca) {
    return builder->CreateLoad(alloca->getAllocatedType(), alloca);
}

void LLVMIRBuilder::visit(Branch& node) {
    static uint64_t sBranchId = 0;

    uint64_t branch_id = sBranchId;

    auto true_branch = llvm::BasicBlock::Create(
        m_context, (std::stringstream{} << "true" << branch_id).str().c_str(),
        m_main);

    auto false_branch = llvm::BasicBlock::Create(
        m_context, (std::stringstream{} << "false" << branch_id).str().c_str(),
        m_main);

    auto end_branch = llvm::BasicBlock::Create(
        m_context, (std::stringstream{} << "end" << branch_id).str().c_str(),
        m_main);

    visitVariant(*node.condition);

    llvm::Type* bool_type = llvm::IntegerType::getInt8Ty(m_context);
    auto cmp_value = m_builder.CreateICmpSGT(
        m_value, llvm::ConstantInt::get(m_context, llvm::APInt(8, 0, true)));

    m_builder.CreateCondBr(cmp_value, true_branch, false_branch);

    m_current_block = true_branch;
    m_builder.SetInsertPoint(true_branch);
    if (node.true_branch) node.true_branch->accept(*this);

    m_current_block = false_branch;
    m_builder.SetInsertPoint(false_branch);
    if (node.false_branch) node.false_branch->accept(*this);

    m_current_block = end_branch;

    m_builder.SetInsertPoint(true_branch);
    m_builder.CreateBr(end_branch);
    m_builder.SetInsertPoint(false_branch);
    m_builder.CreateBr(end_branch);
    m_builder.SetInsertPoint(end_branch);
}

void LLVMIRBuilder::visit(Print& node) {
    visitVariant(*node.expr);

    llvm::PointerType* pointer_type =
        llvm::PointerType::get(llvm::Type::getInt8Ty(m_context), 0);

    auto printf_function_type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(m_context), {pointer_type}, true);

    static auto printf_function = llvm::Function::Create(
        printf_function_type, llvm::Function::ExternalLinkage, "printf",
        m_module);

    std::string int_string = "%d\n";
    auto fmt_int = llvm::ConstantDataArray::getString(m_context, int_string);
    auto string_int_alloca = m_builder.CreateAlloca(fmt_int->getType());
    auto string_int_value = m_builder.CreateStore(fmt_int, string_int_alloca);

    auto formatted_string_int =
        m_builder.CreateBitCast(string_int_alloca, pointer_type);

    m_builder.CreateCall(printf_function, {formatted_string_int, m_value});
}

void LLVMIRBuilder::visit(Constant& node) {
    std::visit(
        [&](const auto& value) { m_value = MakeConst(m_context, value); },
        node.value);
}

void LLVMIRBuilder::visit(Variable& node) {
    m_value = CreateLoad(&m_builder, m_variables[node.entry]);
}

void LLVMIRBuilder::visit(BinaryOperator& node) {
    visitVariant(*node.left);
    llvm::Value* left_value = m_value;

    visitVariant(*node.right);
    llvm::Value* right_value = m_value;

    switch (node.type) {
        case BinaryOperator::Type::Add: {
            m_value = m_builder.CreateAdd(left_value, right_value);
        } break;
        case BinaryOperator::Type::Subtract: {
            m_value = m_builder.CreateSub(left_value, right_value);
        } break;
        case BinaryOperator::Type::Or: {
            m_value = m_builder.CreateOr(left_value, right_value);
        } break;
        case BinaryOperator::Type::And: {
            m_value = m_builder.CreateAnd(left_value, right_value);
        } break;
        case BinaryOperator::Type::Equal: {
            m_value = m_builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ,
                                          left_value, right_value);
        } break;
    }
}

void LLVMIRBuilder::finish() {
    llvm::Type* i32_type = llvm::IntegerType::getInt32Ty(m_context);
    m_builder.CreateRet(llvm::ConstantInt::get(i32_type, 0, true));
}

void writeModuleToFile(llvm::Module* module) {
    std::string error;
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    const llvm::Target* target =
        llvm::TargetRegistry::lookupTarget(targetTriple, error);
    llvm::TargetOptions options;
    llvm::Reloc::Model relocModel = llvm::Reloc::DynamicNoPIC;
    llvm::CodeModel::Model codeModel = llvm::CodeModel::Medium;
    llvm::CodeGenOptLevel optLevel = *llvm::CodeGenOpt::getLevel(1);

    llvm::TargetMachine* targetMachine = target->createTargetMachine(
        targetTriple, "generic", "", options, relocModel, codeModel, optLevel);

    llvm::legacy::PassManager pass;
    llvm::CodeGenFileType fileType = llvm::CodeGenFileType::ObjectFile;

    std::error_code EC;
    std::string outputFilename = "program.o";
    llvm::raw_fd_ostream outputFile(outputFilename, EC, llvm::sys::fs::OF_None);
    if (EC) {
        std::cerr << EC.message() << std::endl;
        return;
    }

    if (targetMachine->addPassesToEmitFile(pass, outputFile, nullptr,
                                           fileType)) {
        return;
    }

    pass.run(*module);
    outputFile.flush();

    outputFile.close();
}
