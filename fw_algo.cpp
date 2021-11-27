#include <iostream>
#include <climits>
#include<stdio.h>
#include <iomanip>
#include<chrono>
#include<vector>
#include<omp.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include<bits/stdc++.h>

using namespace std;
using namespace std::chrono;

#define N 4
#define M INT_MAX
void printPath(int path[][N], int v, int u)
{
    if (path[v][u] == v)
        return;

    printPath(path, v, path[v][u]);
    cout << path[v][u] << " ";
}
void printSolution(int cost[N][N], int path[N][N])
{
    for (int v = 0; v < N; v++)
    {
        for (int u = 0; u < N; u++)
        {
            if (u != v && path[v][u] != -1)
            {
                cout << "Shortest Path from " << v << " -> " << u << " is ("
                     << v << " ";
                printPath(path, v, u);
                cout << u << ")" << endl;
            }
        }
    }
}
    

int main()
{
    int nthreads;
    int v,u,k;
    cout<<"Weigh graph is : "<<endl;
    int adjMatrix[N][N] =
    // {
    //     { 0, M, -2, M },
    //     { 4, 0, 3, M },
    //     { M, M, 0, 2 },
    //     { M, -1, M, 0 }
    // };
    { 
        {0,   5,  M, 10},
        {M, 0,   3, M},
        {M, M, 0,   1},
        {M, M, M, 0}
    };
//assignment of matrix values as cost 
    // #pragma omp parallel
    int cost[N][N], path[N][N];
    for (int v = 0; v < N; v++)
    {
        for (int u = 0; u < N; u++)
        {
            cost[v][u] = adjMatrix[v][u];
            cout<<cost[v][u]<<"\t";
            if (v == u)
                path[v][u] = 0;
            else if (cost[v][u] != INT_MAX)
                path[v][u] = v;
            else
                path[v][u] = -1;
        }
        cout<<endl;
    }

    // auto start = high_resolution_clock::now();
    double start_time = omp_get_wtime();
    //for loop to find the shortest path between two vertices in sequential execution
    for (int k = 0; k < N; k++)
    {
        for (int v = 0; v < N; v++)
        {
            for (int u = 0; u < N; u++)
            {
                if ( cost[v][k] != INT_MAX && cost[k][u] != INT_MAX
                    && cost[v][k] + cost[k][u] < cost[v][u] )
                {
                    cost[v][u] = cost[v][k] + cost[k][u];
                    path[v][u] = path[k][u];
                }
            }
            if (cost[v][v] < 0)
            {
                cout << "Negative Weight Cycle Found!!";
                return 0;
            }
        }
    }
    cout<<"\nSequential Execution:"<<endl;
    //print all pairs shortest path distance
    printSolution(cost, path);
    cout<<endl;
    double time = omp_get_wtime() - start_time;

    cout<<"Time taken for sequential : "<< time <<" seconds \n"<<endl;


    //Parallel Execution Starts
    omp_set_num_threads(nthreads);
    double start_time1 = omp_get_wtime();
    // for(nthreads=1; nthreads <= 10; nthreads++) 
    // {
    //     #pragma omp parallel shared(cost)
    //     for ( k=0; k < N; k++)
    //     {
    //         #pragma omp parallel for private (u,v) schedule(dynamic)
    //         {
    //         double start_time2 = omp_get_wtime();
    //         for( v=0; v<N  ; v++)
    //         {
    //             int midvk = cost[v][k];
    //             for ( u=0; u<N && u!=k; u++)
    //             {
    //                 int midku = cost[k][u];
    //                 if (midvk != M && midku != M
    //                     && midvk + midku < cost[v][u])
    //                 {
    //                     cost[v][u] = midvk + midku;
    //                     path[v][u] = path[k][u];
    //                 }
    //             }
    //         }
    //         double time3 = (omp_get_wtime() - start_time2) ;
    //         // double time1= time3/pow(10,-6);
    //         //print time taken by each thread
    //         cout<<"Time taken by thread "<< nthreads<<" :"<< time3 <<" seconds \n"<<endl;
    //         }
        
    //     }
    // }
    #pragma omp parallel shared(cost)
    {
        for ( k=0; k < N; k++)
        {
            for(nthreads=0; nthreads < 4; nthreads++) 
            {  
                double start_time2 = omp_get_wtime();
                #pragma omp parallel for private (u,v) schedule(dynamic)
                for( v=0; v < N; v++)
                {
                    int midvk = cost[v][k];
                    for ( u=0; u < N ; u++)
                    {
                        int midku = cost[k][u];
                        if (midvk != M && midku != M && midvk + midku < cost[v][u])
                        {
                            cost[v][u] = midvk + midku;
                            path[v][u] = path[k][u];
                        }
                    }
                }
                double time3 = (omp_get_wtime() - start_time2) ;
                //print time taken by each thread
                cout<<"Time taken by thread "<< nthreads<<" :"<< time3 <<" seconds\n";
                
            }
        }
        cout<<endl;
    }

    double time4 = (omp_get_wtime() - start_time1) ;
    cout<<"Parallel Execution: "<<endl;
    printSolution(cost,path);
    cout<<"Total time for thread "<< nthreads<<" :"<< time4 <<" seconds \n"<<endl;




    return 0;
}