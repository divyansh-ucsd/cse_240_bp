//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN 0
#define TAKEN 1

// The Different Predictor Types
#define STATIC 0
#define GSHARE 1
#define TOURNAMENT 2
#define CUSTOM 3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN 0 // predict NT, strong not taken
#define WN 1 // predict NT, weak not taken
#define WT 2 // predict T, weak taken
#define ST 3 // predict T, strong taken

// Definitions for 2-bit counters
#define SN 0 // predict NT, strong not taken
#define WN 1 // predict NT, weak not taken
#define WT 2 // predict T, weak taken
#define ST 3 // predict T, strong taken

// Definitions for 3-bit counters
#define N_3 0  // predict NT, strong not taken
#define N_2 1  // predict NT, moderate strong not taken
#define N_1 2  // predict NT, weak not taken
#define N_0 3  // predict NT, very weak not taken
#define T_0 4  // predict T, very weak taken
#define T_1 5  // predict T, weak taken
#define T_2 6  // predict T, moderate strong taken
#define T_3 7  // predict T, strong taken


// Definitions for 2-bit counters (tournament choice selection)
#define SG 0 // predict G, strong global
#define WG 1 // predict G, weak global
#define WL 2 // predict L, weak local
#define SL 3 // predict L, strong local

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint32_t make_prediction(uint32_t pc, uint32_t target, uint32_t direct);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint32_t target, uint32_t outcome, uint32_t condition, uint32_t call, uint32_t ret, uint32_t direct);

// Please add your code below, and DO NOT MODIFY ANY OF THE CODE ABOVE
// 



#endif
