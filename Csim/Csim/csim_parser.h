#ifndef CSIM_PARSER_H
#define CSIM_PARSER_H

// - Includes
#include "csim_core.h"

#include <stdio.h>

// - Definitions
#define CSIM_MAX_TOKEN_COUNT 5

// - Enums
// Error Codes
enum csim_parser_error_codes {
    CSIM_PARSER_ER_INV_OPCODE,
    CSIM_PARSER_ER_INV_OP_COUNT,
};

// - Function Declerations

// - Function Definitions
csim_token_t csim_extract_line(char **input) {
    // Storing line start
    char *line_start = *input;

    // Finding end of the line
    while (**input != '\n' && **input != '\0' && **input != '\r') {
        (*input)++;
    }

    // Build the token for the line
    csim_token_t line = {
        .start = line_start,
        .size  = *input - line_start
    };

    // Skip new line and carriage return characters
    while (**input == '\n' || **input == '\r') {
        (*input)++;
    }

    return line;
}

int csim_tokenize_line(csim_token_t line, csim_token_t *operands) {
    // Local variables
    char *current = line.start;
    char *end = line.start + line.size;

    // Operand count acts as the index and the return value
    int operand_count = 0;

    // Loop through the line 
    while (current < end) {
        // Tokenize the operand
        csim_token_t token = {0};
        token.start = current;
        while (current < end && (*current != ' ' && *current != ',' && *current != '\t' && *current != '(' && *current != ')')) {
            current++;
        }
        token.size = current - token.start;

        // Add the token to operands array
        operands[operand_count++] = token;

        // Skip delimiters
        while (current < end && (*current == ' ' || *current == ',' || *current == '\t' || *current == '(' || *current == ')')) {
            current ++;
        }
    }

    return operand_count;
}

int csim_is_register(csim_isa_t *isa, csim_token_t token, uint64_t *out_value) {
    // Check for register prefix
    if (token.start[0] == isa->reg_prefix) {
        // Extract numeric value
        uint64_t reg_index = 0;
        for (int i = 1; i < token.size; i++) {
            if (token.start[i] >= '0' && token.start[i] <= '9') {
                reg_index = reg_index * 10 + (token.start[i] - '0');
            } else {
                return 0; // Not a valid digit, can't be a register
            }
        }

        // Validate register index
        if (reg_index >= isa->register_count) {
            return 0; // Invalid register index
        }

        // Return register index via OUT_VALUE
        *out_value = reg_index;
        return 1; // Is a register
    }

    // Return if ISA doesnt have custom register names
    if (isa->reg_names == NULL) {
        return 0;
    }

    // Try to match the token with register names if prefix is not found
    for (int i = 0; i < isa->register_count; i++) {
        // Local char*
        char *reg_name = isa->reg_names[i];

        // Compare the letters
        int is_match = 1;
        for (int j = 0; reg_name[j] != '\0'; j++) {
            if (token.start[j] != reg_name[j]) {
                is_match = 0;
                break;
            }
        }

        // Return the register index if a match is found via OUT_VALUE
        if (is_match) {
            *out_value = i;
            return 1; // Is a register
        }
    }

    return 0; // Not a register
}

int csim_is_immediate(csim_token_t token, uint64_t *out_value) {
    // Counter
    int i = 0;

    // Is immediate positive or negative
    int is_negative = 0;
    if (token.start[0] == '-') {
        is_negative = 1;
        i++; // Skip the first char if its '-' during numeric value extraction
    }
    
    // Extracting numeric value
    int64_t value = 0;
    for (; i < token.size; i++) {
        if (token.start[i] >= '0' && token.start[i] <= '9') {
            value = (value * 10) + token.start[i] - '0';
        } else {
            return 0; // Token contains non-digit
        }
    }

    // Multiply with -1 to make the value negative
    if (is_negative) {
        value = value * -1;
    }

    // Return value as unsigned
    *out_value = (uint64_t)value;

    return 1;
}

int csim_is_operand(csim_isa_t *isa, csim_token_t token, csim_operand_t *out_operand){
    // Check if the token is empty
    if (token.size <= 0) {
        return 1;
    }

    if (csim_is_register(isa, token, &out_operand->value)) {
        out_operand->type = CSIM_OP_REG;
        return 0; // Is a register
    }

    if (csim_is_immediate(token, &out_operand->value)) {
        out_operand->type = CSIM_OP_IMM;
        return 0; // Is an immediate
    }

    out_operand->value = 0; // Default to 0 for opcodes that don't have a value
    out_operand->type = CSIM_OP_ERR; // Default to immediate type
    return 1;
}

int csim_is_opcode(csim_isa_t *isa, csim_token_t token, uint64_t *out_value) {
    // Check if the token is empty
    if (token.size <= 0) {
        return 1;
    }

    // Compare the token with instruction keywords
    for (int i = 0; i < isa->instruction_count; i++) {
        int is_match = 1;

        // Compare the letters
        for (int j = 0; j < token.size; j++) {
            if (token.start[j] != isa->instructions[i].opcode[j]) {
                is_match = 0;
                break;
            }
        }

        // Return instruction index via OUT_VALUE if a match is found
        if (is_match) {
            if (isa->instructions[i].opcode[token.size] != '\0') {
                continue; // Ensure the token is not a prefix of a longer instruction
            }

            *out_value = i;
            return 0; // Is an opcode
        }
    }

    // No match is found
    return 1;
}

int csim_preprocess_program(char *program_name, char **program_buffer, int *out_line_count) {
    // Opening the program file
    FILE *program_file = fopen(program_name, "rb");
    if (program_file == NULL) {
        return 1;
    }

    // Calculate file size
    fseek(program_file, 0, SEEK_END);
    int program_size = ftell(program_file);
    fseek(program_file, 0, SEEK_SET);

    // Allocate the program buffer
    *program_buffer = (char *)malloc(program_size + 1); // +1 for null terminator
    if (*program_buffer == NULL) {
        fclose(program_file);
        return 1;
    }

    // Read the program into the buffer
    fread(*program_buffer, 1, program_size, program_file);
    (*program_buffer)[program_size] = '\0'; // Null terminate the string
    fclose(program_file);

    // Preprocess the program to count the number of lines and remove comments
    int line_count = 0;
    int line_length = 0;
    int program_index = 0;

    for (int i = 0; i < program_size; i++) {
        char current_char = (*program_buffer)[i];

        switch (current_char) {
            case '\n': // New line character, increment line count and reset line length
                line_count++;
                 (*program_buffer)[program_index++] = current_char; // Copy non-comment characters
                line_length = 0;
                break;
            case '#': // Comment character, skip until the end of the line
                while (i < program_size && (*program_buffer)[i] != '\n') {
                    i++;
                }
                if (line_length > 0) {
                    line_count++; // Increment line count if there was code before the comment
                    (*program_buffer)[program_index++] = '\n'; // Copy non-comment characters
                }
                break;

            case '\r': // Carriage return, ignore it
                break;
            default:
                (*program_buffer)[program_index++] = current_char; // Copy non-comment characters
                line_length++;
                break;
        }
    }

    // Return the line count via OUT_LINE_COUNT
    char last_char = (*program_buffer)[program_size - 1];
    if (last_char != '\n') {
        line_count++; // Account for the last line if it doesn't end with a newline
    }
    *out_line_count = line_count;

    // NULL terminate the processed program buffer
    (*program_buffer)[program_index] = '\0';

    // Reallocate the program buffer to fit the processed program
    char *processed_buffer = (char *)realloc(*program_buffer, program_index + 1);
    if (processed_buffer == NULL) {
        free(*program_buffer);
        return 1;
    }

    return 0;
}

int csim_assemble_ir(csim_isa_t *isa, char *program_buffer, int line_count, csim_program_t *out_program) {
    // Creating and allocating the IR buffer
    csim_ir_t *ir_buffer = (csim_ir_t *)malloc(sizeof(csim_ir_t) * line_count);
    if (ir_buffer == NULL) {
        return 1;
    }

    // Filling the IR buffer by iterating through the lines of the program
    int instruction_count = 0;
    for (int i = 0; i < line_count; i++) {
        // Extract line
        csim_token_t line = csim_extract_line(&program_buffer);
        if (line.start == NULL || line.size <= 0) {
           continue; // Skip empty lines
        }

        // Tokenize the line
        csim_token_t tokens[CSIM_MAX_TOKEN_COUNT] = {0};
        int token_count = csim_tokenize_line(line, tokens);
        if (token_count <= 0) {
            continue;
        }

        // Initialize the IR entry for the line
        ir_buffer[i].instruction_id = 0;
        ir_buffer[i].operands.count = 0;
        ir_buffer[i].operands.operands[0].type = 0;
        ir_buffer[i].operands.operands[0].value = 0;
        ir_buffer[i].operands.operands[1].type = 0;
        ir_buffer[i].operands.operands[1].value = 0;
        ir_buffer[i].operands.operands[2].type = 0;
        ir_buffer[i].operands.operands[2].value = 0;

        // Evaluate opcode
        if (csim_is_opcode(isa, tokens[0], &ir_buffer[i].instruction_id)) {
            printf("Error: Invalid opcode at line %i\n", i + 1);
            free(ir_buffer);
            return 1;
        }
        
        // Evaluate tokens
        for (int j = 1; j < token_count; j++) {
            csim_operand_t operand_value = {0};
            if (csim_is_operand(isa, tokens[j], &operand_value)) {
                printf("Error: Invalid operand at line %i\n", i + 1);
                free(ir_buffer);
                return 1;
            }

            ir_buffer[i].operands.operands[ir_buffer[i].operands.count++] = operand_value;
        }

        // Increment instruction count
        instruction_count++;
    }

    // Return csim_program_t struct via OUT_PROGRAM
    *out_program = (csim_program_t){
        .instructions = ir_buffer,
        .instruction_count = instruction_count
    };

    return 0;
}

int csim_assemble_program(csim_isa_t *isa, char *program_name, csim_program_t *out_program) {
    // Local variables
    char *program_buffer = NULL;
    int line_count = 0;

    // Preprocess the program to read it into a buffer and count the number of lines
    if (csim_preprocess_program(program_name, &program_buffer, &line_count)) {
        printf("Error: Failed to preprocess the program\n");
        return 1;
    }

    // Assemble the intermediate representation from the program buffer
    if (csim_assemble_ir(isa, program_buffer, line_count, out_program)) {
        printf("Error: Failed to assemble the program\n");
        free(program_buffer);
        return 1;
    }

    // Free the program buffer as it's no longer needed after assembling
    free(program_buffer);

    return 0;
}
#endif // CSIM_PARSER_H