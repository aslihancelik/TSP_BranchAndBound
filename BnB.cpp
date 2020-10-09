//
//  BnB.cpp
//  BnB
//
//  Created by Aslıhan Celik on 12/1/19.
//  Copyright © 2019 Aslihan Celik. All rights reserved.
//

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

#include "BnB.hpp"

#define INF INT_MAX

BranchandBound::BranchandBound(
    weight_t* weights,
    int       numLocations,
    int       timeLimit,
    int       seed,
    FILE*     solFile,
    FILE*     traceFile
)
    : temperature_(timeLimit),
      dimension_(numLocations),
      solFile_(solFile),
      traceFile_(traceFile),
      startTime_(std::chrono::time_point_cast< centiseconds >(hrclock::now())),
      timeLimit_(
          std::chrono::duration_cast< centiseconds >(siseconds(timeLimit))
      ),
     
      timeTrace_(0, 0),
      qualityTrace_(0, 0)
{
   
}



//Create a new node(i,j) representing a visit from city i to city j

Nodee* newNode(int** ancestorMtrx, vector<pair<int,int>> const &trackpath, int level, int i , int j, int Vnum )
{
   struct Nodee* node = new Nodee;
   
   node->trackpath = trackpath; //in order to keep track of ancestor edges of state space tree
   
   node->mtrxReduced = new int*[Vnum];
   for(int z=0; z< Vnum; ++z){
       node->mtrxReduced[z] =new int[Vnum];
   }
   
   if(level != 0)
       node->trackpath.push_back(make_pair(i,j)); // add current edge to trackpath
   
   
   for(int y=0; y < Vnum; ++y){  //to copy data from ancestor node to current node
       for(int r=0; r < Vnum; ++r){
           node->mtrxReduced[y][r]=ancestorMtrx[y][r];
       }
   }
   
   //change all entries of row i and column j to infinity (skip this for source node)
   for(int k=0; level!=0 && k<Vnum; k++)
   {
     
       node->mtrxReduced[i][k] = INF; //outgoing edges for city i are set to infinity
      
       node->mtrxReduced[k][j] = INF; //incoming edges to city j are set to infinity
   }
   
   node->mtrxReduced[j][0]= INF;  // here start node is 0
   node->level = level; //number of cities visited until now are set
   node-> cityNum = j; //update current city number
   
   return node;
   
   
};


//Reduce each row. There must be at least one zero in each row.
void reduceRow(int**mtrxReduced, int* row, int Vnum)
{
   //fill_n(row, Vnum, INF); //initialize row array to INF
   
   for(int i=0; i<Vnum; i++){
       row[i]= INF;
   }

   
   //finding the row[i] that contains the min in row i
   for(int i=0; i< Vnum; i++)
       for(int j=0; j<Vnum; j++)
           if(mtrxReduced[i][j] < row[i])
               row[i] = mtrxReduced[i][j];
   
   
   //subtract the min value from each element in each row
   for( int i=0; i <Vnum; i++)
       for(int j=0; j<Vnum; j++)
           if(mtrxReduced[i][j] != INF && row[i] != INF)
               mtrxReduced[i][j] -= row[i];
  
};

//Reduce each column. There must be at least one zero in each column.
void reduceCol(int**mtrxReduced, int* col, int Vnum)
{
   fill_n(col, Vnum, INF); //initialize column array to INF
   
   //find row[i] contains the min in row i
   for(int i=0; i< Vnum; i++)
       for(int j=0; j<Vnum; j++)
           if(mtrxReduced[i][j] < col[j])
               col[j] = mtrxReduced[i][j];
   
   
   //subtract the min value from each element in each column
   for( int i=0; i <Vnum; i++)
       for(int j=0; j<Vnum; j++)
           if(mtrxReduced[i][j] != INF && col[j] != INF)
               mtrxReduced[i][j] -= col[j];
  
};

//Find lower bound on the path starting at current minimum node

int costFind(int** mtrxReduced, int Vnum)
{
   int cost=0; //intialize cost to 0
   
   int *row = new int[Vnum];
   
   reduceRow(mtrxReduced, row, Vnum); //row reduction
   
   int *col = new int[Vnum];
    
   reduceCol(mtrxReduced, col, Vnum);//column reduction

   
   for(int i=0; i <Vnum; i++){
       cost += (row[i] != INT_MAX) ? row[i] : 0;
       cost += (col[i] != INT_MAX) ? col[i] : 0;

   }
   
   //clear out all memory
   
    delete [] row;
    delete [] col;
   

   return cost;
   
};
// print path of cities visited
void printTrackPath(vector<pair<int, int>> const &list, FILE* pF1)
{
   
   for(int i = 0; i < list.size(); i++){
                         
                         fprintf(pF1, "%d", list[i].first);

                       if(i < list.size() - 1) //cout<<", ";
                           fprintf(pF1, "%2s", ",");
   }
}

int BranchandBound::BnB(int** costMtrx, int Vnum, FILE* pF1, FILE* pF2)
{
    
    

   priority_queue<Nodee*, vector<Nodee*>, criterion> pq; //create PQ to store live nodes of search tree
   vector<pair<int,int>> r;
   Nodee* source = newNode(costMtrx, r, 0 ,-1, 0, Vnum); //create and calculate the cost of source node & TSP starts from first city which is node 0
   source->cost= costFind(source->mtrxReduced, Vnum); //find lower bound of path starting at node 0
   
   
   pq.push(source); //add source to list of live nodes
   
   int trackcost = INF; //initial upperbound
    
//    int trackcost = 917371; //initial upperbound
   
   //finds node with least cost, add its children to PQ and finally deletes it from PQ
   while( !pq.empty())
   {
       Nodee* leastCost = pq.top(); //find node with least cost
       pq.pop();  // this node is deleted from PQ
       if(leastCost->cost >= trackcost){
           continue;
       }
       int i = leastCost-> cityNum; // i is the current city number
       
       //when all cities are visited
       if( leastCost-> level == Vnum-1)
       {
           leastCost->trackpath.push_back(make_pair(i,0));//return to the startpoint(first city)
           
           trackcost = leastCost->cost; //in order to update upperbound
           cout << "Current upperbound: " << trackcost << endl;
           fprintf(pF1, "%d\n", trackcost);
           
           
           printTrackPath(leastCost->trackpath,pF1); //print list of cities visited
           
           timeTrace_.push_back(
               (std::chrono::time_point_cast< centiseconds >(hrclock::now())
                - startTime_)
                   .count()
               / 100.0
           );
           qualityTrace_.push_back(trackcost);
           
//            fprintf(pF2, "%d \n", trackcost);  //CHECK!!!!!!!

//                                if(i < list.size() - 1) //cout<<", ";
//                                fprintf(pF2, "%2s", ", ");
        
           
       }
       
       
       //do this for each child of leastCost node which (i,j) forms an edge in space tree
       for(int j=0; j<Vnum; j++)
       {
           if(leastCost->mtrxReduced[i][j] != INF)
           {   //create a child node and calculate the cost for it

               Nodee* child = newNode(leastCost->mtrxReduced,leastCost->trackpath, leastCost->level + 1, i, j, Vnum);
             
               // child cost = ancestor node cost + edge (i,j) cost + lower bound of path starting at node j
               
               child->cost = leastCost->cost + leastCost->mtrxReduced[i][j];
               child->cost += costFind(child->mtrxReduced,Vnum);
               
               //add child to PQ
               
               if(child->cost < trackcost){
               pq.push(child);
               }
               
           }
       }
  
       //clear out all memory
       for(int f = 0; f< Vnum; ++f ){
           delete[] leastCost->mtrxReduced[f];
       }
       delete [] leastCost->mtrxReduced;
       
       
       delete leastCost; //remove ancestor node when printing solution

   }
    for (std::vector< double >::size_type i = 0; i < timeTrace_.size(); ++i)
       {
           fprintf(traceFile_, "%.2f, %d\n", timeTrace_[i], qualityTrace_[i]);
       }
   return trackcost;
}


#reference: https://www.techiedelight.com/travelling-salesman-problem-using-branch-and-bound/
