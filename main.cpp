#include "player.h"
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
  srand(time(NULL)); // Seed del generador de números random.

  pid_t playerPID = fork();
  if (playerPID < 0)
  {
    cerr << "Fork de jugador fallido." << endl;
    exit(-1);
  }

  pid_t wrestlerPID1;
  pid_t wrestlerPID2;
  pid_t wrestlerPID3;

  if (playerPID > 0)
  {
    wrestlerPID1 = fork();
    if (wrestlerPID1 > 0)
    {
      wrestlerPID2 = fork();
      if (wrestlerPID2 > 0)
      {
        wrestlerPID3 = fork();
        if (wrestlerPID3 < 0)
        {
          cerr << "Fork de luchador 3 fallido" << endl;
          exit(-1);
        }
      }
      else if (wrestlerPID2 < 0)
      {
        cerr << "Fork de luchador 2 fallido" << endl;
        exit(-1);
      }
    }
    else if (wrestlerPID1 < 0)
    {
      cerr << "Fork de luchador 1 fallido" << endl;
      exit(-1);
    }
  }

  Player player1;
  Player player2;
  player1.print_stats();
  player2.print_stats();
  return 0;
}