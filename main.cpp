#include <iostream>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include "functions.h"

using namespace std;

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

  // Cálculo de stats
  int HP = 100;
  int ATK = randInt(30, 40);
  int DEF = randInt(10, 25);
  int EVA = 60 - DEF;

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
  else if (!playerPID || !wrestlerPID2 || !wrestlerPID3 || !wrestlerPID4)
  {
    int pipe_index, signal_index;

    // Índices utilizados en cada hijo, para saber cual pipe cerrar
    if (playerPID == 0){pipe_index = 0; signal_index = 0;}
    else if (wrestlerPID2 == 0){pipe_index = 2; signal_index = 1;}
    else if (wrestlerPID3 == 0){pipe_index = 4; signal_index = 2;}
    else if (wrestlerPID4 == 0){pipe_index = 6; signal_index = 3;}

    int pipes_used[][2] = {{pipe_index, 1}, {pipe_index + 1, 0}};
    int signal_pipes_used[][2] = {{signal_index, 0}, {signal_index + 1, 1}};

    // Se cierran los pipes no utilizados
    closePipesNotUsed(pipes, pipes_used, sizeof(pipes_used) / sizeof(pipes_used[0]));
    closeSignalPipesNotUsed(signalpipes, signal_pipes_used, sizeof(signal_pipes_used) / sizeof(signal_pipes_used[0]));

    // Se guardan las stats en array temporal
    pStat[0] = HP;
    pStat[1] = ATK;
    pStat[2] = DEF;
    pStat[3] = EVA;

    // Se envía el array al padre
    write(pipes[pipe_index][1], pStat, 4 * sizeof(int));
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

  
  int signal = 1;
  int evadesAttack = 0;
  int evadeArray[4] = {0, 0, 0, 0};
  int attacksArray[4];
  int attacked;
  bool alive[4] = {1, 1, 1, 1};
  int winner_index = -1; // índice del jugador ganador
  int noWinner = 1;

  for (round = 1; noWinner; round++)
  {
    int choice;
    bool isInvalid = 1;
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
        printf("Salud Luchador %i %s %i %s", i+1, (!i) ? " (Tú)" : ": ", stats[i][0], (stats[i][0] <=0) ? "(Muerto)\n" : "\n");
      }

      write(signalpipes[0][1], &signal, sizeof(signal)); // Señal de partida

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
      }

      // En el array attacksArray, se encuentran los blancos de ataque de cada luchador. 0: Jugador, 1: Luchador 2, etc
      // En el array evadeArray, se guardan en binario la evasión de cada luchador. Cada luchador tiene un 0 si no evade y un 1 si lo hace
      for (i = 0; i < 4; i++)
      {
        attacked = attacksArray[i];
        if (attacked != -1)
        {
          winner_index = i + 1;
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

      write(signalpipes[0][1], &signal, sizeof(signal)); // Señal de continuar
    }
    else if (!playerPID || !wrestlerPID2 || !wrestlerPID3 || !wrestlerPID4)
    {
      int sg_index; // Index de donde recibe la señal 
      int wf_index; // Index desde el luchador al padre
      int wrestler_index; // Index del luchador

      if (playerPID == 0){ sg_index = 0; wf_index = 0; wrestler_index = 0;}
      else if (wrestlerPID2 == 0){ sg_index = 1; wf_index = 2; wrestler_index = 1;}
      else if (wrestlerPID3 == 0){ sg_index = 2; wf_index = 4; wrestler_index = 2;}
      else if (wrestlerPID4 == 0){ sg_index = 3; wf_index = 6; wrestler_index = 3;} 

      read(signalpipes[sg_index][0], &signal, sizeof(signal)); // Recibe señal de continuar
      if (wrestler_index != 3)
      {
        write(signalpipes[sg_index + 1][1], &signal, sizeof(signal));// Envía señal para continuar al luchador siguiente
      }
       
      // Comprueba que existan jugadores vivos
      if (round > 1)
      {
        read(pipes[wf_index + 1][0], &alive, sizeof(alive)); // Lee el pipe del padre al luchador
        int c = alive[0] + alive[1] + alive[2] + alive[3]; // Cantidad de jugadores vivos
        if (c == 0 || c == 1)
        {
          break;
        }
      }

      if (alive[wrestler_index])
      {
        if (wrestler_index == 0) // Se encuentra en el jugador
        {
          while(isInvalid)
          {
            // Interacción con el jugador
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
        }
        else
        {
          // Interacción con los demás luchadores
          while(isInvalid)
          {
            choice = randInt(1, 4);
            if (choice != (wrestler_index+1) && alive[choice - 1]) // Si la elección aleatoria no es el mismo luchador, es válida
            {
              isInvalid = 0;
            }
          }
        }

        write(pipes[wf_index][1], &choice, sizeof(int)); // Envía su elección al padre
        evadesAttack = checkEvasion(EVA);               // Calcula evasión
        write(pipes[wf_index][1], &evadesAttack, sizeof(int)); // Envía evasión al padre
      }

      read(signalpipes[sg_index][0], &signal, sizeof(signal));  // Recibe señal de fin de ronda
      if (wrestler_index != 3)
      {
        write(signalpipes[sg_index + 1][1], &signal, sizeof(signal)); // Transmite señal al luchador siguiente
      }
      
    }
  }

  if (playerPID && wrestlerPID2 && wrestlerPID3 && wrestlerPID4)
  {
    int c = alive[0] + alive[1] + alive[2] + alive[3];
    if (c == 0)
    {
      printf("El ganador es el Luchador %i!\n\n", winner_index);
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