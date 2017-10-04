#include <stdio.h>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <random>

#define DATA_FRAME_SIZE 1500 /* bytes */
#define SLOT_DURATION 20.0 /* micro-seconds */
#define SIFS_DURATION 20.0 /* micro-seconds */
#define SIM_DURATION 10 /* seconds */
#define CW_MAX 1024 /* slots */
#define CW_0 4 /* slots */
#define TRANS_RATE 6 /* Mbps */
#define ACK_SIZE 30 /* bytes */
#define RTS_SIZE 30 /* bytes */
#define CTS_SIZE 30 /* bytes */
#define DIFS_DURATION 40.0 /* micro-seconds */

#define NUM_RATES 4
#define SLACK 200

typedef struct Node_struct {
    int* packet_arrivals[NUM_RATES];
    int dist_len[NUM_RATES];
    
} Node;

void ScenarioA_CSMA(Node* node1, Node* node2);
void ScenarioA_VCS();
void ScenarioB_CSMA();
void ScenarioB_VCS();
Node* Get_Packet_Dist(int rates[]);

int main(){
    
    int Frame_Rates[] = {50, 100, 200, 300};
    
    /* Establishing the nodes and packet data */
    Node* A = Get_Packet_Dist(Frame_Rates);
    Node* A = Get_Packet_Dist(Frame_Rates);
    
    /* TEST SIMULATIONS */
    ScenarioA_CSMA(A, C);
    ScenarioA_VCS(A, C);
    ScenarioB_CSMA(A, C);
    ScenarioB_VCS(A, C);

	return 0;
}

void ScenarioA_CSMA(Node* node1, Node* node2){
    
    
}

void ScenarioA_VCS(Node* node1, Node* node2){
    
}

void ScenarioB_CSMA(Node* node1, Node* node2){
    
}

void ScenarioB_VCS(Node* node1, Node* node2){
    
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
