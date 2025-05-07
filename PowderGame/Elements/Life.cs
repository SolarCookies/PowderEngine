using System;

public class ScriptableElement
{
    public string Name = "Life";
    public int ID = 0;
    public int Sleep = 0;
    public float Density = 0.0f;
    public bool HasDensity = false;
    public int Color = e.Color(0, 255, 0, 255); // Green
    public int ID2 = 666; // Used to identify this type in C++

    public bool Dirty = false; //Should do non threaded tick

    public void BeginPlay(int x, int y, int frame)
    {
    }

    public void Tick(int x, int y, int frame)
    {
        Dirty = false;
        int aliveNeighbors = 0;

        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                if (ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                if (PowderGrid.GetInt(nx, ny, "ID2") == 666)
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

        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                if (ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                // Only spawn in empty cells
                if (!PowderGrid.IsEmpty(nx, ny)) continue;

                int neighbors = 0;

                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        if (i == 0 && j == 0) continue;

                        int tx = nx + i;
                        int ty = ny + j;

                        if (!PowderGrid.IsEmpty(tx, ty) && PowderGrid.GetInt(tx, ty, "ID2") == 666)
                        {
                            neighbors++;
                        }
                    }
                }

                if (neighbors == 3)
                {
                    PowderGrid.Spawn(nx, ny, "Life");
                }
            }
        }

        //PowderGrid.Sleep(x, y, 100); // optional to prevent double-ticking
      //  PowderGrid.Move(x, y, x, y); //force draw
    }

    public void SafeTick(int x, int y, int frame)
    {
        int aliveNeighbors = 0;

        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                if (ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                if (PowderGrid.GetInt(nx, ny, "ID2") == 666)
                {
                    aliveNeighbors++;
                }
            }
        }

        if (aliveNeighbors < 2 || aliveNeighbors > 3)
        {
            Dirty = true;
            return;
        }

        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                if (ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                // Only spawn in empty cells
                if (!PowderGrid.IsEmpty(nx, ny)) continue;

                int neighbors = 0;

                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        if (i == 0 && j == 0) continue;

                        int tx = nx + i;
                        int ty = ny + j;

                        if (!PowderGrid.IsEmpty(tx, ty) && PowderGrid.GetInt(tx, ty, "ID2") == 666)
                        {
                            neighbors++;
                        }
                    }
                }

                if (neighbors == 3)
                {
                    Dirty = true;
                }
            }
        }

        //PowderGrid.Sleep(x, y, 100); // optional to prevent double-ticking
        //  PowderGrid.Move(x, y, x, y); //force draw
    }

    public void SleepTick(int x, int y, int frame)
    {

    }
}