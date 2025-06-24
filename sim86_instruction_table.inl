/*
   NOTE: This instruction table is a direct translation of table 4-12 in the Intel 8086 manual.
   The macros are designed to allow direct transcription, without changing the order or manner
   of specification in the table in any way. Additional "implicit" versions of the macros are provided
   so that hard-coded fields can be supplied uniformly.
   
   The table is also designed to allow you to include it multiple times to "pull out" other things
   from the table, such as opcode mnemonics as strings or enums, etc.
*/

#ifndef INST
#define INST(Mnemonic, Encoding, ...) {Op_##Mnemonic, Encoding, __VA_ARGS__},
#endif

#ifndef INSTALT
#define INSTALT INST
#endif

