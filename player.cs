using Godot;
using System;

public partial class player : Node
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
{
	// Inicializar AnimatedSprite2D con la animación BOAR_NW_IDLE
	var animatedSprite = GetNode<AnimatedSprite2D>("AnimatedSprite2D");
	if (animatedSprite != null)
	{
		animatedSprite.Animation = "BOAR_NW_IDLE";
		animatedSprite.Play();
	}
}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}
}
