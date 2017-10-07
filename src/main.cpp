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

#define NUM_RATES 4
#define SLACK 200

#define NUM_TESTS 1000

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
    int Frame_Rates[] = {50, 100, 200, 300};
    unsigned long AvgCollision[NUM_RATES] = {0, 0, 0, 0};
    unsigned long AvgSuccessA[NUM_RATES] = {0, 0, 0, 0};
    unsigned long AvgSuccessC[NUM_RATES] = {0, 0, 0, 0};
    int j = 1;
    
    for(j = 0; j < NUM_TESTS; j++){
        for(int i = 0; i < NUM_RATES; i++){
            /* Establishing the nodes and packet data */
            Node* A = Get_Packet_Dist(Frame_Rates[i]);
            Node* C = Get_Packet_Dist(Frame_Rates[i]);
            
            /* TEST SIMULATIONS */
            ScenarioA_CSMA(A, C);
            
            AvgCollision[i] += A->packet_collision;
            AvgSuccessA[i] += A->packet_success;
            AvgSuccessC[i] += C->packet_success;
            //ScenarioA_VCS(A, C);
            //ScenarioB_CSMA(A, C);
            //ScenarioB_VCS(A, C);
        }
    }
    
    printf("\n\nFINAL\n\n\tCollision Avg: %lu\n\tSuccess Avg A: %lu\n\tSuccess Avg C: %lu\n\n", (AvgCollision[0] / j), (AvgSuccessA[0] / j), (AvgSuccessC[0] / j));
    printf("\n\nFINAL\n\n\tCollision Avg: %lu\n\tSuccess Avg A: %lu\n\tSuccess Avg C: %lu\n\n", (AvgCollision[1] / j), (AvgSuccessA[1] / j), (AvgSuccessC[1] / j));
    printf("\n\nFINAL\n\n\tCollision Avg: %lu\n\tSuccess Avg A: %lu\n\tSuccess Avg C: %lu\n\n", (AvgCollision[2] / j), (AvgSuccessA[2] / j), (AvgSuccessC[2] / j));
    printf("\n\nFINAL\n\n\tCollision Avg: %lu\n\tSuccess Avg A: %lu\n\tSuccess Avg C: %lu\n\n", (AvgCollision[3] / j), (AvgSuccessA[3] / j), (AvgSuccessC[3] / j));

	return 0;
}

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
                        if(compPeriodA - DIFS_SIZE > 0){
                            curBackoffB -= compPeriodA - DIFS_SIZE;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodA - DIFS_SIZE > 0){
                            curBackoffB -= compPeriodA - DIFS_SIZE;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodA - DIFS_SIZE > 0){
                            curBackoffB -= compPeriodA - DIFS_SIZE;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodA - DIFS_SIZE > 0){
                            curBackoffB -= compPeriodA - DIFS_SIZE;
                        }
                    }else{ // NodeB transmits first successfully
                        Clock += compPeriodB + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                        packet_successB++; // successful packet transmit
                        curCollisionsB = 0;
                        curPacketB++;
                        
                        /* Reduces backoff counter of packet that didnt send */
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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
                        if(compPeriodB - DIFS_SIZE > 0){
                            curBackoffA -= compPeriodB - DIFS_SIZE;
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

