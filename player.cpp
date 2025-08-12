
/* Jenova C++ Node Base Script - Versión Compatible */

#include <Godot/godot.hpp>
#include <Godot/classes/node.hpp>
#include <Godot/classes/node2d.hpp>
#include <Godot/variant/variant.hpp>
#include <Godot/classes/animated_sprite2d.hpp>
#include <Godot/classes/character_body2d.hpp>
#include <Godot/classes/input.hpp>
#include <Godot/classes/tile_map_layer.hpp>
#include <Godot/classes/timer.hpp>
#include <Godot/classes/h_box_container.hpp>
#include <Godot/classes/color_rect.hpp>
#include <JenovaSDK.h>

using namespace godot;
using namespace jenova::sdk;
JENOVA_CLASS_NAME("Player C++ Script")
JENOVA_SCRIPT_BEGIN
void SetupHealthBar(Caller* instance);
void UpdateHealthBar(Caller* instance);
void attack_to_player(Caller* instance);
JENOVA_PROPERTY(int, MaxHealth, 4)
// Variables por instancia usando propiedades del nodo
void set_instance_data(Node* node, Timer* timer, const Vector2i& grid_pos, bool initialized) {
    node->set_meta("move_timer", timer);
    node->set_meta("grid_position", grid_pos);
    node->set_meta("initialized", initialized);
}

Timer* get_move_timer(Node* node) {
    return Object::cast_to<Timer>(node->get_meta("move_timer"));
}

Vector2i get_grid_position(Node* node) {
    return node->get_meta("grid_position");
}

bool is_initialized(Node* node) {
    return node->get_meta("initialized");
}

void set_grid_position(Node* node, const Vector2i& pos) {
    node->set_meta("grid_position", pos);
}

void set_initialized(Node* node, bool value) {
    node->set_meta("initialized", value);
}

// Funciones auxiliares básicas
AnimatedSprite2D* get_sprite(Node* node) {
    return Object::cast_to<AnimatedSprite2D>(node->find_child("AnimatedSprite2D"));
}

TileMapLayer* get_layer(Node* node, const String& name) {
    return Object::cast_to<TileMapLayer>(node->get_parent()->find_child(name));
}

Node2D* get_player(Node* node) {
    return Object::cast_to<Node2D>(node->get_parent()->find_child("player2"));
}

bool can_move(TileMapLayer* layer1, TileMapLayer* layer2, const Vector2i& target) {
    TypedArray<Vector2i> cells = layer1->get_used_cells();
    if (!cells.has(target)) return false;

    TileData* tile = layer2->get_cell_tile_data(target);
    if (tile) {
        int src_id = layer2->get_cell_source_id(target);
        Vector2i coords = layer2->get_cell_atlas_coords(target);
        if ((src_id == 0 && coords == Vector2i(10, 5)) || (src_id == 0 && coords == Vector2i(4, 5))) {
            return false;
        }
    }
    return true;
}

// Funciones principales
void OnAwake(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    Timer* timer = memnew(Timer);
    self->add_child(timer);
    timer->connect("timeout", Callable(self, "_on_move_timeout"));
    timer->set_wait_time(1.0);
    timer->start();
    set_instance_data(self, timer, Vector2i(0, 0), false);
}

void OnReady(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    AnimatedSprite2D* sprite = get_sprite(GetSelf<Node>(instance));
    // Configurar sprite
    if (sprite) {
        sprite->set_animation("BOAR_NW_IDLE");
        sprite->play();
    }
    // Inicializar propiedades por instancia
    self->set_meta("current_health", MaxHealth);
    self->set_meta("should_delete", false);
    
    
    // Crear o obtener HealthContainer
    HBoxContainer* healthContainer = Object::cast_to<HBoxContainer>(self->find_child("HealthContainer"));
    
    if (!healthContainer) {
        healthContainer = memnew(HBoxContainer);
        healthContainer->set_name("HealthContainer");
        healthContainer->set_position(Vector2(0, -20));
        self->add_child(healthContainer);
    }
    
    // Guardar referencia en metadatos
    self->set_meta("health_container", healthContainer);
    
    SetupHealthBar(instance);
}

void OnProcess(Caller* instance, double delta) {
    Node* self = GetSelf<Node>(instance);
    if (!is_initialized(self)) {
        TileMapLayer* layer1 = get_layer(self, "Layer1");
        if (layer1) {
            set_grid_position(self, layer1->local_to_map(GetSelf<Node2D>(instance)->get_position()));
            set_initialized(self, true);
        }
    }
    // Verificar eliminación
    if (self->get_meta("should_delete", false)) {
        Output("Eliminando nodo: ", self->get_name());
        self->queue_free();
        return;
    }
}

void _on_move_timeout(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    Timer* timer = get_move_timer(self);
    Vector2i grid_pos = get_grid_position(self);

    TileMapLayer* layer1 = get_layer(self, "Layer1");
    TileMapLayer* layer2 = get_layer(self, "Layer2");
    Node2D* player = get_player(self);
    AnimatedSprite2D* sprite = get_sprite(self);

    if (!layer1 || !layer2 || !player || !sprite) return;

    Vector2i target = layer1->local_to_map(player->get_position());
    Vector2i diff = target - grid_pos;
    Vector2i dir;
    if (Math::abs(diff.x) > Math::abs(diff.y)) {
        dir = Vector2i((diff.x > 0) ? 1 : -1, 0);
    }
    else {
        dir = Vector2i(0, (diff.y > 0) ? 1 : -1);
    }

    Vector2i new_pos = grid_pos + dir;

    if (can_move(layer1, layer2, new_pos)) {
        set_grid_position(self, new_pos);
        GetSelf<Node2D>(instance)->set_position(layer1->map_to_local(new_pos));

        if (dir.x < 0) sprite->play("boar_NW_RUN");
        else if (dir.x > 0) sprite->play("boar_SE_RUN");
        else if (dir.y < 0) sprite->play("boar_NE_RUN");
        else if (dir.y > 0) sprite->play("boar_SW_RUN");
    }
    attack_to_player(instance);
}


// Versión corregida del sistema de muerte de enemigos

void play_death_animation(Caller* instance) {
     
    Node* self = GetSelf<Node>(instance);

    // Evitar múltiples llamadas
    if (self->get_meta("is_dying", false)) return;


    Timer* timer = get_move_timer(self);
    timer->stop();
    self->set_meta("is_dying", true);
    self->set_meta("should_delete", false);
    Output("Iniciando animación de muerte");

    AnimatedSprite2D* sprite = get_sprite(self);
    if (!sprite) {
        Output("no hay animacion de muerte");
        self->queue_free();
        return;
    }

    // Conectar señal solo si no está conectada
    sprite->stop();
    sprite->connect("animation_finished", Callable(self, "_on_death_animation_finished"));
    sprite->play("death_animation");
}

void _on_death_animation_finished(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    Output("Animación de muerte completada");

    // Marcar ESTA instancia para eliminación
    self->set_meta("should_delete", true);

    // Desconectar señal
    AnimatedSprite2D* sprite = get_sprite(self);
    if (sprite && sprite->is_connected("animation_finished", Callable(self, "_on_death_animation_finished"))) {
        sprite->disconnect("animation_finished", Callable(self, "_on_death_animation_finished"));
    }

}

void OnDestroy(Caller* instance) {

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
void TakeDamage(Caller* instance, int damage) {
    Node* self = GetSelf<Node>(instance);
    int currentHealth = MAX(0, (int)self->get_meta("current_health") - damage);
    self->set_meta("current_health", currentHealth);

    UpdateHealthBar(instance);

    if (currentHealth <= 0) {
        play_death_animation(instance);
    }
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
void attack_to_player(Caller* instance) {
    Node* self = GetSelf<Node>(instance);
    TileMapLayer* layer1 = get_layer(self, "Layer1");
    if (!layer1) return;

    // Obtener posición actual del enemigo en celdas
    Vector2i enemy_cell = layer1->local_to_map(GetSelf<Node2D>(instance)->get_position());

    // Buscar al jugador
    Node* player = get_player(self);
    if (!player) return;

    // Obtener posición del jugador
    Vector2i player_cell = layer1->local_to_map(Object::cast_to<Node2D>(player)->get_position());

    // Verificar si están en la misma celda
    if (enemy_cell == player_cell) {
        Output("¡Colisión con el jugador en celda: ", enemy_cell);

        // Llamar a TakeDamage si existe el método
        if (player->has_method("TakeDamage")) {
            player->call("TakeDamage", 1); // 1 punto de daño
        }
        else {
            Output("¡El jugador no tiene método TakeDamage!");
        }
    }
}
JENOVA_SCRIPT_END

