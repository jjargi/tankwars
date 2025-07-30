
//JENOVA_SCRIPT_END
/* Jenova C++ Node Base Script (Meteora) */

// Godot SDK
#include <Godot/godot.hpp>
#include <Godot/classes/node.hpp>
#include <Godot/classes/node2d.hpp>
#include <Godot/variant/variant.hpp>
#include <Godot/classes/animated_sprite2d.hpp>
#include <Godot/classes/character_body2d.hpp>
#include <Godot/classes/input.hpp>
#include <Godot/classes/tile_map_layer.hpp>
#include <Godot/classes/timer.hpp>

// Jenova SDK
#include <JenovaSDK.h>

// Namespaces
using namespace godot;
using namespace jenova::sdk;

// Jenova Script Block Start
JENOVA_SCRIPT_BEGIN

// Variables miembro
AnimatedSprite2D* animatedSprite = nullptr;
TileMapLayer* tileMapLayer1 = nullptr;
TileMapLayer* tileMapLayer2 = nullptr;
TileMapLayer* tileMapLayer3 = nullptr;
Node2D* player2 = nullptr;
Timer* moveTimer = nullptr;
Vector2i gridPosition = Vector2i(0, 0);
bool initialized = false;

// Funciones auxiliares
bool can_move_to(const Vector2i& target) {
    // Verificar que la celda existe en layer1
    TypedArray<Vector2i> used_cells = tileMapLayer1->get_used_cells();
    if (!used_cells.has(target)) {
        return false;
    }

    // Verificar si layer2 tiene un tile en esa celda
    TileData* tile_data = tileMapLayer2->get_cell_tile_data(target);
    if (tile_data != nullptr) {
        int source_id = tileMapLayer2->get_cell_source_id(target);
        Vector2i atlas_coords = tileMapLayer2->get_cell_atlas_coords(target);

        // Si coincide con los valores bloqueados, no permitir movimiento
        if ((source_id == 0 && atlas_coords == Vector2i(10, 5)) ||
            (source_id == 0 && atlas_coords == Vector2i(4, 5))) {
            Output("Movimiento bloqueado por layer2 en celda ", target);
            return false;
        }
    }
    return true;
}

void set_player_position(Caller* instance, const Vector2i& cell_pos) {
    if (tileMapLayer1 == nullptr) {
        return;
    }

    Vector2 world_position = tileMapLayer1->map_to_local(cell_pos);

    Node2D* self2d = GetSelf<Node2D>(instance);
    if (self2d) {
        self2d->set_position(world_position);
    }
    Output("Jugador movido a celda: ", cell_pos, " -> posición global: ", world_position);
}

Vector2i calculate_direction(const Vector2i& current, const Vector2i& target) {
    Vector2i direction = Vector2i();
    Vector2i difference = target - current;

    // Priorizar movimiento horizontal o vertical según mayor distancia
    if (Math::abs(difference.x) > Math::abs(difference.y)) {
        direction.x = (target.x > current.x) ? 1 : -1;
    }
    else {
        direction.y = (target.y > current.y) ? 1 : -1;
    }

    return direction;
}

void update_animation(const Vector2i& direction) {
    if (animatedSprite == nullptr) return;

    if (direction.x < 0) animatedSprite->play("boar_NW_RUN");
    else if (direction.x > 0) animatedSprite->play("boar_SE_RUN");
    else if (direction.y < 0) animatedSprite->play("boar_NE_RUN");
    else if (direction.y > 0) animatedSprite->play("boar_SW_RUN");
}

void move_towards_player2(Caller* instance) {
    if (player2 == nullptr || tileMapLayer1 == nullptr) return;

    // Obtener posición actual de player2 en el grid
    Vector2i targetPosition = tileMapLayer1->local_to_map(player2->get_position());

    // Calcular dirección óptima
    Vector2i direction = calculate_direction(gridPosition, targetPosition);
    Vector2i newPosition = gridPosition + direction;

    // Verificar si puede moverse
    if (can_move_to(newPosition)) {
        gridPosition = newPosition;
        set_player_position(instance, gridPosition);
        update_animation(direction);
        Output("Movimiento a celda: ", gridPosition);
    }
}

// Called When Node Enters Scene Tree
void OnAwake(Caller* instance)
{
    // Crear e inicializar el Timer
    moveTimer = memnew(Timer);
    Node* self = GetSelf<Node>(instance);
    self->add_child(moveTimer);

    // Configurar el timer
    if (!moveTimer->is_connected("timeout", Callable(self, "_on_move_timeout")))
    {
        moveTimer->connect("timeout", Callable(self, "_on_move_timeout"));
    }
    moveTimer->set_wait_time(1.0);
    moveTimer->set_one_shot(false); // Para que se repita automáticamente
}

// Called When Node Exits Scene Tree
void OnDestroy(Caller* instance)
{
    if (moveTimer != nullptr) {
        moveTimer->queue_free();
        moveTimer = nullptr;
    }
}

// Called When Node and All It's Children Entered Scene Tree
void OnReady(Caller* instance)
{
    // Inicializar componentes
    Node* node = GetSelf<Node>(instance);
    animatedSprite = Object::cast_to<AnimatedSprite2D>(node->find_child("AnimatedSprite2D"));

    Node* parent = node->get_parent();
    tileMapLayer1 = Object::cast_to<TileMapLayer>(parent->find_child("Layer1"));
    tileMapLayer2 = Object::cast_to<TileMapLayer>(parent->find_child("Layer2"));
    tileMapLayer3 = Object::cast_to<TileMapLayer>(parent->find_child("Layer3"));
    player2 = Object::cast_to<Node2D>(parent->find_child("player2"));

    if (animatedSprite != nullptr) {
        animatedSprite->set_animation("BOAR_NW_IDLE");
        animatedSprite->play();
    }

    // Iniciar movimiento
    moveTimer->start();
}

// Called On Every Frame
void OnProcess(Caller* instance, double _delta)
{
    if (!initialized && tileMapLayer1 != nullptr) {
        // Inicializar posición basada en la posición manual
        Node2D* self2d = GetSelf<Node2D>(instance);
        gridPosition = tileMapLayer1->local_to_map(self2d->get_position());
        Output("Player inicializado en posición manual: celda ", gridPosition);
        initialized = true;
    }
}

// Método para manejar el timeout del timer
void _on_move_timeout(Caller* instance)
{
    move_towards_player2(instance);
}

// Jenova Script Block End
JENOVA_SCRIPT_END

