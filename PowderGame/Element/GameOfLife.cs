using System;

public class ScriptableElement
{
    public void Update(int x, int y, int frame)
    {
        int life = PowderGrid.GetLife(x, y);

        // First-time init: treat as alive
        if (life == -1)
        {
            PowderGrid.SetLife(x, y, 3); // Mark as alive
            return;
        }

        int aliveNeighbors = 0;

        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                if (ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                if (!PowderGrid.IsEmpty(nx, ny) &&
                    PowderGrid.GetName(nx, ny) == "GameOfLife")
                {
                    aliveNeighbors++;
                }
            }
        }


        if (aliveNeighbors < 2 || aliveNeighbors > 3)
        {
            PowderGrid.Kill(x, y); // Dies
            return;
        }

        // Survival case: cell stays alive
        PowderGrid.SetLife(x, y, 3);

        // Now check nearby empty cells for births
        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                if (ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                if (PowderGrid.IsEmpty(nx, ny))
                {
                    int neighbors = 0;

                    for (int i = -1; i <= 1; i++)
                    {
                        for (int j = -1; j <= 1; j++)
                        {
                            if (i == 0 && j == 0) continue;

                            int tx = nx + i;
                            int ty = ny + j;

                            if (!PowderGrid.IsEmpty(tx, ty) &&
                                PowderGrid.GetName(tx, ty) == "GameOfLife")
                            {
                                neighbors++;
                            }
                        }
                    }

                    if (neighbors == 3)
                    {
                        PowderGrid.Spawn(nx, ny, "GameOfLife");
                        PowderGrid.SetLife(nx, ny, -1);
                    }
                }
            }
        }

        //PowderGrid.Sleep(x, y, 100); // optional to prevent double-ticking
        PowderGrid.Move(x,y,x,y); //force draw
    }

    public int GetColor(int x, int y, int frame)
    {
        int life = PowderGrid.GetLife(x, y);

        // Clamp to [1, 3]
        if (life < 1) life = 1;
        if (life > 3) life = 3;

        // Interpolation factor: 1 -> 0.0 (red), 2 -> 0.5, 3 -> 1.0 (green)
        float t = (life - 1) / 2.0f;

        int r = (int)(255 * (1 - t));     // Red decreases as life increases
        int g = (int)(255 * t);           // Green increases as life increases
        int b = 0;

        return e.Color(r, g, b, 255);
    }

    public string GetName()
    {
        return "GameOfLife"; // Must match filename: GameOfLife.cs
    }

    public float GetTemprature(float currentTemp)
    {
        return currentTemp;
    }

    public float GetDensity(float currentDensity)
    {
        return currentDensity;
    }
}
