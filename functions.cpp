#include "functions.h"
#include <random>
#include <unistd.h>

using namespace std;

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