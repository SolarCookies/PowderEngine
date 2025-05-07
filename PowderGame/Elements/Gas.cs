using System;

public class ScriptableElement
{
    // Required public fields for reflection access from C++
    public string Name = "Gas";
    public int ID = 0; //Random Int Assigned by C++ on construct
    public int Sleep = 0; // no sleep by default
    public float Density = 0.0f; // extremely low density
    public bool HasDensity = true; // gases should not be swapped like solids/liquids
    public int Color = e.Color(255, 255, 0, 255); // RGBA: Yellow
    public bool Dirty = false; //Should do non threaded tick

    // Called when first spawned
    public void BeginPlay(int x, int y, int frame)
    {
        // Could initialize any state here
    }

    // Called every tick
    public void Tick(int x, int y, int frame)
    {
        Dirty = false;
        int directionX = Rand.Int(ID + frame, -1, 1);
        int directionY = Rand.Int(ID * 2 + frame, -1, 1);

        if (PowderGrid.IsEmpty(x + directionX, y + directionY))
        {
            PowderGrid.Move(x, y, x + directionX, y + directionY);
        }
    }

    // Called every tick
    public void SafeTick(int x, int y, int frame)
    {
        int directionX = Rand.Int(ID + frame, -1, 1);
        int directionY = Rand.Int(ID * 2 + frame, -1, 1);

        if (PowderGrid.IsEmpty(x + directionX, y + directionY))
        {
            Dirty = true;
        }
    }

    // Called instead of Tick when sleeping (not used here)
    public void SleepTick(int x, int y, int frame)
    {
    }
}













