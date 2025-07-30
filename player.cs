
//using Godot;

//public partial class player : Node2D
//{
//    private AnimatedSprite2D animatedSprite;
//    private TileMapLayer tileMapLayer1;
//    private TileMapLayer tileMapLayer2;
//    private TileMapLayer tileMapLayer3;
//    private Vector2I gridPosition;
//    private bool useGridSystem = true; // Cambiar a false para desactivar el sistema de celdas

//    public override void _Ready()
//    {
//        animatedSprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");
//        tileMapLayer1 = GetParent().GetNode<TileMapLayer>("Layer1");
//        tileMapLayer2 = GetParent().GetNode<TileMapLayer>("Layer2");
//        tileMapLayer3 = GetParent().GetNode<TileMapLayer>("Layer3");

//        if (animatedSprite != null)
//        {
//            animatedSprite.Animation = "BOAR_NW_IDLE";
//            animatedSprite.Play();
//        }

//        if (useGridSystem)
//        {
//            // Solo calcular posición inicial si usamos el sistema de grid
//            gridPosition = tileMapLayer1.LocalToMap(Position);
//            GD.Print($"Player inicializado en celda: {gridPosition}");
//        }
//    }

//    public override void _Process(double delta)
//    {
//        if (!useGridSystem) return;

//        bool moved = false;
//        Vector2I direction = Vector2I.Zero;

//        if (Input.IsActionJustPressed("up"))
//        {
//            direction = new Vector2I(-1, 0);
//            animatedSprite.Play("boar_NW_RUN");
//        }
//        else if (Input.IsActionJustPressed("right"))
//        {
//            direction = new Vector2I(0, -1);
//            animatedSprite.Play("boar_NE_RUN");
//        }
//        else if (Input.IsActionJustPressed("down"))
//        {
//            direction = new Vector2I(1, 0);
//            animatedSprite.Play("boar_SE_RUN");
//        }
//        else if (Input.IsActionJustPressed("left"))
//        {
//            direction = new Vector2I(0, 1);
//            animatedSprite.Play("boar_SW_RUN");
//        }

//        if (direction != Vector2I.Zero)
//        {
//            Vector2I target = gridPosition + direction;

//            if (CanMoveTo(target))
//            {
//                gridPosition = target;
//                Position = tileMapLayer1.MapToLocal(gridPosition);
//                moved = true;
//                GD.Print($"Movido a celda: {gridPosition}");
//            }
//            else
//            {
//                GD.Print($"Movimiento bloqueado en celda: {target}");
//            }
//        }
//    }

//    private bool CanMoveTo(Vector2I target)
//    {
//        if (!tileMapLayer1.GetUsedCells().Contains(target))
//            return false;

//        TileData tileData = tileMapLayer2.GetCellTileData(target);
//        if (tileData != null)
//        {
//            int sourceId = tileMapLayer2.GetCellSourceId(target);
//            Vector2I atlasCoords = tileMapLayer2.GetCellAtlasCoords(target);

//            if ((sourceId == 0 && atlasCoords == new Vector2I(10, 5)) ||
//                (sourceId == 0 && atlasCoords == new Vector2I(4, 5)))
//            {
//                return false;
//            }
//        }
//        return true;
//    }
//}
using Godot;

public partial class player : Node2D
{
    private AnimatedSprite2D animatedSprite;
    private TileMapLayer tileMapLayer1;
    private TileMapLayer tileMapLayer2;
    private TileMapLayer tileMapLayer3;
    private Vector2I gridPosition;
    private Node2D player2;  // Referencia a player2
    private Timer moveTimer; // Timer para controlar la frecuencia de movimiento

    public override void _Ready()
    {
        animatedSprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");
        tileMapLayer1 = GetParent().GetNode<TileMapLayer>("Layer1");
        tileMapLayer2 = GetParent().GetNode<TileMapLayer>("Layer2");
        tileMapLayer3 = GetParent().GetNode<TileMapLayer>("Layer3");

        // Buscar player2 en la escena (ajusta el path según tu estructura)
        player2 = GetParent().GetNodeOrNull<Node2D>("player2");

        if (animatedSprite != null)
        {
            animatedSprite.Animation = "BOAR_NW_IDLE";
            animatedSprite.Play();
        }

        // Configurar timer
        moveTimer = new Timer();
        AddChild(moveTimer);
        moveTimer.Timeout += OnMoveTimerTimeout;
        moveTimer.WaitTime = 1.0f; // 1 segundo entre movimientos
        moveTimer.Start();

        gridPosition = tileMapLayer1.LocalToMap(Position);
        GD.Print($"Player inicializado en celda: {gridPosition}");
    }

    private void OnMoveTimerTimeout()
    {
        if (player2 == null) return;

        // Obtener posición de player2 en celdas
        Vector2I targetGridPosition = tileMapLayer1.LocalToMap(player2.GlobalPosition);

        // Calcular dirección hacia player2
        Vector2I direction = CalculateDirection(gridPosition, targetGridPosition);

        // Intentar mover
        Vector2I target = gridPosition + direction;

        if (CanMoveTo(target))
        {
            gridPosition = target;
            Position = tileMapLayer1.MapToLocal(gridPosition);

            // Actualizar animación según dirección
            UpdateAnimation(direction);

            GD.Print($"Movido a celda: {gridPosition}");
        }
    }

    private Vector2I CalculateDirection(Vector2I current, Vector2I target)
    {
        Vector2I direction = Vector2I.Zero;

        // Priorizar movimiento horizontal o vertical según mayor distancia
        if (Mathf.Abs(target.X - current.X) > Mathf.Abs(target.Y - current.Y))
        {
            direction.X = target.X > current.X ? 1 : -1;
        }
        else
        {
            direction.Y = target.Y > current.Y ? 1 : -1;
        }

        return direction;
    }

    private void UpdateAnimation(Vector2I direction)
    {
        if (animatedSprite == null) return;

        if (direction.X < 0) animatedSprite.Play("boar_NW_RUN");
        else if (direction.X > 0) animatedSprite.Play("boar_SE_RUN");
        else if (direction.Y < 0) animatedSprite.Play("boar_NE_RUN");
        else if (direction.Y > 0) animatedSprite.Play("boar_SW_RUN");
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