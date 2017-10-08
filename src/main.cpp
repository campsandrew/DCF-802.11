#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <random>

#define DATA_FRAME_SIZE_SLOTS 100 /* slots */
#define DATA_FRAME_SIZE 12000 /* bits */
#define CW_MAX 8
#define CW_0 4 /* slots */
#define SIFS_SIZE 1 /* slots */
#define ACK_SIZE 2 /* slots */
#define RTS_SIZE 2 /* slots */
#define CTS_SIZE 2 /* slots */
#define DIFS_SIZE 2 /* slots */
#define SIM_DURATION_CLOCK 250000//500000 /* slots */
#define SIM_DURATION 5//10 /* seconds */
#define SLOT_DURATION 20 /* microseconds */

#define NUM_RATES 10
#define SLACK 200

#define NUM_TESTS 1000

#define DEBUG 0

typedef struct Node_struct {
    int* packet_arrivals;
    int dist_len;
    int packet_success;
    int packet_collision;
} Node;

void ScenarioA_CSMA(Node* nodeA, Node* nodeB);
void ScenarioA_VCS(Node* nodeA, Node* nodeB);
void ScenarioB_CSMA(Node* nodeA, Node* nodeB);
void ScenarioB_VCS(Node* nodeA, Node* nodeB);
void Data_toFile(char* filename);
int Get_Backoff(int fails);
Node* Get_Packet_Dist(int rate);

int main(){
    int Frame_Rates[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900};
    
    unsigned long AvgCollision[NUM_RATES];
    unsigned long AvgSuccessA[NUM_RATES];
    unsigned long AvgSuccessC[NUM_RATES];
    double AvgThroughputA[NUM_RATES];
    double AvgThroughputC[NUM_RATES];
    unsigned long AvgCollision2[NUM_RATES];
    unsigned long AvgSuccessA2[NUM_RATES];
    unsigned long AvgSuccessC2[NUM_RATES];
    double AvgThroughputA2[NUM_RATES];
    double AvgThroughputC2[NUM_RATES];
    
    unsigned long AvgPacketsA[NUM_RATES];
    unsigned long AvgPacketsC[NUM_RATES];
    
    
    int j;
    
    for(j = 0; j < NUM_RATES; j++){
        AvgCollision[j] = 0;
        AvgSuccessA[j] = 0;
        AvgSuccessC[j] = 0;
        AvgThroughputA[j] = 0;
        AvgThroughputC[j] = 0;
        AvgCollision2[j] = 0;
        AvgSuccessA2[j] = 0;
        AvgSuccessC2[j] = 0;
        AvgThroughputA2[j] = 0;
        AvgThroughputC2[j] = 0;
        AvgPacketsA[j] = 0;
        AvgPacketsC[j] = 0;
    }
    
    for(j = 0; j < NUM_TESTS; j++){
        for(int i = 0; i < NUM_RATES; i++){
            /* Establishing the nodes and packet data */
            Node* A = Get_Packet_Dist(Frame_Rates[i]);
            Node* C = Get_Packet_Dist(Frame_Rates[i]);
            
            AvgPacketsA[i] = AvgPacketsA[i] + A->dist_len;
            AvgPacketsC[i] = AvgPacketsC[i] + C->dist_len;
            
            /* TEST SIMULATIONS */
            ScenarioA_CSMA(A, C);
            
            AvgCollision[i] = AvgCollision[i] + A->packet_collision;
            AvgSuccessA[i] = AvgSuccessA[i] + A->packet_success;
            AvgSuccessC[i] = AvgSuccessC[i] + C->packet_success;
            
            /*
            ScenarioA_VCS(A, C);
            //ScenarioB_CSMA(A, C);
            //ScenarioB_VCS(A, C);
            
            AvgCollision2[i] = AvgCollision2[i] + A->packet_collision;
            AvgSuccessA2[i] = AvgSuccessA2[i] + A->packet_success;
            AvgSuccessC2[i] = AvgSuccessC2[i] + C->packet_success;
            */
            //printf("CHECK: %lu\n", AvgCollision[2]);
            
        }
    }
    
    int k = 0;
    
    for(k = 0; k < NUM_RATES; k++){
        // ScenarioA_CSMA
        AvgCollision[k] = (AvgCollision[k] / j);
        AvgSuccessA[k] = (AvgSuccessA[k] / j);
        AvgSuccessC[k] = (AvgSuccessC[k] / j);
        
        AvgThroughputA[k] = ((AvgSuccessA[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        AvgThroughputC[k] = ((AvgSuccessC[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        
        /*
        // ScenarioA_VCS
        AvgCollision2[k] = (AvgCollision2[k] / j);
        AvgSuccessA2[k] = (AvgSuccessA2[k] / j);
        AvgSuccessC2[k] = (AvgSuccessC2[k] / j);
        
        AvgThroughputA2[k] = ((AvgSuccessC2[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        AvgThroughputC2[k] = ((AvgSuccessC2[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
         */
        
        // ScenarioB_CSMA
        // ScenarioB_VCS
    }
    
    
    printf("\nSTART CSMA\n");
    for(k = 0; k < NUM_RATES; k++){
        
        printf("RATE: %d frames/sec\n\tThroughput Avg A: %f Mbps\n\tThroughput Avg C: %f Mbps\n\tCollision Avg: %lu\n\n", Frame_Rates[k], AvgThroughputA[k], AvgThroughputC[k], AvgCollision[k]);
        
    }
    /*
    printf("\nSTART VCS\n");
    for(k = 0; k < NUM_RATES; k++){
        
        printf("RATE: %d frames/sec\n\tThroughput Avg A: %f Mbps\n\tThroughput Avg C: %f Mbps\n\tCollision Avg: %lu\n\n", Frame_Rates[k], AvgThroughputA2[k], AvgThroughputC2[k], AvgCollision2[k]);
        
    }
     */
    
    /*
    printf("\nSTART CSMA\n");
    
    for(k = 0; k < NUM_RATES; k++){
        
        printf("FINAL\n\n\tCollision Avg: %lu\n\tSuccess Avg A: %lu\n\tNum Packet A: %lu\n\tSuccess Avg C: %lu\n\tNum Packet C: %lu\n", (AvgCollision[k] / j), (AvgSuccessA[k] / j), (AvgPacketsA[k] / j), (AvgSuccessC[k] / j), (AvgPacketsC[k] / j));
        
    }
    printf("\nSTART VCS\n");
    
    for(k = 0; k < NUM_RATES; k++){
        
        printf("FINAL\n\n\tCollision Avg: %lu\n\tSuccess Avg A: %lu\n\tNum Packet A: %lu\n\tSuccess Avg C: %lu\n\tNum Packet C: %lu\n", (AvgCollision2[k] / j), (AvgSuccessA2[k] / j), (AvgPacketsA[k] / j), (AvgSuccessC2[k] / j), (AvgPacketsC[k] / j));
        
    }
     */

	return 0;
}

/*
 *
 *
 *
 */
void ScenarioA_CSMA(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator

    int Clock = 0;
    int curPacketA = 0;
    int curPacketB = 0;
    int curBackoffA = 0;
    int curBackoffB = 0;
    int curCollisionsA = 0;
    int curCollisionsB = 0;
    int totCollisions = 0;
    int packet_successA = 0;
    int packet_successB = 0;
    
    /* Loop till end of simulation */
    while(Clock < SIM_DURATION_CLOCK){
    
#if DEBUG
        printf("\nNodeA: %d, NodeB: %d, CLock: %d\n", nodeA->packet_arrivals[curPacketA], nodeB->packet_arrivals[curPacketB], Clock);
#endif
        
        /* Clock is ahead of both packets */
        if(Clock >= nodeA->packet_arrivals[curPacketA] && Clock >= nodeB->packet_arrivals[curPacketB]){
            // DON'T UPDATE CLOCK
#if DEBUG
            printf("\t\t\t\tHERE: B equal A\n");
            printf("CURRENT BACKOFF A(%d)\n", curBackoffA);
#endif
            if(curBackoffA == 0){
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
#if DEBUG
                printf("NEW BACKOFF A(%d)\n", curBackoffA);
#endif
            }
#if DEBUG
            printf("CURRENT BACKOFF B(%d)\n", curBackoffB);
#endif
            if(curBackoffB == 0){
                curBackoffB = Get_Backoff(curCollisionsB);
#if DEBUG
                printf("NEW BACKOFF B(%d)\n", curBackoffB);
#endif
            }
            
            int compPeriodA = curBackoffA;
            int compPeriodB = curBackoffB;
            Clock = Clock + DIFS_SIZE; // Update clock to beginning of competition period
            
            /* Check if there will be a collision */
            if(compPeriodB == compPeriodA){ /* Collision */
                Clock = Clock + compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock to where ack should be received
#if DEBUG
                printf("\t\t----->COLLISION A equal B, CLOCK: %d\n", Clock);
#endif
                curBackoffA = 0;
                curBackoffB = 0;
                // There is a collision, update counters //
                curCollisionsB++;
                curCollisionsA++;
                totCollisions++;
            } /* If no collison, send which packet wins */
            else{
                /* NodeB transmits first successfully */
                if(compPeriodB < compPeriodA){
                    Clock = Clock + compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock till end of transmission
#if DEBUG
                    printf("\t\t->SUCCESS B, CLOCK: %d\n", Clock);
#endif
                    packet_successB++; // successful packet transmit
                    curCollisionsB = 0;
                    curPacketB++;
                    
                    // Reduces backoff counter of packet that didnt send //
                    curBackoffA = curBackoffA - compPeriodB;
                    curBackoffB = 0;
                } /* NodeA transmits first successfully */
                else{
                    Clock = Clock + compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock till end of transmission
#if DEBUG
                    printf("\t\t->SUCCESS A, CLOCK: %d\n", Clock);
#endif
                    packet_successA++; // successful packet transmit
                    curCollisionsA = 0;
                    curPacketA++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffB = curBackoffB - compPeriodA;
                    curBackoffA = 0;
                }
            }
            
            
        }
        
        /* Clock is ahead of nodeA packets and behind to nodeB packets */
        else if(Clock >= nodeA->packet_arrivals[curPacketA]){
            // DON'T UPDATE CLOCK
#if DEBUG
            printf("\t\t\t\tHERE: A before B\n");
            printf("CURRENT BACKOFF A(%d)\n", curBackoffA);
#endif
            if(curBackoffA == 0){
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time for A
#if DEBUG
                printf("NEW BACKOFF A(%d)\n", curBackoffA);
#endif
            }
            int compPeriodA = DIFS_SIZE + curBackoffA; // Calculate competition period for A
            
            /* No possible competition or Collision/Packet race */
            if((Clock + compPeriodA) <= (nodeB->packet_arrivals[curPacketB] - 1)){
                Clock = Clock + compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock after transmission
#if DEBUG
                printf("\t\t->AB-SUCCESS A, CLOCK: %d\n", Clock);
#endif
                packet_successA++; // successful packet transmit
                curCollisionsA = 0;
                curBackoffA = 0;
                curPacketA++;
            } /* Collision/Packet race */
            else{
#if DEBUG
                printf("CURRENT BACKOFF B(%d)\n", curBackoffB);
#endif
                if(curBackoffB == 0){
                    curBackoffB = Get_Backoff(curCollisionsB); // Gets the backoff time for B
#if DEBUG
                    printf("NEW BACKOFF B(%d)\n", curBackoffB);
#endif
                    
                }
                int compPeriodB = DIFS_SIZE + curBackoffB;
#if DEBUG
                printf("Comp Period B: %d\n", compPeriodB);
#endif
                int compDiff = nodeB->packet_arrivals[curPacketB] - Clock;
#if DEBUG
                printf("NEVER SHOULD BE NEG: %d\n", compDiff);
#endif
                Clock += compDiff; // Updating clock to beginning of competition period
                compPeriodA = compPeriodA - compDiff; // Removing time from the compPeriod of A
                
                /* Collision */
                if(compPeriodA == compPeriodB){
                    Clock = Clock + compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock to where ack should be received
#if DEBUG
                    printf("\t\t----->COLLISION A before B, CLOCK: %d\n", Clock);
#endif
                    curBackoffA = 0;
                    curBackoffB = 0;
                    curCollisionsA++; // Know there is a collision, update counters
                    curCollisionsB++;
                    totCollisions++;
                } /* If no collison, send which packet wins */
                else{
                    
                    /* NodeA transmits first */
                    if(compPeriodA < compPeriodB){
                        Clock = Clock + compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->AB SUCCESS A, CLOCK: %d, Comp A %d\n", Clock, compPeriodA);
#endif
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        curBackoffB = curBackoffB - (compPeriodA - DIFS_SIZE);
                        curBackoffA = 0;
                    } /* NodeB transmits first */
                    else{
                        Clock = Clock + compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->AB SUCCESS B, CLOCK: %d, Comp B %d\n", Clock, compPeriodB);
#endif
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffA = (curBackoffA - compPeriodB) + 1;
                        }else{
                            curBackoffA = curBackoffA - compPeriodB;
                        }
                        curBackoffB = 0;
                    }
                }
            }
        }
        
        /* Clock is ahead of nodeB packets and behind to nodeA packets */
        else if(Clock >= nodeB->packet_arrivals[curPacketB]){
            // DON'T UPDATE CLOCK
#if DEBUG
            printf("\t\t\t\tHERE: B before A\n");
            printf("CURRENT BACKOFF B(%d)\n", curBackoffB);
#endif
            if(curBackoffB == 0){
                curBackoffB = Get_Backoff(curCollisionsB); // Gets the backoff time for B
#if DEBUG
                printf("NEW BACKOFF B(%d)\n", curBackoffB);
#endif
            }
            int compPeriodB = DIFS_SIZE + curBackoffB; // Calculate competition period for B
            
            /* No possible competition or Collision/Packet race */
            if((Clock + compPeriodB) <= (nodeA->packet_arrivals[curPacketA] - 1)){
                Clock = Clock + compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock after transmission
#if DEBUG
                printf("\t\t->BA-SUCCESS B, CLOCK: %d\n", Clock);
#endif
                packet_successB++; // successful packet transmit
                curCollisionsB = 0;
                curBackoffB = 0;
                curPacketB++;
            } /* Collision/Packet race */
            else{
#if DEBUG
                printf("CURRENT BACKOFF A(%d)\n", curBackoffA);
#endif
                if(curBackoffA == 0){
                    curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time for A
#if DEBUG
                    printf("NEW BACKOFF A(%d)\n", curBackoffA);
#endif
                }
                int compPeriodA = DIFS_SIZE + curBackoffA;
#if DEBUG
                printf("Comp Period A: %d\n", compPeriodA);
#endif
                int compDiff = nodeA->packet_arrivals[curPacketA] - Clock;
#if DEBUG
                printf("NEVER SHOULD BE NEG: %d\n", compDiff);
#endif
                Clock += compDiff; // Updating clock to beginning of competition period
                compPeriodB = compPeriodB - compDiff; // Removing time from the compPeriod of B
                
                /* Collision */
                if(compPeriodB == compPeriodA){
                    Clock = Clock + compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock to where ack should be received
#if DEBUG
                    printf("\t\t----->COLLISION B before A, CLOCK: %d\n", Clock);
#endif
                    curBackoffA = 0;
                    curBackoffB = 0;
                    curCollisionsA++; // Know there is a collision, update counters
                    curCollisionsB++;
                    totCollisions++;
                } /* If no collison, send which packet wins */
                else{
                    
                    /* NodeB transmits first */
                    if(compPeriodB < compPeriodA){
                        Clock = Clock + compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->BA SUCCESS B, CLOCK: %d, Comp B %d\n", Clock, compPeriodB);
#endif
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        curBackoffA = curBackoffA - (compPeriodB - DIFS_SIZE);
                        curBackoffB = 0;
                    } /* NodeA transmits first */
                    else{
                        Clock = Clock + compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->BA SUCCESS A, CLOCK: %d, Comp A %d\n", Clock, compPeriodA);
#endif
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffB = (curBackoffB - compPeriodA) + 1;
                        }else{
                            curBackoffB = curBackoffB - compPeriodA;
                        }
                        curBackoffA = 0;
                    }
                }
            }
        }
        
        /* Clock is behind both nodeA and nodeB packets */
        else{
#if DEBUG
            printf("\t\t\t\tHERE: Clock update\n");
#endif
            
            /* NodeA has a packet that comes first */
            if(nodeA->packet_arrivals[curPacketA] < nodeB->packet_arrivals[curPacketB]){
                Clock += (nodeA->packet_arrivals[curPacketA] - Clock); // Update clock to catch it up
            } /* NodeB has a packet that comes first */
            else if(nodeA->packet_arrivals[curPacketA] > nodeB->packet_arrivals[curPacketB]){
                Clock += (nodeB->packet_arrivals[curPacketB] - Clock); // Update clock to catch it up
            } /* NodeA and NodeB packets come at the same time */
            else{
                Clock += (nodeA->packet_arrivals[curPacketA] - Clock); // Update clock to catch it up
            }
        }
    }

    //printf("\n\nNodeA Success: %d\n", packet_successA);
    //printf("NodeB Success: %d\n", packet_successB);
    //printf("Total Collisions: %d\n\n", totCollisions);
    
    nodeA->packet_success = packet_successA;
    nodeB->packet_success = packet_successB;
    nodeA->packet_collision = totCollisions;
    nodeB->packet_collision = totCollisions;
    
}

/*
 *
 *
 *
 */
void ScenarioA_VCS(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator
    
    int Clock = 0;
    int curPacketA = 0;
    int curPacketB = 0;
    int curBackoffA = 0;
    int curBackoffB = 0;
    int curCollisionsA = 0;
    int curCollisionsB = 0;
    int totCollisions = 0;
    int packet_successA = 0;
    int packet_successB = 0;
    
    while(Clock < SIM_DURATION_CLOCK){
        
        /* Checks if NodeA has a packet that comes first */
        if(nodeA->packet_arrivals[curPacketA] < nodeB->packet_arrivals[curPacketB]){
            if(Clock < nodeA->packet_arrivals[curPacketA]){ // Check if next packet is ahead of clock
                Clock += nodeA->packet_arrivals[curPacketA] - Clock;
            }
            if(!curBackoffA){
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
            }
            int compPeriodA = DIFS_SIZE + curBackoffA; // Calculate competition period
            
            /* Check for competition cases: No possible competition or Collision/Packet race */
            if((Clock + compPeriodA) <= nodeB->packet_arrivals[curPacketB] + 1){  // No other packet able to collide
                Clock += compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                packet_successA++; // successful packet transmit
                curCollisionsA = 0;
                curPacketA++;
            }else{ /* Collision/Packet race */
                curBackoffB = Get_Backoff(curCollisionsB);
                int compPeriodB = DIFS_SIZE + curBackoffB;
                int compDiff = nodeB->packet_arrivals[curPacketB] - Clock;
                Clock += compDiff; // Getting location where both packets are in the competition period
                compPeriodA -= compDiff; // Removing time from the compPeriod of packet that entered period first
                
                /* Check if there will be a collision */
                if(compPeriodA == compPeriodB){ /* Collision */
                    Clock += compPeriodA + RTS_SIZE + SIFS_SIZE + CTS_SIZE; // Increase clock to where cts should be received
                    curCollisionsA++; // Know there is a collision, update counters
                    curCollisionsB++;
                    totCollisions++;
                }else{ /* If no collison, send which packet wins */
                    if(compPeriodA < compPeriodB){ // NodeA transmits first successfully
                        Clock += compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Advance clock till the end of transmission
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff > DIFS_SIZE){
                            curBackoffB = curBackoffB - compPeriodA + 1;
                        }else{
                            curBackoffB -= compPeriodA;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffA = curBackoffA - compPeriodB + 1;
                        }else{
                            curBackoffA -= compPeriodB;
                        }
                    }
                }
            }
            
            /* Checks if NodeB has a packet that comes first */
        }else if(nodeA->packet_arrivals[curPacketA] > nodeB->packet_arrivals[curPacketB]){
            if(Clock < nodeB->packet_arrivals[curPacketB]){ // Check if next packet is ahead of clock
                Clock += nodeB->packet_arrivals[curPacketB] - Clock;
            }
            if(!curBackoffB){
                curBackoffB = Get_Backoff(curCollisionsB); // Gets the backoff time
            }
            int compPeriodB = DIFS_SIZE + curBackoffB; // Calculate competition period
            
            /* Check for competition cases: No possible competition or Collision/Packet race */
            if((Clock + compPeriodB) <= nodeA->packet_arrivals[curPacketA] + 1){  // No other packet able to collide
                Clock += compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                packet_successB++; // successful packet transmit
                curCollisionsB = 0;
                curPacketB++;
            }else{ /* Collision/Packet race */
                curBackoffA = Get_Backoff(curCollisionsA);
                int compPeriodA = DIFS_SIZE + curBackoffA;
                int compDiff = nodeA->packet_arrivals[curPacketA] - Clock;
                Clock += compDiff; // Getting location where both packets are in the competition period
                compPeriodA -= compDiff; // Removing time from the compPeriod of packet that entered period first
                
                /* Check if there will be a collision */
                if(compPeriodB == compPeriodA){ /* Collision */
                    Clock += compPeriodB + RTS_SIZE + SIFS_SIZE + CTS_SIZE; // Increase clock to where ack should be received
                    curCollisionsB++; // Know there is a collision, update counters
                    curCollisionsA++;
                    totCollisions++;
                }else{ /* If no collison, send which packet wins */
                    if(compPeriodB < compPeriodA){ // NodeB transmits first successfully
                        Clock += compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Advance clock till the end of transmission
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff > DIFS_SIZE){
                            curBackoffB = curBackoffB - compPeriodA + 1;
                        }else{
                            curBackoffB -= compPeriodA;
                        }
                    }else{ // NodeA transmits first successfully
                        Clock += compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffA = curBackoffA - compPeriodB + 1;
                        }else{
                            curBackoffA -= compPeriodB;
                        }
                    }
                }
            }
            
            /* There is a tie and boths nodes have a packet that comes at the same time */
        }else{
            if(Clock < nodeA->packet_arrivals[curPacketA]){ // Check if next packet is ahead of clock
                Clock += nodeA->packet_arrivals[curPacketA] - Clock;
            }
            if(!curBackoffA)
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
            if(!curBackoffB)
                curBackoffB = Get_Backoff(curCollisionsB);
            
            int compPeriodA = curBackoffA;
            int compPeriodB = curBackoffB;
            Clock += DIFS_SIZE; // Getting location where both packets are in the competition period
            
            /* Check if there will be a collision */
            if(compPeriodB == compPeriodA){ /* Collision */
                Clock += compPeriodB + RTS_SIZE + SIFS_SIZE + CTS_SIZE; // Increase clock to where ack should be received
                curCollisionsB++; // Know there is a collision, update counters
                curCollisionsA++;
                totCollisions++;
            }else{ /* If no collison, send which packet wins */
                if(compPeriodB < compPeriodA){ // NodeB transmits first successfully
                    Clock += compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Advance clock till the end of transmission
                    packet_successB++; // successful packet transmit
                    curCollisionsB = 0;
                    curPacketB++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffA -= compPeriodB;
                }else{ // NodeA transmits first successfully
                    Clock += compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                    packet_successA++; // successful packet transmit
                    curCollisionsA = 0;
                    curPacketA++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffB -= compPeriodA;
                }
            }
        }
    }
    
    //printf("\n\nNodeA Success: %d\n", packet_successA);
    //printf("NodeB Success: %d\n", packet_successB);
    //printf("Total Collisions: %d\n\n", totCollisions);
    
    nodeA->packet_success = packet_successA;
    nodeB->packet_success = packet_successB;
    nodeA->packet_collision = totCollisions;
    nodeB->packet_collision = totCollisions;

}

/*
 *
 *
 *
 */
void ScenarioB_CSMA(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator
    
    int Clock = 0;
    int curPacketA = 0;
    int curPacketB = 0;
    int curBackoffA = 0;
    int curBackoffB = 0;
    int curCollisionsA = 0;
    int curCollisionsB = 0;
    int totCollisions = 0;
    int packet_successA = 0;
    int packet_successB = 0;
    
    while(Clock < SIM_DURATION_CLOCK){
        
        /* Checks if NodeA has a packet that comes first */
        if(nodeA->packet_arrivals[curPacketA] < nodeB->packet_arrivals[curPacketB]){
            if(Clock < nodeA->packet_arrivals[curPacketA]){ // Check if next packet is ahead of clock
                Clock += nodeA->packet_arrivals[curPacketA] - Clock;
            }
            if(!curBackoffA){
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
            }
            int compPeriodA = DIFS_SIZE + curBackoffA; // Calculate competition period
            
            /* Check for competition cases: No possible competition or Collision/Packet race */
            if((Clock + compPeriodA) <= nodeB->packet_arrivals[curPacketB] + 1){  // No other packet able to collide
                Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                packet_successA++; // successful packet transmit
                curCollisionsA = 0;
                curPacketA++;
            }else{ /* Collision/Packet race */
                curBackoffB = Get_Backoff(curCollisionsB);
                int compPeriodB = DIFS_SIZE + curBackoffB;
                int compDiff = nodeB->packet_arrivals[curPacketB] - Clock;
                Clock += compDiff; // Getting location where both packets are in the competition period
                compPeriodA -= compDiff; // Removing time from the compPeriod of packet that entered period first
                
                /* Check if there will be a collision */
                if(compPeriodA == compPeriodB){ /* Collision */
                    Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Increase clock to where ack should be received
                    curCollisionsA++; // Know there is a collision, update counters
                    curCollisionsB++;
                    totCollisions++;
                }else{ /* If no collison, send which packet wins */
                    if(compPeriodA < compPeriodB){ // NodeA transmits first successfully
                        Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock till the end of transmission
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff > DIFS_SIZE){
                            curBackoffB = curBackoffB - compPeriodA + 1;
                        }else{
                            curBackoffB -= compPeriodA;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffA = curBackoffA - compPeriodB + 1;
                        }else{
                            curBackoffA -= compPeriodB;
                        }
                    }
                }
            }
            
            /* Checks if NodeB has a packet that comes first */
        }else if(nodeA->packet_arrivals[curPacketA] > nodeB->packet_arrivals[curPacketB]){
            if(Clock < nodeB->packet_arrivals[curPacketB]){ // Check if next packet is ahead of clock
                Clock += nodeB->packet_arrivals[curPacketB] - Clock;
            }
            if(!curBackoffB){
                curBackoffB = Get_Backoff(curCollisionsB); // Gets the backoff time
            }
            int compPeriodB = DIFS_SIZE + curBackoffB; // Calculate competition period
            
            /* Check for competition cases: No possible competition or Collision/Packet race */
            if((Clock + compPeriodB) <= nodeA->packet_arrivals[curPacketA] + 1){  // No other packet able to collide
                Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                packet_successB++; // successful packet transmit
                curCollisionsB = 0;
                curPacketB++;
            }else{ /* Collision/Packet race */
                curBackoffA = Get_Backoff(curCollisionsA);
                int compPeriodA = DIFS_SIZE + curBackoffA;
                int compDiff = nodeA->packet_arrivals[curPacketA] - Clock;
                Clock += compDiff; // Getting location where both packets are in the competition period
                compPeriodA -= compDiff; // Removing time from the compPeriod of packet that entered period first
                
                /* Check if there will be a collision */
                if(compPeriodB == compPeriodA){ /* Collision */
                    Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Increase clock to where ack should be received
                    curCollisionsB++; // Know there is a collision, update counters
                    curCollisionsA++;
                    totCollisions++;
                }else{ /* If no collison, send which packet wins */
                    if(compPeriodB < compPeriodA){ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock till the end of transmission
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff > DIFS_SIZE){
                            curBackoffB = curBackoffB - compPeriodA + 1;
                        }else{
                            curBackoffB -= compPeriodA;
                        }
                    }else{ // NodeA transmits first successfully
                        Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compPeriodA - DIFS_SIZE > 0){
                            curBackoffB -= compPeriodA - DIFS_SIZE;
                        }
                    }
                }
            }
            
            /* There is a tie and boths nodes have a packet that comes at the same time */
        }else{
            if(Clock < nodeA->packet_arrivals[curPacketA]){ // Check if next packet is ahead of clock
                Clock += nodeA->packet_arrivals[curPacketA] - Clock;
            }
            if(!curBackoffA)
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
            if(!curBackoffB)
                curBackoffB = Get_Backoff(curCollisionsB);
            
            int compPeriodA = curBackoffA;
            int compPeriodB = curBackoffB;
            Clock += DIFS_SIZE; // Getting location where both packets are in the competition period
            
            /* Check if there will be a collision */
            if(compPeriodB == compPeriodA){ /* Collision */
                Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Increase clock to where ack should be received
                curCollisionsB++; // Know there is a collision, update counters
                curCollisionsA++;
                totCollisions++;
            }else{ /* If no collison, send which packet wins */
                if(compPeriodB < compPeriodA){ // NodeB transmits first successfully
                    Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock till the end of transmission
                    packet_successB++; // successful packet transmit
                    curCollisionsB = 0;
                    curPacketB++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffA -= compPeriodB;
                }else{ // NodeA transmits first successfully
                    Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                    packet_successA++; // successful packet transmit
                    curCollisionsA = 0;
                    curPacketA++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffB -= compPeriodA;
                }
            }
        }
    }
    
    //printf("\n\nNodeA Success: %d\n", packet_successA);
    //printf("NodeB Success: %d\n", packet_successB);
    //printf("Total Collisions: %d\n\n", totCollisions);
    
    nodeA->packet_success = packet_successA;
    nodeB->packet_success = packet_successB;
    nodeA->packet_collision = totCollisions;
    nodeB->packet_collision = totCollisions;
    
}

/*
 *
 *
 *
 */
void ScenarioB_VCS(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator
    
    int Clock = 0;
    int curPacketA = 0;
    int curPacketB = 0;
    int curBackoffA = 0;
    int curBackoffB = 0;
    int curCollisionsA = 0;
    int curCollisionsB = 0;
    int totCollisions = 0;
    int packet_successA = 0;
    int packet_successB = 0;
    
    while(Clock < SIM_DURATION_CLOCK){
        
        /* Checks if NodeA has a packet that comes first */
        if(nodeA->packet_arrivals[curPacketA] < nodeB->packet_arrivals[curPacketB]){
            if(Clock < nodeA->packet_arrivals[curPacketA]){ // Check if next packet is ahead of clock
                Clock += nodeA->packet_arrivals[curPacketA] - Clock;
            }
            if(!curBackoffA){
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
            }
            int compPeriodA = DIFS_SIZE + curBackoffA; // Calculate competition period
            
            /* Check for competition cases: No possible competition or Collision/Packet race */
            if((Clock + compPeriodA) <= nodeB->packet_arrivals[curPacketB] + 1){  // No other packet able to collide
                Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                packet_successA++; // successful packet transmit
                curCollisionsA = 0;
                curPacketA++;
            }else{ /* Collision/Packet race */
                curBackoffB = Get_Backoff(curCollisionsB);
                int compPeriodB = DIFS_SIZE + curBackoffB;
                int compDiff = nodeB->packet_arrivals[curPacketB] - Clock;
                Clock += compDiff; // Getting location where both packets are in the competition period
                compPeriodA -= compDiff; // Removing time from the compPeriod of packet that entered period first
                
                /* Check if there will be a collision */
                if(compPeriodA == compPeriodB){ /* Collision */
                    Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Increase clock to where ack should be received
                    curCollisionsA++; // Know there is a collision, update counters
                    curCollisionsB++;
                    totCollisions++;
                }else{ /* If no collison, send which packet wins */
                    if(compPeriodA < compPeriodB){ // NodeA transmits first successfully
                        Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock till the end of transmission
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff > DIFS_SIZE){
                            curBackoffB = curBackoffB - compPeriodA + 1;
                        }else{
                            curBackoffB -= compPeriodA;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffA = curBackoffA - compPeriodB + 1;
                        }else{
                            curBackoffA -= compPeriodB;
                        }
                    }
                }
            }
            
            /* Checks if NodeB has a packet that comes first */
        }else if(nodeA->packet_arrivals[curPacketA] > nodeB->packet_arrivals[curPacketB]){
            if(Clock < nodeB->packet_arrivals[curPacketB]){ // Check if next packet is ahead of clock
                Clock += nodeB->packet_arrivals[curPacketB] - Clock;
            }
            if(!curBackoffB){
                curBackoffB = Get_Backoff(curCollisionsB); // Gets the backoff time
            }
            int compPeriodB = DIFS_SIZE + curBackoffB; // Calculate competition period
            
            /* Check for competition cases: No possible competition or Collision/Packet race */
            if((Clock + compPeriodB) <= nodeA->packet_arrivals[curPacketA] + 1){  // No other packet able to collide
                Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                packet_successB++; // successful packet transmit
                curCollisionsB = 0;
                curPacketB++;
            }else{ /* Collision/Packet race */
                curBackoffA = Get_Backoff(curCollisionsA);
                int compPeriodA = DIFS_SIZE + curBackoffA;
                int compDiff = nodeA->packet_arrivals[curPacketA] - Clock;
                Clock += compDiff; // Getting location where both packets are in the competition period
                compPeriodA -= compDiff; // Removing time from the compPeriod of packet that entered period first
                
                /* Check if there will be a collision */
                if(compPeriodB == compPeriodA){ /* Collision */
                    Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Increase clock to where ack should be received
                    curCollisionsB++; // Know there is a collision, update counters
                    curCollisionsA++;
                    totCollisions++;
                }else{ /* If no collison, send which packet wins */
                    if(compPeriodB < compPeriodA){ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock till the end of transmission
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff > DIFS_SIZE){
                            curBackoffB = curBackoffB - compPeriodA + 1;
                        }else{
                            curBackoffB -= compPeriodA;
                        }
                    }else{ // NodeA transmits first successfully
                        Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compDiff < DIFS_SIZE){
                            curBackoffA = curBackoffA - compPeriodB + 1;
                        }else{
                            curBackoffA -= compPeriodB;
                        }
                    }
                }
            }
            
            /* There is a tie and boths nodes have a packet that comes at the same time */
        }else{
            if(Clock < nodeA->packet_arrivals[curPacketA]){ // Check if next packet is ahead of clock
                Clock += nodeA->packet_arrivals[curPacketA] - Clock;
            }
            if(!curBackoffA)
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
            if(!curBackoffB)
                curBackoffB = Get_Backoff(curCollisionsB);
            
            int compPeriodA = curBackoffA;
            int compPeriodB = curBackoffB;
            Clock += DIFS_SIZE; // Getting location where both packets are in the competition period
            
            /* Check if there will be a collision */
            if(compPeriodB == compPeriodA){ /* Collision */
                Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Increase clock to where ack should be received
                curCollisionsB++; // Know there is a collision, update counters
                curCollisionsA++;
                totCollisions++;
            }else{ /* If no collison, send which packet wins */
                if(compPeriodB < compPeriodA){ // NodeB transmits first successfully
                    Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock till the end of transmission
                    packet_successB++; // successful packet transmit
                    curCollisionsB = 0;
                    curPacketB++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffA -= compPeriodB;
                }else{ // NodeA transmits first successfully
                    Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                    packet_successA++; // successful packet transmit
                    curCollisionsA = 0;
                    curPacketA++;
                    
                    /* Reduces backoff counter of packet that didnt send */
                    curBackoffB -= compPeriodA;
                }
            }
        }
    }
    
    //printf("\n\nNodeA Success: %d\n", packet_successA);
    //printf("NodeB Success: %d\n", packet_successB);
    //printf("Total Collisions: %d\n\n", totCollisions);
    
    nodeA->packet_success = packet_successA;
    nodeB->packet_success = packet_successB;
    nodeA->packet_collision = totCollisions;
    nodeB->packet_collision = totCollisions;
}

int Get_Backoff(int fails){
    int power;
    
    power = pow(2, fails);
    if(fails > CW_MAX) // Prevents going over max contention window
        power = pow(2, CW_MAX);
    
    return rand() % (power * CW_0);
}

/*
 * Function to return a node with a poissan distribution of packet arrival data
 */
Node* Get_Packet_Dist(int rate){
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0); // Uniform dist
    Node* node;
    
    node = (Node*)malloc(sizeof(Node));
    //Should check for mem error here //
    
    double time_dist[rate * SIM_DURATION + SLACK];
    int slot_dist[rate * SIM_DURATION + SLACK];
    double sum = 0.0;
    int i = 0;

    generator.seed(std::chrono::system_clock::now().time_since_epoch().count()); // New seed based on time
    
    while(sum < SIM_DURATION){
        time_dist[i] = (-1.0 / rate) * log(1.0 - distribution(generator)); // Exponential dist of times
        slot_dist[i] = (int) ceil(time_dist[i] / (SLOT_DURATION * pow(10.0, -6.0))); // Exponential dist of slots
        sum += time_dist[i];
        i++;
    }
    
    //printf("SUM: %f, NUMS: %d\n", sum, i);

    node->packet_arrivals = (int *)malloc(i * sizeof(int));
    // Should check for mem error here //
    
    node->dist_len = i;
    node->packet_arrivals[0] = slot_dist[0]; // Adds the first packet arrival slot
    
    for(int k = 1; k < i; k++){
        node->packet_arrivals[k] = slot_dist[k] + node->packet_arrivals[k - 1];
    }
    
    
    return node; // Returns the node with packet dist created
}

void Data_toFile(char* filename){
    
}

