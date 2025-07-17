
/* Jenova C++ Node Base Script (Meteora) */

// Godot SDK
#include <Godot/godot.hpp>
#include <Godot/classes/node.hpp>
#include <Godot/variant/variant.hpp>
#include <Godot/classes/tile_map_layer.hpp>
// Jenova SDK
#include <JenovaSDK.h>

// Namespaces
using namespace godot;
using namespace jenova::sdk;

// Jenova Script Block Start
JENOVA_SCRIPT_BEGIN
TileMapLayer* tile_map_layer = nullptr;
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

}

// Called On Every Frame
void OnProcess(Caller* instance, double _delta)
{
    // Obtener el nodo TileMapLayer exportado


 
    // Inicializar AnimatedSprite2D con la animación BOAR_NW_IDLE
    Node* node = GetSelf<Node>(instance);
    //tile_map_layer = Object::cast_to<TileMapLayer>(node->find_child("Layer1"));
    auto tile_map_layer = (TileMapLayer*)instance->self;

    if (!tile_map_layer) {
        Output("ERROR: TileMapLayer no asignado en el Inspector.");
        return;
    }

    Array used_cells = tile_map_layer->get_used_cells();

    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            Vector2i target_tile(x, y);

            if (used_cells.has(target_tile)) {
                Output("Celda en x:%lld y:%lld EXISTE",x,y);
            }
            else {
                Output("Celda en x:%lld y:%lld NO EXISTE", x, y);
            }
        }
    }


}

// Jenova Script Block End
JENOVA_SCRIPT_END