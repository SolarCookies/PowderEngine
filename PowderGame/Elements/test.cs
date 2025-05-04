using System;

public class ScriptableElement
{
    // Called once per tick
    public void Update(int x, int y, int gridPtr)
    {
        if (PowderGrid.IsEmpty(x, y +1))
        {
            // No Element below
            PowderGrid.Move(x, y, x, y +1);
        }
        else
        {
            // Element below
        }
    }

    //color
    public int GetColor()
    {
        //ARGB
        return unchecked((int)0xFF00FFFF);
    }
}











