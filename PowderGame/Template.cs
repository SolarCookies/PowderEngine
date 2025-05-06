using System;


public class ScriptableElement
{
    // These fields are accessed from C++ using mono_field_get/set
    public string Name = "Template"; //Same as the name of the file i.e "Template.cs"
    public int ID = 0; //Random Int Assigned by C++ on construct
    public int Sleep = 0; // sleeps for x ticks
    public float Density = 1.0f;
    public bool HasDensity = true; //If this is false it tells other elements that it shouldnt be swaped
    public int Color = e.Color(255,00,255,255);//RGBA

    // Called once when spawned in the world
    public void BeginPlay(int x, int y, int frame)
    {
        // Optional: initialization logic
        // For example: set Life = 100;
    }

    // Called every tick
    public void Tick(int x, int y, int frame)
    {
        
    }

    // Called every tick while sleeping
    public void SleepTick(int x, int y, int frame)
    {

    }
}