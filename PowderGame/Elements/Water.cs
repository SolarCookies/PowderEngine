using System;

public class ScriptableElement
{
    public string Name = "Water";
    public int ID = 0; // Assigned by C++
    public int Sleep = 0;
    public float Density = 1.0f; // Lower than sand
    public bool HasDensity = true;
    public int Color = e.Color(80, 120, 255, 255); // Default blue
    public bool Dirty = false; //Should do non threaded tick

    public static int Clamp(int value, int min, int max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    public void BeginPlay(int x, int y, int frame)
    {
        int baseR = 60;
        int baseG = 100;
        int baseB = 255;

        int offset = Rand.Int(ID + frame, -10, 10);
        int r = Clamp(baseR + offset, 40, 100);
        int g = Clamp(baseG + offset, 80, 140);
        int b = Clamp(baseB + offset, 220, 255);

        Color = e.Color(r, g, b, 255);
    }

    public void Tick(int x, int y, int frame)
    {
        Dirty = false;
        // Fall straight down
        if (PowderGrid.IsEmpty(x, y + 1))
        {
            PowderGrid.Move(x, y, x, y + 1);
            return;
        }
        else if (PowderGrid.GetBool(x, y + 1, "HasDensity") &&
                 PowderGrid.GetFloat(x, y + 1, "Density") < Density)
        {
            PowderGrid.Swap(x, y, x, y + 1);
            return;
        }

        // Flow sideways
        int direction = Rand.Int(ID + frame, 0, 1);
        int dx = direction == 0 ? 1 : -1;

        // Try side down
        if (PowderGrid.IsEmpty(x + dx, y + 1))
        {
            PowderGrid.Move(x, y, x + dx, y + 1);
            return;
        }
        else if (PowderGrid.GetBool(x + dx, y + 1, "HasDensity") &&
                 PowderGrid.GetFloat(x + dx, y + 1, "Density") < Density)
        {
            PowderGrid.Swap(x, y, x + dx, y + 1);
            return;
        }

        // Try horizontal flow if blocked
        if (PowderGrid.IsEmpty(x + dx, y))
        {
            PowderGrid.Move(x, y, x + dx, y);
            return;
        }
        else if (PowderGrid.GetBool(x + dx, y, "HasDensity") &&
                 PowderGrid.GetFloat(x + dx, y, "Density") < Density)
        {
            PowderGrid.Swap(x, y, x + dx, y);
            return;
        }

      //  PowderGrid.SetInt(x, y, "Sleep", 3);
    }

    public void SafeTick(int x, int y, int frame)
    {
        // Fall straight down
        if (PowderGrid.IsEmpty(x, y + 1))
        {
            Dirty = true;
            return;
        }
        else if (PowderGrid.GetBool(x, y + 1, "HasDensity") &&
                 PowderGrid.GetFloat(x, y + 1, "Density") < Density)
        {
            Dirty = true;
            return;
        }

        // Flow sideways
        int direction = Rand.Int(ID + frame, 0, 1);
        int dx = direction == 0 ? 1 : -1;

        // Try side down
        if (PowderGrid.IsEmpty(x + dx, y + 1))
        {
            Dirty = true;
            return;
        }
        else if (PowderGrid.GetBool(x + dx, y + 1, "HasDensity") &&
                 PowderGrid.GetFloat(x + dx, y + 1, "Density") < Density)
        {
            Dirty = true;
            return;
        }

        // Try horizontal flow if blocked
        if (PowderGrid.IsEmpty(x + dx, y))
        {
            Dirty = true;
            return;
        }
        else if (PowderGrid.GetBool(x + dx, y, "HasDensity") &&
                 PowderGrid.GetFloat(x + dx, y, "Density") < Density)
        {
            Dirty = true;
            return;
        }

        //Sleep = 3;
    }

    public void SleepTick(int x, int y, int frame)
    {
        // Optional: gentle settling behavior?
    }
}