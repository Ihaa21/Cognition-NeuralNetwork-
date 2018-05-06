#if !defined(COGNITION_MEMORY_H)

struct mem_arena
{
    mm Size;
    mm Used;
    u8* Mem;
};

struct temp_mem
{
    mem_arena* Arena;
    mm Used;
};

inline mem_arena InitArena(void* Mem, mm Size)
{
    mem_arena Result = {};
    Result.Size = Size;
    Result.Used = 0;
    Result.Mem = (u8*)Mem;

    return Result;
}

inline void ClearArena(mem_arena* Arena)
{
    Arena->Used = 0;
}

inline temp_mem BeginTempMem(mem_arena* Arena)
{
    // NOTE: This function lets us take all memory allocated past this point and later
    // free it
    temp_mem TempMem = {};
    TempMem.Arena = Arena;
    TempMem.Used = Arena->Used;

    return TempMem;
}

inline void EndTempMem(temp_mem TempMem)
{
    TempMem.Arena->Used = TempMem.Used;
}

#define PushStruct(Arena, type) (type*)PushSize(Arena, sizeof(type))
#define PushArray(Arena, type, count) (type*)PushSize(Arena, sizeof(type)*count)
inline void* PushSize(mem_arena* Arena, mm Size)
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void* Result = Arena->Mem + Arena->Used;
    Arena->Used += Size;

    return Result;
}

inline mem_arena SubArena(mem_arena* Arena, mm Size)
{
    mem_arena Result = {};
    Result.Size = Size;
    Result.Used = 0;
    Result.Mem = (u8*)PushSize(Arena, Size);

    return Result;
}

inline void ClearMem(void* Mem, mm Size)
{
    u8* CurrByte = (u8*)Mem;
    for (mm Byte = 0; Byte < Size; ++Byte)
    {
        *CurrByte++ = 0;
    }
}

inline void Copy(void* Mem, void* Dest, mm Size)
{
    u8* CurrentByte = (u8*)Mem;
    u8* DestByte = (u8*)Dest;
    for (mm Byte = 0; Byte < Size; ++Byte)
    {
        *DestByte++ = *CurrentByte++;
    }
}

#define COGNITION_MEMORY_H
#endif
