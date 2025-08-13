
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
#include <Godot/classes/h_box_container.hpp>
#include <Godot/classes/color_rect.hpp>
#include <JenovaSDK.h>
#include <Godot/classes/timer.hpp>
using namespace godot;
using namespace jenova::sdk;

// Enumerador para direcciones
enum Direction { DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT };
// Class Name
JENOVA_CLASS_NAME("Enemy C++ Script")
JENOVA_SCRIPT_BEGIN

JENOVA_PROPERTY(int, MaxHealth, 4)
// Variables globales
AnimatedSprite2D* animatedSprite = nullptr;
TileMapLayer* tileMapLayer1 = nullptr;
TileMapLayer* tileMapLayer2 = nullptr;
TileMapLayer* tileMapLayer3 = nullptr;
Vector2i gridPosition = Vector2i(0, 0);
Direction last_direction = DIR_DOWN;  // Usamos el enumerador
void SetupHealthBar(Caller* instance);
void UpdateHealthBar(Caller* instance);
void play_death_animation(Caller* instance);

bool is_invulnerable = false;
Timer* invuln_timer = nullptr;
Timer* blink_timer = nullptr;


// Funciones auxiliares básicas
AnimatedSprite2D* get_sprite(Node* node) {
    return Object::cast_to<AnimatedSprite2D>(node->find_child("AnimatedSprite2D"));
}
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

        //// Verificar si el enemigo puede morir
        // con el método TakeDamage en caso de que no tenga puntos de vida
        if (enemy->has_method("TakeDamage")) {
            enemy->call("TakeDamage", 1); // se le resta 1 punto de daño por ataque
        }
        else {
            // Sistema de respaldo si no tiene TakeDamage
            bool is_dying = enemy->get_meta("is_dying", false);
            if (!is_dying && enemy->has_method("play_death_animation")) {
                enemy->call("play_death_animation");
            }
        }
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

    // Crear Timers para invulnerabilidad y parpadeo
    invuln_timer = memnew(Timer);
    invuln_timer->set_wait_time(2.0); // 2 segundos
    invuln_timer->set_one_shot(true);
    node->add_child(invuln_timer);
    invuln_timer->connect("timeout", Callable(node, "_on_invuln_timeout"));

    blink_timer = memnew(Timer);
    blink_timer->set_wait_time(0.2); // cada 0.2s alterna visibilidad
    blink_timer->set_one_shot(false);
    node->add_child(blink_timer);
    blink_timer->connect("timeout", Callable(node, "_on_blink_timeout"));

    Node* parent = node->get_parent();
    tileMapLayer1 = Object::cast_to<TileMapLayer>(parent->find_child("Layer1"));
    tileMapLayer2 = Object::cast_to<TileMapLayer>(parent->find_child("Layer2"));
    tileMapLayer3 = Object::cast_to<TileMapLayer>(parent->find_child("Layer3"));

    set_player_position(instance, gridPosition);

    // Inicializar propiedades por instancia
    //node->set_meta("current_health", MaxHealth);//para que no regenera la vida todo el rato
    // Solo inicializar current_health si no existe
    if (!node->has_meta("current_health")) {
        node->set_meta("current_health", MaxHealth);
    }
    // Crear o obtener HealthContainer
    HBoxContainer* healthContainer = Object::cast_to<HBoxContainer>(node->find_child("HealthContainer"));

    if (!healthContainer) {
        healthContainer = memnew(HBoxContainer);
        healthContainer->set_name("HealthContainer");
        healthContainer->set_position(Vector2(0, -30));
        node->add_child(healthContainer);
    }

    // Guardar referencia en metadatos
    node->set_meta("health_container", healthContainer);

    SetupHealthBar(instance);
}

void OnProcess(Caller* instance, double _delta) {
    Node* node = GetSelf<Node>(instance);
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
        Output(" salud del jugador a : %i ", (int)node->get_meta("current_health"));
        Output("maxima salud del jugador a : %i ", MaxHealth);

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

void SetupHealthBar(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    HBoxContainer* healthContainer = Object::cast_to<HBoxContainer>(self->get_meta("health_container"));


    // Obtener vida actual (con conversión explícita)
    int current_health = (int)self->get_meta("current_health", 0);

    // Crear nuevos indicadores
    for (int i = 0; i < MaxHealth; i++) {
        ColorRect* lifeRect = memnew(ColorRect);

        // Versión corregida sin ambigüedad
        /*Primer valor (0): Componente rojo
        Segundo valor (1): Componente verde
        Tercer valor (0): Componente azul*/
        lifeRect->set_color(i < current_health ? Color(0, 0, 1) : Color(1, 0, 0));
        lifeRect->set_custom_minimum_size(Vector2(5, 5));
        lifeRect->set_h_size_flags(Control::SIZE_SHRINK_CENTER);

        healthContainer->add_child(lifeRect);
    }
}
//void TakeDamage(Caller* instance, int damage) {
//    Node* self = GetSelf<Node>(instance);
//    int currentHealth = MAX(0, (int)self->get_meta("current_health") - damage);
//    self->set_meta("current_health", currentHealth);
//
//    UpdateHealthBar(instance);
//
//    if (currentHealth <= 0) {
//        play_death_animation(instance);
//    }
//}
void TakeDamage(Caller* instance, int damage) {
    Node* self = GetSelf<Node>(instance);

    // Si está invulnerable, ignorar el daño
    if (is_invulnerable) return;

    int currentHealth = MAX(0, (int)self->get_meta("current_health") - damage);
    self->set_meta("current_health", currentHealth);
    UpdateHealthBar(instance);

    // Activar invulnerabilidad y parpadeo
    is_invulnerable = true;
    invuln_timer->start();
    blink_timer->start();

    if (currentHealth <= 0) {
        blink_timer->stop();
        AnimatedSprite2D* sprite = get_sprite(self);
        if (sprite) sprite->set_visible(true);
        play_death_animation(instance);
    }
}

void play_death_animation(Caller* instance) {

    Node* self = GetSelf<Node>(instance);

    // Evitar múltiples llamadas
    if (self->get_meta("is_dying", false)) return;



    self->set_meta("is_dying", true);
    self->set_meta("should_delete", false);
    Output("Iniciando animación de muerte");

    AnimatedSprite2D* sprite = get_sprite(self);
    // Conectar señal solo si no está conectada
    sprite->stop();
    sprite->connect("animation_finished", Callable(self, "_on_death_animation_finished"));
    sprite->play("death_animation");
}
void UpdateHealthBar(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    HBoxContainer* healthContainer = Object::cast_to<HBoxContainer>(self->get_meta("health_container"));
    int current_health = (int)self->get_meta("current_health", 0);

    Array children = healthContainer->get_children();
    for (int i = 0; i < children.size(); i++) {
        ColorRect* rect = Object::cast_to<ColorRect>(children[i]);
        if (rect) {
            rect->set_color(i < current_health ? Color(0, 1, 0) : Color(1, 0, 0));
        }
    }
}
// Añade esto en JENOVA_SCRIPT_BEGIN
int get_max_health() { return MaxHealth; }
void _on_invuln_timeout(Caller* instance) {
    // Fin de invulnerabilidad
    is_invulnerable = false;
    blink_timer->stop();
    AnimatedSprite2D* sprite = get_sprite(GetSelf<Node>(instance));
    if (sprite) sprite->set_visible(true);
}

void _on_blink_timeout(Caller* instance) {
    AnimatedSprite2D* sprite = get_sprite(GetSelf<Node>(instance));
    if (sprite) sprite->set_visible(!sprite->is_visible());
}

JENOVA_SCRIPT_END