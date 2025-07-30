
 /* Jenova C++ Node Base Script (Meteora) */

 // Godot SDK
#include <Godot/godot.hpp>
#include <Godot/classes/node.hpp>
#include <Godot/classes/node2d.hpp>
#include <Godot/variant/variant.hpp>
#include <Godot/classes/animated_sprite2d.hpp>
#include <Godot/classes/animatable_body2d.hpp>
#include <Godot/classes/input.hpp>
#include <Godot/classes/tile_map_layer.hpp>
#include <Godot/classes/resource_loader.hpp>
#include <Godot/classes/packed_scene.hpp>
 // Jenova SDK
 #include <JenovaSDK.h>

 // Namespaces
 using namespace godot;
 using namespace jenova::sdk;

 // Jenova Script Block Start
 JENOVA_SCRIPT_BEGIN
 void InitializeLevelComponents(Caller* instance);
 void LoadLevel(const String& levelPath, Caller* instance);
 void CheckLevelChangeTrigger(Caller* instance);
 void ChangeToNextLevel(Caller* instance);
 Node2D* _currentLevel = nullptr;
 Node2D* levelScene = nullptr;
 TileMapLayer* _tileMapLayer = nullptr;
 Node* _player = nullptr;
 Vector<String> _levelPaths;
 int _currentLevelIndex = 0;
 Vector<Vector2i> _levelChangePositions;
 Vector<Vector2i> _playerSpawnCells;
 // Called When Node Enters Scene Tree
 void OnAwake(Caller* instance)
 {
    // Inicializar el diccionario de posiciones
    _levelChangePositions.push_back(Vector2i(5, -2));  // Cambio desde nivel1
    _levelChangePositions.push_back(Vector2i(21, -7)); // Cambio desde nivel2
    // Celdas donde spawnea el jugador por nivel
    _playerSpawnCells.push_back(Vector2i(0, 7));   // Spawn en nivel1
    _playerSpawnCells.push_back(Vector2i(11, 7));  // Spawn en nivel2
 }
 // Called When Node Exits Scene Tree
 void OnDestroy(Caller* instance)
 {
    // Limpiar referencias
    _currentLevel = nullptr;
    _player = nullptr;
    _tileMapLayer = nullptr;
 }


 void OnReady(Caller* instance)
 {
     // Obtener el ResourceLoader
     _levelPaths.push_back("res://niveles/nivel1.tscn");
     _levelPaths.push_back("res://niveles/nivel2.tscn");

     Output("chivato antes de entrar al loadlevel");
     LoadLevel(_levelPaths[_currentLevelIndex], instance);
 }
 // Called On Every Frame
 void OnProcess(Caller* instance, double _delta)
 {
    if (_player == nullptr || _tileMapLayer == nullptr) return;
    CheckLevelChangeTrigger(instance);
 }
 void LoadLevel(const String& levelPath, Caller* instance)
{
    // Limpiar nivel anterior
    if (_currentLevel != nullptr)
    {
        Output("chivato limpiando currentlevel");
        _currentLevel->queue_free();
        _currentLevel = nullptr;
    }
    _player = nullptr;
    //_tileMapLayer = nullptr;

    // Cargar nuevo nivel
    Output("chivato al cargar nivel currentlevel");
    ResourceLoader* loader = ResourceLoader::get_singleton();
    //Ref<PackedScene> levelScene = ResourceLoader::get_singleton()->load(levelPath);
    Ref<PackedScene> nivelScene = loader->load(levelPath);
    _currentLevel = Object::cast_to<Node2D>(nivelScene->instantiate());
    if (!_currentLevel)
    {
        Output("Error al cargar el nivel: %s", GetCStr(levelPath));
        return;
    }
    //// Añadir al árbol si es necesario
    Node* self = GetSelf<Node>(instance);
    self->add_child(_currentLevel);


    //self->add_child(_currentLevel);
    InitializeLevelComponents(instance);
}

 void InitializeLevelComponents(Caller* instance)
 {
    // Obtener el jugador y la capa de tiles
    Node* self = GetSelf<Node>(instance);
    _player = _currentLevel->get_node_or_null("Player2");
    if (_player == nullptr)
    {
        _player = _currentLevel->get_node_or_null("player2");
    }

    _tileMapLayer = Object::cast_to<TileMapLayer>(_currentLevel->get_node_or_null("Layer1"));

    if (_player == nullptr) Output("No se encontró el jugador");
    if (_tileMapLayer == nullptr) Output("No se encontró TileMapLayer");

    // Establecer posición del jugador
    if (_player != nullptr && _tileMapLayer != nullptr)
    {
        if (_currentLevelIndex >= 0 && _currentLevelIndex < _playerSpawnCells.size())
        {
            Vector2i spawnCell = _playerSpawnCells[_currentLevelIndex];
            Vector2 spawnPos = _tileMapLayer->map_to_local(spawnCell);
            Vector2i posicionInicial = _playerSpawnCells[_currentLevelIndex];
            _player->call("SetGridPositionExternamente", posicionInicial);

        }
    }
 }
 void CheckLevelChangeTrigger(Caller* instance)
{
    // Obtener posición actual del jugador en celdas
    Vector2 playerPos = Object::cast_to<Node2D>(_player)->get_global_position();
    Vector2i currentCell = _tileMapLayer->local_to_map(playerPos);

    // Verificar si está en la posición de cambio de nivel
    Variant triggerCellVariant = _levelChangePositions.get(_currentLevelIndex);
    if (triggerCellVariant.get_type() != Variant::NIL)
    {
        Vector2i triggerCell = (Vector2i)triggerCellVariant;
        if (currentCell == triggerCell)
        {
            ChangeToNextLevel(instance);
            
        }
    }
}
 void ChangeToNextLevel(Caller* instance)
{
    _currentLevelIndex = (_currentLevelIndex + 1) % 2; // 2 es el tamaño del array _levelPaths
    LoadLevel(_levelPaths[_currentLevelIndex],instance);
    Output("Cambiando a nivel ", _currentLevelIndex + 1);
}

 // Jenova Script Block End
 JENOVA_SCRIPT_END
