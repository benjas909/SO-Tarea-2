#ifndef PLAYER_H
#define PLAYER_H

class Player {
  public:
    // Constructor
    Player();

    // Getters
    int getHP();
    int getATK();
    int getDEF();
    int getEVA();

    // Funciones
    void print_stats();
    void take_damage(int damage);
    void attack(Player& enemy);
   
  private:
    // Stats
    int _HP;
    int _ATK;
    int _DEF;
    int _EVA;
};

#endif
