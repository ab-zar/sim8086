char const *OpcodeMnemonics[] = 
{
    "",

#define INST(Mnemonic, ...) #Mnemonic,
#define INSTALT(...)
#include "sim86_instruction_table.inl"
};

static char const *GetOpcodeMnemonic(operation_type Op)
{
    char const *Result = OpcodeMnemonics[Op];
    return Result;
}

static char const *GetRegName(register_access Reg)
{
    char const *Names[][3] = 
    {
        {"", "", ""},
        {"al", "ah", "ax"},
        {"bl", "bh", "bx"},
        {"cl", "ch", "cx"},
        {"dl", "dh", "dx"},
        {"sp", "sp", "sp"},
        {"bp", "bp", "bp"},
        {"si", "si", "si"},
        {"di", "di", "di"},
        {"es", "es", "es"},
        {"cs", "cs", "cs"},
        {"ss", "ss", "ss"},
        {"ds", "ds", "ds"},
        {"ip", "ip", "ip"},
        {"flags", "flags", "flags"}
    };
    static_assert(ArrayCount(Names) == Register_count, "Register name table size mismatch");

    char const *Result = Names[Reg.Index][(Reg.Count == 2) ? 2 : Reg.Offset&1];
    return Result;
}

static char const *GetEffectiveAddressExpression(effective_address_expression Address)
{
    char const *RMBase[] = 
    {
        "",
        "bx+si",
        "bx+di",
        "bp+si",
        "bp+di",
        "si",
        "di",
        "bp",
        "bx",
    };
    static_assert(ArrayCount(RMBase) == EffectiveAddress_count, "Text table mismatch for effective_base_address");

    char const *Result = Names[Reg.Index][(Reg.Count == 2) ? 2 : Reg.Offset&1];
    return Result;
}


static b32 isPrintable(instruction Instruction)
{
    b32 Result = !((Instruction.Op == Op_lock) ||
                    (Instruction.Op == Op_rep) ||
                    (Instruction.Op == Op_segment));
    return Result;
}