#include <stdio.h>
#include "component.h"

//----struct----
//used for obstacle avoidance
double UNIT_LENGTH = 10;
double ROBOT_LENGTH = 30;
int ROW_COUNT = 200/UNIT_LENGTH;
int COLUMN_COUNT = 200/UNIT_LENGTH;
int TOTAL_GRID_COUNT = ROW_COUNT*COLUMN_COUNT;
double OBSTACLE_SIZE = 10;  //total length of an obstacle, the obstacle is a square
double OBSTACLE_BOUNDARY = 15;  // + to each side of the obstacle

//Vertex();
void Vertex::printVertex(){
    printf("vertex values: %d, %d, %d, %f, %f, %f, %f\n", id, g_cost, h_cost, x_left, x_right, y_high, y_low);
}


//used to read the obstacles

Obstacle::Obstacle(int id, double x_coor, double y_coor){
    this->x_coor = x_coor;
    this->y_coor = y_coor;
    this->id = id;
}


//used in the search

//----Queue functions----
SortedQueue::SortedQueue(){
    head= 0;
    tail= 0;
    current_size=0;
}
//add new node to queue
void SortedQueue::enqueue(Vertex* vertex){
    QueueNode* new_node = new QueueNode;
    if(new_node == NULL){
        printf("not enough memory\n");
    }
    new_node->q_vertex=vertex;
    new_node->next=NULL;
    //q empty
    if(head==NULL){
        head = new_node;
    }
    //q not empty
    if(tail!=NULL){
        tail->next = new_node;
    }
    //update tail and size regardless since queue is FIFO
    tail = new_node;
    current_size++;
}
//remove a node from the head of queue and return the vertex pointer of the node that was removed
Vertex* SortedQueue::dequeue(){
    Vertex* result = new Vertex;
    //q is empty
    if(head==NULL){
        return NULL;
    }
    result = head->q_vertex;
    head = head->next;
    current_size--;
    //if q is now empty
    if(head==NULL){
        tail = NULL;
    }
    return result;
}
        //sorts the queue according in ascending order of g_cost
void SortedQueue::sort_queue(){

    int i, j, k, min_index, size, min_cost, curr_cost;
    Vertex* min = new Vertex;
    Vertex* temp = new Vertex;
    QueueNode* curr = new QueueNode;
    if(head!=NULL){
        size = current_size;
        //loops through the whole queue
        for(i=0;i<size;i++){
            curr= head;
            min = head->q_vertex; //just a large number
            min_index =0;
            //find the index and min value for the first size-i elements in the queue
            for(j=0;j<size-i;j++){
                min_cost = min->g_cost+ min->h_cost;
                curr_cost = curr->q_vertex->g_cost + curr->q_vertex->h_cost;
                if(min_cost > curr_cost){
                    min = curr->q_vertex;
                    min_index = j;
                }
                curr=curr->next;
            }
            //dequeue all elements and enqueue them in order, except for the min value
            for(k=0;k<size;k++){
                temp = dequeue();
                if(k!=min_index){
                    enqueue(temp);
                }
            }
            //enqueue min value at the end
            enqueue(min);
        }
    }
}
