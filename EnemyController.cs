
using Godot;
using System;

public partial class EnemyController : Node2D
{
    private Timer moveTimer;
    private Vector2I gridPosition = new Vector2I(0, 0);
    private bool initialized = false;
    private bool isDying = false;

    private AnimatedSprite2D sprite;
    private TileMapLayer layer1;
    private TileMapLayer layer2;
    private Node2D player;

    public override void _Ready()
    {
        sprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");
        if (sprite != null)
        {
            sprite.Animation = "BOAR_NW_IDLE";
            sprite.Play();

        }

        layer1 = GetParent().GetNode<TileMapLayer>("Layer1");
        layer2 = GetParent().GetNode<TileMapLayer>("Layer2");
        player = GetParent().GetNode<Node2D>("player2");

        moveTimer = new Timer();
        AddChild(moveTimer);
        moveTimer.WaitTime = 1.0;
        moveTimer.Timeout += OnMoveTimeout;
        moveTimer.Start();
    }

    public override void _Process(double delta)
    {
        if (!initialized && layer1 != null)
        {
            gridPosition = layer1.LocalToMap(Position);
            initialized = true;
        }

    }

    private void OnMoveTimeout()
    {
        if (layer1 == null || layer2 == null || player == null || sprite == null) return;

        Vector2I target = layer1.LocalToMap(player.Position);
        Vector2I diff = target - gridPosition;
        Vector2I dir;

        if (Mathf.Abs(diff.X) > Mathf.Abs(diff.Y))
            dir = new Vector2I(diff.X > 0 ? 1 : -1, 0);
        else
            dir = new Vector2I(0, diff.Y > 0 ? 1 : -1);

        Vector2I newPos = gridPosition + dir;

        if (CanMove(newPos))
        {
            gridPosition = newPos;
            Position = layer1.MapToLocal(newPos);

            if (dir.X < 0) sprite.Play("boar_NW_RUN");
            else if (dir.X > 0) sprite.Play("boar_SE_RUN");
            else if (dir.Y < 0) sprite.Play("boar_NE_RUN");
            else if (dir.Y > 0) sprite.Play("boar_SW_RUN");
        }
    }

    private bool CanMove(Vector2I target)
    {
        if (layer1 == null || layer2 == null) return false;

        var usedCells = layer1.GetUsedCells();
        if (!usedCells.Contains(target)) return false;

        var tileData = layer2.GetCellTileData(target);
        if (tileData != null)
        {
            int sourceId = layer2.GetCellSourceId(target);
            Vector2I atlasCoords = layer2.GetCellAtlasCoords(target);

            if ((sourceId == 0 && atlasCoords == new Vector2I(10, 5)) ||
                (sourceId == 0 && atlasCoords == new Vector2I(4, 5)))
            {
                return false;
            }
        }

        return true;
    }

    public void PlayDeathAnimation()
    {
        if (isDying) return;

        isDying = true;
        GD.Print("Iniciando animación de muerte");

        if (sprite == null)
        {
            GD.PrintErr("Error: Sprite no encontrado");
            QueueFree();
            return;
        }

        // 1. Conectar el evento
        sprite.AnimationFinished += OnDeathAnimationFinished;

        // 2. Detener todos los procesos
        moveTimer.Stop();
        // 3. Forzar reinicio y reproducción
        sprite.Stop();
        sprite.Play("death_animation");

        GD.Print("Señal animation_finished conectada correctamente");
    }


    private void OnDeathAnimationFinished()
    {
        GD.Print("Animación de muerte completada - Entrando en callback");
        QueueFree();

        // Limpiar la suscripción
        if (sprite != null)
        {
            sprite.AnimationFinished -= OnDeathAnimationFinished;
            GD.Print("Señal desconectada correctamente");
        }
    }

    public override void _ExitTree()
    {
        // Limpieza final garantizada
        if (sprite != null)
        {
            sprite.AnimationFinished -= OnDeathAnimationFinished;
        }
    }
}
