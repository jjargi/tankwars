//using Godot;
//using System;

//public partial class PlayerController : CharacterBody2D
//{
//	public const float Speed = 300.0f;
//	public const float JumpVelocity = -400.0f;

//	public override void _PhysicsProcess(double delta)
//	{
//		Vector2 velocity = Velocity;

//		// Add the gravity.
//		if (!IsOnFloor())
//		{
//			velocity += GetGravity() * (float)delta;
//		}

//		// Handle Jump.
//		if (Input.IsActionJustPressed("ui_accept") && IsOnFloor())
//		{
//			velocity.Y = JumpVelocity;
//		}

//		// Get the input direction and handle the movement/deceleration.
//		// As good practice, you should replace UI actions with custom gameplay actions.
//		Vector2 direction = Input.GetVector("ui_left", "ui_right", "ui_up", "ui_down");
//		if (direction != Vector2.Zero)
//		{
//			velocity.X = direction.X * Speed;
//		}
//		else
//		{
//			velocity.X = Mathf.MoveToward(Velocity.X, 0, Speed);
//		}

//		Velocity = velocity;
//		MoveAndSlide();
//	}
//}

using Godot;
using System;
using System.Collections.Generic;

public partial class PlayerController : Node2D
{
    private AnimatedSprite2D animatedSprite;
    private TileMapLayer tileMapLayer1;
    private TileMapLayer tileMapLayer2;
    private TileMapLayer tileMapLayer3;

    private Vector2I gridPosition = new Vector2I(0, 0);

    private enum Direction { Down, Up, Left, Right }
    private Direction lastDirection = Direction.Down;

    public override void _Ready()
    {
        animatedSprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");

        if (animatedSprite != null)
        {
            animatedSprite.Animation = "idle_down";
            animatedSprite.Play();
        }

        var parent = GetParent();
        tileMapLayer1 = parent.GetNode<TileMapLayer>("Layer1");
        tileMapLayer2 = parent.GetNode<TileMapLayer>("Layer2");
        tileMapLayer3 = parent.GetNode<TileMapLayer>("Layer3");

        SetPlayerPosition(gridPosition);
    }

    public override void _Process(double delta)
    {
        var input = Input.Singleton;
        Vector2I direction = new Vector2I();

        if (input.IsActionJustPressed("up"))
        {
            direction = new Vector2I(-1, 0);
            animatedSprite.Play("run_up");
            lastDirection = Direction.Up;
        }
        else if (input.IsActionJustPressed("right"))
        {
            direction = new Vector2I(0, -1);
            animatedSprite.Play("run_right");
            lastDirection = Direction.Right;
        }
        else if (input.IsActionJustPressed("down"))
        {
            direction = new Vector2I(1, 0);
            animatedSprite.Play("run_down");
            lastDirection = Direction.Down;
        }
        else if (input.IsActionJustPressed("left"))
        {
            direction = new Vector2I(0, 1);
            animatedSprite.Play("run_left");
            lastDirection = Direction.Left;
        }

        if (direction != Vector2I.Zero)
        {
            Vector2I target = gridPosition + direction;
            if (CanMoveTo(target))
            {
                gridPosition = target;
                SetPlayerPosition(gridPosition);
            }
        }

        if (input.IsActionJustPressed("attack"))
        {
            switch (lastDirection)
            {
                default:
                    GD.Print("no reproduce?!!!");
                    return;
                case Direction.Up: animatedSprite.Play("attack_up"); break;
                case Direction.Right: animatedSprite.Play("attack_right"); break;
                case Direction.Down: animatedSprite.Play("attack_down"); break;
                case Direction.Left: animatedSprite.Play("attack_left"); break;
            }

            AttackEnemy();
        }
    }

    private void SetPlayerPosition(Vector2I cellPos)
    {
        if (tileMapLayer1 == null) return;
        Vector2 worldPos = tileMapLayer1.MapToLocal(cellPos);
        Position = worldPos;
        GD.Print($"jugador posicionado correctamente en x: {Position.X} ,y: {Position.X}");
    }

    private bool CanMoveTo(Vector2I target)
    {
        if (tileMapLayer1 == null || tileMapLayer2 == null) return false;

        var usedCells = tileMapLayer1.GetUsedCells();
        if (!usedCells.Contains(target)) return false;

        var tileData = tileMapLayer2.GetCellTileData(target);
        if (tileData != null)
        {
            int sourceId = tileMapLayer2.GetCellSourceId(target);
            Vector2I atlasCoords = tileMapLayer2.GetCellAtlasCoords(target);

            if ((sourceId == 0 && atlasCoords == new Vector2I(10, 5)) ||
                (sourceId == 0 && atlasCoords == new Vector2I(4, 5)) ||
                (sourceId == 0 && atlasCoords == new Vector2I(0, 5)))
            {
                return false;
            }
        }

        return true;
    }

    private Node2D FindEnemyAtPosition(Vector2I cellPos)
    {
        var parent = GetParent();
        if (parent == null) return null;

        var enemies = GetTree().GetNodesInGroup("player");

        foreach (var obj in enemies)
        {
            if (obj is Node2D enemy && tileMapLayer1 != null)
            {
                Vector2I enemyPos = tileMapLayer1.LocalToMap(enemy.Position);
                if (enemyPos == cellPos)
                {
                    return enemy;
                }
            }
        }

        return null;
    }

    private void AttackEnemy()
    {
        Vector2I attackPos = gridPosition;

        switch (lastDirection)
        {
            case Direction.Up: attackPos += new Vector2I(-1, 0); break;
            case Direction.Right: attackPos += new Vector2I(0, -1); break;
            case Direction.Down: attackPos += new Vector2I(1, 0); break;
            case Direction.Left: attackPos += new Vector2I(0, 1); break;
        }

        var enemy = FindEnemyAtPosition(attackPos);
        if (enemy != null)
        {
            GD.Print($"ENEMIGO DETECTADO EN POSICION: {attackPos}");
            GD.Print("EJECUTANDO ANIMACION DE MUERTE");
            enemy.Call("PlayDeathAnimation");

        }
    }

    public void SetGridPositionExternally(Vector2I newGridPosition)
    {
        gridPosition = newGridPosition;
        SetPlayerPosition(gridPosition);
    }
    public void SetInitialGridPosition(Vector2I cellPos, TileMapLayer tileMap)
    {
        gridPosition = cellPos;
        tileMapLayer1 = tileMap;
        Position = tileMap.MapToLocal(cellPos);
        GD.Print($"jugador posicionado correctamente en x:{Position.X},y:{Position.X}");
    }
}
