//
// RISC-V代码生成器
// Created for CACT Compiler RISC-V Backend
//

#ifndef CACT_COMPILER_RISCVCODEGEN_H
#define CACT_COMPILER_RISCVCODEGEN_H

#include "RiscVInstr.h"
#include "../mir/Function.h"
#include "../mir/BasicBlock.h"
#include "../mir/Instr.h"
#include "../mir/Value.h"
#include "../mir/Type.h"
#include "../mir/Constant.h"
#include "../util/Manager.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>

namespace RiscV {

// 虚拟寄存器表示
class VirtualReg {
private:
    static int nextId;
    int id;
    Type* type;
    bool spilled;
    Register physicalReg;
    int stackOffset;
    
public:
    explicit VirtualReg(Type* t) : id(nextId++), type(t), spilled(false), 
                                  physicalReg(Register::ZERO), stackOffset(-1) {}
    
    int getId() const { return id; }
    Type* getType() const { return type; }
    bool isSpilled() const { return spilled; }
    Register getPhysicalReg() const { return physicalReg; }
    int getStackOffset() const { return stackOffset; }
    
    void setPhysicalReg(Register reg) { physicalReg = reg; spilled = false; }
    void setSpilled(int offset) { stackOffset = offset; spilled = true; }
    bool isFloat() const { return type->is_float_type(); }
};

// 寄存器分配器
class RegisterAllocator {
private:
    // 可用的通用寄存器 (排除系统寄存器)
    std::vector<Register> availableIntRegs = {
        Register::T0, Register::T1, Register::T2, Register::T3, Register::T4, Register::T5, Register::T6,
        Register::A0, Register::A1, Register::A2, Register::A3, Register::A4, Register::A5, Register::A6, Register::A7,
        Register::S2, Register::S3, Register::S4, Register::S5, Register::S6, Register::S7, Register::S8, Register::S9, Register::S10, Register::S11
    };
    
    // 可用的浮点寄存器
    std::vector<Register> availableFloatRegs = {
        Register::FT0, Register::FT1, Register::FT2, Register::FT3, Register::FT4, Register::FT5, Register::FT6, Register::FT7,
        Register::FA0, Register::FA1, Register::FA2, Register::FA3, Register::FA4, Register::FA5, Register::FA6, Register::FA7,
        Register::FS2, Register::FS3, Register::FS4, Register::FS5, Register::FS6, Register::FS7, Register::FS8, Register::FS9, Register::FS10, Register::FS11,
        Register::FT8, Register::FT9, Register::FT10, Register::FT11
    };
    
    // 寄存器使用状态
    std::unordered_map<Register, bool> intRegUsed;
    std::unordered_map<Register, bool> floatRegUsed;
    
    // 栈偏移计数
    int stackOffset;
    
public:
    RegisterAllocator();
    
    // 分配寄存器
    Register allocateIntReg();
    Register allocateFloatReg();
    void freeReg(Register reg);
    
    // 栈分配
    int allocateStack(int size);
    int getCurrentStackOffset() const { return stackOffset; }
    void resetStack() { stackOffset = 0; }
    
    // 重置分配器状态
    void reset();
};

// RISC-V代码生成器
class RiscVCodeGen {
private:
    Manager* manager;
    std::vector<std::shared_ptr<RiscVInstr>> instructions;
    RegisterAllocator regAlloc;
    
    // 值到虚拟寄存器的映射
    std::unordered_map<Value*, std::shared_ptr<VirtualReg>> valueToVReg;
    
    // 当前函数信息
    Function* currentFunction;
    int currentFunctionStackSize;
    std::unordered_map<std::string, int> labelIds;
    int nextLabelId;
    
    // 函数调用约定
    struct CallingConvention {
        static const int MAX_INT_ARGS = 8;   // a0-a7
        static const int MAX_FLOAT_ARGS = 8; // fa0-fa7
        
        static Register getIntArgReg(int index) {
            static const Register intArgRegs[] = {
                Register::A0, Register::A1, Register::A2, Register::A3,
                Register::A4, Register::A5, Register::A6, Register::A7
            };
            return (index < MAX_INT_ARGS) ? intArgRegs[index] : Register::ZERO;
        }
        
        static Register getFloatArgReg(int index) {
            static const Register floatArgRegs[] = {
                Register::FA0, Register::FA1, Register::FA2, Register::FA3,
                Register::FA4, Register::FA5, Register::FA6, Register::FA7
            };
            return (index < MAX_FLOAT_ARGS) ? floatArgRegs[index] : Register::FT0;
        }
    };
    
public:
    explicit RiscVCodeGen(Manager* mgr);
    
    // 主生成接口
    void generateCode();
    void outputAssembly(const std::string& filename);
    void outputAssembly(std::ostream& stream);
    
private:
    // 函数级别代码生成
    void generateFunction(Function* func);
    void generateFunctionPrologue(Function* func);
    void generateFunctionEpilogue(Function* func);
    
    // 基本块代码生成
    void generateBasicBlock(BasicBlock* bb);
    
    // 指令级别代码生成
    void generateInstruction(Instr* instr);
    
    // 具体指令类型处理
    void generateAlu(INSTR::Alu* alu);
    void generateIcmp(INSTR::Icmp* icmp);
    void generateFcmp(INSTR::Fcmp* fcmp);
    void generateZext(INSTR::Zext* zext);
    void generateFPtosi(INSTR::FPtosi* fptosi);
    void generateSItofp(INSTR::SItofp* sitofp);
    void generateAlloc(INSTR::Alloc* alloc);
    void generateLoad(INSTR::Load* load);
    void generateStore(INSTR::Store* store);
    void generateGetElementPtr(INSTR::GetElementPtr* gep);
    void generateCall(INSTR::Call* call);
    void generatePhi(INSTR::Phi* phi);
    void generateBranch(INSTR::Branch* branch);
    void generateJump(INSTR::Jump* jump);
    void generateReturn(INSTR::Return* ret);
    
    // 全局变量处理
    void generateGlobals();
    void generateGlobalVariable(GlobalValue* global, Initial* init);
    
    // 工具方法
    std::shared_ptr<VirtualReg> getOrCreateVReg(Value* value);
    Register getPhysicalReg(Value* value);
    void emitInstr(std::shared_ptr<RiscVInstr> instr);
    void emitLabel(const std::string& label);
    std::string getUniqueLabel(const std::string& prefix);
    
    // 常量处理
    void loadConstant(Register dest, int value);
    void loadConstant(Register dest, float value);
    void loadGlobalAddress(Register dest, const std::string& global);
    
    // 内存操作辅助
    void loadFromMemory(Register dest, Register base, int offset, Type* type);
    void storeToMemory(Register src, Register base, int offset, Type* type);
    
    // 类型处理
    bool isFloatType(Type* type);
    int getTypeSize(Type* type);
    RiscV::InstrType getLoadInstr(Type* type);
    RiscV::InstrType getStoreInstr(Type* type);
    
    // 立即数处理
    bool canUseImmediate(int value);
    void handleLargeImmediate(Register dest, int value);
    
    // 栈帧管理
    void calculateStackFrame(Function* func);
    int getStackOffset(Value* value);
    
    // 寄存器分配辅助
    void spillRegister(Register reg);
    void restoreRegister(Register reg);
    void saveCallerSavedRegs();
    void restoreCallerSavedRegs();
};

} // namespace RiscV

#endif // CACT_COMPILER_RISCVCODEGEN_H 