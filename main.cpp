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
  pid_t wrestlerPID2;
  pid_t wrestlerPID3;
  pid_t wrestlerPID4;

  if (playerPID > 0)
  {
    wrestlerPID2 = fork();
    if (wrestlerPID2 > 0)
    {
      wrestlerPID3 = fork();
      if (wrestlerPID3 > 0)
      {
        wrestlerPID4 = fork();
        if (wrestlerPID4 < 0)
        {
          cerr << "Fork de luchador 3 fallido" << endl;
          exit(-1);
        }
      }
      else if (wrestlerPID3 < 0)
      {
        cerr << "Fork de luchador 2 fallido" << endl;
        exit(-1);
      }
    }
    else if (wrestlerPID2 < 0)
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
  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    close(pipes[0][1]);
    close(pipes[1][0]);
    close(pipes[1][1]);
    close(pipes[2][1]);
    close(pipes[3][0]);
    close(pipes[3][1]);
    close(pipes[5][0]);
    close(pipes[5][1]);
    close(pipes[6][1]);
    close(pipes[7][0]);
    close(pipes[7][1]);

    read(pipes[0][0], stats[0], 4 * sizeof(int));
    read(pipes[2][0], stats[1], 4 * sizeof(int));
    read(pipes[4][0], stats[2], 4 * sizeof(int));
    read(pipes[6][0], stats[3], 4 * sizeof(int));
  }
  else if (playerPID == 0)
  {
    close(pipes[0][0]);
    close(pipes[1][0]);
    close(pipes[1][1]);
    close(pipes[2][0]);
    close(pipes[2][1]);
    close(pipes[3][0]);
    close(pipes[3][1]);
    close(pipes[4][0]);
    close(pipes[4][1]);
    close(pipes[5][0]);
    close(pipes[5][1]);
    close(pipes[6][0]);
    close(pipes[6][1]);
    close(pipes[7][0]);
    close(pipes[7][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[0][1], pStat, 4 * sizeof(int));
  }
  else if (wrestlerPID2 == 0)
  {
    close(pipes[0][0]);
    close(pipes[0][1]);
    close(pipes[1][0]);
    close(pipes[1][1]);
    close(pipes[2][0]);
    close(pipes[3][0]);
    close(pipes[3][1]);
    close(pipes[4][0]);
    close(pipes[4][1]);
    close(pipes[5][0]);
    close(pipes[5][1]);
    close(pipes[6][0]);
    close(pipes[6][1]);
    close(pipes[7][0]);
    close(pipes[7][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[2][1], pStat, 4 * sizeof(int));
  }
  else if (wrestlerPID3 == 0)
  {
    close(pipes[0][0]);
    close(pipes[0][1]);
    close(pipes[1][0]);
    close(pipes[1][1]);
    close(pipes[2][0]);
    close(pipes[2][1]);
    close(pipes[3][0]);
    close(pipes[3][1]);
    close(pipes[4][0]);
    close(pipes[5][0]);
    close(pipes[5][1]);
    close(pipes[6][0]);
    close(pipes[6][1]);
    close(pipes[7][0]);
    close(pipes[7][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[4][1], pStat, 4 * sizeof(int));
  }
  else if (wrestlerPID4 == 0)
  {
    close(pipes[0][0]);
    close(pipes[0][1]);
    close(pipes[1][0]);
    close(pipes[1][1]);
    close(pipes[2][0]);
    close(pipes[2][1]);
    close(pipes[3][0]);
    close(pipes[3][1]);
    close(pipes[4][0]);
    close(pipes[4][1]);
    close(pipes[5][0]);
    close(pipes[5][1]);
    close(pipes[6][0]);
    close(pipes[7][0]);
    close(pipes[7][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[6][1], pStat, 4 * sizeof(int));
  }

  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
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

  int choice;

  int isInvalid = 1;
  for (round; noWinner; round++)
  {
    if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
    {
      cout << "Ronda ";
      cout << round << endl;

      read(pipes[0][0], &choice, sizeof(int));
      cout << "El jugador 1 ataca al luchador ";
      cout << choice << endl;

      read(pipes[2][0], &choice, sizeof(int));
      cout << "El jugador 2 ataca al luchador ";
      cout << choice << endl;

      read(pipes[4][0], &choice, sizeof(int));
      cout << "El jugador 3 ataca al luchador ";
      cout << choice << endl;

      read(pipes[6][0], &choice, sizeof(int));
      cout << "El jugador 4 ataca al luchador ";
      cout << choice << endl;
    }
    if (playerPID == 0)
    {

      // Aquí debe ir la lógica del jugador en cada ronda
      // close(pipes[0][0]);
      cout << "Tu salud: ";
      cout << HP << endl;
      cout << "¿A quién deseas atacar?... Parcero.\n\t1)Luchador 1\n\t2)Luchador 2\n\t3)Luchador 3" << endl;
      cin >> choice;
      write(pipes[0][1], &choice, sizeof(int));

      // Hasta aquí
    }
    else if (wrestlerPID2 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Salud luchador 2: ";
      cout << HP << endl;
      while (isInvalid)
      {
        choice = randInt(1, 4);
        if (choice != 2)
        {
          isInvalid = 0;
        }
      }
      write(pipes[2][1], &choice, sizeof(int));

      // Hasta acá
    }
    else if (wrestlerPID3 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Salud luchador 3: ";
      cout << HP << endl;
      while (isInvalid)
      {
        choice = randInt(1, 4);
        if (choice != 3)
        {
          isInvalid = 0;
        }
      }
      write(pipes[4][1], &choice, sizeof(int));
      // Hasta acá
    }
    else if (wrestlerPID4 == 0)
    {
      // Aquí debe ir la lógica del NPC en cada ronda
      cout << "Salud luchador 4: ";
      cout << HP << endl;
      while (isInvalid)
      {
        choice = randInt(1, 4);
        if (choice != 4)
        {
          isInvalid = 0;
        }
      }
      write(pipes[6][1], &choice, sizeof(int));
      // Hasta acá
    }

    if (round == 5) // Número arbitrario para testeo, hay que hacer la lógica de fin del juego
    {
      noWinner = 0;
    }
  }

  return 0;
}