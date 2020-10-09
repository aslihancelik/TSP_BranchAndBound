//
//  main.cpp
//  BnB
//
//  Created by Aslıhan Celik on 12/1/19.
//  Copyright © 2019 Aslihan Celik. All rights reserved.
//



#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cstring>

#define INF INT_MAX

int main(int argc, const char * argv[]) {
    
    int** costMatrix = new int*[nVNum];
    for(int i=0; i< nVNum; ++i){
        costMatrix[i] =new int[nVNum];
        
    }
    for(int i = 0; i < nVNum; i++)
    {
        for(int j = 0; j < nVNum; j++)
        {
            costMatrix[i][j] = mtxWeight[i * nVNum + j];
            if(i==j){
                costMatrix[i][j] = INF;
            }
            
        }
        
    }
    cout << "\n\nTotal Cost is " << BnB(costMatrix, nVNum, pF1, pF2);
    return 0;
    
}
