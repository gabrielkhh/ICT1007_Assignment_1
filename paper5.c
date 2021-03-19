#include <stdio.h>

#define LOW_BT 200

// i and j used in for loops, n is the number of processes. JQ[i] is the Job Queue Flag.
int i, j, n, doneProcesses, JQ[10], MainReadyQueue[10][4], contextSwitches;
/* Format is something like MainReadyQueue[[ProcessNo#, BurstTime, ArrTime, Priority], [ProcessNo#, BurstTime, ArrTime, Priority]]
lowTQ, medTQ, highTQ is the TQ for the various priorities and the s suffix infront is for the if statements */
float TQ = 0, lowTQ = 0, highTQ = 0, sLowTQ = 0, sMedTQ = 0, sHighTQ = 0, effectiveTQ = 0, sEffectiveTQ = 0, remainingBurstTime[10][2], tat[10], wt[10];
// ttat = total turnaround time, twt = total waiting time, awt = avg Waiting time, atat = avg turnaround time.
float ttat = 0;
float twt = 0;
float awt = 0;
float atat = 0;

//Time now is 0
float t = 0;

void main()
{
    doneProcesses = 0;

    printf("Enter the no of processes: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++)
    {
        // Fill in the Process No #.
        MainReadyQueue[i][0] = i + 1;
        // Fill in the Burst Time
        printf("Enter burst time for process %d: ", i + 1);
        scanf("%d", &MainReadyQueue[i][1]);
        remainingBurstTime[i][0] = i + 1;
        remainingBurstTime[i][1] = MainReadyQueue[i][1];
        // Fill in the Arr Time
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &MainReadyQueue[i][2]);
        // Fill in the priority
        printf("Enter priority time for process %d: ", i + 1);
        scanf("%d", &MainReadyQueue[i][3]);

        JQ[i] = 0;
    }
    printf("Enter Time Quantum: ");
    scanf("%f", &TQ);

    lowTQ = (float)0.8 * TQ;
    highTQ = (float)1.2 * TQ;
    sLowTQ = (float)(lowTQ + (0.2 * lowTQ));
    sMedTQ = (float)(TQ + (0.2 * TQ));
    sHighTQ = (float)(highTQ + (0.3 * highTQ));

    //Check for any processes that have lowBT (<200) and finish it first.
    for (i = 0; i < n; i++)
    {
        if ((remainingBurstTime[i][1] < (float)LOW_BT) && (JQ[i] == 0) && ((float)MainReadyQueue[i][2] <= t)) {
            //Burst time is low (less than 200) and that the flag is 0 (Not done processing yet)
            //According to algo, we just finish it and set flag to true.
            t += remainingBurstTime[i][1];
            remainingBurstTime[i][1] = 0;
            //Set the turnaround time (time now - arr time)
            tat[i] = t - (float)MainReadyQueue[i][2];
            //Set the waiting time (turnaround time - burst time)
            wt[i] = tat[i] - (float)MainReadyQueue[i][1];
            JQ[i] = 1;
            doneProcesses++;
            contextSwitches++;
            //For gantt chart
            printf("|\tP%d (%.2f)", i + 1, t);
        }
    }

    //Now the tough part lai liao, need to check the against TQ and burst time like siao..
    //While not all processes are done...
    while (doneProcesses != n)
    {
        for (i = 0; i < n; i++)
        {
            //Check if process has arrived at the ready queue.
            if ((float)MainReadyQueue[i][2] <= t) {
                //We need to know what is its TQ first via the priority number
                if (MainReadyQueue[i][3] == 1) {
                    effectiveTQ = lowTQ;
                    sEffectiveTQ = sLowTQ;
                } else if (MainReadyQueue[i][3] == 2) {
                    effectiveTQ = TQ;
                    sEffectiveTQ = sMedTQ;
                } else if (MainReadyQueue[i][3] == 3) {
                    effectiveTQ = highTQ;
                    sEffectiveTQ = sHighTQ;
                }

                //Now is about checking how long we are gonna run the process
                if ((remainingBurstTime[i][1] <= effectiveTQ) && (JQ[i] == 0)) {
                    //According to algo, we just finish it and set flag to true.
                    t += remainingBurstTime[i][1];
                    remainingBurstTime[i][1] = 0;
                    //Set the turnaround time (time now - arr time)
                    tat[i] = t - (float)MainReadyQueue[i][2];
                    //Set the waiting time (turnaround time - burst time)
                    wt[i] = tat[i] - (float)MainReadyQueue[i][1];
                    JQ[i] = 1;
                    doneProcesses++;
                    contextSwitches++;
                    //For gantt chart
                    printf("|\tP%d (%.2f)", i + 1, t);
                } else if ((MainReadyQueue[i][3] == 1 || MainReadyQueue[i][3] == 2) && (JQ[i] == 0)) {
                    //Priority is medium or Low
                    if ((remainingBurstTime[i][1] > effectiveTQ) && (remainingBurstTime[i][1] <= sEffectiveTQ)) {
                        //According to algo, we just finish it and set flag to true.
                        t += remainingBurstTime[i][1];
                        remainingBurstTime[i][1] = 0;
                        //Set the turnaround time (time now - arr time)
                        tat[i] = t - (float)MainReadyQueue[i][2];
                        //Set the waiting time (turnaround time - burst time)
                        wt[i] = tat[i] - (float)MainReadyQueue[i][1];
                        JQ[i] = 1;
                        doneProcesses++;
                        contextSwitches++;
                        //For gantt chart
                        printf("|\tP%d (%.2f)", i + 1, t);
                    } else {
                        //Just run for its assigned TQ..
                        t += effectiveTQ;
                        remainingBurstTime[i][1] = remainingBurstTime[i][1] - effectiveTQ;
                        contextSwitches++;
                        //For gantt chart
                        printf("|\tP%d (%.2f)", i + 1, t);
                    }
                } else if ((MainReadyQueue[i][3] == 3) && (JQ[i] == 0)) {
                    //Priority is high
                    if ((remainingBurstTime[i][1] > effectiveTQ) && (remainingBurstTime[i][1] <= sEffectiveTQ)) {
                        //According to algo, we just finish it and set flag to true.
                        t += remainingBurstTime[i][1];
                        remainingBurstTime[i][1] = 0;
                        //Set the turnaround time (time now - arr time)
                        tat[i] = t - (float)MainReadyQueue[i][2];
                        //Set the waiting time (turnaround time - burst time)
                        wt[i] = tat[i] - (float)MainReadyQueue[i][1];
                        JQ[i] = 1;
                        doneProcesses++;
                        contextSwitches++;
                        printf("|\tP%d (%.2f)", i + 1, t);
                    } else {
                        //Just run for its assigned TQ..
                        t += effectiveTQ;
                        remainingBurstTime[i][1] = remainingBurstTime[i][1] - effectiveTQ;
                        contextSwitches++;
                        //For gantt chart
                        printf("|\tP%d (%.2f)", i + 1, t);
                    }
                }
            }
        }
    }

    //End of gantt chart line.
    printf("\n\n");

    // Loop through and print out information about each process.
    for (i = 0; i < n; i++)
    {
        printf("Process %d waiting time is %.2f and turnaround time is %.2f\n", (i + 1), wt[i], tat[i]);
    }

    // Calculate the total waiting time and total turnaround time.
    for (i = 0; i < n; i++)
    {
        twt += wt[i];
        ttat += tat[i];
    }

    // Calculate average waiting time
    awt = twt / (float)n;
    // Calculate average turnaround time
    atat = ttat / (float)n;

    printf("Average waiting time is %.2f\n", awt);
    printf("Average turnaround time is %.2f\n", atat);
    printf("Number of context switches : %d\n", contextSwitches - 1);
}
