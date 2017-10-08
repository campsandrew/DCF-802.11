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
#define SIM_DURATION_CLOCK 500000 /* slots */
#define SIM_DURATION 10 /* seconds */
#define SLOT_DURATION 20 /* microseconds */
#define SLACK 200


#define NUM_RATES 10
#define NUM_TESTS 250
#define RATE_OFFSET 1

#define DEBUG 0

typedef struct Node_struct {
    int* packet_arrivals;
    int dist_len;
    int packet_success;
    int packet_collision;
    int slots_used;
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
    unsigned long AvgSlotsUsedA[NUM_RATES];
    unsigned long AvgSlotsUsedC[NUM_RATES];
    double AvgThroughputA[NUM_RATES];
    double AvgThroughputC[NUM_RATES];
    double AvgFairnessIndex[NUM_RATES];
    unsigned long AvgCollision2[NUM_RATES];
    unsigned long AvgSuccessA2[NUM_RATES];
    unsigned long AvgSuccessC2[NUM_RATES];
    unsigned long AvgSlotsUsedA2[NUM_RATES];
    unsigned long AvgSlotsUsedC2[NUM_RATES];
    double AvgThroughputA2[NUM_RATES];
    double AvgThroughputC2[NUM_RATES];
    double AvgFairnessIndex2[NUM_RATES];
    
    unsigned long AvgPacketsA[NUM_RATES];
    unsigned long AvgPacketsC[NUM_RATES];
    
    
    int j;
    
    for(j = 0; j < NUM_RATES; j++){
        AvgCollision[j] = 0;
        AvgSuccessA[j] = 0;
        AvgSuccessC[j] = 0;
        AvgThroughputA[j] = 0;
        AvgThroughputC[j] = 0;
        AvgFairnessIndex[j] = 0;
        AvgSlotsUsedA[j] = 0;
        AvgSlotsUsedC[j] = 0;
        AvgCollision2[j] = 0;
        AvgSuccessA2[j] = 0;
        AvgSuccessC2[j] = 0;
        AvgThroughputA2[j] = 0;
        AvgThroughputC2[j] = 0;
        AvgFairnessIndex2[j] = 0;
        AvgSlotsUsedA2[j] = 0;
        AvgSlotsUsedC2[j] = 0;
        
        AvgPacketsA[j] = 0;
        AvgPacketsC[j] = 0;
    }
    
    for(j = 0; j < NUM_TESTS; j++){
        for(int i = 0; i < NUM_RATES; i++){
            /* Establishing the nodes and packet data */
            Node* A = Get_Packet_Dist(RATE_OFFSET * Frame_Rates[i]);
            Node* C = Get_Packet_Dist(Frame_Rates[i]);
            
            AvgPacketsA[i] = AvgPacketsA[i] + A->dist_len;
            AvgPacketsC[i] = AvgPacketsC[i] + C->dist_len;
            
            double FairnessIndex;
            
            /* TEST SIMULATIONS */
            ScenarioA_CSMA(A, C);
            
            FairnessIndex = ((double) A->slots_used / (double) C->slots_used); // Not an average
            
            AvgSlotsUsedA[i] = AvgSlotsUsedA[i] + A->slots_used;
            AvgSlotsUsedC[i] = AvgSlotsUsedC[i] + C->slots_used;
            
            AvgFairnessIndex[i] = AvgFairnessIndex[i] + FairnessIndex;
            AvgCollision[i] = AvgCollision[i] + A->packet_collision;
            AvgSuccessA[i] = AvgSuccessA[i] + A->packet_success;
            AvgSuccessC[i] = AvgSuccessC[i] + C->packet_success;
            
            ScenarioA_VCS(A, C);
            
            FairnessIndex = ((double) A->slots_used / (double) C->slots_used); // Not an average
            
            AvgSlotsUsedA2[i] = AvgSlotsUsedA2[i] + A->slots_used;
            AvgSlotsUsedC2[i] = AvgSlotsUsedC2[i] + C->slots_used;
            
            AvgFairnessIndex2[i] = AvgFairnessIndex2[i] + FairnessIndex;
            //printf("%f\n", AvgFairnessIndex2[1]);
            
            
            AvgCollision2[i] = AvgCollision2[i] + A->packet_collision;
            AvgSuccessA2[i] = AvgSuccessA2[i] + A->packet_success;
            AvgSuccessC2[i] = AvgSuccessC2[i] + C->packet_success;
            
            //ScenarioB_CSMA(A, C);
            //ScenarioB_VCS(A, C);
            
            //printf("CHECK: %lu\n", AvgCollision[2]);
            
        }
    }
    
    int k = 0;
    
    for(k = 0; k < NUM_RATES; k++){
        // ScenarioA_CSMA
        AvgCollision[k] = (AvgCollision[k] / j);
        AvgSuccessA[k] = (AvgSuccessA[k] / j);
        AvgSuccessC[k] = (AvgSuccessC[k] / j);
        AvgFairnessIndex[k] = (AvgFairnessIndex[k] / (double) j);
        AvgSlotsUsedA[k] = (AvgSlotsUsedA[k] / j);
        AvgSlotsUsedC[k] = (AvgSlotsUsedC[k] / j);
        
        AvgThroughputA[k] = ((AvgSuccessA[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        AvgThroughputC[k] = ((AvgSuccessC[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        
        // ScenarioA_VCS
        AvgCollision2[k] = (AvgCollision2[k] / j);
        AvgSuccessA2[k] = (AvgSuccessA2[k] / j);
        AvgSuccessC2[k] = (AvgSuccessC2[k] / j);
        AvgFairnessIndex2[k] = (AvgFairnessIndex2[k] / (double) j);
        AvgSlotsUsedA2[k] = (AvgSlotsUsedA2[k] / j);
        AvgSlotsUsedC2[k] = (AvgSlotsUsedC2[k] / j);
        
        AvgThroughputA2[k] = ((AvgSuccessA2[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        AvgThroughputC2[k] = ((AvgSuccessC2[k] * DATA_FRAME_SIZE) / SIM_DURATION) / 1000000.0;
        
        // ScenarioB_CSMA
        // ScenarioB_VCS
    }
    
    
    printf("\n\n\n\t\t\t\t\tSTART CSMA\n");
    for(k = 0; k < NUM_RATES; k++){
        
        printf("RATE: %d frames/sec\n\tThroughput Avg A: %f Mbps\n\tThroughput Avg C: %f Mbps\n\tFairness Index Avg: %f A/B\n\tCollision Avg: %lu Packets\n", Frame_Rates[k], AvgThroughputA[k], AvgThroughputC[k], AvgFairnessIndex[k], AvgCollision[k]);
        printf("\tSlots Used Avg A: %lu\n\tSlots Used Avg C: %lu\n", AvgSlotsUsedA[k], AvgSlotsUsedC[k]);
        printf("\tSuccess Avg A: %lu Packets\n\tSuccess Avg C: %lu Packets\n\n", AvgSuccessA[k], AvgSuccessC[k]);
        
        
    }
    
    printf("\n\n\n\t\t\t\t\tSTART VCS\n");
    for(k = 0; k < NUM_RATES; k++){
        
        printf("RATE: %d frames/sec\n\tThroughput Avg A: %f Mbps\n\tThroughput Avg C: %f Mbps\n\tFairness Index Avg: %f A/B\n\tCollision Avg: %lu Packets\n", Frame_Rates[k], AvgThroughputA2[k], AvgThroughputC2[k], AvgFairnessIndex2[k], AvgCollision2[k]);
        printf("\tSlots Used Avg A: %lu\n\tSlots Used Avg C: %lu\n", AvgSlotsUsedA2[k], AvgSlotsUsedC2[k]);
        printf("\tSuccess Avg A: %lu Packets\n\tSuccess Avg C: %lu Packets\n\n", AvgSuccessA2[k], AvgSuccessC2[k]);
        
    }
    
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
    int slots_usedA = 0;
    int slots_usedB = 0;
    
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
                    slots_usedB += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                    slots_usedA += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                slots_usedA += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                        slots_usedA += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                        slots_usedB += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                slots_usedB += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                        slots_usedB += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
                        slots_usedA += DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
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
    
    /* Update values for nodes */
    nodeA->packet_success = packet_successA;
    nodeA->packet_collision = totCollisions;
    nodeA->slots_used = slots_usedA;
    nodeB->packet_success = packet_successB;
    nodeB->packet_collision = totCollisions;
    nodeB->slots_used = slots_usedB;
    
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
    int slots_usedA = 0;
    int slots_usedB = 0;
    
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
                Clock = Clock + compPeriodA + RTS_SIZE + SIFS_SIZE + CTS_SIZE; // Update clock to where ack should be received
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
                    Clock = Clock + compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock till end of transmission
#if DEBUG
                    printf("\t\t->SUCCESS B, CLOCK: %d\n", Clock);
#endif
                    slots_usedB += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                    packet_successB++; // successful packet transmit
                    curCollisionsB = 0;
                    curPacketB++;
                    
                    // Reduces backoff counter of packet that didnt send //
                    curBackoffA = curBackoffA - compPeriodB;
                    curBackoffB = 0;
                } /* NodeA transmits first successfully */
                else{
                    Clock = Clock + compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock till end of transmission
#if DEBUG
                    printf("\t\t->SUCCESS A, CLOCK: %d\n", Clock);
#endif
                    slots_usedA += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
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
                Clock = Clock + compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock after transmission
#if DEBUG
                printf("\t\t->AB-SUCCESS A, CLOCK: %d\n", Clock);
#endif
                slots_usedA += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
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
                    Clock = Clock + compPeriodA + RTS_SIZE + SIFS_SIZE + CTS_SIZE; // Update clock to where ack should be received
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
                        Clock = Clock + compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->AB SUCCESS A, CLOCK: %d, Comp A %d\n", Clock, compPeriodA);
#endif
                        slots_usedA += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                        packet_successA++; // successful packet transmit
                        curCollisionsA = 0;
                        curPacketA++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        curBackoffB = curBackoffB - (compPeriodA - DIFS_SIZE);
                        curBackoffA = 0;
                    } /* NodeB transmits first */
                    else{
                        Clock = Clock + compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->AB SUCCESS B, CLOCK: %d, Comp B %d\n", Clock, compPeriodB);
#endif
                        slots_usedB += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
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
                Clock = Clock + compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock after transmission
#if DEBUG
                printf("\t\t->BA-SUCCESS B, CLOCK: %d\n", Clock);
#endif
                slots_usedB += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
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
                    Clock = Clock + compPeriodA + RTS_SIZE + SIFS_SIZE + CTS_SIZE; // Update clock to where ack should be received
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
                        Clock = Clock + compPeriodB + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->BA SUCCESS B, CLOCK: %d, Comp B %d\n", Clock, compPeriodB);
#endif
                        slots_usedB += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        curBackoffA = curBackoffA - (compPeriodB - DIFS_SIZE);
                        curBackoffB = 0;
                    } /* NodeA transmits first */
                    else{
                        Clock = Clock + compPeriodA + RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE; // Update clock till the end of transmission
#if DEBUG
                        printf("\t\t->BA SUCCESS A, CLOCK: %d, Comp A %d\n", Clock, compPeriodA);
#endif
                        slots_usedA += RTS_SIZE + 3 * SIFS_SIZE + CTS_SIZE + DATA_FRAME_SIZE_SLOTS + ACK_SIZE;
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
    
    /* Update values for nodes */
    nodeA->packet_success = packet_successA;
    nodeA->packet_collision = totCollisions;
    nodeA->slots_used = slots_usedA;
    nodeB->packet_success = packet_successB;
    nodeB->packet_collision = totCollisions;
    nodeB->slots_used = slots_usedB;

}

/*
 *
 *
 *
 */
void ScenarioB_CSMA(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator
    
    
    
}

/*
 *
 *
 *
 */
void ScenarioB_VCS(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator
    
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

