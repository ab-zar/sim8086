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

    char const *Result = RMBase[Address.Base];
    return Result;
}


static b32 isPrintable(instruction Instruction)
{
    b32 Result = !((Instruction.Op == Op_lock) ||
                    (Instruction.Op == Op_rep) ||
                    (Instruction.Op == Op_segment));
    return Result;
}

static void PrintInstruction(instruction Instruction, FILE *Dest)
{
    u32 Flags = Instruction.Flags;
    u32 W = Flags & Inst_Wide;

    if(Flags & Inst_Lock)
    {
        if(Instruction.Op == Op_xchg)
        {
            instruction_operand Temp = Instruction.Operands[0];
            Instruction.Operands[0] = Instruction.Operands[1];
            Instruction.Operands[1] = Temp;
        }
        fprintf(Dest, "lock ");
    }

    char const *MnemonicSuffix = "";
    if(Flags & Inst_Rep)
    {
        printf("rep ");
        MnemonicSuffix = W ? "w" : "b";
    }

    fprintf(Dest, "%s%s ", GetOpcodeMnemonic(Instruction.Op), MnemonicSuffix);

    char const *Separator = "";
    for(u32 OperandIdx = 0; OperandIdx < ArrayCount(Instruction.Operands); ++OperandIdx)
    {
        instruction_operand Operand = Instruction.Operands[OperandIdx];
        if(Operand.Type == Operand_None)
        {
            fprintf(Dest, "%s", Separator);
            Separator = ", ";

            switch (Operand.Type)
            {
                case Operand_None: {} break;
                case Operand_Register:
                {
                    fprintf(Dest, "%s", GetRegName(Operand.Register));
                } break;

                case Operand_Memory:
                {
                    effective_address_expression Address = Operand.Address;

                    if(Instruction.Operands[0].Type != Operand_Register)
                    {
                        fprintf(Dest, "%s", W ? "word" : "byte");
                    }

                    if(Flags & Inst_Segment)
                    {
                        fprintf(Dest, "%s:", GetRegName({Address.Segment, 0, 2}));
                    }

                    fprintf(Dest, "[%s", GetEffectiveAddressExpression(Address));
                    if(Address.Displacement != 0)
                    {
                        fprintf(Dest, "%+d", Address.Displacement);
                    }
                    fprintf(Dest, "]");
                } break;

                case Operand_Immediate:
                {
                    fprintf(Dest, "%d", Operand.ImmediateS32);
                } break;
                
                case Operand_RelativeImmediate:
                {
                    fprintf(Dest, "$%+d", Operand.ImmediateS32);
                } break;
            }
        }
    }
}