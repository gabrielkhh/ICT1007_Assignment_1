#include <stdio.h>
#include <stdbool.h>

/*int n is the number of processes.
ProcessData[x][2] stores info like this [[BurstTime, ProcessNo#, Arrival Time],[BurstTime, ProcessNo#, Arrival Time]...]
array ct[i] is like the static burst time for each process before process is started to calculate total waiting time later on.*/
int i, j, n, totalBurst, ProcessData[10][3], staticBurst[10], ArrTime[10];
int countTrack = 0;
int LTQTrack = 0;
int HTQTrack = 0;
//Light Task Queue(LTQ), Heavy Task Queue(HTQ), Medium Burst Time(MBT)
float awt = 0, att = 0, MBT = 0, t = 0, max = 0, temp = 0, turnaroundTime[10][2], LTQ[10][3], HTQ[10][3], tempMove[2], waitingTime[10];
float MainReadyQueue[10][3];

bool AllArrivalSame = true;

//Function Prototypes
void processReadyQueue();
void sortTurnAroundTime();
void proposedRoundRobin(int m, int startOffset);

void main()
{
    printf("Enter the no of processes: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++)
    {
        //Get the burst time for each process
        printf("\nEnter Burst Time for process %d: ", i + 1);
        scanf("%d", &ProcessData[i][0]);
        ProcessData[i][1] = i + 1;
        printf("\nEnter Arrival Time for process %d: ", i + 1);
        scanf("%d", &ProcessData[i][2]);
        ArrTime[i] = ProcessData[i][2];
        staticBurst[i] = ProcessData[i][0];
    }

    //Check if the arrival time for all the processes are the same or varying.
    for (i = 0; i < n; i++) {
        if (ProcessData[i][2] != 0) {
            AllArrivalSame = false;
            break;
        }
    }

    if (AllArrivalSame == true) {
        //Populate the ready queue
        for (int g = 0; g < n; g++) {
            MainReadyQueue[g][0] = ProcessData[g][0];
            MainReadyQueue[g][1] = ProcessData[g][1];
            MainReadyQueue[g][2] = ProcessData[g][2];
        }

        //Run the proposed RR algorithm, pass in n as number of processes in ready queue, 0 as there was no process that was computed before this cycle.
        proposedRoundRobin(n, 0);

        //Sorting the turnaroundTime array so that we can print the results in running order based on Process No#.
        sortTurnAroundTime();
    } else {
        //The processes have varying arrival time.
        processReadyQueue();

        //Sorting the turnaroundTime array so that we can print the results in running order based on Process No#.
        sortTurnAroundTime();
    }


    // Calculate and print the waiting time and the turnaround time for each process.
    temp = 0;
    for (i = 0; i < n; i++)
    {
        //temp will store the cumulative waiting time which is = to turnaround time - burst time.
        temp = turnaroundTime[i][0] - (float)staticBurst[i];
        waitingTime[i] = temp;
        printf("Process %d waiting time is %.2f and turnaround time is %.2f\n", (i + 1), temp, turnaroundTime[i][0]);
    }

    // Calculate and print the average waiting time and the average turnaround time.
    float ttat = 0;
    float twt = 0;
    for (i = 0; i < n; i++)
    {
        twt += waitingTime[i];
        ttat += turnaroundTime[i][0];
    }

    //Calculating the average waiting time and the average turnaround time.
    awt = twt / (float)n;
    att = ttat / (float)n;

    printf("Average waiting time is %.2f\n", awt);
    printf("Average turnaround time is %.2f\n", att);
}

void processReadyQueue() {
    int m = 0;
    int startOffset = countTrack;

    //Populate the ready queue
    for (int g = countTrack; g < n; g++) {
        if (ProcessData[g][2] <= temp) {
            MainReadyQueue[m][0] = ProcessData[g][0];
            MainReadyQueue[m][1] = ProcessData[g][1];
            MainReadyQueue[m][2] = ProcessData[g][2];
            m++;
            countTrack++;
        }
    }

    if (m > 1) {
        //Multiple processes in ready queue

        //Run the proposed RR Algorithm
        proposedRoundRobin(m, startOffset);

        if (countTrack != n) {
            //Not all processes have been done.
            processReadyQueue();
        }


    } else {
        //Only one process in ready queue
        t = MainReadyQueue[0][0];
        //Temp will track the cumulative time so that the waiting time for subsequent processes is properly compounded.
        temp = temp + MainReadyQueue[0][0];
        //turnaroundTime[x][0] is the turnaround time (waiting time + burst time) for each process. turnaroundTime[x][1] is the process no. #
        turnaroundTime[countTrack - 1][0] = temp - MainReadyQueue[0][2];
        turnaroundTime[countTrack - 1][1] = MainReadyQueue[0][1];

        if (countTrack != n) {
            //Not all processes have been done.
            processReadyQueue();
        }
    }
}

void sortTurnAroundTime() {
    /* This function will sort the global array storing the turn around time for each process, so that printing out results
     * will be in running order of Process No#. */
    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (turnaroundTime[i][1] > turnaroundTime[j][1])
            {
                tempMove[0] = turnaroundTime[i][0];
                tempMove[1] = turnaroundTime[i][1];

                turnaroundTime[i][0] = turnaroundTime[j][0];
                turnaroundTime[i][1] = turnaroundTime[j][1];

                turnaroundTime[j][0] = tempMove[0];
                turnaroundTime[j][1] = tempMove[1];
            }
        }
    }
}

void proposedRoundRobin(int m, int startOffset) {
    /* This function contains the logic for the proposed Round Robin algorithm where processes are divided
     * into LTQ (Light task queue) and HTQ (Heavy task queue) based on the MBT (Medium burst time) of all processes
     * and then cleared in the following order LTQ then HTQ. Burst time for each queue is also based on the MBT of the
     * queue. */

    //Sort the readyqueue in ascending order based on burst time of processes.
    for (i = 0; i < m; i++)
    {
        for (j = i + 1; j < m; j++)
        {
            if (MainReadyQueue[i][0] > MainReadyQueue[j][0])
            {
                tempMove[0] = MainReadyQueue[i][0];
                tempMove[1] = MainReadyQueue[i][1];

                MainReadyQueue[i][0] = MainReadyQueue[j][0];
                MainReadyQueue[i][1] = MainReadyQueue[j][1];

                MainReadyQueue[j][0] = tempMove[0];
                MainReadyQueue[j][1] = tempMove[1];
            }
        }
    }

    //Find the MBT
    totalBurst = 0;
    for (i = 0; i < m; i++) {
        totalBurst += MainReadyQueue[i][0];
    }
    MBT = (float)totalBurst / (float)m;

    //Divide the tasks into LTQ and HTQ based on MBT
    LTQTrack = 0;
    HTQTrack = 0;
    for (i = 0; i < m; i++) {
        if ((float)MainReadyQueue[i][0] < MBT) {
            //Put in LTQ
            LTQ[LTQTrack][0] = (float)MainReadyQueue[i][0];
            LTQ[LTQTrack][1] = (float)MainReadyQueue[i][1];
            LTQTrack++;
        } else {
            //Put in HTQ
            HTQ[HTQTrack][0] = (float)MainReadyQueue[i][0];
            HTQ[HTQTrack][1] = (float)MainReadyQueue[i][1];
            HTQTrack++;
        }
    }

    //Time to clear the LTQ
    //Get the Time quantum for LTQ
    totalBurst = 0;
    for (int k = 0; k < LTQTrack; k++) {
        totalBurst += LTQ[k][0];
    }
    //The time quantum for LTQ (Light Task Queue) which is equals to the MBT (Medium Burst Time) of LTQ.
    t = (float)totalBurst / (float)LTQTrack;

    //Know the max burst time in this queue (LTQ Light Task Queue).
    max = LTQ[LTQTrack - 1][0];

    //This is the juicy part, The heart and soul of RR. We will clear LTQ first.
    //for loop where it loops for the maximum no. of times a process will ever need to queue again.
    for (j = 0; j < (max / t) + 1; j++)
    {
        //for loop for each process in LTQ
        for (i = 0; i < LTQTrack; i++)
        {
            //if the process burst time is not 0,
            if (LTQ[i][0] != 0)
            {
                //if the burst time is less than or equals to time quantum
                if (LTQ[i][0] <= t)
                {
                    //Temp will track the cumulative time so that the waiting time for subsequent processes is properly compounded.
                    temp = temp + LTQ[i][0];
                    //turnaroundTime[][] is the turnaround time (time now - arrival time) for each process.
                    turnaroundTime[startOffset + i][0] = temp - ArrTime[(int)LTQ[i][1] - 1];
                    turnaroundTime[startOffset + i][1] = LTQ[i][1];
                    //Because the remaining burst time is =< 0, after running the process, the remaining burst time must be 0, indicating
                    //that the process has completed.
                    LTQ[i][0] = 0;
                }
                else
                {
                    //Comes here when the remaining burst time is more than time quantum, so the remaining burst time is still going
                    //To be more than 0 after this cycle.
                    LTQ[i][0] = LTQ[i][0] - t;
                    temp = temp + t;
                }
            }
        }
        //Sort LTQ again based on ascending order of their remaining burst time.
        for (i = 0; i < LTQTrack; i++)
        {
            for (int a = i + 1; a < LTQTrack; a++)
            {
                if (LTQ[i][0] > LTQ[a][0])
                {
                    tempMove[0] = LTQ[i][0];
                    tempMove[1] = LTQ[i][1];

                    LTQ[i][0] = LTQ[a][0];
                    LTQ[i][1] = LTQ[a][1];

                    LTQ[a][0] = tempMove[0];
                    LTQ[a][1] = tempMove[1];
                }
            }
        }
    }

    //Now that LTQ is done, Time to process HTQ
    //Get the Time quantum for HTQ
    totalBurst = 0;
    for (int k = 0; k < HTQTrack; k++) {
        totalBurst += HTQ[k][0];
    }
    //The time quantum for HTQ (Heavy Task Queue) which is equals to the MBT (Medium Burst Time) of HTQ.
    t = (float)totalBurst / (float)HTQTrack;

    //Know the max burst time in this queue (HTQ Heavy Task Queue).
    max = HTQ[HTQTrack - 1][0];

    //RR logic lai liao. Now clear HTQ with RR.
    for (j = 0; j < (max / t) + 1; j++)
    {
        //for loop for each process
        for (i = 0; i < HTQTrack; i++)
        {
            //if the process burst time is not 0,
            if (HTQ[i][0] != 0)
            {
                //if the burst time is less than or equals to time quantum
                if (HTQ[i][0] <= t)
                {
                    //Temp will track the cumulative time so that the waiting time for subsequent processes is properly compounded.
                    temp = temp + HTQ[i][0];
                    //turnaroundTime[][] is the turnaround time (time now - arrival time) for each process.
                    turnaroundTime[(startOffset + LTQTrack) + i][0] = temp - ArrTime[(int)HTQ[i][1] - 1];
                    turnaroundTime[(startOffset + LTQTrack) + i][1] = HTQ[i][1];
                    //Because the remaining burst time is =< 0, after running the process, the remaining burst time must be 0, indicating
                    //that the process has completed.
                    HTQ[i][0] = 0;
                }
                else
                {
                    //Comes here when the remaining burst time is more than time quantum, so the remaining burst time is still going
                    //To be more than 0 after this cycle.
                    HTQ[i][0] = HTQ[i][0] - t;
                    temp = temp + t;
                }
            }
        }
        //Sort HTQ again based on ascending order of their remaining burst time.
        for (i = 0; i < HTQTrack; i++)
        {
            for (int b = i + 1; b < HTQTrack; b++)
            {
                if (HTQ[i][0] > HTQ[b][0])
                {
                    tempMove[0] = HTQ[i][0];
                    tempMove[1] = HTQ[i][1];

                    HTQ[i][0] = HTQ[b][0];
                    HTQ[i][1] = HTQ[b][1];

                    HTQ[b][0] = tempMove[0];
                    HTQ[b][1] = tempMove[1];
                }
            }
        }
    }
}

