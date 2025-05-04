using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class PowderGrid
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Move(int x, int y, int dx, int dy);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Swap(int x1, int y1, int x2, int y2);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int GetDensity(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string GetName(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int GetID(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsSolid(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern float GetTemperature(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern float GetEnergy(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsEmpty(int x, int y);
}