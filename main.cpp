#include "player.h"
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
  srand(time(NULL)); // Seed del generador de números random.

  // pid_t playerPID;
  // pid_t wrestlerPID1;
  // pid_t wrestlerPID2; 
  // pid_t wrestlerPID3;
  
  Player player1;
  Player player2;
  player1.print_stats();
  player2.print_stats();
  return 0;
}