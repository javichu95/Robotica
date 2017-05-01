typedef short TMutex;

//TMutex test = 0;  // Important to initialize to zero. Not acquired.

/*
* Método que adquiere el semáforo.
*/
void AcquireMutex(TMutex &nMutex){

  while (true)
  {
    // Loop until mutex is obtained

   ++nMutex;
    if (nMutex == 1)
      return;  // Succeeded

    // Failed, try again
    --nMutex;
    wait1Msec(1); // To force timeslice to end and give other threads time
  }

}

/*
* Método que libera el semáforo.
*/
void ReleaseMutex(TMutex &nMutex){

  --nMutex;

}
