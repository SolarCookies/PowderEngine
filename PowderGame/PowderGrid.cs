using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System;

public class e
{
    public static int Color(int r, int g, int b, int a)
    {
        return unchecked((int)((a << 24) | (r << 16) | (g << 8) | b));
    }
}


public class PowderGrid
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Move(int x, int y, int dx, int dy); // This is destructive and you should run IsEmpty() to make sure its okay to move

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Swap(int x1, int y1, int x2, int y2); // This will swap the elements without deleting

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Kill(int x, int y); //Removes a element at location

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Spawn(int x, int y, string name); //Trys to spawn a element with a name, if the name is invalid then this does nothing

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Log(string message); // Displays a cout message (Lags on tick)

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsEmpty(int x, int y);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int GetInt(int x, int y, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string GetString(int x, int y, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern float GetFloat(int x, int y, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool GetBool(int x, int y, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int GetColor(int x, int y, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetInt(int x, int y, string name, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GetString(int x, int y, string name, string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GetFloat(int x, int y, string name, float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GetBool(int x, int y, string name, bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GetColor(int x, int y, string name, int value);
}

public static class Rand
{
    public static int PcgHash(int input)
    {
        unchecked
        {
            uint x = (uint)input;
            x ^= x >> 16;
            x *= 0x85ebca6b;
            x ^= x >> 13;
            x *= 0xc2b2ae35;
            x ^= x >> 16;
            return (int)x;
        }
    }

    public static float Float(int seed)
    {
        int hashed = PcgHash(seed);
        return (float)hashed / int.MaxValue;
    }

    public static int Int(int seed, int min, int max)
    {
        uint hashed = (uint)PcgHash(seed);
        int range = max - min + 1;
        return (int)(hashed % (uint)range) + min;
    }
}