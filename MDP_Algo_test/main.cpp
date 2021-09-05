#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "component.h"
#include "tripPlan.h"
#include "config.h"
#include "search.h"

using namespace std;

//----not done----
double* trip_planning(vector<Vertex> obstacle_array, vector<vector<Vertex>> vertex_array, Robot* robot);//a*search with obstacle avoidance, returns the path and direction(theta)
bool obstacle_avoidance(vector<Vertex> obstacle_array, Robot* robot);//obstacle avoidance checking and redirection, returns true if obstacle found, false if no obstacle
int check_turn_condition(Vertex* start_loc, Vertex* end_loc);//calculates which of the 3 scenarios the source and vertex falls under and returns 1, 2, or 3
void update_robot(Robot* robot);//update the robot's coordinates

//----test functions----
void test_queue();

int main()
{
    // src=0;
    // target=13;
    //----test vertex functions----
    Map map;
    map.initialize_vertex(ROW_COUNT,COLUMN_COUNT,UNIT_LENGTH,TOTAL_GRID_COUNT);

    // test vertices. I didn't use src and target
    Vertex source = map.getVertexArray()[5][5];
    Vertex target = map.getVertexArray()[20][10];
    vector<Obstacle> obstacle_array = {Obstacle(0,50 ,10)};
    AStarSearch search;

    map.add_obstacle(obstacle_array, OBSTACLE_SIZE,OBSTACLE_BOUNDARY,TOTAL_GRID_COUNT);
    for(int i = 0; i < ROW_COUNT; i++){
        for(int j = 0; j < COLUMN_COUNT; j++){
            cout << i*ROW_COUNT + COLUMN_COUNT << endl;
            map.getVertexArray()[i][j].printVertex();
        }
    }


    // printf("index %d, obstacle = %d\n",vertex_array[25].id,vertex_array[25].is_obstacle);

    update_h_cost(map.getVertexArray(), &source, &target, TOTAL_GRID_COUNT);
    a_star_search(map.getVertexArray(), &source, &target,TOTAL_GRID_COUNT);

    //----end of test vertex----

    //test_queue();
    return 0;
}


//----test functions----
void test_queue(){
    int i;
    vector<Vertex> qv = {
    { 0,10},{ 1,15},{ 2,5},{ 3,25},{ 4,20}
    };
    Vertex* qptr;
    SortedQueue q;
    for(i=0;i<5;i++){
        q.enqueue(&qv[i]);
    }
    QueueNode* curr = q.head;
    for(i = 0; i < q.current_size;i++){
        printf("%d ",curr->q_vertex->g_cost);
        curr=curr->next;
    }
    printf("\n");
    q.sort_queue();
    curr=q.head;
    for(i = 0; i < q.current_size; i++){
        printf("%d ",curr->q_vertex->g_cost);
        curr=curr->next;
    }
}
