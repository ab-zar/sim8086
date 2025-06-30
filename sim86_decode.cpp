instruction_format instructionFormats[] = 
{
#include "sim86_instruction_table.inl"
};

static disasm_context DefaultDisAsmContext(void) {
    disasm_context Result = {};

    Result.DefaultSegment = Register_ds;
    return Result;
}

static instruction_operand GetRegOperand(u32 IntelRegIndex, b32 Wide)
{
    register_access RegTable[][2] = 
    {
        {{Register_a, 0, 1}, {Register_a, 0, 2}},
        {{Register_c, 0, 1}, {Register_c, 0, 2}},
        {{Register_d, 0, 1}, {Register_d, 0, 2}},
        {{Register_b, 0, 1}, {Register_b, 0, 2}},
        {{Register_a, 1, 1}, {Register_sp, 0, 2}},
        {{Register_c, 1, 1}, {Register_bp, 0, 2}},
        {{Register_d, 1, 1}, {Register_si, 0, 2}},
        {{Register_b, 1, 1}, {Register_di, 0, 2}},
    };

    instruction_operand Result = {};
    Result.Type = Operand_Register;
    Result.Register = RegTable[IntelRegIndex & 0x7][(Wide != 0)];

    return Result;
}

static u32 ParseDataValue(memory *Memory, segmented_access *Access, b32 Exists, b32 Wide, b32 SignExtended)
{
    u32 Result = {};

    if(Exists)
    {
        if(Wide)
        {
            u8 D0 = ReadMemory(Memory, GetAbsoluteAddressOf(*Access, 0));
            u8 D1 = ReadMemory(Memory, GetAbsoluteAddressOf(*Access, 1));
            Result = (D1 << 8) | D0;
            Access->Offset += 2;
        }
        else
        {
            Result = ReadMemory(Memory, GetAbsoluteAddressOf(*Access));
            if(SignExtended)
            {
                Result = (s32)*(s8 *)&Result;
            }
            Access->Offset += 1;
        }
    }
    return Result;
}