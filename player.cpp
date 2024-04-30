#include "player.h"
#include <cstdlib> 
#include <time.h>
#include "iostream"

using namespace std;

int RANDOM (int MIN, int MAX) {
    return rand() % ( MAX - MIN + 1) + MIN;
}

// Constructor
Player::Player() {
    _HP = 100;
    _ATK = RANDOM(30, 40);
    _DEF = RANDOM(10, 25);
    _EVA = 60 - _DEF;
}

// Getters

int Player::getHP() {
    return _HP;
}

int Player::getATK() {
    return _ATK;
}

int Player::getDEF() {
    return _DEF;
}

int Player::getEVA() {
    return _EVA;
}

// Funciones
void Player::print_stats() {
    printf("HP: %i ATK: %i DEF: %i EVA: %i\n", getHP(), getATK(), getDEF(), getEVA());
    return;
}

void Player::take_damage(int damage) {
    _HP -= damage;
}

void Player::attack(Player& enemy) {
    int probability_num = RANDOM(0, 100);
    int enemy_eva = enemy.getEVA();

    bool DODGE = (enemy_eva <= probability_num) ? true : false;
    if (DODGE){
        cout << "Dodge" << endl;
        return;
    }
    // QUIZÁ HAY QUE CAMBIAR POR ENUNCIADO:
    /*
    La partida se desarrolla por rondas. Cada ronda sigue el siguiente proceso:
    1. Se muestra la vida actual de todos los jugadores. El jugador controlado por consola elige a
    quien atacar, mientras que los otros procesos o jugadores lo hacen de forma aleatoria.
    2. Se muestra quienes atacan a cada jugador, seguido de quienes lograron esquivar los ataques.
    3. Se realiza la reduccion de la vida de cada jugador segun los daños recibidos; si no recibio
    daño, no se reduce la vida.
    */
    int DAMAGE = getATK() - enemy.getDEF();
    enemy.take_damage(DAMAGE);
}