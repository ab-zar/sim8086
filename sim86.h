#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifndef SIM86_H
#define SIM86_H

#endif //SIM86_H

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int32_t b32;

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

enum operation_type {
    Op_none,

    #define INST(Mnemonic, ...) Op_##Mnemonic,
    #define INSTALT(...)
    #include "sim86_instruction_table.inl"

    Op_count,
};

enum instruction_flag {
    Inst_Lock = (1 << 0),
    Inst_Rep = (1 << 1), 
    Inst_Segment = (1 << 2),
    Inst_Wide = (1 << 3),
};

enum register_index {
    Register_none,

    Register_a,
    Register_b,
    Register_c,
    Register_d,
    Register_sp,
    Register_bp,
    Register_si,
    Register_di,
    Register_es,
    Register_cs,
    Register_ss,
    Register_ds,
    Register_ip,
    Register_flags,

    Register_count,
};

enum effective_address_base {
    EffectiveAddress_direct,

    EffectiveAddress_bx_si,
    EffectiveAddress_bx_di,
    EffectiveAddress_bp_si,
    EffectiveAddress_bp_di,
    EffectiveAddress_si,
    EffectiveAddress_di,
    EffectiveAddress_bp,
    EffectiveAddress_bx,

    EffectiveAddress_count,
};

struct effective_address_expression {
    register_index Segment;
    effective_address_base Base;
    s32 Displacement; // 32-bit displacement for the effective address
};

struct register_access {
    register_index Index;
    u8 Offset;
    u8 Count;
};

enum operand_type {
    Operand_None,
    Operand_Register,
    Operand_Memory,
    Operand_Immediate,
    Operand_RelativeImmediate,
};

struct instruction_operand {
    operand_type Type;
    union {
        effective_address_expression Address;
        register_access Register;
        
        s32 ImmediateU32; // 32-bit immediate value
        s32 ImmediateS32; // 16-bit relative immediate value
    };
};

struct instruction {
    u32 Address;
    u32 Size;

    operation_type Op;
    u32 Flags;

    instruction_operand Operands[2]; // Up to 2 operands per instruction
};

