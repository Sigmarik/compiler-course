#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Value.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

#include "ast/ast.hpp"
#include "symbol_resolver.h"

class LLVMIRBuilder : public ASTVisitor {
   public:
    LLVMIRBuilder(const SymbolTable<DataType>& table);

    virtual void visit(Module& node) override {}
    virtual void visit(Function& node) override {}
    virtual void visit(Sequence& node) override;

    virtual void visit(Assignment& node) override;
    virtual void visit(Branch& node) override;
    virtual void visit(Print& node) override;
    virtual void visit(Return& node) override {}

    virtual void visit(FunctionCall& node) override {}
    virtual void visit(Constant& node) override;
    virtual void visit(Variable& node) override;
    virtual void visit(BinaryOperator& node) override;

    llvm::LLVMContext m_context;

    llvm::Module& getModule() { return m_module; }

    void finish();

   private:
    SymbolTable<DataType> m_table;
    llvm::IRBuilder<> m_builder;
    llvm::Module m_module;
    bool m_initialized_variables = false;
    llvm::Function* m_main = nullptr;
    llvm::BasicBlock* m_current_block = nullptr;
    llvm::Value* m_value = nullptr;
    SymbolTable<llvm::AllocaInst*> m_variables;
};
