
#include "search.h"
#include <iostream>

class aStar{
    public:
        //----Search functions
        //add obstacle avoidance and stop search once the goal has been found
        Vertex* a_star_search(vector<vector<Vertex>> vertex_array, Vertex* src_vertex, Vertex* target_vertex, int TOTAL_GRID_COUNT){
            SortedQueue q;
            src_vertex->g_cost=0;
            q.enqueue(src_vertex);
            Vertex* curr_vertex = src_vertex;
            
            //while q to explore is not empty
            while(q.head!=NULL){
                //visit head of sorted q
                curr_vertex = q.dequeue();
                curr_vertex->visited=true;
                if(curr_vertex->id == target_vertex->id){
                    print_path(src_vertex,target_vertex);
                    return target_vertex;
                }
                //for each neighbour in the adjacency matrix
                Vertex* neighbour_vertex = &vertex_array[curr_vertex->row - 1][curr_vertex->column];
                if(neighbour_vertex->visited == false && neighbour_vertex->is_border == false && neighbour_vertex->is_obstacle == false ){
                    //compare g costs of neighbour, if new g_cost is lower, replace the old g_cost and prev_vertex
                    double temp_g_cost = calculate_g_cost(curr_vertex,neighbour_vertex);
                    if(temp_g_cost < neighbour_vertex->g_cost){
                        neighbour_vertex->g_cost = temp_g_cost;
                        neighbour_vertex->prev_vertex = curr_vertex;
                        //add neighbour to exploration queue
                        q.enqueue(neighbour_vertex);        // currently no h cost in queue yet
                    }
                }
                //move the lowest f_cost vertex to front of q
                q.sort_queue();
            }
            // should have failed
            cout << "fail to find a path" << endl;
            print_path(src_vertex,target_vertex);
            return target_vertex;
        }
        //prints the path from a star search backwards, to implement forward, save the id into an array starting from the last element or use a stack
        void print_path(Vertex* src_vertex, Vertex* target_vertex){
            Vertex* print_vertex;
            print_vertex = target_vertex;
            printf("path to reach vertex %d from source %d\n",target_vertex->id, src_vertex->id);
            while(print_vertex->id != src_vertex->id){
                printf("Vertex: %d\n",print_vertex->prev_vertex->id);
                printf("x_left: %.2f, y_low: %.2f\n",print_vertex->prev_vertex->x_left,print_vertex->prev_vertex->y_low);
                print_vertex=print_vertex->prev_vertex;
            }
        }
        //updates the h_cost for every vertex using Euclidean distance from previous vertex
        double calculate_g_cost(Vertex* prev_vertex, Vertex* current_vertex){
            int x1, x2, y1, y2;
            double x1 = prev_vertex->x_left;
            double y1 = prev_vertex->y_low;
            double x2 = current_vertex->x_left;
            double y2 = current_vertex->y_low;
            return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))+prev_vertex->g_cost;
        }
        //updates the h_cost for every vertex using Euclidean distance from target
        void update_h_cost(vector<vector<Vertex>> vertex_array, Vertex* src_vertex, Vertex* target_vertex, int TOTAL_GRID_COUNT){
            src_vertex->h_cost=0;
            double x2 = target_vertex->x_left;
            double y2 = target_vertex->y_low;
            for(int i = 0;i < ROW_COUNT; i++){
                for(int j = 0; j < COLUMN_COUNT; j++){
                    double x1 = vertex_array[i][j].x_left;
                    double y1 = vertex_array[i][j].y_low;
                    vertex_array[i][j].h_cost= sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1));
                }
            }
        }

};