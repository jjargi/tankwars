
/* Jenova C++ Node Base Script (Meteora) */

// Godot SDK
 #include <Godot/godot.hpp>
 #include <Godot/classes/node.hpp>
 #include <Godot/variant/variant.hpp>
 #include <Godot/classes/animated_sprite2d.hpp>
 #include <Godot/classes/character_body2d.hpp>
 #include <Godot/classes/input.hpp>
// Jenova SDK
#include <JenovaSDK.h>

// Namespaces
using namespace godot;
using namespace jenova::sdk;

// Jenova Script Block Start
JENOVA_SCRIPT_BEGIN
AnimatedSprite2D* animatedSprite = nullptr;
//String anim_to_play = "BOAR_NW_IDLE";
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
     Output("DEBUG: AnimatedSprite2D inicializado");
}

// Called On Every Frame
void OnProcess(Caller* instance, double _delta)
{
    Input* input = Input::get_singleton();


    if (input->is_action_pressed("up")) {
        animatedSprite->play("boar_NW_RUN");
        Output("DEBUG: AnimatedSprite2D moviendose: boar_NW_RUN");
    }
    if (input->is_action_pressed("right")) {
        animatedSprite->play("boar_NE_RUN");
        Output("DEBUG: AnimatedSprite2D moviendose: boar_NE_RUN");
    }
    if (input->is_action_pressed("down")) {
        animatedSprite->play("boar_SE_RUN");
        Output("DEBUG: AnimatedSprite2D moviendose: boar_SE_RUN");
        }
    if (input->is_action_pressed("left")) {
        //anim_to_play = "boar_SW_RUN";
        animatedSprite->play("boar_SW_RUN");
        Output("DEBUG: AnimatedSprite2D moviendose: boar_SW_RUN");
    }
        

    

}

// Jenova Script Block End
JENOVA_SCRIPT_END