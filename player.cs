using Godot;
using System;

public partial class player : Node2D
{
    // Called when the node enters the scene tree for the first time.
    private AnimatedSprite2D animatedSprite;
    private TileMapLayer tileMapLayer1;
    private TileMapLayer tileMapLayer2;
    private TileMapLayer tileMapLayer3;
    private Vector2I gridPosition = new Vector2I(0, 0); // posición en celdas
    public override void _Ready()
{
	// Inicializar AnimatedSprite2D con la animación BOAR_NW_IDLE
	animatedSprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");
    tileMapLayer1 = GetParent().GetNode<TileMapLayer>("Layer1");
    tileMapLayer2 = GetParent().GetNode<TileMapLayer>("Layer2");
    tileMapLayer3 = GetParent().GetNode<TileMapLayer>("Layer3");
        if (animatedSprite != null)
	{
		animatedSprite.Animation = "BOAR_NW_IDLE";
		animatedSprite.Play();
	}
        // Posicionar inicialmente al jugador en la celda (0,0)
        SetPlayerPosition(gridPosition);
}

    // Called every frame. 'delta' is the elapsed time since the previous frame.



    public override void _Process(double delta)
    {
        bool moved = false;

        Vector2I direction = Vector2I.Zero;

        if (Input.IsActionJustPressed("up"))
        {
            direction = new Vector2I(-1, 0);
            animatedSprite.Play("boar_NW_RUN");
        }
        else if (Input.IsActionJustPressed("right"))
        {
            direction = new Vector2I(0, -1);
            animatedSprite.Play("boar_NE_RUN");
        }
        else if (Input.IsActionJustPressed("down"))
        {
            direction = new Vector2I(1, 0);
            animatedSprite.Play("boar_SE_RUN");
        }
        else if (Input.IsActionJustPressed("left"))
        {
            direction = new Vector2I(0, 1);
            animatedSprite.Play("boar_SW_RUN");
        }

        if (direction != Vector2I.Zero)
        {
            Vector2I target = gridPosition + direction;

            if (CanMoveTo(target))
            {
                gridPosition = target;
                SetPlayerPosition(gridPosition);
                moved = true;
            }
            else
            {
                GD.Print($"Movimiento bloqueado: no hay celda en {target}");
            }
        }
    }

    private void SetPlayerPosition(Vector2I cellPos)
    {
        if (tileMapLayer1 == null)
            return;

        // Convertir celda a posición mundial
        Vector2 worldPosition = tileMapLayer1.MapToLocal(cellPos);
        Position = worldPosition;

        GD.Print($"Jugador movido a celda: {cellPos} -> posición global: {worldPosition}");
    }
    private bool CanMoveTo(Vector2I target)
    {
        // Verificar que la celda existe en layer1
        if (!tileMapLayer1.GetUsedCells().Contains(target))
            return false;

        // Verificar si layer2 tiene un tile en esa celda
        TileData tileData = tileMapLayer2.GetCellTileData(target);
        if (tileData != null)
        {
            int sourceId = tileMapLayer2.GetCellSourceId(target);
            Vector2I atlasCoords = tileMapLayer2.GetCellAtlasCoords(target);

            // Si coincide con los valores bloqueados, no permitir movimiento
            if ((sourceId == 0 && atlasCoords == new Vector2I(10, 5))|| 
                (sourceId == 0 && atlasCoords == new Vector2I(4, 5)))
                {
                GD.Print($"Movimiento bloqueado por layer2 en celda {target}");
                return false;
            }
        }

        return true;
    }


}
