using Godot;
using System.Collections.Generic;

public partial class Main : Node2D
{
    private readonly string[] _levelPaths = {
        "res://niveles/nivel1.tscn",
        "res://niveles/nivel2.tscn"
    };

    private int _currentLevelIndex = 0;
    private Node2D _currentLevel;
    private PlayerController  _player;
    private TileMapLayer _tileMapLayer;

    // Posiciones que activan el cambio de nivel (nivelIndex, posición)
    private readonly Dictionary<int, Vector2I> _levelChangePositions = new()
    {
        {0, new Vector2I(5, -2)},  // Cambio desde nivel1 (celda 10,5)
        {1, new Vector2I(21, -7)}    // Cambio desde nivel2 (celda 3,8)
    };
    // Posiciones donde aparece el jugador por nivel
    private readonly Dictionary<int, Vector2I> _playerSpawnCells = new()
{
    {0, new Vector2I(0, 7)},
    {1, new Vector2I(11, 7)}
};
    public override void _Ready()
    {
        LoadLevel(_levelPaths[_currentLevelIndex]);
    }

    public void LoadLevel(string levelPath)
    {
        _currentLevel?.QueueFree();
        _player = null;
        _tileMapLayer = null;

        var levelScene = GD.Load<PackedScene>(levelPath);
        if (levelScene == null)
        {
            GD.PushError($"Error al cargar el nivel: {levelPath}");
            return;
        }

        _currentLevel = levelScene.Instantiate<Node2D>();
        AddChild(_currentLevel);

        InitializeLevelComponents();
    }


    //private void InitializeLevelComponents()
    //{
    //    _player = _currentLevel.GetNodeOrNull<PlayerController>("Player2") ??
    //              _currentLevel.GetNodeOrNull<PlayerController>("player2");

    //    _tileMapLayer = _currentLevel.GetNodeOrNull<TileMapLayer>("Layer1");

    //    if (_player == null) GD.PushError("No se encontró el jugador");
    //    if (_tileMapLayer == null) GD.PushError("No se encontró TileMapLayer");

    //    // Posición inicial deseada (por ejemplo, celda 0,0)
    //    Vector2I initialCell = new Vector2I(0, 0);

    //    // Posicionar al jugador correctamente
    //    //_player.SetInitialGridPosition(initialCell, _tileMapLayer);
    //    _player.SetGridPositionExternally(initialCell);
    //}
    //private void InitializeLevelComponents()
    //{
    //    _player = _currentLevel.GetNodeOrNull<PlayerController>("Player2") ??
    //              _currentLevel.GetNodeOrNull<PlayerController>("player2");

    //    _tileMapLayer = _currentLevel.GetNodeOrNull<TileMapLayer>("Layer1");

    //    if (_player == null) GD.PushError("No se encontró el jugador");
    //    if (_tileMapLayer == null) GD.PushError("No se encontró TileMapLayer");

    //    // Obtener la posición inicial desde el diccionario
    //    if (_levelChangePositions.TryGetValue(_currentLevelIndex, out Vector2I initialCell))
    //    {
    //        _player.SetGridPositionExternally(initialCell);
    //    }
    //    else
    //    {
    //        GD.PushWarning($"No se definió posición inicial para el nivel {_currentLevelIndex}");
    //    }
    //}
    private void InitializeLevelComponents()
    {
        _player = _currentLevel.GetNodeOrNull<PlayerController>("Player2") ??
                  _currentLevel.GetNodeOrNull<PlayerController>("player2");

        _tileMapLayer = _currentLevel.GetNodeOrNull<TileMapLayer>("Layer1");

        if (_player == null) GD.PushError("No se encontró el jugador");
        if (_tileMapLayer == null) GD.PushError("No se encontró TileMapLayer");

        if (_playerSpawnCells.TryGetValue(_currentLevelIndex, out Vector2I spawnCell))
        {
            Vector2 spawnPos = _tileMapLayer.MapToLocal(spawnCell);
            _player.SetGridPositionExternally(spawnCell);
        }
        else
        {
            GD.PushWarning($"No se definió posición de spawn para el nivel {_currentLevelIndex}");
        }
    }

    public override void _Process(double delta)
    {
        if (_player == null || _tileMapLayer == null) return;

        CheckLevelChangeTrigger();
    }

    private void CheckLevelChangeTrigger()
    {
        // Obtener posición actual del jugador en celdas
        Vector2I currentCell = _tileMapLayer.LocalToMap(_player.GlobalPosition);

        // Verificar si está en la posición de cambio de nivel
        if (_levelChangePositions.TryGetValue(_currentLevelIndex, out Vector2I triggerCell))
        {
            if (currentCell == triggerCell)
            {
                ChangeToNextLevel();
            }
        }
    }

    public void ChangeToNextLevel()
    {
        _currentLevelIndex = (_currentLevelIndex + 1) % _levelPaths.Length;
        LoadLevel(_levelPaths[_currentLevelIndex]);
        GD.Print($"Cambiando a nivel {_currentLevelIndex + 1}");
    }
}