bool checkEvasion(double probability);

void closePipesNotUsed(int pipes[8][2], int pipesUsed[][2], int numPipesUsed);

void closeSignalPipesNotUsed(int signalPipes[5][2], int signalPipesUsed[][2], int numSignalPipesUsed);

int randInt(int low, int high);

bool checkWinner(bool alive[4]);