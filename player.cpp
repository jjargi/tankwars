
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
// Jenova SDK
#include <JenovaSDK.h>

// Namespaces
using namespace godot;
using namespace jenova::sdk;

// Jenova Script Block Start
JENOVA_SCRIPT_BEGIN
AnimatedSprite2D* animatedSprite = nullptr;
TileMapLayer* tileMapLayer1 = nullptr;
TileMapLayer* tileMapLayer2 = nullptr;
TileMapLayer* tileMapLayer3 = nullptr;
Vector2i gridPosition = Vector2i(0, 0);

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
            (source_id == 0 && atlas_coords == Vector2i(4, 5))||
            (source_id == 0 && atlas_coords == Vector2i(0, 5))){
            Output("Movimiento bloqueado por layer2 en celda ", target);
            return false;
        }
    }
    Output("Movimiento NO bloqueado por layer2 en celda");
    return true;
}
//void set_player_position(const Vector2i& cell_pos) {
void set_player_position(Caller* instance, const Vector2i& cell_pos){
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
// Called When Node Enters Scene Tree
void OnAwake(Caller* instance)
{

}

// Called When Node Exits Scene Tree
void OnDestroy(Caller* instance)
{

}

// Called When Node and All It's Children Entered Scene Tree
void OnReady(Caller* instance)
{
    // Inicializar AnimatedSprite2D con la animación BOAR_NW_IDLE
     Node* node = GetSelf<Node>(instance);
     animatedSprite = Object::cast_to<AnimatedSprite2D>(node->find_child("AnimatedSprite2D"));
     animatedSprite->set_animation("BOAR_NW_IDLE");
     animatedSprite->play("BOAR_NW_IDLE");
     Node* parent = node->get_parent();
     tileMapLayer1 = Object::cast_to<TileMapLayer>(parent->find_child("Layer1"));
     tileMapLayer2 = Object::cast_to<TileMapLayer>(parent->find_child("Layer2"));
     tileMapLayer3 = Object::cast_to<TileMapLayer>(parent->find_child("Layer3"));
    if(!tileMapLayer1 || !tileMapLayer2 || !tileMapLayer3) {
        Output("ERROR: No se encontraron las capas de TileMapLayer en el Inspector.");
        return;
    }
    if(!parent) {
        Output("ERROR: No se encontró el padre del nodo.");
        return;
    }

     set_player_position(instance, gridPosition);

     Output("DEBUG: AnimatedSprite2D inicializado");
}

// Called On Every Frame
void OnProcess(Caller* instance, double _delta)
{
    bool moved = false;
    Vector2i direction = Vector2i();

    Input* input = Input::get_singleton();

    if (input->is_action_just_pressed("up")) {
        direction = Vector2i(-1, 0);
        animatedSprite->play("boar_NW_RUN");
    }
    else if (input->is_action_just_pressed("right")) {
        direction = Vector2i(0, -1);
        animatedSprite->play("boar_NE_RUN");
    }
    else if (input->is_action_just_pressed("down")) {
        direction = Vector2i(1, 0);
        animatedSprite->play("boar_SE_RUN");
    }
    else if (input->is_action_just_pressed("left")) {
        direction = Vector2i(0, 1);
        animatedSprite->play("boar_SW_RUN");
    }

    if (direction != Vector2i()) {
        Vector2i target = gridPosition + direction;

        if (can_move_to(target)) {
            gridPosition = target;
            set_player_position(instance, gridPosition);
            moved = true;
            Output("DEBUG: AnimatedSprite2D inicializado en celdax:%lfceldax:celday: %lf", (double)gridPosition.x, (double)gridPosition.y);
        }
        else {
            Output("Movimiento bloqueado: no hay celda en ", target);
        }
    }


}
void SetGridPositionExternamente(Caller* instance, const Vector2i& newGridPosition) {
    gridPosition = newGridPosition;
    set_player_position(instance, gridPosition);
}
// Jenova Script Block End
JENOVA_SCRIPT_END