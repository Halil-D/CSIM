#ifndef CSIM_CORE_H
#define CSIM_CORE_H

// - Includes
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// - Definitions
#define CSIM_MAX_OPERANDS 3
#define CSIM_ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

// - Enumarators
enum csim_register_sizes {
    CSIM_REG_8 = 8,
    CSIM_REG_16 = 16,
    CSIM_REG_32 = 32,
    CSIM_REG_64 = 64
};

enum csim_register_size_masks {
    CSIM_REG_8_MASK  = 0xFF,
    CSIM_REG_16_MASK = 0xFFFF,
    CSIM_REG_32_MASK = 0xFFFFFFFF,
    CSIM_REG_64_MASK = 0xFFFFFFFFFFFFFFFF
};

// Error Codes:
enum csim_error_codes {
    CSIM_SUCCES,
    CSIM_CPU_EXIT,
    CSIM_ER_INV_REG_IDX,
    CSIM_ER_INV_MEM_ADR,
    CSIM_ER_INV_PARAM,
    CSIM_ER_MEM_ALLOC,
    CSIM_ER_DIV_BY_ZERO
};

enum csim_operand_types {
    CSIM_OP_ERR,
    CSIM_OP_REG,
    CSIM_OP_IMM,
};

// Byte Orders:
enum csim_byte_orders {
    CSIM_B_ENDIAN,
    CSIM_L_ENDIAN
};

// - Structures
typedef struct csim_cpu_t csim_cpu_t;
typedef struct csim_peripheral_t csim_peripheral_t;
typedef struct csim_operands_t csim_operands_t;
typedef struct csim_ir_t csim_ir_t;

// Csim CPU structure definition
typedef struct csim_cpu_t {
    uint64_t pc;            // Program Counter

    // Register file
    int register_count;          // Number of registers
    int register_size;           // Number of bits in the registers
    uint64_t register_size_mask; // // Mask to apply to ensure 
    uint64_t *registers;         // Array of registers

    // Memory
    uint64_t memory_size;        // Total size of the memory in bytes
    uint8_t *memory;             // Pointer for the memory

    // Flags and state
    uint64_t flags;               // General purpose flags (can be used for condition codes, etc.)
    int halted;                   // Flag to indicate if the CPU is halted
    int error_code;               // Flag to indicate if an error occurred

    int(*handle_interupts)(struct csim_cpu_t *cpu);

    // Peripherals
    int active_peripheral_count;  // Index of the currently active peripheral
    int passive_peripheral_count; // Number of peripherals

    csim_peripheral_t *active_peripherals;
    csim_peripheral_t *passive_peripherals;
} csim_cpu_t;

typedef struct csim_operand_t {
    int type; // 0 = register, 1 = immediate, 2 = memory address
    uint64_t value; // The value of the operand (register index, immediate value, or memory address)
} csim_operand_t;

// Csim general operand structure definition
typedef struct csim_operands_t {
    int count;                            // Number of operands
    csim_operand_t operands[CSIM_MAX_OPERANDS]; // Array to hold operand values
} csim_operands_t;

// Csim instruction structure definition
typedef struct csim_instruction_t {
    char *opcode;
    int min_operands;
    int max_operands;
    int (*execute)(csim_cpu_t *cpu, csim_operands_t *operands);
} csim_instruction_t;

// Csim intermediate representation structure definition
typedef struct csim_ir_t {
    uint64_t instruction_id;                 // instruction_id of the instruction, stored as an index to the instruction in the ISA
    csim_operands_t operands;        // Operands for the instruction, stored in the general format
} csim_ir_t;

// Csim program structure definition: Holds the array of intermediate representation 
// instructions and the count of instructions
typedef struct csim_program_t {
    csim_ir_t *instructions; // Array of intermediate representation instructions
    int instruction_count;   // Number of instructions in the program
} csim_program_t;

typedef struct csim_token_t {
    char *start; // Start addres of the token in the input string
    int size;    // Size of the token in characters
} csim_token_t;

// Csim instruction set architecture structure definition: 
typedef struct csim_isa_t {
    csim_instruction_t *instructions;
    int instruction_count;
    char **reg_names;
    int register_count;
    char reg_prefix;
    int (*encode)(csim_ir_t *ir, uint8_t *out);
} csim_isa_t;

typedef struct csim_peripheral_t {
    char *name;
    uint64_t base_address;
    int mem_size;
    int (*behaviour)(csim_cpu_t *cpu, csim_peripheral_t *peripheral);
} csim_peripheral_t;

// - Core Helper Function Declerations
int csim_write_register(csim_cpu_t *cpu, int reg_index, uint64_t value);
int csim_read_register(csim_cpu_t *cpu, int reg_index, uint64_t *out_value);

int csim_write_memory(csim_cpu_t *cpu, int mem_addres, int data_size, int byte_order, uint64_t value);
int csim_read_memory(csim_cpu_t *cpu, int mem_addres, int data_size, int byte_order, uint64_t *out_value);

// - Core Helper Function Definitions
// Csim Write to Register: Writes the given VALUE in to register specified by the REG_INDEX
// then applys bit masking to ensure register size.
int csim_write_register(csim_cpu_t *cpu, int reg_index, uint64_t value) {
    if (reg_index < 0 || reg_index >= cpu->register_count) return CSIM_ER_INV_REG_IDX;
    cpu->registers[reg_index] = value & cpu->register_size_mask;
    return CSIM_SUCCES;
} 
// Csim Read From Register:
int csim_read_register(csim_cpu_t *cpu, int reg_index, uint64_t *out_value) {
    if (reg_index < 0 || reg_index >= cpu->register_count) return CSIM_ER_INV_REG_IDX;
    *out_value = cpu->registers[reg_index];
    return CSIM_SUCCES;
}

// Csim Write to Memory:
int csim_write_memory(csim_cpu_t *cpu, int mem_addres, int data_size, int byte_order, uint64_t value) {
    if (mem_addres < 0 || mem_addres >= cpu->memory_size)  return CSIM_ER_INV_MEM_ADR;

    if (byte_order == CSIM_B_ENDIAN) {
        int shift_amount = data_size - 1;
        while (shift_amount >= 0) {
            cpu->memory[mem_addres++] = (value >> (shift_amount * 8)) & 0xFF;  
            shift_amount--;  
        }
    } else if (byte_order == CSIM_L_ENDIAN) {
        int shift_amount = 0;
        while (shift_amount < data_size) { 
            cpu->memory[mem_addres++] = (value >> (shift_amount * 8)) & 0xFF;  
            shift_amount++;
        }
    } else {
        return CSIM_ER_INV_PARAM;
    }

    // Run passive peripherals
    if (cpu->passive_peripheral_count > 0) {
        for (int i = 0; i < cpu->passive_peripheral_count; i++) {
            if (mem_addres - data_size == cpu->passive_peripherals[i].base_address) {
                csim_peripheral_t *peripheral = &cpu->passive_peripherals[i];
                peripheral->behaviour(cpu, peripheral);
            }
        }
    }

    return CSIM_SUCCES;
}

// Csim Read From Memory:
int csim_read_memory(csim_cpu_t *cpu, int mem_addres, int data_size, int byte_order, uint64_t *out_value) {
    if (mem_addres < 0 || mem_addres >= cpu->memory_size)  return CSIM_ER_INV_MEM_ADR;

    uint64_t value = 0;
    if (byte_order == CSIM_B_ENDIAN) {
        int shift_amount = data_size - 1;
        while (shift_amount >= 0) {
            value |= (uint64_t)cpu->memory[mem_addres++] << (shift_amount * 8);
            shift_amount--;
        }
        *out_value = value;
    } else if (byte_order == CSIM_L_ENDIAN) {
        int shift_amount = 0;
        while (shift_amount < data_size) {
            value |= (uint64_t)cpu->memory[mem_addres++] << (shift_amount * 8);
            shift_amount++;
        }
        *out_value = value;
    } else {
        return CSIM_ER_INV_PARAM;
    }

    return CSIM_SUCCES;
}

int csim_init_cpu(csim_cpu_t *cpu) {
    cpu->pc = 0;
    cpu->register_size_mask = (1ULL << cpu->register_size) - 1; // Create a mask with the number of bits in the register
    cpu->registers = (uint64_t *)malloc(cpu->register_count * sizeof(uint64_t));
    if (cpu->registers == NULL) return CSIM_ER_MEM_ALLOC;
    for (int i = 0; i < cpu->register_count; i++) {
        cpu->registers[i] = 0;
    }

    cpu->memory_size = cpu->memory_size;
    cpu->memory = (uint8_t *)malloc(cpu->memory_size * sizeof(uint8_t));
    if (cpu->memory == NULL) return CSIM_ER_MEM_ALLOC;
    for (int i = 0; i < cpu->memory_size; i++) {
        cpu->memory[i] = 0;
    }

    cpu->flags = 0;
    cpu->halted = 0;
    cpu->error_code = 0;

    return CSIM_SUCCES;
}

int csim_free_cpu(csim_cpu_t *cpu) {
    free(cpu->registers);
    free(cpu->memory);

    return CSIM_SUCCES;
}

void csim_print_operands(csim_cpu_t *cpu, csim_operands_t *operands) {
    int hex_digits = cpu->register_size / 4;

    for (int i = 0; i < operands->count; i++) {
        int operand_value = operands->operands[i].value;

        switch (operands->operands[i].type) {
            case CSIM_OP_REG:
            int register_index = operands->operands[i].value;
            int register_value = cpu->registers[register_index];
                printf("R%i: 0x%0*x(%i), ", operand_value, hex_digits, register_value, register_value);
                break;
            case CSIM_OP_IMM:
                printf("#%i,  ", operand_value);
                break;
            default:
                printf("?%i,  ", operand_value);
                break;
        }
    }
}
void csim_print_cycle_info(csim_cpu_t *cpu, csim_program_t *program , csim_isa_t *isa, int pc) {
    static int cycle_count = 0;
    static int total_cycle_count = 0;
    printf("Cycle %i | PC: %i | ", total_cycle_count++, pc);
    printf("%s - ", isa->instructions[program->instructions[pc].instruction_id].opcode);
    csim_print_operands(cpu, &program->instructions[pc].operands);

    printf("\n");
}

int csim_execute_cycle(csim_cpu_t *cpu, csim_program_t *program, csim_isa_t *isa, int debug) {
    // Execute the instruction pointed by the PC of the cpu then increments it
    if (cpu->halted != 1 && cpu->error_code == 0) {
        // Local Variables
        csim_instruction_t *instruction = &isa->instructions[program->instructions[cpu->pc].instruction_id];
        csim_operands_t *operands = &program->instructions[cpu->pc].operands;

        int executed_pc = cpu->pc; // Saves the current PC to correctly show jumps and branches that modify the PC
        int result = instruction->execute(cpu, operands);

        if (result != CSIM_SUCCES) {
            return result; // Return if an error occurred during instruction execution
        }

        if (debug) csim_print_cycle_info(cpu, program, isa, executed_pc); // Print cycle info 
        cpu->pc++;
    }

    // Run active peripherals
    if (cpu->active_peripheral_count > 0) {
        if (debug) printf("Running Active Peripherals...\n");
        for (int i = 0; i < cpu->active_peripheral_count; i++) {
            csim_peripheral_t *peripheral = &cpu->active_peripherals[i];
            peripheral->behaviour(cpu, peripheral);
        }
    }

    // Handle interupts
    if (cpu->handle_interupts != NULL) {
        if (cpu->halted == 1) {
            return cpu->handle_interupts(cpu);
        }
    }

    return 0;
}    
#endif // CSIM_CORE_H
