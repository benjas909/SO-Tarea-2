#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <random>
#include <stdio.h>
#include <fcntl.h>

using namespace std;

bool checkPipeClosed(int fd)
{
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1){
    perror("fnctl");
    return -1; //Error al obtener flags
  }
  if (flags & O_WRONLY)
  {
    return -1;
  }

  if (flags & O_RDONLY)
  {
    return -1;
  }
  return 0; // Pipe abierto
}

bool checkEvasion(double probability)
{
  probability = probability / 100;
  return rand() < probability * ((double)RAND_MAX + 1.0);
}

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
      close(pipes[i][1]);
    }
    else if (writeUsed == true)
    {
      close(pipes[i][0]);
    }
    else
    {
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
      close(signalPipes[i][1]);
    }
    else if (writeUsed == true)
    {
      close(signalPipes[i][0]);
    }
    else
    {
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

  // Cálculo de stats
  HP = 100;
  ATK = randInt(30, 40);
  DEF = randInt(10, 25);
  EVA = 60 - DEF;

  int round;

  // Se calculan los stats para cada luchador y se envían por un pipe al padre para guardar en stats[][]
  int pStat[4]; // Arreglo temporal para guardar las stats de cada luchador y enviarlas al proceso padre
  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    int pipesUsed[][2] = {{0, 0}, {1, 1}, {2, 0}, {3, 1}, {4, 0}, {5, 1}, {6, 0}, {7, 1}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed) / sizeof(pipesUsed[0]));

    int signalPipesUsed[][2] = {{0, 1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, 1);

    // Padre recibe array de stats de cada luchador y los guarda en array propio
    read(pipes[0][0], stats[0], 4 * sizeof(int));
    read(pipes[2][0], stats[1], 4 * sizeof(int));
    read(pipes[4][0], stats[2], 4 * sizeof(int));
    read(pipes[6][0], stats[3], 4 * sizeof(int));
  }
  else if (!playerPID)
  {
    int pipesUsed[][2] = {{0, 1}, {1, 0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed) / sizeof(pipesUsed[0]));

    int signalPipesUsed[][2] = {{0, 0}, {1, 1}, {4, 0}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed) / sizeof(signalPipesUsed[0]));

    // Se guardan las stats en array temporal
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[0][1], pStat, 4 * sizeof(int)); // Se envía el array al padre
  }
  else if (!wrestlerPID2)
  {
    int pipesUsed[][2] = {{2, 1}, {3, 0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed) / sizeof(pipesUsed[0]));

    int signalPipesUsed[][2] = {{1, 0}, {2, 1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed) / sizeof(signalPipesUsed[0]));

    // Se guardan las stats en array temporal
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[2][1], pStat, 4 * sizeof(int)); // Se envía el array al padre
  }
  else if (!wrestlerPID3)
  {
    int pipesUsed[][2] = {{4, 1}, {5, 0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed) / sizeof(pipesUsed[0]));

    int signalPipesUsed[][2] = {{2, 0}, {3, 1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed) / sizeof(signalPipesUsed[0]));

    // Se guardan las stats en array temporal
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[4][1], pStat, 4 * sizeof(int)); // Se envía el array al padre
  }
  else if (!wrestlerPID4)
  {
    int pipesUsed[][2] = {{6, 1}, {7, 0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed) / sizeof(pipesUsed[0]));

    int signalPipesUsed[][2] = {{3, 0}, {4, 1}};
    closeSignalPipesNotUsed(signalpipes, signalPipesUsed, sizeof(signalPipesUsed) / sizeof(signalPipesUsed[0]));

    // Se guardan las stats en array temporal
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    write(pipes[6][1], pStat, 4 * sizeof(int)); // Se envía el array al padre
  }

  // Se muestran los stats de cada uno antes de comenzar la partida
  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    for (i = 0; i < 4; i++)
    {
      printf("Stats Luchador %i\n", i+1);
      printf("Salud: %i\n", stats[i][0]);
      printf("Ataque: %i\n", stats[i][1]);
      printf("Defensa: %i\n", stats[i][2]);
      printf("Evasión: %i\n\n", stats[i][3]);
    }
  }

  int choice;

  int isInvalid = 1;
  int signal = 1;
  int evadesAttack = 0;
  int evadeArray[4] = {0, 0, 0, 0};
  int attacksArray[4];
  int attacked;
  bool alive[4] = {1, 1, 1, 1};
  int w = -1; // índice del jugador ganador
  for (round = 1; noWinner; round++)
  {
    isInvalid = 1;
    if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
    {
      if (round != 1){
        cout << endl;
      }
      int c = alive[0] + alive[1] + alive[2] + alive[3];
      if (c == 0 || c == 1)
      {
        noWinner = 0;
        break;
      }

      printf("Ronda %i\n", round);

      // Muestra la salud de cada luchador al principio de cada ronda
      for (i = 0; i < 4; i++)
      {
        cout << "Salud Luchador ";
        cout << i + 1;
        if (!i)
        {
          cout << " (Tú)";
        }
        cout << ": ";
      
        if (stats[i][0] <= 0){
          cout << stats[i][0]; 
          cout << "(Muerto)" << endl;
        } else {
          cout << stats[i][0] << endl; 
        }
      }

      write(pipes[1][1], &signal, sizeof(signal)); // Señal de partida
      write(pipes[3][1], &signal, sizeof(signal));
      write(pipes[5][1], &signal, sizeof(signal));
      write(pipes[7][1], &signal, sizeof(signal));

      // Comunicación con cada luchador
      for (i = 0; i < 4; i++)
      {
        if (alive[i])
        {
          read(pipes[i*2][0], &choice, sizeof(int)); // Recibe elección de ataque del jugador
          attacksArray[i] = choice - 1;            // Se guarda su elección en un array en el índice del luchador correspondiente
          printf("El Luchador %i ataca al Luchador %i\n", i+1, choice);
          read(pipes[i*2][0], &evadeArray[i], sizeof(int)); // Recibe un 1 si el jugador evade y un 0 en caso contrario y lo guarda en un array
        }
        else
        {
          attacksArray[i] = -1;
        }
      }
      

      // En el array attacksArray, se encuentran los blancos de ataque de cada luchador. 0: Jugador, 1: Luchador 2, etc
      // En el array evadeArray, se guardan en binario la evasión de cada luchador. Cada luchador tiene un 0 si no evade y un 1 si lo hace
      for (i = 0; i < 4; i++)
      {
        attacked = attacksArray[i];
        if (attacked != -1)
        {
          w = i + 1;
          if (!evadeArray[attacked]) // Si no evade
          {
            stats[attacked][0] -= stats[i][1] - stats[attacked][2]; // HP = ATK atacante - Daño atacado
            if (stats[attacked][0] <= 0){
              stats[attacked][0] = 0;
              alive[attacked] = 0;
            }
          }
          else
          {
            printf("(!) El luchador %i evadió el ataque del luchador %i!\n", attacked + 1, i+1);
            evadeArray[attacked] = 0; // Cada luchador puede ser atacado más de una vez por ronda, por lo que su evasión de cambia a 0 al evadir una vez
          }
        }
      }
      // Se envía del padre a cada hijo el array de los luchadores en pie.
      write(pipes[1][1], &alive, sizeof(alive));
      write(pipes[3][1], &alive, sizeof(alive));
      write(pipes[5][1], &alive, sizeof(alive));
      write(pipes[7][1], &alive, sizeof(alive));

      write(pipes[1][1], &signal, sizeof(signal)); // Señal de partida
      write(pipes[3][1], &signal, sizeof(signal));
      write(pipes[5][1], &signal, sizeof(signal));
      write(pipes[7][1], &signal, sizeof(signal));
    }
    else if (!playerPID)
    {
      
      read(pipes[1][0], &signal, sizeof(signal)); // Señal de comienzo
      
      if (round > 1)
      {
        read(pipes[1][0], &alive, sizeof(alive));

        int c = alive[0] + alive[1] + alive[2] + alive[3];
        if (c == 0 || c == 1)
        {
          break;
        }
      }

      if (alive[0])
      {
        while (isInvalid)
        {
          int option;
          cout << "¿A quién deseas atacar?... Parcero.\n";
          for (i = 1; i <= 3; i++)
          {
            if (alive[i])
            {
              printf("\t%i)Luchador %i\n", i, i+1);
            }
          }
          cout << "> ";
          cin >> option;
          if (option >= 1 && option <= 3 && alive[option]) 
          {
            choice = option + 1;
            isInvalid = 0;
          }
          else
          {
            cout << "Elección inválida." << endl;
          }
        }
        
        write(pipes[0][1], &choice, sizeof(int)); // Envía su elección al padre

        evadesAttack = checkEvasion(EVA);               // Calcula evasión
        write(pipes[0][1], &evadesAttack, sizeof(int)); // Envía evasión al padre
      }
      read(pipes[1][0], &signal, sizeof(signal)); // Señal de fin
      
    }
    else if (!wrestlerPID2)
    {
      read(pipes[3][0], &signal, sizeof(signal)); // Recibe señal para continuar

      if (round > 1)
      {
        read(pipes[3][0], &alive, sizeof(alive));
        int c = alive[0] + alive[1] + alive[2] + alive[3];
        
        if (c == 0 || c == 1)
        {
          break;
        }
      }

      if (alive[1])
      {
        while (isInvalid)
        {
          choice = randInt(1, 4);
          if (choice != 2 && alive[choice - 1]) // Si la elección aleatoria no es el mismo luchador, es válida
          {
            isInvalid = 0;
          }
        }

        write(pipes[2][1], &choice, sizeof(int)); // Comunica su elección de ataque al padre
        evadesAttack = checkEvasion(EVA);               // Calcula evasión
        write(pipes[2][1], &evadesAttack, sizeof(int)); // Le dice al padre si evade o no
      }

      read(pipes[3][0], &signal, sizeof(signal)); // Señal de fin
    }
    else if (!wrestlerPID3)
    {
      read(pipes[5][0], &signal, sizeof(signal)); // Recibe señal para continuar

      if (round > 1)
      {
        read(pipes[5][0], &alive, sizeof(alive));
        int c = alive[0] + alive[1] + alive[2] + alive[3];
        if (c == 0 || c == 1)
        {
          break;
        }
      }
      
      if (alive[2])
      {
        while (isInvalid)
        {
          choice = randInt(1, 4);
          if (choice != 3 && alive[choice - 1]) // Si la elección aleatoria no es el mismo luchador, se válida
          {
            isInvalid = 0;
          }
        }

        write(pipes[4][1], &choice, sizeof(int)); // Comunica su elección de ataque al padre

        evadesAttack = checkEvasion(EVA);               // Calcula si evade o no
        write(pipes[4][1], &evadesAttack, sizeof(int)); // Comunica al padre si evade o no
      }
      read(pipes[5][0], &signal, sizeof(signal)); // Señal de fin
      // Hasta acá
    }
    else if (!wrestlerPID4) 
    {
      read(pipes[7][0], &signal, sizeof(signal)); // Recibe señal para continuar

      if (round > 1)
      {
        read(pipes[7][0], &alive, sizeof(alive));
        int c = alive[0] + alive[1] + alive[2] + alive[3];
        if (c == 0 || c == 1)
        {
          break;
        }
      }

      if (alive[3])
      {
        while (isInvalid)
        {
          choice = randInt(1, 4);
          if (choice != 4 && alive[choice - 1]) // Si elección aleatoria de ataque no es el mismo luchador, es válida
          {
            isInvalid = 0;
          }
        }
        write(pipes[6][1], &choice, sizeof(int)); // Envía su elección al padre

        evadesAttack = checkEvasion(EVA);               // Calcula si evade o no
        write(pipes[6][1], &evadesAttack, sizeof(int)); // Envía lo calculado al padre
      }
      read(pipes[7][0], &signal, sizeof(signal)); // Señal de fin
      // Hasta acá
    }
  }

  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    int c = alive[0] + alive[1] + alive[2] + alive[3];
    if (c == 0)
    {
      printf("El ganador es el Luchador %i!\n\n", w);
    }
    else if (c == 1)
    {
      for (i = 0; i < 4; i++)
      {
        if (alive[i])
        {
          printf("¡El ganador es el Luchador %i!\n\n", i+1);
        }
      }
    }
    int pipesUsed[][2] = {{0, 1}, {1, 0}, {2, 1}, {3, 0}, {4, 1}, {5, 0}, {6, 1}, {7, 0}};
    closePipesNotUsed(pipes, pipesUsed, sizeof(pipesUsed) / sizeof(pipesUsed[0]));

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