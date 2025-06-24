#include "sim86.h"

#include "sim86_memory.h"
#include "sim86_text.h"
#include "sim86_decode.h"

#include "sim86_memory.cpp"
#include "sim86_text.cpp"
#include "sim86_decode.cpp"

static void DisAsm8086(memory *Memory, u32 DisAsmByteCount, segmented_access DisAsmStart) {
    segmented_access At = DisAsmStart;

    disasm_context Context = DefaultDisAsmContext();


}

int main(int ArgCount, char **Args) {
    memory *Memory = (memory *)malloc(sizeof(memory));

    if (ArgCount > 1) {
        for(int ArgIndex = 1; ArgIndex < ArgCount; ++ArgIndex) {
            char *FileName = Args[ArgIndex];
            u32 BytesRead = LoadMemoryFromFile(FileName, Memory, 0);

            printf("; %s disassembly:\n", FileName);
            printf("bits 16\n");
            DisAsm8086(Memory, BytesRead, {});
        }
    } else {
        fprintf(stderr, "Usage: %s [8086 machine code file] ...\n", Args[0]);
    }

    return 0;
}
