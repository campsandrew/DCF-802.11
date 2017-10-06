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

typedef struct Node_struct {
    int* packet_arrivals[NUM_RATES];
    int dist_len[NUM_RATES];
    int packet_success[NUM_RATES];
    int packet_collision[NUM_RATES];
} Node;

void ScenarioA_CSMA(Node* nodeA, Node* nodeC);
void ScenarioA_VCS(Node* node1, Node* node2);
void ScenarioB_CSMA(Node* node1, Node* node2);
void ScenarioB_VCS(Node* node1, Node* node2);
void Data_toFile(char* filename);
int Get_Backoff(int fails);
Node* Get_Packet_Dist(int rates[]);

int main(){
    int Frame_Rates[] = {50, 100, 200, 300};
    
    /* Establishing the nodes and packet data */
    Node* A = Get_Packet_Dist(Frame_Rates);
    Node* C = Get_Packet_Dist(Frame_Rates);
    
    /* TEST SIMULATIONS */
    ScenarioA_CSMA(A, C);
    ScenarioA_VCS(A, C);
    ScenarioB_CSMA(A, C);
    ScenarioB_VCS(A, C);

	return 0;
}

void ScenarioA_CSMA(Node* nodeA, Node* nodeB){
    srand(time(NULL)); // Seed the backoff time generator
    
    /* Test all rates */
    for(int i = 0; i < NUM_RATES; i++){
        int Clock = 0;
        int curPacketA = 0;
        int curPacketB = 0;
        int curBackoffA = 0;
        int curBackoffB = 0;
        int curCollisionsA = 0;
        int curCollisionsB = 0;
        
        while(Clock < SIM_DURATION_CLOCK){
            
            /* Get start time of first pack */
            if(nodeA->packet_arrivals[i][curPacketA] < nodeB->packet_arrivals[i][curPacketB]){
                Clock = nodeA->packet_arrivals[i][curPacketA];
                curBackoffA = Get_Backoff(curCollisionsA); // Gets the backoff time
                
                int compPeriodA = DIFS_SIZE + curBackoffA;
                
                if((Clock + compPeriodA) <= nodeB->packet_arrivals[i][curPacketB] - 1){  // No other packet able to collide
                    Clock += compPeriodA + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE;
                    nodeA->packet_success[i]++; // successful packet transmit
                    curCollisionsA = 0;
                    curPacketA++;
                }else{
                    curBackoffB = Get_Backoff(curCollisionsB);
                    
                    int compPeriodB = DIFS_SIZE + curBackoffB;
                    
                    
                    
                    /* Check if there is collision */
                    
                    /* If no collison, send which packet wins */
                    
                }
                
                
            }else if(nodeA->packet_arrivals[i][curPacketA] > nodeB->packet_arrivals[i][curPacketB]){
                Clock = nodeB->packet_arrivals[i][curPacketB];
                curBackoffB = Get_Backoff(curCollisionsB); // Gets the backoff time
                
                int compPeriod = DIFS_SIZE + curBackoffB;
                
                if((Clock + compPeriod) <= nodeA->packet_arrivals[i][curPacketA] - 1){  // No other packet able to collide
                    Clock += compPeriod + DATA_FRAME_SIZE_SLOTS + SIFS_SIZE + ACK_SIZE; // Advance clock
                    nodaB->packet_success[i]++; // successful packet transmit
                    curCollisionsA = 0;
                    curPacketB++;
                }else{
                    
                    
                    
                }
                
                
            }else{
                Clock = nodeA->packet_arrivals[i][curPacketA];
                
                
                
            }
                /* Compare current packets */
                
                
                /* Get lowest slot packet from the two */
                
                /* Perform CSMA with DIFS, etc. */
            
            
        }
    }
}

void ScenarioA_VCS(Node* node1, Node* node2){
    srand(time(NULL)); // Seed the backoff time generator
    
    /* Test all rates */
    for(int i = 0; i < NUM_RATES; i++){
        int Clock = 0;
        
        
        
        
        
        
    }
    
}

void ScenarioB_CSMA(Node* node1, Node* node2){
    srand(time(NULL)); // Seed the backoff time generator
    
    /* Test all rates */
    for(int i = 0; i < NUM_RATES; i++){
        int Clock = 0;
        
        
    }
    
}

void ScenarioB_VCS(Node* node1, Node* node2){
    srand(time(NULL)); // Seed the backoff time generator
    
    /* Test all rates */
    for(int i = 0; i < NUM_RATES; i++){
        int Clock = 0;
        
        
    }
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
Node* Get_Packet_Dist(int rates[]){
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0); // Uniform dist
    Node* node;
    
    node = (Node*)malloc(sizeof(Node));
    //Should check for mem error here //
    
    for(int j = 0; j < 1; j++){
    
        double time_dist[rates[j] * SIM_DURATION + SLACK];
        int slot_dist[rates[j] * SIM_DURATION + SLACK];
        double sum = 0.0;
        int i = 0;

        generator.seed(std::chrono::system_clock::now().time_since_epoch().count()); // New seed based on time
        
        while(sum < SIM_DURATION){
            time_dist[i] = (-1.0 / rates[j]) * log(1.0 - distribution(generator)); // Exponential dist of times
            slot_dist[i] = (int) ceil(time_dist[i] / (SLOT_DURATION * pow(10.0, -6.0))); // Exponential dist of slots
            sum += time_dist[i];
            i++;
        }
        
        printf("SUM: %f, NUMS: %d\n", sum, i);

        node->packet_arrivals[j] = (int *)malloc(i * sizeof(int));
        // Should check for mem error here //
        
        node->dist_len[j] = i;
        node->packet_arrivals[j][0] = slot_dist[0]; // Adds the first packet arrival slot
        
        for(int k = 1; k < i; k++){
            node->packet_arrivals[j][k] = slot_dist[k] + node->packet_arrivals[j][k - 1];
        }
        
    }
    
    return node; // Returns the node with packet dist created
}

void Data_toFile(char* filename){
    
}

