
/* Jenova C++ Node Base Script (Meteora) */

#include <Godot/godot.hpp>
#include <Godot/classes/node.hpp>
#include <Godot/classes/node2d.hpp>
#include <Godot/variant/variant.hpp>
#include <Godot/classes/animated_sprite2d.hpp>
#include <Godot/classes/character_body2d.hpp>
#include <Godot/classes/input.hpp>
#include <Godot/classes/tile_map_layer.hpp>
#include <Godot/classes/sprite_frames.hpp>
#include <JenovaSDK.h>

using namespace godot;
using namespace jenova::sdk;

// Enumerador para direcciones
enum Direction { DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT };

JENOVA_SCRIPT_BEGIN

// Variables globales
AnimatedSprite2D* animatedSprite = nullptr;
TileMapLayer* tileMapLayer1 = nullptr;
TileMapLayer* tileMapLayer2 = nullptr;
TileMapLayer* tileMapLayer3 = nullptr;
Vector2i gridPosition = Vector2i(0, 0);
Direction last_direction = DIR_DOWN;  // Usamos el enumerador

// Función para buscar enemigos en una celda específica
Node* find_enemy_at_position(Caller* instance, const Vector2i& cell_pos) {
    Node* parent = GetSelf<Node>(instance)->get_parent();
    if (!parent) return nullptr;

    // Buscar todos los nodos con grupo "enemy"
    Array enemies = parent->get_tree()->get_nodes_in_group("player");

    for (int i = 0; i < enemies.size(); i++) {
        Node2D* enemy = Object::cast_to<Node2D>(enemies[i]);
        if (enemy && tileMapLayer1) {
            Vector2i enemy_pos = tileMapLayer1->local_to_map(enemy->get_position());
            if (enemy_pos == cell_pos) {
                return enemy;
            }
        }
    }
    return nullptr;
}
//void attack_enemy(Caller* instance) {
//    Vector2i attack_position = gridPosition;
//
//    // Calcular dirección de ataque
//    switch (last_direction) {
//    case DIR_UP: attack_position += Vector2i(-1, 0); break;
//    case DIR_RIGHT: attack_position += Vector2i(0, -1); break;
//    case DIR_DOWN: attack_position += Vector2i(1, 0); break;
//    case DIR_LEFT: attack_position += Vector2i(0, 1); break;
//    }
//
//    Node* enemy = find_enemy_at_position(instance, attack_position);
//    if (enemy) {
//        Output("ENEMIGO DETECTADO EN POSICION: ", attack_position);
//
//        // Verificar si el enemigo está vivo
//        if (enemy->has_method("play_death_animation") && !Object::cast_to<Node>(enemy)->get_meta("is_dying", false)) {
//            Output("EJECUTANDO ANIMACION DE MUERTE");
//            enemy->call("play_death_animation");
//            // Marcar como que está muriendo
//            Object::cast_to<Node>(enemy)->set_meta("is_dying", true);
//        }
//    }
//}
void attack_enemy(Caller* instance) {
    Vector2i attack_position = gridPosition;

    // Calcular dirección de ataque
    switch (last_direction) {
    case DIR_UP: attack_position += Vector2i(-1, 0); break;
    case DIR_RIGHT: attack_position += Vector2i(0, -1); break;
    case DIR_DOWN: attack_position += Vector2i(1, 0); break;
    case DIR_LEFT: attack_position += Vector2i(0, 1); break;
    }

    Node* enemy = find_enemy_at_position(instance, attack_position);
    if (enemy) {
        Output("ENEMIGO DETECTADO EN POSICION: ", attack_position);

        // Verificar si el enemigo puede morir
        //if (enemy->has_method("play_death_animation")) {
            bool is_dying = enemy->get_meta("is_dying", false);
            if (!is_dying) {
                Output("EJECUTANDO ANIMACION DE MUERTE");
                enemy->call("play_death_animation");
            }
        //}
  
    }
}
bool can_move_to(const Vector2i& target) {
    if (!tileMapLayer1 || !tileMapLayer2) return false;

    TypedArray<Vector2i> used_cells = tileMapLayer1->get_used_cells();
    if (!used_cells.has(target)) return false;

    TileData* tile_data = tileMapLayer2->get_cell_tile_data(target);
    if (tile_data) {
        int source_id = tileMapLayer2->get_cell_source_id(target);
        Vector2i atlas_coords = tileMapLayer2->get_cell_atlas_coords(target);
        if ((source_id == 0 && atlas_coords == Vector2i(10, 5)) ||
            (source_id == 0 && atlas_coords == Vector2i(4, 5)) ||
            (source_id == 0 && atlas_coords == Vector2i(0, 5))) {
            return false;
        }
    }
    return true;
}

void set_player_position(Caller* instance, const Vector2i& cell_pos) {
    if (!tileMapLayer1) return;
    Vector2 world_position = tileMapLayer1->map_to_local(cell_pos);
    Node2D* self2d = GetSelf<Node2D>(instance);
    if (self2d) self2d->set_position(world_position);
}

void OnReady(Caller* instance) {
    Node* node = GetSelf<Node>(instance);
    animatedSprite = Object::cast_to<AnimatedSprite2D>(node->find_child("AnimatedSprite2D"));

    if (animatedSprite) {
        animatedSprite->set_animation("idle_down");
        animatedSprite->play();
    }

    Node* parent = node->get_parent();
    tileMapLayer1 = Object::cast_to<TileMapLayer>(parent->find_child("Layer1"));
    tileMapLayer2 = Object::cast_to<TileMapLayer>(parent->find_child("Layer2"));
    tileMapLayer3 = Object::cast_to<TileMapLayer>(parent->find_child("Layer3"));

    set_player_position(instance, gridPosition);
}

void OnProcess(Caller* instance, double _delta) {
    Input* input = Input::get_singleton();
    Vector2i direction = Vector2i();

    if (input->is_action_just_pressed("up")) {
        direction = Vector2i(-1, 0);
        animatedSprite->play("run_up");
        last_direction = DIR_UP;
    }
    else if (input->is_action_just_pressed("right")) {
        direction = Vector2i(0, -1);
        animatedSprite->play("run_right");
        last_direction = DIR_RIGHT;
    }
    else if (input->is_action_just_pressed("down")) {
        direction = Vector2i(1, 0);
        animatedSprite->play("run_down");
        last_direction = DIR_DOWN;
    }
    else if (input->is_action_just_pressed("left")) {
        direction = Vector2i(0, 1);
        animatedSprite->play("run_left");
        last_direction = DIR_LEFT;
    }

    if (direction != Vector2i()) {
        Vector2i target = gridPosition + direction;
        if (can_move_to(target)) {
            gridPosition = target;
            set_player_position(instance, gridPosition);
        }
    }

    // Animación de ataque basada en enumerador
    if (input->is_action_just_pressed("attack")) {

        switch (last_direction) {
		default:
            Output("no reproduce?!!!");
            return;
        case DIR_UP: animatedSprite->play("attack_up"); break;
        case DIR_RIGHT: animatedSprite->play("attack_right"); break;
        case DIR_DOWN: animatedSprite->play("attack_down"); break;
        case DIR_LEFT: animatedSprite->play("attack_left"); break;
        }
        attack_enemy(instance); // Llamar a la función de ataque
    }
}

void SetGridPositionExternamente(Caller* instance, const Vector2i& newGridPosition) {
    gridPosition = newGridPosition;
    set_player_position(instance, gridPosition);
}

JENOVA_SCRIPT_END