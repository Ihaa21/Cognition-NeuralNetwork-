// NOTE: Lets us use fopen
#pragma warning(disable:4996)

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef size_t mm;
typedef uintptr_t umm;

typedef int32_t b32;

#define internal static
#define global static
#define local_global static

#define Assert(Expression) if (!(Expression)) {*(int*)0 = 0;}
#define InvalidCodePath Assert(!"Invalid Code Path")
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define KiloBytes(Val) ((Val)*1024LL)
#define MegaBytes(Val) (KiloBytes(Val)*1024LL)
#define GigaBytes(Val) (MegaBytes(Val)*1024LL)

//
//
//

#include "cognition_memory.h"

global mem_arena Arena_ = InitArena(malloc(MegaBytes(100)), MegaBytes(100));
global mem_arena* Arena = &Arena_;

#include "cognition_matrix.h"
#include "cognition_dataset.h"
#include "cognition_network.h"

//
// NOTE: Window bitmap
//

global u32 ResX = 1000;
global u32 ResY = 800;
global BITMAPINFO BitmapInfo;
global u32* FrameBuffer;

//
// NOTE: Network learning properties
//

#define BACKPROP_MOMENTUM 0
global f32 LearningRate = 0.2f;
global f32 MomentumRate = 0.3f;

//
// NOTE: Choose what kind of network you want to train
//

#define TRAIN_CLASSIFIER 0
#define TRAIN_AUTOENCODER 1

#include "cognition_network.cpp"

inline void RenderMatrix(mat Mat, u32 Scale, u32 StartX, u32 StartY);

inline void TrainClassifier()
{
    // NOTE: Build the neural network
    hidden_layer* Layers = PushArray(Arena, hidden_layer, 3);
    Layers[0] = HiddenLayer(28*28, 16, 0.2f);
    Layers[1] = HiddenLayer(16, 16);
    Layers[2] = HiddenLayer(16, 10);
    
    mat* DigitData[] =
    {
        LoadDigitSet("data0"),
        LoadDigitSet("data1"),
        LoadDigitSet("data2"),
        LoadDigitSet("data3"),
        LoadDigitSet("data4"),
        LoadDigitSet("data5"),
        LoadDigitSet("data6"),
        LoadDigitSet("data7"),
        LoadDigitSet("data8"),
        LoadDigitSet("data9"),
    };

    // NOTE: The ground truth output values (1 for the digit in the img, 0 for everything else)
    mat Outputs[10];
    {
        f32 Vals0[] =
        {
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
        Outputs[0] = Mat(1, 10, Vals0);

        f32 Vals1[] =
        {
            0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        };
        Outputs[1] = Mat(1, 10, Vals1);

        f32 Vals2[] =
        {
            0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        };
        Outputs[2] = Mat(1, 10, Vals2);

        f32 Vals3[] =
        {
            0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        };
        Outputs[3] = Mat(1, 10, Vals3);

        f32 Vals4[] =
        {
            0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        };
        Outputs[4] = Mat(1, 10, Vals4);

        f32 Vals5[] =
        {
            0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        };
        Outputs[5] = Mat(1, 10, Vals5);

        f32 Vals6[] =
        {
            0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
        };
        Outputs[6] = Mat(1, 10, Vals6);

        f32 Vals7[] =
        {
            0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        };
        Outputs[7] = Mat(1, 10, Vals7);

        f32 Vals8[] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        };
        Outputs[8] = Mat(1, 10, Vals8);

        f32 Vals9[] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        };
        Outputs[9] = Mat(1, 10, Vals9);
    }

    // NOTE: Train the neural network
    for (u32 Epoch = 0; Epoch < 1000; ++Epoch)
    {
        for (u32 Digit = 0; Digit < 10; ++Digit)
        {
            temp_mem Temp = BeginTempMem(Arena);

            // NOTE: Build input and output digit
            mat Inputs = Reshape(DigitData[Digit][Epoch], 1, 28*28);
            
            mat Activ0 = Forward(Layers + 0, Inputs);
            mat Activ1 = Forward(Layers + 1, Activ0);
            mat Prediction = Forward(Layers + 2, Activ1);

            mat PredictionError = Outputs[Digit] - Prediction;
            mat Layer2Error = BackProp(Layers + 2, PredictionError, Activ1);
            mat Layer1Error = BackProp(Layers + 1, Layer2Error, Activ0);
            mat Layer0Error = BackProp(Layers + 0, Layer1Error, Inputs);
        
            if (Epoch % 100 == 0)
            {
                f32 Error = Mean(PredictionError);
                char str[256];
                sprintf_s(str, "Error: %f\n", Error);

                OutputDebugString(str);
            }
        
            EndTempMem(Temp);
        }
    }
}

inline void TrainDigitAutoEncoder()
{
    // NOTE: Build the neural network
    hidden_layer* Layers = PushArray(Arena, hidden_layer, 3);
    Layers[0] = HiddenLayer(28*28, 32);
    Layers[1] = HiddenLayer(32, 32);
    Layers[2] = HiddenLayer(32, 28*28);
    
    mat* DigitData[] =
    {
        LoadDigitSet("data0"),
        LoadDigitSet("data1"),
        LoadDigitSet("data2"),
        LoadDigitSet("data3"),
        LoadDigitSet("data4"),
        LoadDigitSet("data5"),
        LoadDigitSet("data6"),
        LoadDigitSet("data7"),
        LoadDigitSet("data8"),
        LoadDigitSet("data9"),
    };

    // NOTE: Train the network to reconstruct the input
    for (u32 Epoch = 0; Epoch < 1000; ++Epoch)
    {
        for (u32 Digit = 0; Digit < 10; ++Digit)
        {
            temp_mem Temp = BeginTempMem(Arena);

            // NOTE: Build input and output digit
            mat DigitImg = Reshape(DigitData[Digit][Epoch], 1, 28*28);
            
            mat Activ0 = Forward(Layers + 0, DigitImg);
            mat Activ1 = Forward(Layers + 1, Activ0);
            mat Prediction = Forward(Layers + 2, Activ1);

            mat PredictionError = DigitImg - Prediction;
            mat Layer2Error = BackProp(Layers + 2, PredictionError, Activ1);
            mat Layer1Error = BackProp(Layers + 1, Layer2Error, Activ0);
            mat Layer0Error = BackProp(Layers + 0, Layer1Error, DigitImg);
        
            if (Epoch % 100 == 0)
            {
                RenderMatrix(Reshape(DigitImg, 28, 28), 2, 0, Digit*28*2);
                RenderMatrix(Reshape(Prediction, 28, 28), 2, 2*28, Digit*28*2);
                
                f32 Error = Mean(PredictionError);
                char str[256];
                sprintf_s(str, "Error: %f\n", Error);

                OutputDebugString(str);
            }
        
            EndTempMem(Temp);
        }
    }
}

inline void RenderMatrix(mat T, u32 Scale, u32 StartX, u32 StartY)
{
    for (u32 Y = 0; Y < T.Rows; ++Y)
    {
        for (u32 X = 0; X < T.Cols; ++X)
        {
            u8 Gray = (u8)(T[T.Rows - Y - 1][X]*255.0f);
            u32 PixelVal = Gray << 16 | Gray << 8 | Gray;

            u32 NewY = Scale*Y;
            u32 NewX = Scale*X;
            for (u32 i = 0; i < Scale; ++i)
            {
                for (u32 j = 0; j < Scale; ++j)
                {
                    FrameBuffer[(NewY + i + StartY)*ResX + NewX + j + StartX] = PixelVal;
                }
            }
        }
    }
}

internal LRESULT CALLBACK Win32MainWindowCallBack(HWND Window, UINT Message, WPARAM WParam,
                                                  LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            exit(1);
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);

            RECT WindowRect;
            GetClientRect(Window, &WindowRect);
            u32 WindowWidth = WindowRect.right - WindowRect.left;
            u32 WindowHeight = WindowRect.bottom - WindowRect.top;
            
            StretchDIBits(GetDC(Window),
                          0, 0, WindowWidth, WindowHeight,
                          0, 0, ResX, ResY,
                          FrameBuffer, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
            EndPaint(Window, &Paint);
        } break;

        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break; 
    }
    
    return Result;
}

inline void Win32DisplayBufferInWindow(HWND Window)
{
    RECT WindowRect;
    GetClientRect(Window, &WindowRect);
    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;
        
    StretchDIBits(GetDC(Window),
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, ResX, ResY,
                  FrameBuffer, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

internal DWORD ThreadProc(LPVOID lpParam)
{
#if TRAIN_CLASSIFIER
    TrainClassifier();
#elif TRAIN_AUTOENCODER
    TrainDigitAutoEncoder();
#endif
    return 1;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{    
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = hInstance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.lpszClassName = "CognitionClass";

    if (!RegisterClassA(&WindowClass))
    {
        InvalidCodePath;
    }
    
    HWND Window = CreateWindowExA(0,
                             WindowClass.lpszClassName,
                             "Haw Yeah",
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             ResX,
                             ResY,
                             0,
                             0,
                             hInstance,
                             0);

    if (!Window)
    {
        InvalidCodePath;
    }

    FrameBuffer = (u32*)malloc(ResX*ResY*sizeof(u32));
    BITMAPINFOHEADER* BitmapHeader = &BitmapInfo.bmiHeader;
    BitmapHeader->biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapHeader->biWidth = ResX;
    BitmapHeader->biHeight = ResY;
    BitmapHeader->biPlanes = 1;
    BitmapHeader->biBitCount = 32;
    BitmapHeader->biCompression = BI_RGB;

    // NOTE: Train neural network on seperate thread to not block the window thread
    DWORD ThreadId;
    HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, 0, 0, &ThreadId);
    CloseHandle(ThreadHandle);
    
    while(true)
    {
        MSG Message;
                
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessageA(&Message);
        }
        
        Win32DisplayBufferInWindow(Window);
        Sleep(100);
    }
    
    return 1;
}
