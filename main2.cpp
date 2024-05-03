#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <random>
using namespace std;

int randInt(int low, int high)
{
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distr(low, high);

  return distr(gen);
}

int main()
{
  int pipes[8][2];

  int i = 0;
  for (i; i < 8; i++)
  {
    pipe(pipes[i]);
  }

  int stats[4][4];
  int ATK, DEF, HP, EVA;
  pid_t playerPID = fork();
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

  int noWinner = 1;

  HP = 100;
  ATK = randInt(30, 40);
  DEF = randInt(10, 25);
  EVA = 60 - DEF;

  int round = 1;

  int pStat[4];
  if (playerPID && wrestlerPID1 && wrestlerPID2 && wrestlerPID3)
  {
    read(pipes[0][0], stats[0], 4 * sizeof(int));
    read(pipes[1][0], stats[1], 4 * sizeof(int));
    read(pipes[2][0], stats[2], 4 * sizeof(int));
    read(pipes[3][0], stats[3], 4 * sizeof(int));
  }
  else if (playerPID == 0)
  {
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[0][1], pStat, 4 * sizeof(int));
  }
  else if (wrestlerPID1 == 0)
  {
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[1][1], pStat, 4 * sizeof(int));
  }
  else if (wrestlerPID2 == 0)
  {
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[2][1], pStat, 4 * sizeof(int));
  }
  else if (wrestlerPID3 == 0)
  {
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[3][1], pStat, 4 * sizeof(int));
  }

  for (round; noWinner; round++)
  {
    if (playerPID && wrestlerPID1 && wrestlerPID2 && wrestlerPID3)
    {
      for (i = 0; i < 4; i++)
      {
        cout << "Stats Luchador ";
        cout << i + 1 << endl;
        cout << "Salud: ";
        cout << stats[i][0] << endl;
        cout << "Ataque: ";
        cout << stats[i][1] << endl;
        cout << "Defensa: ";
        cout << stats[i][2] << endl;
        cout << "Evasión: ";
        cout << stats[i][3] << endl;
        cout << endl;
      }
    }

    else if (playerPID == 0)
    {

      // Aquí debe ir la lógica del jugador en cada ronda
      cout << "Soy un jugador ";
      cout << "en la ronda ";
      cout << round << endl;
      // Hasta aquí
    }
    else if (wrestlerPID1 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Soy el luchador 1 ";
      cout << "en la ronda ";
      cout << round << endl;
      cout << "mi PID es ";
      cout << wrestlerPID1 << endl;
      // Hasta acá
    }
    else if (wrestlerPID2 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Soy el luchador 2 ";
      cout << "en la ronda ";
      cout << round << endl;
      cout << "mi PID es ";
      cout << wrestlerPID2 << endl;
      // Hasta acá
    }
    else if (wrestlerPID3 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Soy el luchador 3 ";
      cout << "en la ronda ";
      cout << round << endl;
      cout << "mi PID es ";
      cout << wrestlerPID3 << endl;
      // Hasta acá
    }

    if (round == 5) // Número arbitrario para testeo, hay que hacer la lógica de fin del juego
    {
      noWinner = 0;
    }
  }

  return 0;
}