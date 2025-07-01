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

// NOTE(abzar): ParseDataValue is not a real function, it's basically just a macro that is used in
// TryParse. It should never be called otherwise, but that is not something you can do in C++.
// In other languages it would be a "local function".
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

static instruction TryDecode(disasm_context *Context, instruction_format *InstF, memory *Memory, segmented_access At)
{
    instruction Dest = {};
    u32 HasBits = 0;
    u32 Bits[Bits_Count] = {};
    b32 Valid = true;

    u32 StartingAddress = GetAbsoluteAddressOf(At);

    u8 BitsPendingCount = 0;
    u8 BitsPending = 0;

    for(u32 BitsIndex = 0; Valid && (BitsIndex < ArrayCount(InstF->Bits)); ++BitsIndex)
    {
        instruction_bits TestBits = InstF->Bits[BitsIndex];
        if((TestBits.Usage == Bits_Literal) && (TestBits.BitCount == 0))
        {
            // this is the end
            break;
        }

        u32 ReadBits = TestBits.Value;
        if(TestBits.BitCount != 0)
        {
            if(BitsPendingCount == 0)
            {
                BitsPendingCount = 8;
                BitsPending = ReadMemory(Memory, GetAbsoluteAddressOf(At));
                ++At.Offset;
            }

            assert(TestBits.BitCount <= BitsPendingCount); // if this fires - error in the table

            BitsPendingCount -= TestBits.BitCount;
            ReadBits = BitsPending;
            ReadBits >>= BitsPendingCount;
            ReadBits &= ~(0xff << TestBits.BitCount);
        }

        if(TestBits.Usage == Bits_Literal)
        {
            Valid = Valid && (ReadBits == TestBits.Value);
        }
        else
        {
            Bits[TestBits.Usage] |= (ReadBits << TestBits.Shift);
            HasBits |= (1 << TestBits.Usage);
        }
    }

    if(Valid)
    {
        u32 Mod = Bits[Bits_MOD];
        u32 RM = Bits[Bits_RM];
        u32 W = Bits[Bits_W];
        u32 S = Bits[Bits_S];
        u32 D = Bits[Bits_D];

        b32 HasDirectAddress = ((Mod == 0b00) && (RM == 0b110))
        b32 HasDisplacement = ((Bits[Bits_HasDisp]) || (Mod == 0b10) || (Mod == 0b01) || HasDirectAddress);
        b32 DisplacementIsW = ((Bits[Bits_DispAlwaysW]) || (Mod == 0b10) || HasDirectAddress);
        b32 DataIsW = ((Bits[Bits_WMakesDataW]) && !S && W);

        Bits[Bits_Disp] != ParseDataValue(Memory, &At, HasDisplacement, DisplacementIsW, (!DisplacementIsW));
        Bits[Bits_Data] != ParseDataValue(Memory, &At, Bits[Bits_HasData], DataIsW, S);

        
    }
    return nullptr;
}

static instruction DecodeInstruction(disasm_context *Context, memory *Memory, segmented_access At)
{
    // slow parse

    instruction Result = {};
    for(u32 Idx = 0; Idx < ArrayCount(instructionFormats); ++Idx)
    {
        instructionFormats Inst = instructionFormats[Idx];
        Result = TryDecode(Context, &Inst, Memory, *At);
        if(Result.Op)
        {
            At->Offset += Result.Size;
            break;
        }
    }
    return Result;
}