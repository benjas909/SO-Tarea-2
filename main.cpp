#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <random>

using namespace std;

void closePipesNotUsed(int pipes[8][2], int pipesUsed[][2], int numPipesUsed)
{
  for (int i = 0; i < 8; i++)
  {
    bool readUsed = false;
    bool writeUsed = false;

    for (int j = 0; j < numPipesUsed; j++)
    {
      if (pipesUsed[j][0] == i)
      {
        if (pipesUsed[j][1] == 0)
        {
          readUsed = true;
        }
        else
        {
          writeUsed = true;
        }
      }
    }

    if (readUsed == true)
    {
      printf("close pipes[%i][1]\n", i);
      close(pipes[i][1]);
    }
    else if (writeUsed == true)
    {
      printf("close pipes[%i][0]\n", i);
      close(pipes[i][0]);
    }
    else
    {
      printf("close pipes[%i][0]\n", i);
      printf("close pipes[%i][1]\n", i);
      close(pipes[i][0]);
      close(pipes[i][1]);
    }
  }
}

void closeSignalPipesNotUsed(int signalPipes[5][2], int signalPipesUsed[][2], int numSignalPipesUsed)
{
  for (int i = 0; i < 5; i++)
  {
    bool readUsed = false;
    bool writeUsed = false;

    for (int j = 0; j < numSignalPipesUsed; j++)
    {
      if (signalPipesUsed[j][0] == i)
      {
        if (signalPipesUsed[j][1] == 0)
        {
          readUsed = true;
        }
        else
        {
          writeUsed = true;
        }
      }
    }

    if (readUsed == true)
    {
      printf("close signal pipes[%i][1]\n", i);
      close(signalPipes[i][1]);
    }
    else if (writeUsed == true)
    {
      printf("close signal pipes[%i][0]\n", i);
      close(signalPipes[i][0]);
    }
    else
    {
      printf("close signal pipes[%i][0]\n", i);
      printf("close signal pipes[%i][1]\n", i);
      close(signalPipes[i][0]);
      close(signalPipes[i][1]);
    }
  }
}

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
  /*
  0: Jugador a Padre
  1: Padre a Jugador
  2: Luchador 2 a Padre
  3: Padre a Luchador 2
  4: Luchador 3 a Padre
  5: Padre a Luchador 3
  6: Luchador 4 a Padre
  7: Padre a Luchador 4
  */

  int signalpipes[5][2];
  /*
  0: Padre a Jugador
  1: Jugador a Luchador 2
  2: Luchador 2 a Luchador 3
  3: Luchador 3 a Luchador 4
  4: Luchador 4 a Jugador
  */

  int i;
  for (i = 0; i < 8; i++)
  {
    if (i < 5)
    {
      pipe(signalpipes[i]);
    }
    pipe(pipes[i]);
  }

  int stats[4][4]; // Se guardan las stats de cada luchador. 0: Jugador, 1: Luchador 1, etc. De la forma [HP, ATK, DEF, EVA]
  int ATK, DEF, HP, EVA;
  pid_t playerPID = fork();
  pid_t wrestlerPID2;
  pid_t wrestlerPID3;
  pid_t wrestlerPID4;

  // Creación de forks
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

  int round;

  int pStat[4]; // Arreglo temporal para guardar las stats de cada luchador y enviarlas al proceso padre
  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    int pipesUsed[][2] = {{0, 0}, {1, 1}, {2, 0}, {3, 1}, {4, 0}, {5, 1}, {6, 0}, {7, 1}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed)/ sizeof(pipesUsed[0]));
    // close(pipes[0][1]);
    // close(pipes[1][0]);
    // close(pipes[2][1]);
    // close(pipes[3][0]);
    // close(pipes[4][1]);
    // close(pipes[5][0]);
    // close(pipes[6][1]);
    // close(pipes[7][0]);

    int signalPipesUsed[][2] = {{0,1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, 1);
    // close(signalpipes[0][0]);
    // close(signalpipes[1][0]);
    // close(signalpipes[1][1]);
    // close(signalpipes[2][0]);
    // close(signalpipes[2][1]);
    // close(signalpipes[3][0]);
    // close(signalpipes[3][1]);
    // close(signalpipes[4][0]);
    // close(signalpipes[4][1]);

    read(pipes[0][0], stats[0], 4 * sizeof(int));
    read(pipes[2][0], stats[1], 4 * sizeof(int));
    read(pipes[4][0], stats[2], 4 * sizeof(int));
    read(pipes[6][0], stats[3], 4 * sizeof(int));
  }
  else if (!playerPID)
  {
    int pipesUsed[][2] = {{0, 1}, {1, 0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed)/sizeof(pipesUsed[0]));
    // close(pipes[0][0]);
    // close(pipes[1][1]);
    // close(pipes[2][0]);
    // close(pipes[2][1]);
    // close(pipes[3][0]);
    // close(pipes[3][1]);
    // close(pipes[4][0]);
    // close(pipes[4][1]);
    // close(pipes[5][0]);
    // close(pipes[5][1]);
    // close(pipes[6][0]);
    // close(pipes[6][1]);
    // close(pipes[7][0]);
    // close(pipes[7][1]);
    int signalPipesUsed[][2] = {{0,0},{1,1},{4,0}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed)/sizeof(signalPipesUsed[0]));

    // close(signalpipes[0][1]);
    // close(signalpipes[1][0]);
    // close(signalpipes[2][0]);
    // close(signalpipes[2][1]);
    // close(signalpipes[3][0]);
    // close(signalpipes[3][1]);
    // close(signalpipes[4][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[0][1], pStat, 4 * sizeof(int));
  }
  else if (!wrestlerPID2)
  {
    int pipesUsed[][2] = {{2,1}, {3,0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed)/sizeof(pipesUsed[0]));
    // close(pipes[0][0]);
    // close(pipes[0][1]);
    // close(pipes[1][0]);
    // close(pipes[1][1]);
    // close(pipes[2][0]);
    // close(pipes[3][1]);
    // close(pipes[4][0]);
    // close(pipes[4][1]);
    // close(pipes[5][0]);
    // close(pipes[5][1]);
    // close(pipes[6][0]);
    // close(pipes[6][1]);
    // close(pipes[7][0]);
    // close(pipes[7][1]);
    int signalPipesUsed[][2] = {{1,0},{2,1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed)/sizeof(signalPipesUsed[0]));
    // close(signalpipes[0][0]);
    // close(signalpipes[0][1]);
    // close(signalpipes[1][1]);
    // close(signalpipes[2][0]);
    // close(signalpipes[3][0]);
    // close(signalpipes[3][1]);
    // close(signalpipes[4][0]);
    // close(signalpipes[4][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[2][1], pStat, 4 * sizeof(int));
  }
  else if (!wrestlerPID3)
  {
    int pipesUsed[][2] = {{4,1}, {5,0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed)/sizeof(pipesUsed[0]));
    // close(pipes[0][0]);
    // close(pipes[0][1]);
    // close(pipes[1][0]);
    // close(pipes[1][1]);
    // close(pipes[2][0]);
    // close(pipes[2][1]);
    // close(pipes[3][0]);
    // close(pipes[3][1]);
    // close(pipes[4][0]);
    // close(pipes[5][1]);
    // close(pipes[6][0]);
    // close(pipes[6][1]);
    // close(pipes[7][0]);
    // close(pipes[7][1]);
    int signalPipesUsed[][2] = {{2,0},{3,1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed)/sizeof(signalPipesUsed[0]));
    // close(signalpipes[0][0]);
    // close(signalpipes[0][1]);
    // close(signalpipes[1][0]);
    // close(signalpipes[1][1]);
    // close(signalpipes[2][1]);
    // close(signalpipes[3][0]);
    // close(signalpipes[4][0]);
    // close(signalpipes[4][1]);

    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[4][1], pStat, 4 * sizeof(int));
  }
  else if (!wrestlerPID4)
  {
    int pipesUsed[][2] = {{6,1},{7,0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed)/sizeof(pipesUsed[0]));
    // close(pipes[0][0]);
    // close(pipes[0][1]);
    // close(pipes[1][0]);
    // close(pipes[1][1]);
    // close(pipes[2][0]);
    // close(pipes[2][1]);
    // close(pipes[3][0]);
    // close(pipes[3][1]);
    // close(pipes[4][0]);
    // close(pipes[4][1]);
    // close(pipes[5][0]);
    // close(pipes[5][1]);
    // close(pipes[6][0]);
    // close(pipes[7][1]);
    int signalPipesUsed[][2] = {{3,0},{4,1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed)/sizeof(signalPipesUsed[0]));
    // close(signalpipes[0][0]);
    // close(signalpipes[0][1]);
    // close(signalpipes[1][0]);
    // close(signalpipes[1][1]);
    // close(signalpipes[2][0]);
    // close(signalpipes[2][1]);
    // close(signalpipes[3][1]);
    // close(signalpipes[4][0]);

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
  int signal = 1;
  for (round = 1; noWinner; round++)
  {
    if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
    {
      cout << "Ronda ";
      cout << round << endl;

      write(signalpipes[0][1], &signal, sizeof(signal));

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

      write(signalpipes[0][1], &signal, sizeof(signal));
      // write(pipes[3][1], &signal, sizeof(int));
      // write(pipes[5][1], &signal, sizeof(int));
      // write(pipes[7][1], &signal, sizeof(int));
    }
    else if (!playerPID)
    {

      // Aquí debe ir la lógica del jugador en cada ronda

      read(signalpipes[0][0], &signal, sizeof(signal));

      cout << "Tu salud: ";
      cout << HP << endl;

      write(signalpipes[1][1], &signal, sizeof(signal));
      read(signalpipes[4][0], &signal, sizeof(signal));

      cout << "¿A quién deseas atacar?... Parcero.\n\t1)Luchador 1\n\t2)Luchador 2\n\t3)Luchador 3\n> ";
      cin >> choice;

      write(signalpipes[1][1], &signal, sizeof(signal));

      write(pipes[0][1], &choice, sizeof(int));

      read(signalpipes[0][0], &signal, sizeof(signal));
      write(signalpipes[1][1], &signal, sizeof(signal));

      // Hasta aquí
    }
    else if (!wrestlerPID2)
    {
      // Aquí debe ir la lógica del NPC en cada ronda

      read(signalpipes[1][0], &signal, sizeof(signal));

      cout << "Salud luchador 2: ";
      cout << HP << endl;

      write(signalpipes[2][1], &signal, sizeof(signal));
      read(signalpipes[1][0], &signal, sizeof(signal));

      while (isInvalid)
      {
        choice = randInt(1, 4);
        if (choice != 2)
        {
          isInvalid = 0;
        }
      }

      write(signalpipes[2][1], &signal, sizeof(signal));

      write(pipes[2][1], &choice, sizeof(int));

      read(signalpipes[1][0], &signal, sizeof(signal));
      write(signalpipes[2][1], &signal, sizeof(signal));

      // Hasta acá
    }
    else if (!wrestlerPID3)
    {
      // Aquí debe ir la lógica del NPC en cada ronda

      read(signalpipes[2][0], &signal, sizeof(signal));

      cout << "Salud luchador 3: ";
      cout << HP << endl;

      write(signalpipes[3][1], &signal, sizeof(signal));
      read(signalpipes[2][0], &signal, sizeof(signal));

      while (isInvalid)
      {
        choice = randInt(1, 4);
        if (choice != 3)
        {
          isInvalid = 0;
        }
      }

      write(signalpipes[3][1], &signal, sizeof(signal));

      write(pipes[4][1], &choice, sizeof(int));

      read(signalpipes[2][0], &signal, sizeof(signal));
      write(signalpipes[3][1], &signal, sizeof(signal));

      // Hasta acá
    }
    else if (!wrestlerPID4)
    {
      // Aquí debe ir la lógica del NPC en cada ronda

      read(signalpipes[3][0], &signal, sizeof(signal));

      cout << "Salud luchador 4: ";
      cout << HP << endl;

      write(signalpipes[4][1], &signal, sizeof(signal));
      read(signalpipes[3][0], &signal, sizeof(signal));

      while (isInvalid)
      {
        choice = randInt(1, 4);
        if (choice != 4)
        {
          isInvalid = 0;
        }
      }

      write(signalpipes[4][1], &signal, sizeof(signal));
      write(pipes[6][1], &choice, sizeof(int));
      read(signalpipes[3][0], &signal, sizeof(signal));
      // Hasta acá
    }

    if (round == 5) // Número arbitrario para testeo, hay que hacer la lógica de fin del juego
    {
      noWinner = 0;
    }
  }

  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    int pipesUsed[][2] = {{0,1},{1,0},{2,1},{3,0},{4,1},{5,0},{6,1},{7,0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed)/sizeof(pipesUsed[0]));
    // close(pipes[0][0]);
    // close(pipes[1][1]);
    // close(pipes[2][0]);
    // close(pipes[3][1]);
    // close(pipes[4][0]);
    // close(pipes[5][1]);
    // close(pipes[6][0]);
    // close(pipes[7][1]);

    close(signalpipes[0][1]);
  }
  else if (!playerPID)
  {
    close(pipes[0][1]);
    close(pipes[1][0]);

    close(signalpipes[0][0]);
    close(signalpipes[1][1]);
    close(signalpipes[4][0]);
  }
  else if (!wrestlerPID2)
  {
    close(pipes[2][1]);
    close(pipes[3][0]);

    close(signalpipes[1][0]);
    close(signalpipes[2][1]);
  }
  else if (!wrestlerPID3)
  {
    close(pipes[4][1]);
    close(pipes[5][0]);

    close(signalpipes[2][0]);
    close(signalpipes[3][1]);
  }
  else if (!wrestlerPID4)
  {
    close(pipes[6][1]);
    close(pipes[7][0]);

    close(signalpipes[3][0]);
    close(signalpipes[4][1]);
  }

  return 0;
}