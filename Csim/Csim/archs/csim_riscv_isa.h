#ifndef CSIM_RISCV_ISA_H
#define CSIM_RISCV_ISA_H

#include "../csim_core.h"
#include <stdio.h>

// ============ ARITHMETIC INSTRUCTIONS ============

int ADD(csim_cpu_t *cpu, csim_operands_t *ops) {
    // ADD rd, rs1, rs2
    uint64_t result = cpu->registers[ops->operands[1].value] + cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int ADDI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // ADDI rd, rs1, imm
    uint64_t result = cpu->registers[ops->operands[1].value] + ops->operands[2].value;
    return csim_write_register(cpu, ops->operands[0].value, result);
}

int SUB(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SUB rd, rs1, rs2
    uint64_t result = cpu->registers[ops->operands[1].value] - cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int MUL(csim_cpu_t *cpu, csim_operands_t *ops) {
    // MUL rd, rs1, rs2
    uint64_t result = cpu->registers[ops->operands[1].value] * cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int DIV(csim_cpu_t *cpu, csim_operands_t *ops) {
    // DIV rd, rs1, rs2
    if (cpu->registers[ops->operands[2].value] == 0) {
        cpu->error_code = CSIM_ER_DIV_BY_ZERO;
        cpu->halted = 1;
        return CSIM_ER_DIV_BY_ZERO;
    }
    uint64_t result = cpu->registers[ops->operands[1].value] / cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

// ============ MEMORY INSTRUCTIONS ============

int LW(csim_cpu_t *cpu, csim_operands_t *ops) {
    // LW rd, offset(rs1)
    // ops[0] = rd, ops[1] = offset, ops[2] = rs1
    uint64_t addr = cpu->registers[ops->operands[2].value] + ops->operands[1].value;
    
    if (addr < 0 || addr + 4 > cpu->memory_size) {
        cpu->error_code = CSIM_ER_INV_MEM_ADR;
        cpu->halted = 1;
        return CSIM_ER_INV_MEM_ADR;
    }
    
    uint64_t value = 0;
    csim_read_memory(cpu, addr, 4, CSIM_L_ENDIAN, &value);
    csim_write_register(cpu, ops->operands[0].value, value);
    return 0;
}

int SW(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SW rs2, offset(rs1)
    // ops[0] = rs2, ops[1] = offset, ops[2] = rs1
    uint64_t addr = cpu->registers[ops->operands[2].value] + ops->operands[1].value;
    
    if (addr < 0 || addr + 4 > cpu->memory_size) {
        cpu->error_code = CSIM_ER_INV_MEM_ADR;
        cpu->halted = 1;
        return CSIM_ER_INV_MEM_ADR;
    }
    
    csim_write_memory(cpu, addr, 4, CSIM_L_ENDIAN, cpu->registers[ops->operands[0].value]);
    return 0;
}

int LB(csim_cpu_t *cpu, csim_operands_t *ops) {
    // LB rd, offset(rs1) - load byte
    // ops[0] = rd, ops[1] = offset, ops[2] = rs1
    uint64_t addr = cpu->registers[ops->operands[2].value] + ops->operands[1].value;
    
    if (addr < 0 || addr >= cpu->memory_size) {
        cpu->error_code = CSIM_ER_INV_MEM_ADR;
        cpu->halted = 1;
        return CSIM_ER_INV_MEM_ADR;
    }
    
    uint64_t value = 0;
    csim_read_memory(cpu, addr, 1, CSIM_L_ENDIAN, &value);
    csim_write_register(cpu, ops->operands[0].value, value);
    return 0;
}

int SB(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SB rs2, offset(rs1) - store byte
    // ops[0] = rs2, ops[1] = offset, ops[2] = rs1
    uint64_t addr = cpu->registers[ops->operands[2].value] + ops->operands[1].value;
    
    if (addr < 0 || addr >= cpu->memory_size) {
        cpu->error_code = CSIM_ER_INV_MEM_ADR;
        cpu->halted = 1;
        return CSIM_ER_INV_MEM_ADR;
    }
    
    csim_write_memory(cpu, addr, 1, CSIM_L_ENDIAN, cpu->registers[ops->operands[0].value]);
    return 0;
}

// ============ BRANCHING INSTRUCTIONS ============

int BEQ(csim_cpu_t *cpu, csim_operands_t *ops) {
    // BEQ rs1, rs2, label
    // ops[0] = rs1, ops[1] = rs2, ops[2] = label_address
    if (cpu->registers[ops->operands[0].value] == cpu->registers[ops->operands[1].value]) {
        if (ops->operands[2].value >= 0 && ops->operands[2].value < INT_MAX) {
            cpu->pc = ops->operands[2].value - 1;  // -1 because core loop increments
        }
    }
    return 0;
}

int BNE(csim_cpu_t *cpu, csim_operands_t *ops) {
    // BNE rs1, rs2, label
    // ops[0] = rs1, ops[1] = rs2, ops[2] = label_address
    if (cpu->registers[ops->operands[0].value] != cpu->registers[ops->operands[1].value]) {
        if (ops->operands[2].value >= 0 && ops->operands[2].value < INT_MAX) {
            cpu->pc = ops->operands[2].value - 1;
        }
    }
    return 0;
}

int BLT(csim_cpu_t *cpu, csim_operands_t *ops) {
    // BLT rs1, rs2, label (branch if less than)
    // ops[0] = rs1, ops[1] = rs2, ops[2] = label_address
    if ((int64_t)cpu->registers[ops->operands[0].value] < (int64_t)cpu->registers[ops->operands[1].value]) {
        if (ops->operands[2].value >= 0 && ops->operands[2].value < INT_MAX) {
            cpu->pc = ops->operands[2].value - 1;
        }
    }
    return 0;
}

int BGE(csim_cpu_t *cpu, csim_operands_t *ops) {
    // BGE rs1, rs2, label (branch if greater or equal)
    // ops[0] = rs1, ops[1] = rs2, ops[2] = label_address
    if ((int64_t)cpu->registers[ops->operands[0].value] >= (int64_t)cpu->registers[ops->operands[1].value]) {
        if (ops->operands[2].value >= 0 && ops->operands[2].value < INT_MAX) {
            cpu->pc = ops->operands[2].value - 1;
        }
    }
    return 0;
}

int JAL(csim_cpu_t *cpu, csim_operands_t *ops) {
    // JAL rd, label (jump and link)
    // ops[0] = rd (return address register), ops[1] = label_address
    csim_write_register(cpu, ops->operands[0].value, cpu->pc);  // save return address
    if (ops->operands[1].value >= 0 && ops->operands[1].value < INT_MAX) {
        cpu->pc = ops->operands[1].value - 1;
    }
    return 0;
}

int JALR(csim_cpu_t *cpu, csim_operands_t *ops) {
    // JALR rd, rs1, offset (jump and link register)
    // ops[0] = rd, ops[1] = rs1, ops[2] = offset
    csim_write_register(cpu, ops->operands[0].value, cpu->pc);  // save return address
    uint64_t target = cpu->registers[ops->operands[1].value] + ops->operands[2].value;
    if (target >= 0 && target < INT_MAX) {
        cpu->pc = target - 1;
    }
    return 0;
}

// ============ BITWISE INSTRUCTIONS ============

int AND(csim_cpu_t *cpu, csim_operands_t *ops) {
    // AND rd, rs1, rs2
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2
    uint64_t result = cpu->registers[ops->operands[1].value] & cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int ANDI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // ANDI rd, rs1, imm
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate
    uint64_t result = cpu->registers[ops->operands[1].value] & ops->operands[2].value;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int OR(csim_cpu_t *cpu, csim_operands_t *ops) {
    // OR rd, rs1, rs2
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2
    uint64_t result = cpu->registers[ops->operands[1].value] | cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int ORI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // ORI rd, rs1, imm
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate
    uint64_t result = cpu->registers[ops->operands[1].value] | ops->operands[2].value;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int XOR(csim_cpu_t *cpu, csim_operands_t *ops) {
    // XOR rd, rs1, rs2
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2
    uint64_t result = cpu->registers[ops->operands[1].value] ^ cpu->registers[ops->operands[2].value];
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int XORI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // XORI rd, rs1, imm
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate
    uint64_t result = cpu->registers[ops->operands[1].value] ^ ops->operands[2].value;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SLL(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SLL rd, rs1, rs2 (shift left logical)
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2 (shift amount)
    uint64_t shift_amount = cpu->registers[ops->operands[2].value] & 0x3F;  // 6-bit shift
    uint64_t result = cpu->registers[ops->operands[1].value] << shift_amount;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SLLI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SLLI rd, rs1, imm (shift left logical immediate)
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate shift amount
    uint64_t shift_amount = ops->operands[2].value & 0x3F;
    uint64_t result = cpu->registers[ops->operands[1].value] << shift_amount;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SRL(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SRL rd, rs1, rs2 (shift right logical)
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2 (shift amount)
    uint64_t shift_amount = cpu->registers[ops->operands[2].value] & 0x3F;
    uint64_t result = cpu->registers[ops->operands[1].value] >> shift_amount;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SRLI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SRLI rd, rs1, imm (shift right logical immediate)
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate shift amount
    uint64_t shift_amount = ops->operands[2].value & 0x3F;
    uint64_t result = cpu->registers[ops->operands[1].value] >> shift_amount;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SRA(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SRA rd, rs1, rs2 (shift right arithmetic)
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2 (shift amount)
    uint64_t shift_amount = cpu->registers[ops->operands[2].value] & 0x3F;
    int64_t signed_val = (int64_t)cpu->registers[ops->operands[1].value];
    uint64_t result = (uint64_t)(signed_val >> shift_amount);
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SRAI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SRAI rd, rs1, imm (shift right arithmetic immediate)
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate shift amount
    uint64_t shift_amount = ops->operands[2].value & 0x3F;
    int64_t signed_val = (int64_t)cpu->registers[ops->operands[1].value];
    uint64_t result = (uint64_t)(signed_val >> shift_amount);
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

// ============ COMPARISON INSTRUCTIONS ============

int SLT(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SLT rd, rs1, rs2 (set less than)
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2
    int64_t val1 = (int64_t)cpu->registers[ops->operands[1].value];
    int64_t val2 = (int64_t)cpu->registers[ops->operands[2].value];
    uint64_t result = (val1 < val2) ? 1 : 0;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SLTI(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SLTI rd, rs1, imm (set less than immediate)
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate
    int64_t val1 = (int64_t)cpu->registers[ops->operands[1].value];
    int64_t val2 = (int64_t)ops->operands[2].value;
    uint64_t result = (val1 < val2) ? 1 : 0;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SLTU(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SLTU rd, rs1, rs2 (set less than unsigned)
    // ops[0] = rd, ops[1] = rs1, ops[2] = rs2
    uint64_t val1 = cpu->registers[ops->operands[1].value];
    uint64_t val2 = cpu->registers[ops->operands[2].value];
    uint64_t result = (val1 < val2) ? 1 : 0;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

int SLTIU(csim_cpu_t *cpu, csim_operands_t *ops) {
    // SLTIU rd, rs1, imm (set less than immediate unsigned)
    // ops[0] = rd, ops[1] = rs1, ops[2] = immediate
    uint64_t val1 = cpu->registers[ops->operands[1].value];
    uint64_t val2 = ops->operands[2].value;
    uint64_t result = (val1 < val2) ? 1 : 0;
    csim_write_register(cpu, ops->operands[0].value, result);
    return 0;
}

// ============ CONTROL INSTRUCTIONS ============

int NOP(csim_cpu_t *cpu, csim_operands_t *ops) {
    // NOP (no operation)
    // ops are unused
    return 0;
}

int ECALL(csim_cpu_t *cpu, csim_operands_t *ops) {
    // ECALL - environment call (system call)
    // a7 (x17) = syscall number
    // a0 (x10) = first argument
    
    uint64_t syscall_num = cpu->registers[17];  // a7
    
    if (syscall_num == 1) {
        // Real syscall 1: write(fd, buffer, count)
        uint64_t fd = cpu->registers[10];      // a0
        uint64_t buffer = cpu->registers[11];  // a1
        uint64_t count = cpu->registers[12];   // a2
        
        for (int i = 0; i < count; i++) {
            printf("%c", cpu->memory[buffer + i]);
        }
    } 
    else if (syscall_num == 11) {
        // Syscall 11: print character
        printf("%c", (char)(cpu->registers[10] & 0xFF));  // a0
    }
    else if (syscall_num == 10) {
        // Syscall 10: exit
        cpu->halted = 1;
    }
    else {
        // Unknown syscall
        cpu->error_code = CSIM_ER_INV_PARAM;
        cpu->halted = 1;
        return CSIM_ER_INV_PARAM;
    }
    
    return 0;
}

int HALT(csim_cpu_t *cpu, csim_operands_t *ops) {
    // HALT (stop execution)
    // ops are unused
    cpu->halted = 1;
    cpu->error_code = CSIM_CPU_EXIT;
    return 0;
}

// ============ ISA INSTRUCTION TABLE ============

csim_instruction_t riscv_instructions[] = {
    // Arithmetic
    {.opcode = "ADD", .min_operands = 3, .max_operands = 3, .execute = ADD},
    {.opcode = "ADDI", .min_operands = 3, .max_operands = 3, .execute = ADDI},
    {.opcode = "SUB", .min_operands = 3, .max_operands = 3, .execute = SUB},
    {.opcode = "MUL", .min_operands = 3, .max_operands = 3, .execute = MUL},
    {.opcode = "DIV", .min_operands = 3, .max_operands = 3, .execute = DIV},
    
    // Memory
    {.opcode = "LW", .min_operands = 3, .max_operands = 3, .execute = LW},
    {.opcode = "SW", .min_operands = 3, .max_operands = 3, .execute = SW},
    {.opcode = "LB", .min_operands = 3, .max_operands = 3, .execute = LB},
    {.opcode = "SB", .min_operands = 3, .max_operands = 3, .execute = SB},
    
    // Branching
    {.opcode = "BEQ", .min_operands = 3, .max_operands = 3, .execute = BEQ},
    {.opcode = "BNE", .min_operands = 3, .max_operands = 3, .execute = BNE},
    {.opcode = "BLT", .min_operands = 3, .max_operands = 3, .execute = BLT},
    {.opcode = "BGE", .min_operands = 3, .max_operands = 3, .execute = BGE},
    {.opcode = "JAL", .min_operands = 2, .max_operands = 2, .execute = JAL},
    {.opcode = "JALR", .min_operands = 3, .max_operands = 3, .execute = JALR},
    
    // Bitwise
    {.opcode = "AND", .min_operands = 3, .max_operands = 3, .execute = AND},
    {.opcode = "ANDI", .min_operands = 3, .max_operands = 3, .execute = ANDI},
    {.opcode = "OR", .min_operands = 3, .max_operands = 3, .execute = OR},
    {.opcode = "ORI", .min_operands = 3, .max_operands = 3, .execute = ORI},
    {.opcode = "XOR", .min_operands = 3, .max_operands = 3, .execute = XOR},
    {.opcode = "XORI", .min_operands = 3, .max_operands = 3, .execute = XORI},
    {.opcode = "SLL", .min_operands = 3, .max_operands = 3, .execute = SLL},
    {.opcode = "SLLI", .min_operands = 3, .max_operands = 3, .execute = SLLI},
    {.opcode = "SRL", .min_operands = 3, .max_operands = 3, .execute = SRL},
    {.opcode = "SRLI", .min_operands = 3, .max_operands = 3, .execute = SRLI},
    {.opcode = "SRA", .min_operands = 3, .max_operands = 3, .execute = SRA},
    {.opcode = "SRAI", .min_operands = 3, .max_operands = 3, .execute = SRAI},
    
    // Comparison
    {.opcode = "SLT", .min_operands = 3, .max_operands = 3, .execute = SLT},
    {.opcode = "SLTI", .min_operands = 3, .max_operands = 3, .execute = SLTI},
    {.opcode = "SLTU", .min_operands = 3, .max_operands = 3, .execute = SLTU},
    {.opcode = "SLTIU", .min_operands = 3, .max_operands = 3, .execute = SLTIU},
    
    // Control
    {.opcode = "NOP", .min_operands = 0, .max_operands = 0, .execute = NOP},
    {.opcode = "ECALL", .min_operands = 0, .max_operands = 0, .execute = ECALL},
    {.opcode = "HALT", .min_operands = 0, .max_operands = 0, .execute = HALT},
};

// ============ REGISTER NAMES ============
// RISC-V: x0-x31 with ABI names

char *riscv_reg_names[32] = {
    "zero",  // x0  - hardwired zero
    "ra",    // x1  - return address
    "sp",    // x2  - stack pointer
    "gp",    // x3  - global pointer
    "tp",    // x4  - thread pointer
    "t0",    // x5  - temporary
    "t1",    // x6  - temporary
    "t2",    // x7  - temporary
    "s0",    // x8  - saved register / frame pointer
    "s1",    // x9  - saved register
    "a0",    // x10 - argument / return value
    "a1",    // x11 - argument / return value
    "a2",    // x12 - argument
    "a3",    // x13 - argument
    "a4",    // x14 - argument
    "a5",    // x15 - argument
    "a6",    // x16 - argument
    "a7",    // x17 - argument
    "s2",    // x18 - saved register
    "s3",    // x19 - saved register
    "s4",    // x20 - saved register
    "s5",    // x21 - saved register
    "s6",    // x22 - saved register
    "s7",    // x23 - saved register
    "s8",    // x24 - saved register
    "s9",    // x25 - saved register
    "s10",   // x26 - saved register
    "s11",   // x27 - saved register
    "t3",    // x28 - temporary
    "t4",    // x29 - temporary
    "t5",    // x30 - temporary
    "t6",    // x31 - temporary
};

// ============ ISA STRUCTURE ============

csim_isa_t riscv_isa = {
    .instructions = riscv_instructions,
    .instruction_count = CSIM_ARRAY_COUNT(riscv_instructions),
    .reg_names = riscv_reg_names,
    .register_count = 32,
    .reg_prefix = 'x',
    .encode = NULL
};

#endif // CSIM_RISCV_ISA_H
