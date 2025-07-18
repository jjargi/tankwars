
using Godot;
using System.Collections.Generic;

public partial class Main : Node2D
{
    // Configuración de niveles (aunque ahora se posiciona manualmente)
    private readonly string[] _levelPaths = {
        "res://niveles/nivel1.tscn",
        "res://niveles/nivel2.tscn"
    };

    private int _currentLevelIndex = 0;
    private Node2D _currentLevel;

    public override void _Ready()
    {
        LoadLevel(_levelPaths[_currentLevelIndex]);
    }

    public void LoadLevel(string levelPath)
    {
        // Limpiar nivel anterior
        _currentLevel?.QueueFree();

        // Cargar nuevo nivel
        var levelScene = GD.Load<PackedScene>(levelPath);
        if (levelScene == null)
        {
            GD.PushError($"Error al cargar el nivel: {levelPath}");
            return;
        }

        _currentLevel = levelScene.Instantiate<Node2D>();
        AddChild(_currentLevel);

        // El player ya está posicionado manualmente en la escena
        GD.Print($"Nivel cargado: {levelPath}");
    }

    // Método para cambiar de nivel
    public void ChangeToNextLevel()
    {
        _currentLevelIndex = (_currentLevelIndex + 1) % _levelPaths.Length;
        LoadLevel(_levelPaths[_currentLevelIndex]);
    }

    public override void _Process(double delta)
    {
        // Ejemplo: Cambiar de nivel con tecla N
        if (Input.IsActionJustPressed("next"))
        {
            ChangeToNextLevel();
        }
    }
}