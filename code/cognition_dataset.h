#if !defined(COGNITION_DATESET_H)

internal mat* LoadDigitSet(char* Path)
{
    FILE* File = fopen(Path, "rb");
    u8* TempData = (u8*)malloc(1000*28*28*sizeof(u8));
    mm Read = fread(TempData, 1, 1000*28*28*sizeof(u8), File);
    Assert(Read == 1000*28*28*sizeof(u8));
    fclose(File);

    // NOTE: Every file stores 1000 digits, each are 28*28 pixels with only 1 color channel
    u8* CurrPixel = TempData;
    mat* Result = PushArray(Arena, mat, 1000);
    for (u32 DigitIndex = 0; DigitIndex < 1000; ++DigitIndex)
    {
        mat* Digit = Result + DigitIndex;
        *Digit = Mat(28, 28);
        for (u32 PixelIndex = 0; PixelIndex < 28*28; ++PixelIndex)
        {
            Digit->Vals[PixelIndex] = (f32)(*CurrPixel++) / 255.0f;
        }
    }

    return Result;
}

#define COGNITION_DATESET_H
#endif
