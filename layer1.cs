using Godot;
using System;

public partial class layer1 : TileMapLayer
{
    private TileMapLayer tileMapLayer;
    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
	{
        tileMapLayer = GetParent().GetNode<TileMapLayer>("Layer1"); //Obtener el TileMapLayer

        if (tileMapLayer == null)
        {
            GD.PrintErr("Error: TileMapLayer no encontrado dentro de GameBoard.");
            return;
        }
    }

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
        for (int x = 0; x < 15; x++)
        {
            for (int y = 0; y < 15; y++)
            {
                Vector2I targetTile = new Vector2I(x, y);

                if (tileMapLayer.GetUsedCells().Contains(targetTile))
                {
                    GD.Print($"Celda en x:{x} y:{y} EXISTE");
                }
                else
                {
                    GD.Print($"Celda en x:{x} y:{y} NO EXISTE");
                }
            }
        }
    }
}
