//
//  BnB.hpp
//  BnB
//
//  Created by Aslıhan Celik on 12/1/19.
//  Copyright © 2019 Aslihan Celik. All rights reserved.
//

#ifndef BNB_HPP
#define BNB_HPP

#include <stdio.h>
#include <iostream>
#include <stdlib.h> 
#include <cstdio>
#include <cmath>
#include <climits>
#include <chrono>

#include <vector>
#include <queue>
#include <utility>
#include <cstring>

using namespace std::chrono;
using namespace std;

using centiseconds          = std::chrono::duration< int, std::centi >;
using siseconds             = std::chrono::seconds;
using hrclock               = std::chrono::high_resolution_clock;
using timep                 = std::chrono::time_point< hrclock, centiseconds >;

typedef int weight_t;

class BranchandBound
{
    double                temperature_;
    int                   dimension_;
    FILE*                 solFile_;
    FILE*                 traceFile_;
    timep                 startTime_;
    centiseconds          timeLimit_;
    std::vector< double > timeTrace_;
    std::vector< int >    qualityTrace_;

  public:
    BranchandBound(
        weight_t* weights,
        int       numLocations,
        int       timeLimit,
        int       seed,
        FILE*     solFile,
        FILE*     traceFile
    );

    int BnB(int** costMtrx, int Vnum, FILE* pF1, FILE* pF2);
};

struct Nodee
{
    vector<pair<int, int>> trackpath; //stores edges of state space tree & helps to trace path when answer is found
    
    int** mtrxReduced; //stores reduced matrix
    
    int cityNum; //stores current city number i
       
    int level; //stores number of cities visited so far
    
    int cost; //stores the lower bound
    
};

//comparison criterion to order the PQ
struct criterion{
   bool operator()( const Nodee* left, const Nodee* right) const
   {
       return left->cost > right->cost;
   }
};

Nodee* newNode(int** ancestorMtrx, vector<pair<int,int>> const &trackpath, int level, int i , int j, int Vnum );

void reduceRow(int**mtrxReduced, int* row, int Vnum);

void reduceCol(int**mtrxReduced, int* col, int Vnum);

int costFind(int** mtrxReduced, int Vnum);

void printTrackPath(vector<pair<int, int>> const &list, FILE* pF1);

int BnB(int** costMatrix, int Vnum, FILE* pF1, FILE* pF2);


#endif /* BnB_hpp */
