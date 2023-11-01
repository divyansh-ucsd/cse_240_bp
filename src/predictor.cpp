//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "TODO";
const char *studentID = "TODO";
const char *email = "TODO";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

// define number of bits required for indexing the BHT here.
int ghistoryBits = 14; // Number of bits used for Global History
int bpType;            // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
// TODO: Add your own Branch Predictor data structures here
//
// gshare
uint8_t *bht_gshare;
uint64_t ghistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

// gshare functions
void init_gshare()
{
  int bht_entries = 1 << ghistoryBits;
  bht_gshare = (uint8_t *)malloc(bht_entries * sizeof(uint8_t));
  int i = 0;
  for (i = 0; i < bht_entries; i++)
  {
    bht_gshare[i] = WN;
  }
  ghistory = 0;
}

uint8_t gshare_predict(uint32_t pc)
{
  // get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries - 1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries - 1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;
  switch (bht_gshare[index])
  {
  case WN:
    return NOTTAKEN;
  case SN:
    return NOTTAKEN;
  case WT:
    return TAKEN;
  case ST:
    return TAKEN;
  default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
    return NOTTAKEN;
  }
}

void train_gshare(uint32_t pc, uint8_t outcome)
{
  // get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries - 1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries - 1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;

  // Update state of entry in bht based on outcome
  switch (bht_gshare[index])
  {
  case WN:
    bht_gshare[index] = (outcome == TAKEN) ? WT : SN;
    break;
  case SN:
    bht_gshare[index] = (outcome == TAKEN) ? WN : SN;
    break;
  case WT:
    bht_gshare[index] = (outcome == TAKEN) ? ST : WN;
    break;
  case ST:
    bht_gshare[index] = (outcome == TAKEN) ? ST : WT;
    break;
  default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
    break;
  }

  // Update history register
  ghistory = ((ghistory << 1) | outcome);
}

void cleanup_gshare()
{
  free(bht_gshare);
}

//tournament data structures

int ghistoryBits_tournament = 14;
int local_hist_bits = 10; // if this changes, change (local_pht_tournament)
int pc_bits = 10;

// uint64_t ghistory; declared for gshare
uint16_t *local_pht_tournament;
uint16_t *local_bht_tournament;

uint16_t *global_bht_tournament;
uint16_t *choice_pred_tournament;

// tournament functions
void init_tournament()
{ 
  int i;

  // global
  int global_bht_entries = 1 << ghistoryBits_tournament;
  int choice_pred_entries = 1 << ghistoryBits_tournament;

  global_bht_tournament = (uint16_t *)malloc(global_bht_entries * sizeof(uint16_t));
  choice_pred_tournament = (uint16_t *)malloc(choice_pred_entries * sizeof(uint16_t));

  for (i = 0; i < global_bht_entries; i++)
  {
    global_bht_tournament[i] = WN;
  }

  for (i = 0; i < choice_pred_entries; i++)
  {
    choice_pred_tournament[i] = WG;
  }

  // local
  uint32_t local_pht_entries = 1 << pc_bits;
  uint32_t local_bht_entries = 1 << local_hist_bits;

  local_pht_tournament = (uint16_t *)malloc(local_pht_entries * sizeof(uint16_t));
  local_bht_tournament = (uint16_t *)malloc(local_bht_entries * sizeof(uint16_t));

  for (i = 0; i < local_pht_entries; i++)
  {
    local_pht_tournament[i] = 0;
  }
  
  for (i = 0; i < local_bht_entries; i++)
  {
    local_bht_tournament[i] = N_1;
  }

  ghistory = 0;
}

uint8_t
tournament_predict_local(uint32_t pc)
{ 

  uint32_t local_pht_entries = 1 << pc_bits;
  uint32_t pc_lower_bits = pc & (local_pht_entries - 1);

  uint32_t local_bht_index = local_pht_tournament[pc_lower_bits] & ((1 << local_hist_bits) - 1);
  uint32_t local_pred_res = local_bht_tournament[local_bht_index];

  if (local_pred_res == T_0 || local_pred_res == T_1 || local_pred_res == T_2 || local_pred_res == T_3) {
    return TAKEN;
  } else if (local_pred_res == N_0 || local_pred_res == N_1 || local_pred_res == N_2 || local_pred_res == N_3) {
    return NOTTAKEN;
  } else {
    printf("Warning: Undefined state of entry in Local BHT!: %d, %d, \n", local_pred_res, local_bht_index);
    return NOTTAKEN;
  }
}

uint8_t
tournament_predict_global()
{ 
  uint32_t global_bht_entries = 1 << ghistoryBits_tournament;
  uint32_t ghistory_lower_bits = ghistory & (global_bht_entries - 1);

  uint32_t global_pred_res = global_bht_tournament[ghistory_lower_bits];

  if (global_pred_res == WN || global_pred_res == SN) {
    return NOTTAKEN;
  } else if (global_pred_res == ST || global_pred_res == WT) {
    return TAKEN;
  } else {
    printf("Warning: Undefined state of entry in Global BHT!\n");
    return NOTTAKEN;
  }
} 

uint8_t
tournament_predict(uint32_t pc)
{ 

  uint32_t global_bht_entries = 1 << ghistoryBits_tournament;
  uint32_t ghistory_lower_bits = ghistory & (global_bht_entries - 1);
  uint32_t global_or_local = choice_pred_tournament[ghistory_lower_bits];


  if (global_or_local == WG || global_or_local == SG) {    
    return tournament_predict_global();
  } 
  else if (global_or_local == WL || global_or_local == SL) {
    return tournament_predict_local(pc);
  } 
  else {
    printf("Warning: Undefined state of entry in Choice Predict Table!\n");
    return NOTTAKEN;
  }
    
}

void train_tournament_global(uint8_t outcome)
{ 
  uint32_t global_bht_entries = 1 << ghistoryBits_tournament;
  uint32_t ghistory_lower_bits = ghistory & (global_bht_entries - 1);
  
  //update local bht
  switch (global_bht_tournament[ghistory_lower_bits])
  {
    case WT:
      global_bht_tournament[ghistory_lower_bits] = (outcome == TAKEN) ? ST : WN;
      break;
    case ST:
      global_bht_tournament[ghistory_lower_bits] = (outcome == TAKEN) ? ST : WT;
      break;
    case WN:
      global_bht_tournament[ghistory_lower_bits] = (outcome == TAKEN) ? WT : SN;
      break;
    case SN:
      global_bht_tournament[ghistory_lower_bits] = (outcome == TAKEN) ? WN : SN;
      break;
  }

  ghistory = ((ghistory << 1) | outcome);

}

void train_tournament_local(uint32_t pc, uint8_t outcome)
{ 

  uint32_t local_bht_entries = 1 << local_hist_bits;
  uint32_t local_pht_entries = 1 << pc_bits;
  uint32_t pc_lower_bits = pc & (local_pht_entries - 1);
  uint32_t local_bht_index = local_pht_tournament[pc_lower_bits] & ((1 << local_hist_bits) - 1);
  
  // Update local bht based on the actual outcome
  switch (local_bht_tournament[local_bht_index])
  {
    case T_3:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? T_3 : T_2;
      break;
    case T_2:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? T_3 : T_1;
      break;
    case T_1:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? T_2 : T_0;
      break;
    case T_0:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? T_1 : N_0;
      break;
    case N_0:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? T_0 : N_1;
      break;
    case N_1:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? N_0 : N_2;
      break;
    case N_2:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? N_1 : N_3;
      break;
    case N_3:
      local_bht_tournament[local_bht_index] = (outcome == TAKEN) ? N_2 : N_3;
      break;
  }

  local_pht_tournament[pc_lower_bits] = ((local_pht_tournament[pc_lower_bits] << 1) | outcome) & ((1 << local_hist_bits) - 1);
}


void train_tournament(uint32_t pc, uint8_t outcome)
{
  uint8_t local_prediction = tournament_predict_local(pc);
  uint8_t global_prediction = tournament_predict_global();
  uint8_t final_prediction = tournament_predict(pc);

  train_tournament_global(outcome);
  train_tournament_local(pc, outcome);

  uint32_t choice_index = ghistory & ((1 << ghistoryBits_tournament) - 1);
  
  // Update choice table only if one was correct and the other was not
  if (local_prediction != global_prediction) {
    if (final_prediction == outcome) {
      // The chosen one was correct
      if (choice_pred_tournament[choice_index] == WG) {
        choice_pred_tournament[choice_index] = SG;
      } else if (choice_pred_tournament[choice_index] == WL) {
        choice_pred_tournament[choice_index] = SL;
      }
    } else {
      // The chosen one was incorrect but the other was correct
      if (choice_pred_tournament[choice_index] == SG && global_prediction != outcome) {
        choice_pred_tournament[choice_index] = WG;
      } else if (choice_pred_tournament[choice_index] == SL && local_prediction != outcome) {
        choice_pred_tournament[choice_index] = WL;
      } else if (choice_pred_tournament[choice_index] == WG && global_prediction != outcome) {
        choice_pred_tournament[choice_index] = WL;
      } else if (choice_pred_tournament[choice_index] == WL && local_prediction != outcome) {
        choice_pred_tournament[choice_index] = WG;
      }
    }
  }
  // If both predictions were the same, no need to update the choice table
}


void cleanup_tournament()
{
  free(global_bht_tournament);
  free(choice_pred_tournament);
  free(local_pht_tournament);
  free(local_bht_tournament);
}

//CUSTOM: Perceptron

// Custom Predictor Data Structures
uint64_t GHR_custom; 
int8_t weights_table[0x1 << 10][64];
int perceptron_output;
uint16_t weights_table_index;
long int threshold = 75;
int w0 = 2;
int ghistoryBits_custom = 59;
int pcIndexBits_custom = 10;

// Initialize the custom predictor
void init_custom()
{
  GHR_custom = 0;
  perceptron_output = 0;
  int j, k;

  for(j = 0; j < (0x1 << pcIndexBits_custom); j++) {
    for(k = 0; k < ghistoryBits_custom; k++) {
      weights_table[j][k] = 0;
    }
  }
}

// Make a prediction using the custom predictor
uint8_t predict_custom(uint32_t pc)
{
  weights_table_index = (GHR_custom ^ pc) % (0x1 << pcIndexBits_custom);
  perceptron_output = w0; // Initialize to w0
  int k;

  for(k = 0; k < ghistoryBits_custom; k++) {
    if((GHR_custom & (0x1 << k)) == (0x1 << k)) {
      perceptron_output += weights_table[weights_table_index][k];
    } else { 
      perceptron_output -= weights_table[weights_table_index][k];
    }
  }

  return (perceptron_output > 0) ? TAKEN : NOTTAKEN;
}

// Train the custom predictor
void train_custom(uint32_t pc, uint8_t outcome)
{
  int k;
  int x[64];
  int outcome_sign = (outcome == 1) ? 1 : -1;

  // Update logic here
  if(abs(perceptron_output) <= threshold || (perceptron_output > 0 && outcome == 0) || (perceptron_output <= 0 && outcome == 1)) {
    for(k = 0; k < ghistoryBits_custom; k++) {
      x[k] = (GHR_custom & (0x1 << k)) ? 1 : -1;
      weights_table[weights_table_index][k] += (outcome_sign == x[k]) ? 1 : -1;
    }
  }

  // Update history register
  GHR_custom = ((GHR_custom << 1) | outcome) % (0x1 << ghistoryBits_custom);
}

void init_predictor()
{
  switch (bpType)
  {
  case STATIC:
    break;
  case GSHARE:
    init_gshare();
    break;
  case TOURNAMENT:
    init_tournament();
    break;
  case CUSTOM:
    init_custom();
    break;
  default:
    break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint32_t make_prediction(uint32_t pc, uint32_t target, uint32_t direct)
{

  // Make a prediction based on the bpType
  switch (bpType)
  {
  case STATIC:
    return TAKEN;
  case GSHARE:
    return gshare_predict(pc);
  case TOURNAMENT:
    return tournament_predict(pc);
  case CUSTOM:
    return predict_custom(pc);
  default:
    break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_predictor(uint32_t pc, uint32_t target, uint32_t outcome, uint32_t condition, uint32_t call, uint32_t ret, uint32_t direct)
{
  if (condition)
  {
    switch (bpType)
    {
    case STATIC:
      return;
    case GSHARE:
      return train_gshare(pc, outcome);
    case TOURNAMENT:
      train_tournament(pc, outcome);
    case CUSTOM:
      train_custom(pc, outcome);
    default:
      break;
    }
  }
}
