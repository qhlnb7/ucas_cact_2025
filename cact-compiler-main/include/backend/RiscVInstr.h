//
// RISC-V指令定义
// Created for CACT Compiler RISC-V Backend
//

#ifndef CACT_COMPILER_RISCVINSTR_H
#define CACT_COMPILER_RISCVINSTR_H

#include <string>
#include <vector>
#include <memory>

namespace RiscV {

// RISC-V寄存器定义
enum class Register {
    // 零寄存器
    ZERO = 0,   // x0
    
    // 函数返回地址
    RA = 1,     // x1
    
    // 栈指针
    SP = 2,     // x2
    
    // 全局指针
    GP = 3,     // x3
    
    // 线程指针
    TP = 4,     // x4
    
    // 临时寄存器
    T0 = 5, T1 = 6, T2 = 7,     // x5-x7
    
    // 保存/帧指针
    S0 = 8, FP = 8,             // x8
    S1 = 9,                     // x9
    
    // 函数参数/返回值
    A0 = 10, A1 = 11, A2 = 12, A3 = 13,  // x10-x13
    A4 = 14, A5 = 15, A6 = 16, A7 = 17,  // x14-x17
    
    // 保存寄存器
    S2 = 18, S3 = 19, S4 = 20, S5 = 21,  // x18-x21
    S6 = 22, S7 = 23, S8 = 24, S9 = 25,  // x22-x25
    S10 = 26, S11 = 27,                   // x26-x27
    
    // 更多临时寄存器
    T3 = 28, T4 = 29, T5 = 30, T6 = 31,  // x28-x31
    
    // 浮点寄存器
    FT0 = 32, FT1 = 33, FT2 = 34, FT3 = 35,     // f0-f3
    FT4 = 36, FT5 = 37, FT6 = 38, FT7 = 39,     // f4-f7
    FS0 = 40, FS1 = 41,                          // f8-f9
    FA0 = 42, FA1 = 43, FA2 = 44, FA3 = 45,     // f10-f13
    FA4 = 46, FA5 = 47, FA6 = 48, FA7 = 49,     // f14-f17
    FS2 = 50, FS3 = 51, FS4 = 52, FS5 = 53,     // f18-f21
    FS6 = 54, FS7 = 55, FS8 = 56, FS9 = 57,     // f22-f25
    FS10 = 58, FS11 = 59,                        // f26-f27
    FT8 = 60, FT9 = 61, FT10 = 62, FT11 = 63,   // f28-f31
};

// 指令类型
enum class InstrType {
    // 算术指令
    ADD, ADDI, SUB,
    MUL, DIV, REM,
    
    // 逻辑指令
    AND, ANDI, OR, ORI, XOR, XORI,
    SLL, SLLI, SRL, SRLI, SRA, SRAI,
    
    // 比较指令
    SLT, SLTI, SLTU, SLTIU,
    
    // 分支指令
    BEQ, BNE, BLT, BGE, BLTU, BGEU,
    
    // 跳转指令
    JAL, JALR,
    
    // 加载存储指令
    LB, LH, LW, LBU, LHU,
    SB, SH, SW,
    
    // 浮点算术指令
    FADD_S, FSUB_S, FMUL_S, FDIV_S,
    
    // 浮点比较指令
    FEQ_S, FLT_S, FLE_S,
    
    // 浮点加载存储
    FLW, FSW,
    
    // 浮点转换指令
    FCVT_W_S, FCVT_S_W,
    
    // 伪指令
    LI, LA, MV, J, RET, CALL,
    
    // 其他
    NOP, LABEL
};

// 操作数基类
class Operand {
public:
    virtual ~Operand() = default;
    virtual std::string toString() const = 0;
    virtual bool isRegister() const { return false; }
    virtual bool isImmediate() const { return false; }
    virtual bool isLabel() const { return false; }
    virtual bool isMemory() const { return false; }
};

// 寄存器操作数
class RegOperand : public Operand {
private:
    Register reg;
    
public:
    explicit RegOperand(Register r) : reg(r) {}
    
    Register getReg() const { return reg; }
    std::string toString() const override;
    bool isRegister() const override { return true; }
};

// 立即数操作数
class ImmOperand : public Operand {
private:
    int value;
    
public:
    explicit ImmOperand(int v) : value(v) {}
    
    int getValue() const { return value; }
    std::string toString() const override;
    bool isImmediate() const override { return true; }
};

// 标签操作数
class LabelOperand : public Operand {
private:
    std::string label;
    
public:
    explicit LabelOperand(const std::string& l) : label(l) {}
    
    const std::string& getLabel() const { return label; }
    std::string toString() const override;
    bool isLabel() const override { return true; }
};

// 内存操作数 offset(base)
class MemOperand : public Operand {
private:
    int offset;
    Register base;
    
public:
    MemOperand(int off, Register b) : offset(off), base(b) {}
    
    int getOffset() const { return offset; }
    Register getBase() const { return base; }
    std::string toString() const override;
    bool isMemory() const override { return true; }
};

// RISC-V指令基类
class RiscVInstr {
protected:
    InstrType type;
    std::vector<std::shared_ptr<Operand>> operands;
    std::string comment;
    
public:
    RiscVInstr(InstrType t) : type(t) {}
    virtual ~RiscVInstr() = default;
    
    InstrType getType() const { return type; }
    void addOperand(std::shared_ptr<Operand> op) { operands.push_back(op); }
    void setComment(const std::string& c) { comment = c; }
    
    virtual std::string toString() const;
    virtual bool isLabel() const { return type == InstrType::LABEL; }
    virtual bool isBranch() const;
    virtual bool isJump() const;
    virtual bool isCall() const { return type == InstrType::CALL || type == InstrType::JAL; }
    virtual bool isReturn() const { return type == InstrType::RET; }
};

// 标签指令
class LabelInstr : public RiscVInstr {
private:
    std::string labelName;
    
public:
    explicit LabelInstr(const std::string& name) 
        : RiscVInstr(InstrType::LABEL), labelName(name) {}
    
    const std::string& getLabelName() const { return labelName; }
    std::string toString() const override;
};

// R型指令 (寄存器-寄存器)
class RTypeInstr : public RiscVInstr {
public:
    RTypeInstr(InstrType t, Register rd, Register rs1, Register rs2);
    RTypeInstr(InstrType t, Register rd, Register rs1); // 单操作数版本
};

// I型指令 (立即数)
class ITypeInstr : public RiscVInstr {
public:
    ITypeInstr(InstrType t, Register rd, Register rs1, int imm);
    ITypeInstr(InstrType t, Register rd, int imm); // li指令
};

// S型指令 (存储)
class STypeInstr : public RiscVInstr {
public:
    STypeInstr(InstrType t, Register rs2, int offset, Register rs1);
};

// B型指令 (分支)
class BTypeInstr : public RiscVInstr {
public:
    BTypeInstr(InstrType t, Register rs1, Register rs2, const std::string& label);
};

// U型指令 (上位立即数)
class UTypeInstr : public RiscVInstr {
public:
    UTypeInstr(InstrType t, Register rd, int imm);
};

// J型指令 (跳转)
class JTypeInstr : public RiscVInstr {
public:
    JTypeInstr(InstrType t, Register rd, const std::string& label);
    JTypeInstr(InstrType t, const std::string& label); // j指令
};

// 工具函数
std::string getRegisterName(Register reg);
std::string getInstrName(InstrType type);

} // namespace RiscV

#endif // CACT_COMPILER_RISCVINSTR_H 