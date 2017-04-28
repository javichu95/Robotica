typedef short TMutex;

//TMutex test = 0;  // Important to initialize to zero. Not acquired.

/*
* M�todo que adquiere el sem�foro.
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
* M�todo que libera el sem�foro.
*/
void ReleaseMutex(TMutex &nMutex){

  --nMutex;

}
