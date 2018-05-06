#if !defined(COGNITION_TENSOR_H)

inline f32 Max(f32 A, f32 B)
{
    if (A > B)
    {
        return A;
    }
    return B;
}

inline f32 RandVal()
{
    f32 Result = static_cast<f32>(rand()) / static_cast<f32>(RAND_MAX);
    return Result;
}

struct mat
{
    u32 Cols;
    u32 Rows;
    f32* Vals;
    
    inline f32* operator[](u32 Y)
    {
        return Vals + (Y*Cols);
    }
};

inline mat Mat(u32 Rows, u32 Cols)
{
    mat Result = {};
    Result.Cols = Cols;
    Result.Rows = Rows;
    Result.Vals = PushArray(Arena, f32, Result.Cols*Result.Rows);

    return Result;
}

inline mat Mat(u32 Rows, u32 Cols, f32* Vals)
{
    mat Result = {};
    Result.Cols = Cols;
    Result.Rows = Rows;
    Result.Vals = PushArray(Arena, f32, Result.Cols*Result.Rows);
    Copy(Vals, Result.Vals, Result.Cols*Result.Rows*sizeof(f32));

    return Result;
}

inline mat RandDropout(u32 Rows, u32 Cols, f32 DropoutPercent)
{
    mat Result = Mat(Rows, Cols);
    for (u32 Index = 0; Index < Cols*Rows; ++Index)
    {
        Result.Vals[Index] = (f32)(RandVal() > DropoutPercent);
    }

    return Result;
}

inline mat Rand(u32 Rows, u32 Cols)
{
    mat Result = Mat(Rows, Cols);
    for (u32 Index = 0; Index < Cols*Rows; ++Index)
    {
        Result.Vals[Index] = 2.0f*RandVal() - 1.0f;
    }

    return Result;
}

inline mat operator+(mat A, mat B)
{
    Assert(A.Cols == B.Cols);
    Assert(A.Rows == B.Rows);

    mat Result = Mat(A.Rows, A.Cols);
    for (u32 Y = 0; Y < Result.Rows; ++Y)
    {
        for (u32 X = 0; X < Result.Cols; ++X)
        {
            Result[Y][X] = A[Y][X] + B[Y][X];
        }
    }

    return Result;
}

inline mat operator+(mat A, f32 B)
{
    mat Result = Mat(A.Rows, A.Cols);
    for (u32 Y = 0; Y < Result.Rows; ++Y)
    {
        for (u32 X = 0; X < Result.Cols; ++X)
        {
            Result[Y][X] = A[Y][X] + B;
        }
    }

    return Result;
}

inline mat operator+(f32 A, mat B)
{
    return B+A;
}

inline mat& operator+=(mat& A, mat B)
{
    A = A + B;
    return A;
}

inline mat& operator+=(mat& A, f32 B)
{
    A = A + B;
    return A;
}

inline mat operator-(mat A, mat B)
{
    Assert(A.Cols == B.Cols);
    Assert(A.Rows == B.Rows);

    mat Result = Mat(A.Rows, A.Cols);
    for (u32 Y = 0; Y < Result.Rows; ++Y)
    {
        for (u32 X = 0; X < Result.Cols; ++X)
        {
            Result[Y][X] = A[Y][X] - B[Y][X];
        }
    }

    return Result;
}

inline mat operator-(mat A, f32 B)
{
    B = -B;
    return A + B;
}

inline mat operator-(f32 A, mat B)
{
    return B-A;
}

inline mat operator-=(mat& A, mat B)
{
    A = A - B;
    return A;
}

inline mat operator-=(mat& A, f32 B)
{
    A = A - B;
    return A;
}

inline mat operator*(mat A, mat B)
{
    Assert(A.Cols == B.Rows);
    
    mat Result = Mat(A.Rows, B.Cols);

    for (u32 Y = 0; Y < Result.Rows; ++Y)
    {
        for (u32 X = 0; X < Result.Cols; ++X)
        {
            f32 Acc = 0.0f;
            for (u32 ElemIndex = 0; ElemIndex < B.Rows; ++ElemIndex)
            {
                Acc += A[Y][ElemIndex] * B[ElemIndex][X];
            }
            Result[Y][X] = Acc;
        }
    }
    
    return Result;
}

inline mat operator*(mat A, f32 B)
{
    mat Result = Mat(A.Rows, A.Cols);
    for (u32 Y = 0; Y < Result.Rows; ++Y)
    {
        for (u32 X = 0; X < Result.Cols; ++X)
        {
            Result[Y][X] = A[Y][X] * B;
        }
    }

    return Result;
}

inline mat operator*(f32 A, mat B)
{
    return B * A;
}

inline mat& operator*=(mat& A, mat B)
{
    A = A * B;
    return A;
}

inline void Copy(mat* Dest, mat* Val)
{
    Assert(Dest->Rows == Val->Rows);
    Assert(Dest->Cols == Val->Cols);

    for (u32 Index = 0; Index < Dest->Rows*Dest->Cols; ++Index)
    {
        Dest->Vals[Index] = Val->Vals[Index];
    }
}

inline mat ElemMul(mat A, mat B)
{
    Assert(A.Cols == B.Cols);
    Assert(A.Rows == B.Rows);

    mat Result = Mat(A.Rows, A.Cols);
    for (u32 Y = 0; Y < Result.Rows; ++Y)
    {
        for (u32 X = 0; X < Result.Cols; ++X)
        {
            Result[Y][X] = A[Y][X] * B[Y][X];
        }
    }

    return Result;
}

inline mat Reshape(mat T, u32 NewRows, u32 NewCols)
{
    Assert(NewRows*NewCols == T.Rows*T.Cols);

    mat Result = Mat(NewRows, NewCols);
    Copy(T.Vals, Result.Vals, T.Rows*T.Cols*sizeof(f32));

    return Result;
}

inline mat Transpose(mat T)
{
    mat Result = Mat(T.Cols, T.Rows);
    for (u32 Y = 0; Y < T.Rows; ++Y)
    {
        for (u32 X = 0; X < T.Cols; ++X)
        {
            Result[X][Y] = T[Y][X];
        }
    }

    return Result;
}

inline mat Clear(mat T, f32 Val)
{
    mat Result = Mat(T.Rows, T.Cols);
    for (u32 Index = 0; Index < T.Rows*T.Cols; ++Index)
    {
        Result.Vals[Index] = Val;
    }

    return Result;
}

inline f32 Mean(mat T)
{
    f32 Result = 0.0f;

    for (u32 Index = 0; Index < T.Cols*T.Rows; ++Index)
    {
        Result += abs(T.Vals[Index]); 
    }

    Result /= (f32)(T.Cols*T.Rows);

    return Result;
}

inline mat Sigmoid(mat T)
{
    mat Result = Mat(T.Rows, T.Cols);
    for (u32 Index = 0; Index < T.Cols*T.Rows; ++Index)
    {
        Result.Vals[Index] = 1.0f / (1.0f + exp(-T.Vals[Index]));
    }

    return Result;
}

inline mat SigmoidDeriv(mat T)
{
    mat Result = Mat(T.Rows, T.Cols);
    for (u32 Index = 0; Index < T.Cols*T.Rows; ++Index)
    {
        Result.Vals[Index] = T.Vals[Index]*(1.0f - T.Vals[Index]);
    }

    return Result;
}

inline mat Relu(mat T)
{
    mat Result = Mat(T.Rows, T.Cols);
    for (u32 Index = 0; Index < T.Cols*T.Rows; ++Index)
    {
        Result.Vals[Index] = Max(0.1f*T.Vals[Index], T.Vals[Index]);
    }

    return Result;
}

inline mat ReluDeriv(mat T)
{
    mat Result = Mat(T.Rows, T.Cols);
    for (u32 Index = 0; Index < T.Cols*T.Rows; ++Index)
    {
        Result.Vals[Index] = (T.Vals[Index] > 0.0f) + 0.1f*(T.Vals[Index] <= 0.0f);
    }

    return Result;
}

#define COGNITION_TENSOR_H
#endif
