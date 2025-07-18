//
// RISC-V指令实现
// Created for CACT Compiler RISC-V Backend
//

#include "../../include/backend/RiscVInstr.h"
#include <sstream>

namespace RiscV {

// 寄存器名称映射
std::string getRegisterName(Register reg) {
    static const std::string regNames[] = {
        // 通用寄存器 x0-x31
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
        
        // 浮点寄存器 f0-f31
        "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
        "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
        "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
        "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"
    };
    
    int index = static_cast<int>(reg);
    if (index >= 0 && index < 64) {
        return regNames[index];
    }
    return "unknown";
}

// 指令名称映射
std::string getInstrName(InstrType type) {
    switch (type) {
        // 算术指令
        case InstrType::ADD: return "add";
        case InstrType::ADDI: return "addi";
        case InstrType::SUB: return "sub";
        case InstrType::MUL: return "mul";
        case InstrType::DIV: return "div";
        case InstrType::REM: return "rem";
        
        // 逻辑指令
        case InstrType::AND: return "and";
        case InstrType::ANDI: return "andi";
        case InstrType::OR: return "or";
        case InstrType::ORI: return "ori";
        case InstrType::XOR: return "xor";
        case InstrType::XORI: return "xori";
        case InstrType::SLL: return "sll";
        case InstrType::SLLI: return "slli";
        case InstrType::SRL: return "srl";
        case InstrType::SRLI: return "srli";
        case InstrType::SRA: return "sra";
        case InstrType::SRAI: return "srai";
        
        // 比较指令
        case InstrType::SLT: return "slt";
        case InstrType::SLTI: return "slti";
        case InstrType::SLTU: return "sltu";
        case InstrType::SLTIU: return "sltiu";
        
        // 分支指令
        case InstrType::BEQ: return "beq";
        case InstrType::BNE: return "bne";
        case InstrType::BLT: return "blt";
        case InstrType::BGE: return "bge";
        case InstrType::BLTU: return "bltu";
        case InstrType::BGEU: return "bgeu";
        
        // 跳转指令
        case InstrType::JAL: return "jal";
        case InstrType::JALR: return "jalr";
        
        // 加载存储指令
        case InstrType::LB: return "lb";
        case InstrType::LH: return "lh";
        case InstrType::LW: return "lw";
        case InstrType::LBU: return "lbu";
        case InstrType::LHU: return "lhu";
        case InstrType::SB: return "sb";
        case InstrType::SH: return "sh";
        case InstrType::SW: return "sw";
        
        // 浮点算术指令
        case InstrType::FADD_S: return "fadd.s";
        case InstrType::FSUB_S: return "fsub.s";
        case InstrType::FMUL_S: return "fmul.s";
        case InstrType::FDIV_S: return "fdiv.s";
        
        // 浮点比较指令
        case InstrType::FEQ_S: return "feq.s";
        case InstrType::FLT_S: return "flt.s";
        case InstrType::FLE_S: return "fle.s";
        
        // 浮点加载存储
        case InstrType::FLW: return "flw";
        case InstrType::FSW: return "fsw";
        
        // 浮点转换指令
        case InstrType::FCVT_W_S: return "fcvt.w.s";
        case InstrType::FCVT_S_W: return "fcvt.s.w";
        
        // 伪指令
        case InstrType::LI: return "li";
        case InstrType::LA: return "la";
        case InstrType::MV: return "mv";
        case InstrType::J: return "j";
        case InstrType::RET: return "ret";
        case InstrType::CALL: return "call";
        case InstrType::NOP: return "nop";
        
        default: return "unknown";
    }
}

// 操作数类实现
std::string RegOperand::toString() const {
    return getRegisterName(reg);
}

std::string ImmOperand::toString() const {
    return std::to_string(value);
}

std::string LabelOperand::toString() const {
    return label;
}

std::string MemOperand::toString() const {
    std::stringstream ss;
    ss << offset << "(" << getRegisterName(base) << ")";
    return ss.str();
}

// RISC-V指令基类实现
std::string RiscVInstr::toString() const {
    std::stringstream ss;
    
    if (type == InstrType::LABEL) {
        return "";  // 标签单独处理
    }
    
    ss << "\t" << getInstrName(type);
    
    // 添加操作数
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i == 0) ss << " ";
        else ss << ", ";
        ss << operands[i]->toString();
    }
    
    // 添加注释
    if (!comment.empty()) {
        ss << " # " << comment;
    }
    
    return ss.str();
}

bool RiscVInstr::isBranch() const {
    return type == InstrType::BEQ || type == InstrType::BNE ||
           type == InstrType::BLT || type == InstrType::BGE ||
           type == InstrType::BLTU || type == InstrType::BGEU;
}

bool RiscVInstr::isJump() const {
    return type == InstrType::JAL || type == InstrType::JALR ||
           type == InstrType::J;
}

// 标签指令实现
std::string LabelInstr::toString() const {
    return labelName + ":";
}

// R型指令实现
RTypeInstr::RTypeInstr(InstrType t, Register rd, Register rs1, Register rs2) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rd));
    addOperand(std::make_shared<RegOperand>(rs1));
    addOperand(std::make_shared<RegOperand>(rs2));
}

RTypeInstr::RTypeInstr(InstrType t, Register rd, Register rs1) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rd));
    addOperand(std::make_shared<RegOperand>(rs1));
}

// I型指令实现
ITypeInstr::ITypeInstr(InstrType t, Register rd, Register rs1, int imm) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rd));
    addOperand(std::make_shared<RegOperand>(rs1));
    addOperand(std::make_shared<ImmOperand>(imm));
}

ITypeInstr::ITypeInstr(InstrType t, Register rd, int imm) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rd));
    addOperand(std::make_shared<ImmOperand>(imm));
}

// S型指令实现
STypeInstr::STypeInstr(InstrType t, Register rs2, int offset, Register rs1) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rs2));
    addOperand(std::make_shared<MemOperand>(offset, rs1));
}

// B型指令实现
BTypeInstr::BTypeInstr(InstrType t, Register rs1, Register rs2, const std::string& label) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rs1));
    addOperand(std::make_shared<RegOperand>(rs2));
    addOperand(std::make_shared<LabelOperand>(label));
}

// U型指令实现
UTypeInstr::UTypeInstr(InstrType t, Register rd, int imm) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rd));
    addOperand(std::make_shared<ImmOperand>(imm));
}

// J型指令实现
JTypeInstr::JTypeInstr(InstrType t, Register rd, const std::string& label) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<RegOperand>(rd));
    addOperand(std::make_shared<LabelOperand>(label));
}

JTypeInstr::JTypeInstr(InstrType t, const std::string& label) 
    : RiscVInstr(t) {
    addOperand(std::make_shared<LabelOperand>(label));
}

} // namespace RiscV 