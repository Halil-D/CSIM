// - Includes
#include "csim_core.h"
#include "csim_parser.h"

// CPU and the ISA to use
#include "archs/csim_riscv_isa.h"
#include "cpus/csim_test_cpu.h"

#include <stdio.h>

// - Definitions
#define NORMAL_MODE 0
#define DEBUG_MODE 1
#define ONLY_SHOW_IR 2

// - Function Declerations
void csim_print_ir(csim_program_t *program , csim_isa_t *isa);

// - Main Function
int main(int argc, char *argv[]) {
    // Command line arguments
    char *program_name = NULL;
    int mode = 0; // 0 = normal, 1 = debug, 2 = only show IR

    // Exit if command line arguments are invalid
    if (argc < 2) {
        printf("Usage: %s <program_name> [mode]\n", argv[0]);
        printf("Modes:\n");
        printf("0 - Normal Mode: Assemble and execute the program (default)\n");
        printf("1 - Debug Mode: Assemble and execute the program with debug info\n");
        printf("2 - Only Show IR: Only assemble the program and show the intermediate representation\n");
        return 1;
    }

    // Read command line arguments
    program_name = argv[1];
    if (argc >= 3) {
        mode = atoi(argv[2]);
        if (mode < 0 || mode > 2) {
            printf("Invalid mode: %i\n", mode);
            return 1;
        }
    }

    // Assemble the program
    csim_program_t program = {0};
    csim_assemble_program(&riscv_isa, program_name, &program);

    // Initialize the CPU
    csim_cpu_t cpu = test_cpu;
    csim_init_cpu(&cpu);

    // Run the program 
    if (mode != ONLY_SHOW_IR) {
        // Execution Cycle
        int result = 0;
        while (result == 0) {
            result = csim_execute_cycle(&cpu, &program, &riscv_isa, mode);
        }
    }

    // Show the assembled IR at the end
    if (mode != NORMAL_MODE) {
        csim_print_ir(&program, &riscv_isa);
    }
    
    // Cleanup
    free(program.instructions);
    csim_free_cpu(&cpu);
}

void csim_print_ir(csim_program_t *program , csim_isa_t *isa) {
    printf("\nIntermediate Representation:\n");
    for (int i = 0; i < program->instruction_count; i++) {
        // Local variables
        csim_ir_t *ir = &(program->instructions[i]);
        csim_instruction_t *instruction = &isa->instructions[ir->instruction_id];
        csim_operands_t *operands = &program->instructions[i].operands;

        // Printing the IR 
        printf("Instruction %i: %i(%s) - ", i,
            ir->instruction_id, isa->instructions[program->instructions[i].instruction_id].opcode);
            // Printing operands
            for (int j = 0; j < ir->operands.count; j++) {
                int operand_value = program->instructions[i].operands.operands[j].value;
                switch (program->instructions[i].operands.operands[j].type) {
                    case CSIM_OP_REG:
                        printf("R%i ", operand_value);
                        break;
                    case CSIM_OP_IMM:
                        printf("#%i ", operand_value);
                        break;
                    default:
                        printf("?%i ", operand_value);
                        break;
                }
            }
        printf("\n");
    }
}
