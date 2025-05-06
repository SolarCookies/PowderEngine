using System;

public class ScriptableElement
{
    public string Name = "Sand";
    public int ID = 0; //Random Int Assigned by C++ on construct
    public int Sleep = 0;
    public float Density = 2.0f;
    public bool HasDensity = true;
    public int Color = e.Color(235, 220, 180, 255); // default tan

    public static int Clamp(int value, int min, int max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    public void BeginPlay(int x, int y, int frame)
    {
        int baseR = 220;
        int baseG = 200;
        int baseB = 150;

        // Subtle noise using Rand
        int offset = Rand.Int(ID + frame, -10, 10);
        int r = Clamp(baseR + offset, 180, 255);
        int g = Clamp(baseG + offset, 160, 230);
        int b = Clamp(baseB + offset, 120, 210);

        Color = e.Color(r, g, b, 255);
    }

    public void Tick(int x, int y, int frame)
    {
        // Blocked? Sleep.
        if (!PowderGrid.IsEmpty(x, y + 1) && !PowderGrid.IsEmpty(x + 1, y + 1) && !PowderGrid.IsEmpty(x - 1, y + 1))
        {
            if (PowderGrid.GetBool(x, y + 1, "HasDensity") && PowderGrid.GetFloat(x, y + 1, "Density") < Density)
            {
                PowderGrid.Swap(x, y, x, y + 1);
            }
            PowderGrid.SetInt(x, y, "Sleep", 5);
            return;
        }

        // Fall straight down if empty
        if (PowderGrid.IsEmpty(x, y + 1))
        {
            PowderGrid.Move(x, y, x, y + 1);
            return;
        }
        // Swap with lighter material below
        else if (
            PowderGrid.GetBool(x, y + 1, "HasDensity") &&
            PowderGrid.GetFloat(x, y + 1, "Density") < Density)
        {
            PowderGrid.Swap(x, y, x, y + 1);
            return;
        }

        // Pick a random diagonal direction
        int direction = Rand.Int(ID + frame, 0, 1);
        int dx = direction == 0 ? 1 : -1;

        // Move diagonally if empty
        if (PowderGrid.IsEmpty(x + dx, y + 1))
        {
            PowderGrid.Move(x, y, x + dx, y + 1);
            return;
        }
        // Swap diagonally if lighter material
        else if (
            PowderGrid.GetBool(x + dx, y + 1, "HasDensity") &&
            PowderGrid.GetFloat(x + dx, y + 1, "Density") < Density)
        {
            PowderGrid.Swap(x, y, x + dx, y + 1);
            return;
        }
    }

    public void SleepTick(int x, int y, int frame)
    {
        // Optional: behavior while asleep
    }
}