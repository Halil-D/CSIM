#ifndef CSIM_CPU_H
#define CSIM_CPU_H

#include "../csim_core.h"
#include <conio.h>

// Sound play
#include <math.h>
#include <windows.h> 
#include <stdlib.h>
#include <string.h>
// Sound types for the wrapper function
typedef enum {
    TONE_BEEP,
    TONE_PIANO
} SoundType;
static void GenerateAndPlayAudio(int frequency, int durationMs, SoundType type);
void PlayTone(int frequency, int durationMs, SoundType type);

// MEMORY MAP
#define KEYBOARD_CONTROLLER_BASE 1000
#define EXIT_BUTTON_BASE 1000
#define PIANO_KEYBOARD_BASE 1000
#define BUZZER_BASE 1004

int EXIT_BUTTON(csim_cpu_t *cpu, csim_peripheral_t *peripheral) {
    uint64_t last_char;
    csim_read_memory(cpu, KEYBOARD_CONTROLLER_BASE, 4, CSIM_L_ENDIAN, &last_char);
    if (last_char == 'q') { // Assuming 'q' is the exit key
        cpu->halted = 1;
        cpu->error_code = CSIM_CPU_EXIT;              
    }
    
    return 0;
}

int PIANO_KEYBOARD(csim_cpu_t *cpu, csim_peripheral_t *peripheral) {
    uint64_t last_char;
    csim_read_memory(cpu, KEYBOARD_CONTROLLER_BASE, 4, CSIM_L_ENDIAN, &last_char);
    switch (last_char) {
        case '1': // C note
            printf("C note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 261); // Write note to memory
            break;
        case '2': // D note
            printf("D note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 293); // Write note to memory
            break;
        case '3': // E note
            printf("E note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 329); // Write note to memory
            break;
        case '4': // F note
            printf("F note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 349); // Write note to memory
            break;
        case '5': // G note
            printf("G note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 392); // Write note to memory
            break;
        case '6': // A note
            printf("A note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 440); // Write note to memory
            break;
        case '7': // B note
            printf("B note\n");
            csim_write_memory(cpu, BUZZER_BASE, 4, CSIM_L_ENDIAN, 493); // Write note to memory
            break;
        default: // No note
            break;
    }
}

int BUZZER(csim_cpu_t *cpu, csim_peripheral_t *peripheral) {
    uint64_t frequency, duration;
    csim_read_memory(cpu, 1004, 4,  CSIM_L_ENDIAN, &frequency); // Read the note frequency from memory
    csim_read_memory(cpu, 1008, 4, CSIM_L_ENDIAN, &duration); // Read the note duration from memory
    if (frequency > 0) {
        printf("BUZZER: ON, Frequency: %i, Duration: %i\n", frequency, duration);
        PlayTone((int)frequency, (int)duration, TONE_PIANO); // Play the sound tone using the custom function
    }
    return 0;
}

int KEYBOARD_CONTROLLER(csim_cpu_t *cpu, csim_peripheral_t *peripheral) {
    static int key_press = 0;
    if (_kbhit()) {
        char key = _getch();
        printf("Key Pressed: %c\n", key);
        csim_write_memory(cpu, peripheral->base_address, 4, CSIM_L_ENDIAN, (uint64_t)key); // Write key to memory
        key_press = 1;
    } else if (key_press) {
        csim_write_memory(cpu, peripheral->base_address, 4, CSIM_L_ENDIAN, 0);
        key_press = 0;
    }
    return 0;
}

// Define active and passive peripherals: 
// NOTE: Active peripherals are executed every cycle, while passive peripherals are executed only when 
// their base address is written to by the csim_write_memory function. 
// In this test CPU, the keyboard controller is active to continuously check for key presses, 
// while the piano keyboard and exit button are passive and only check for input when their base address is written to.
csim_peripheral_t active_peripherals[] = {
    {.name = "Keyboard Controller", .base_address = KEYBOARD_CONTROLLER_BASE, .mem_size = 4, .behaviour = KEYBOARD_CONTROLLER},
};

csim_peripheral_t passive_peripherals[] = {
    {.name = "Piano Keyboard", .base_address = PIANO_KEYBOARD_BASE, .mem_size = 4, .behaviour = PIANO_KEYBOARD},
    {.name = "Buzzer", .base_address = BUZZER_BASE, .mem_size = 4, .behaviour = BUZZER},
    {.name = "Exit Buton", .base_address = EXIT_BUTTON_BASE, .mem_size = 4, .behaviour = EXIT_BUTTON},
};

int handle_interupts(csim_cpu_t *cpu) {
    printf("Handling interupts...\n\a");
    switch (cpu->error_code) {
        case CSIM_CPU_EXIT: // EXIT
            printf("CPU Exiting.\n");
            return CSIM_CPU_EXIT    ;
            break;
        default:
            printf("Unknown error code: %i. Resetting CPU...\n", cpu->error_code);
            csim_init_cpu(cpu);
            break;
    }

    return CSIM_SUCCES;
}

csim_cpu_t test_cpu = {
    .pc = 0,
    .register_count = 32,
    .register_size = CSIM_REG_32,
    .register_size_mask = CSIM_REG_32_MASK,
    .registers = NULL, // Will be allocated in csim_init_cpu
    .memory_size = 1024, // 1 KB of memory
    .memory = NULL, // Will be allocated in csim_init_cpu
    .flags = 0,
    .halted = 0,
    .error_code = 0,
    .handle_interupts = handle_interupts, // No interupts for this test CPU
    .active_peripheral_count = CSIM_ARRAY_COUNT(active_peripherals),
    .passive_peripheral_count = CSIM_ARRAY_COUNT(passive_peripherals),
    .active_peripherals = active_peripherals,
    .passive_peripherals = passive_peripherals
    
};


// SOUND STUF
// WAV Structure template - FIXED array sizes to stop MinGW memcpy errors
typedef struct {
    char     riffType[4];     // Fixed: treats as array pointer
    uint32_t riffSize;
    char     waveMarker[4];   // Fixed: treats as array pointer
    char     fmtMarker[4];    // Fixed: treats as array pointer
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char     dataMarker[4];   // Fixed: treats as array pointer
    uint32_t dataSize;
} WAVHeader;

// INTERNAL CORE ENGINE: Handles shared WAV structure allocation and playback
static void GenerateAndPlayAudio(int frequency, int durationMs, SoundType type) {
    if (frequency <= 0 || durationMs <= 0) return;

    const uint32_t sampleRate = 44100;
    const double pi = 3.14159265358979323846;
    
    uint32_t totalSamples = (sampleRate * (uint32_t)durationMs) / 1000;
    uint32_t dataSizeBytes = totalSamples * sizeof(int16_t);
    uint32_t totalBufferSize = sizeof(WAVHeader) + dataSizeBytes;
    
    char* memoryBuffer = (char*)malloc(totalBufferSize);
    if (memoryBuffer == NULL) return; 

    // Assemble structural metadata safely
    WAVHeader header;
    memcpy(header.riffType, "RIFF", 4); 
    memcpy(header.waveMarker, "WAVE", 4); 
    memcpy(header.fmtMarker, "fmt ", 4);
    header.riffSize = sizeof(WAVHeader) - 8 + dataSizeBytes;
    header.fmtSize = 16; 
    header.audioFormat = 1; 
    header.channels = 1; 
    header.sampleRate = sampleRate;
    header.bitsPerSample = 16; 
    header.byteRate = sampleRate * sizeof(int16_t); 
    header.blockAlign = sizeof(int16_t);
    memcpy(header.dataMarker, "data", 4); 
    header.dataSize = dataSizeBytes;
    memcpy(memoryBuffer, &header, sizeof(WAVHeader));
    
    int16_t* rawData = (int16_t*)(memoryBuffer + sizeof(WAVHeader));
    const int16_t maxVolume = 16384; 
    
    uint32_t fadeSamples = (sampleRate * 5) / 1000; // 5ms de-clicking window
    uint32_t attackSamples = (sampleRate * 10) / 1000;  
    uint32_t releaseSamples = (sampleRate * 50) / 1000; 

    for (uint32_t i = 0; i < totalSamples; ++i) {
        double time = (double)i / sampleRate;
        double freqHz = (double)frequency;

        if (type == TONE_PIANO) {
            // 1. Piano algorithm: Multi-harmonic waves with exponential decay
            double toneWave = 0.50 * sin(2.0 * pi * freqHz * time)        
                            + 0.25 * sin(2.0 * pi * (freqHz * 2.0) * time) 
                            + 0.15 * sin(2.0 * pi * (freqHz * 3.0) * time) 
                            + 0.10 * sin(2.0 * pi * (freqHz * 4.0) * time); 

            double volumeScale = 1.0;
            if (i < attackSamples) {
                volumeScale = (double)i / attackSamples;
            } else if (i > totalSamples - releaseSamples) {
                uint32_t releaseIndex = i - (totalSamples - releaseSamples);
                volumeScale = 0.2 * (1.0 - ((double)releaseIndex / releaseSamples));
            } else {
                double lifeRatio = (double)(i - attackSamples) / totalSamples;
                volumeScale = 1.0 * exp(-3.5 * lifeRatio); 
                if (volumeScale < 0.2) volumeScale = 0.2; 
            }
            rawData[i] = (int16_t)(maxVolume * volumeScale * toneWave);
        } 
        else {
            // 2. Pure Beep algorithm: Clean single sine wave with edge fading
            double rawSine = sin(2.0 * pi * freqHz * time);
            double volumeScale = 1.0;
            if (i < fadeSamples) {
                volumeScale = (double)i / fadeSamples; 
            } else if (i > totalSamples - fadeSamples) {
                volumeScale = (double)(totalSamples - i) / fadeSamples; 
            }
            rawData[i] = (int16_t)(maxVolume * volumeScale * rawSine);
        }
    }

    // Play synchronously back-to-back
    PlaySound((LPCSTR)memoryBuffer, NULL, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
    free(memoryBuffer); 
}

// THE WRAPPER FUNCTION: Exposes a single clean point of execution
void PlayTone(int frequency, int durationMs, SoundType type) {
    GenerateAndPlayAudio(frequency, durationMs, type);
}
#endif // CSIM_CPU_H