#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
  int ATK, DEF, HP, EVA;
  int playerPID = fork();
  int wrestlerPID1;
  int wrestlerPID2;
  int wrestlerPID3;
  if (playerPID != 0)
  {
    wrestlerPID1 = fork();
    if (wrestlerPID1 != 0)
    {
      wrestlerPID2 = fork();
      if (wrestlerPID2 != 0)
      {
        wrestlerPID3 = fork();
      }
    }
  }

  int noWinner = 1;

  if (playerPID < 0 || wrestlerPID1 < 0 || wrestlerPID2 < 0 || wrestlerPID3 < 0)
  {
    cerr << "Fork Fallido" << endl;
  }

  int round = 1;
  for (round; noWinner; round++)
  {
    if (playerPID == 0)
    {
      // Aquí debe ir la lógica del jugador en cada ronda
      cout << "Soy un jugador ";
      cout << "en la ronda ";
      cout << round << endl;
      // Hasta aquí
    }
    if (wrestlerPID1 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Soy el luchador 1 ";
      cout << "en la ronda ";
      cout << round << endl;
      // Hasta acá
    }
    if (wrestlerPID2 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Soy el luchador 2 ";
      cout << "en la ronda ";
      cout << round << endl;
      // Hasta acá
    }
    if (wrestlerPID3 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Soy el luchador 3 ";
      cout << "en la ronda ";
      cout << round << endl;
      // Hasta acá
    }

    if (round == 5) // Número arbitrario para testeo, hay que hacer la lógica de fin del juego
    {
      noWinner = 0;
    }
  }

  return 0;
}