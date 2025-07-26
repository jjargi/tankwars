
using Godot;

public partial class player : Node2D
{
    private AnimatedSprite2D animatedSprite;
    private TileMapLayer tileMapLayer1;
    private TileMapLayer tileMapLayer2;
    private TileMapLayer tileMapLayer3;
    private Vector2I gridPosition;
    private bool useGridSystem = true; // Cambiar a false para desactivar el sistema de celdas

    public override void _Ready()
    {
        animatedSprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");
        tileMapLayer1 = GetParent().GetNode<TileMapLayer>("Layer1");
        tileMapLayer2 = GetParent().GetNode<TileMapLayer>("Layer2");
        tileMapLayer3 = GetParent().GetNode<TileMapLayer>("Layer3");

        if (animatedSprite != null)
        {
            animatedSprite.Animation = "BOAR_NW_IDLE";
            animatedSprite.Play();
        }

        if (useGridSystem)
        {
            // Solo calcular posición inicial si usamos el sistema de grid
            gridPosition = tileMapLayer1.LocalToMap(Position);
            GD.Print($"Player inicializado en celda: {gridPosition}");
        }
    }

    public override void _Process(double delta)
    {
        if (!useGridSystem) return;

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
                Position = tileMapLayer1.MapToLocal(gridPosition);
                moved = true;
                GD.Print($"Movido a celda: {gridPosition}");
            }
            else
            {
                GD.Print($"Movimiento bloqueado en celda: {target}");
            }
        }
    }

    private bool CanMoveTo(Vector2I target)
    {
        if (!tileMapLayer1.GetUsedCells().Contains(target))
            return false;

        TileData tileData = tileMapLayer2.GetCellTileData(target);
        if (tileData != null)
        {
            int sourceId = tileMapLayer2.GetCellSourceId(target);
            Vector2I atlasCoords = tileMapLayer2.GetCellAtlasCoords(target);

            if ((sourceId == 0 && atlasCoords == new Vector2I(10, 5)) ||
                (sourceId == 0 && atlasCoords == new Vector2I(4, 5)))
            {
                return false;
            }
        }
        return true;
    }
}