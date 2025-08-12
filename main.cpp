

/* Jenova C++ Node Base Script (Meteora) - Versión Corregida */

#include <Godot/godot.hpp>
#include <Godot/classes/node.hpp>
#include <Godot/classes/node2d.hpp>
#include <Godot/variant/variant.hpp>
#include <Godot/classes/animated_sprite2d.hpp>
#include <Godot/classes/tile_map_layer.hpp>
#include <Godot/classes/resource_loader.hpp>
#include <Godot/classes/packed_scene.hpp>
#include <JenovaSDK.h>

using namespace godot;
using namespace jenova::sdk;

JENOVA_SCRIPT_BEGIN
// Variables miembro
Node2D* _currentLevel = nullptr;
TileMapLayer* _tileMapLayer = nullptr;
Node* _player = nullptr;
int _currentLevelIndex = 0;

Vector<String> _levelPaths;
 Vector<Vector2i> _levelChangePositions;
 Vector<Vector2i> _playerSpawnCells;


// Declaraciones de funciones (AÑADIDO)
void InitializeLevelComponents(Caller* instance);
void LoadLevel(const String& levelPath, Caller* instance);
void ChangeToNextLevel(Caller* instance);  // <- DECLARACIÓN AÑADIDA

void OnAwake(Caller* instance) {

    // Inicializar el diccionario de posiciones
    _levelChangePositions.push_back(Vector2i(5, -2));  // Cambio desde nivel1
    _levelChangePositions.push_back(Vector2i(21, -7)); // Cambio desde nivel2
    // Celdas donde spawnea el jugador por nivel
    _playerSpawnCells.push_back(Vector2i(0, 7));   // Spawn en nivel1
    _playerSpawnCells.push_back(Vector2i(11, 7));  // Spawn en nivel2
}

void OnDestroy(Caller* instance) {
    _currentLevel = nullptr;
    _player = nullptr;
    _tileMapLayer = nullptr;
}

void OnReady(Caller* instance) {
    //     // Obtener el ResourceLoader
    _levelPaths.push_back("res://niveles/nivel1.tscn");
    _levelPaths.push_back("res://niveles/nivel2.tscn");
    LoadLevel(_levelPaths[_currentLevelIndex], instance);
}

void OnProcess(Caller* instance, double _delta) {
    if (!_player || !_tileMapLayer) return;

    Output("Player health: %i ", (int)_player->get_meta("current_health", -1));

    Vector2i currentCell = _tileMapLayer->local_to_map(Object::cast_to<Node2D>(_player)->get_global_position());
    if (currentCell == _levelChangePositions[_currentLevelIndex]) {
        ChangeToNextLevel(instance);  // Ahora debería reconocerse
    }
}

void LoadLevel(const String& levelPath, Caller* instance) {
    int savedHealth = _player ? (int)_player->get_meta("current_health", 4) : 4;

    if (_currentLevel) {
        _currentLevel->queue_free();
        _currentLevel = nullptr;
    }

    // CORRECCIÓN: Usar instance() en lugar de instantiate()
    Ref<PackedScene> nivelScene = ResourceLoader::get_singleton()->load(levelPath);
    if (nivelScene.is_valid()) {
        _currentLevel = Object::cast_to<Node2D>(nivelScene->instantiate());  // <- CAMBIO AQUÍ
        GetSelf<Node>(instance)->add_child(_currentLevel);
    }
    else {
        Output("Error al cargar escena: ", levelPath);
        return;
    }

    InitializeLevelComponents(instance);

    if (_player) {
        _player->set_meta("current_health", savedHealth);
        _player->call("UpdateHealthBar");
    }
}

// Implementación de ChangeToNextLevel (AÑADIDO)
void ChangeToNextLevel(Caller* instance) {
    _currentLevelIndex = (_currentLevelIndex + 1) % _levelPaths.size();
    LoadLevel(_levelPaths[_currentLevelIndex], instance);
    Output("Cambiando a nivel: ", _currentLevelIndex + 1);
}

void InitializeLevelComponents(Caller* instance) {
    _player = _currentLevel->get_node_or_null("player2");
    _tileMapLayer = Object::cast_to<TileMapLayer>(_currentLevel->get_node_or_null("Layer1"));

    if (!_player) {
        Output("ERROR: Player no encontrado");
        return;
    }

    _player->set_meta("current_health", _player->get_meta("current_health", _player->call("get_max_health")));

    if (_tileMapLayer && _currentLevelIndex >= 0 && _currentLevelIndex < _playerSpawnCells.size()) {
        _player->call("SetGridPositionExternamente", _playerSpawnCells[_currentLevelIndex]);
    }
}
JENOVA_SCRIPT_END