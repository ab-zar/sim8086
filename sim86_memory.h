struct memory
{
    u8 Bytes[1024 * 1024]; // 1 MB of memory
};
#define MEMORY_ACCESS_MASK 0xFFFFF
static_assert((ArrayCount(memory::Bytes) - 1) == MEMORY_ACCESS_MASK, "Memory size doesn't match access mask");

struct segmented_access
{
    u16 SegmentBase;
    u16 Offset;
};

static u32 GetAbsoluteAddressOf(u16 SegmentBase, u16 Offset, u16 AdditionalOffset = 0);
static u32 GetAbsoluteAddressOf(segmented_access Access, u16 AdditionalOffset = 0);

static u8 ReadMemory(memory *Memory, u32 AbsoluteAddress);

static u32 LoadMemoryFromFile(char *FileName, memory *Memory, u32 Offset = 0);