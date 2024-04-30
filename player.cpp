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

/*
    Checkea si el jugador puede esquivar o si debe recibir daño con un ataque.
*/
bool Player::check_dodge() {
    int probability_num = RANDOM(0, 100);
    int evasion = getEVA();

    bool DODGE = (evasion <= probability_num) ? true : false;
   return DODGE;  
}

void Player::attack(Player& enemy) {
    int DAMAGE = getATK() - enemy.getDEF();
    enemy.take_damage(DAMAGE);
}