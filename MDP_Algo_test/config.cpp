#include <vector>
#include <cmath>
#include <stdexcept>
#include "tripPlan.h"
#include "component.h"

class Map{
    public:
        vector<vector<Vertex>> vertex_array;
        Map(){
            vertex_array.resize(ROW_COUNT, vector<Vertex>(COLUMN_COUNT));
        }
        //----Vertex functions----
        //initialize all the variables for all vertices of the graph
        void initialize_vertex(int ROW_COUNT, int COLUMN_COUNT,int UNIT_LENGTH,int total_count){
            int start_cost = INT_MAX;
            for(int i=0; i < ROW_COUNT; i++){
                for(int j=0; j < COLUMN_COUNT; j++){
                    vertex_array[i][j].id = i*COLUMN_COUNT + j;
                    vertex_array[i][j].row = i;
                    vertex_array[i][j].column = j;
                    vertex_array[i][j].g_cost = start_cost;
                    vertex_array[i][j].h_cost = start_cost;
                    vertex_array[i][j].x_right =((j+1)*UNIT_LENGTH)-1;  // JX: why -1?
                    vertex_array[i][j].x_left =j*UNIT_LENGTH;
                    vertex_array[i][j].y_high =((i+1)*UNIT_LENGTH)-1;   // JX: why -1?
                    vertex_array[i][j].y_low =i*UNIT_LENGTH;
                    vertex_array[i][j].is_obstacle = false;
                    vertex_array[i][j].is_border = false;
                    vertex_array[i][j].visited = false;
                    vertex_array[i][j].prev_vertex = NULL;
                }
            }
        }
        //reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
        void reset_vertex(int ROW_COUNT, int COLUMN_COUNT,int UNIT_LENGTH,int total_count){
            int i,j,start_cost,current_index;
            int start_cost = INT_MAX;
            for(int i=0; i < ROW_COUNT;i++){
                for(int j = 0; j < COLUMN_COUNT;j++){
                    vertex_array[i][j].g_cost = start_cost;
                    vertex_array[i][j].h_cost = start_cost;
                    vertex_array[i][j].is_obstacle = false;
                    vertex_array[i][j].is_border = false;
                    vertex_array[i][j].visited = false;
                    vertex_array[i][j].prev_vertex = NULL;
                }
            }
        }

        //adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
        void add_obstacle(vector<Obstacle> obstacle_array, double obstacle_length,double boundary_size, int total_count){
            int i,j;
            double obstacle_x_left, obstacle_x_right, obstacle_y_high, obstacle_y_low, border_x_left, border_x_right, border_y_high, border_y_low;
            obstacle_length=obstacle_length/2;      // JX: var here means half of obstacle length to find the left, right, high, low coordinates (for loop below)
            boundary_size= boundary_size*2;         // JX: why *2 but not /2? 

            //loop through the obstacle array
            for(i = 0; i < obstacle_array.size(); i++){
                //find the 4 points of the obstacle
                obstacle_x_left = obstacle_array[i].x_coor - obstacle_length;
                obstacle_x_right = obstacle_array[i].x_coor + obstacle_length;
                obstacle_y_high = obstacle_array[i].y_coor + obstacle_length;
                obstacle_y_low = obstacle_array[i].y_coor - obstacle_length;
                printf("obs x and y : %.1f, %.1f, %.1f, %.1f",obstacle_x_left,obstacle_x_right,obstacle_y_high,obstacle_y_low);

                //find the 4 points of the border
                border_x_left = obstacle_array[i].x_coor - boundary_size;
                border_x_right = obstacle_array[i].x_coor + boundary_size;
                border_y_high = obstacle_array[i].y_coor + boundary_size;
                border_y_low = obstacle_array[i].y_coor - boundary_size;

                //check every vertex
                for(i = 0; i < ROW_COUNT; i++){
                    for(j = 0; j < COLUMN_COUNT; j++){
                        // check if a vertex overlaps with an obstacle, if it does set it as an obstacle
                        // x1-------x2 x1<=x4, x2>=x3
                        // x3-------x4
                        if(vertex_array[i][j].x_left <= obstacle_x_right && vertex_array[i][j].x_right >=obstacle_x_left && vertex_array[i][j].y_low <= obstacle_y_high && vertex_array[i][j].y_high >=obstacle_y_low){
                            vertex_array[i][j].is_obstacle = true;
                            //printf("obstacle: %d\n",j);
                        }
                        //if a vertex falls within the x and y coordinates of the border and the vertex is not an obstacle, set it as the border
                        // JX: so if a point is nicely on the obstacle, then it is_obstacle and it is_border?
                        if(vertex_array[i][j].x_left <= border_x_right && vertex_array[i][j].x_right >=border_x_left && vertex_array[i][j].y_low <= border_y_high && vertex_array[i][j].y_high >=border_y_low && vertex_array[i][j].is_obstacle==false){
                            vertex_array[i][j].is_border=true;
                            //printf("border: %d\n",j);
                        }
                    }
                }
            }
        }

        //search for a vertex given the x and y coordinates and returns a pointer to the vertex
        Vertex find_vertex(double x_coor, double y_coor, int total_count){
            // JX: Zhi Yu please check. This uses mathematical calculation instead to use O(1) instead of O(n^2). Note that I didn't modify this function param
            //     to get the UNIT_LENGTH and COLUMN_COUNT. I'm just using the global variables
            int row = ceil(y_coor/ UNIT_LENGTH);
            int col = ceil(x_coor/UNIT_LENGTH);
            try{
                return vertex_array.at(row).at(col);
            }
            catch(const out_of_range &oor){
                printf("vertex could not be found.\n");
            }
        };
        // get vertex array
        vector<vector<Vertex>> getVertexArray(){
            return vertex_array;
        }
};