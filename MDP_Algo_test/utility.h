#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "data_structure.h"
#include "utility.h"

//----queue functions----
void create_empty_queue(Sorted_Queue* q);
void enqueue(Sorted_Queue* q, Vertex* vertex);
Vertex* dequeue(Sorted_Queue* q);
void sort_queue(Sorted_Queue* q);

//----Queue functions----
void create_empty_queue(Sorted_Queue* q){
    q->head=NULL;
    q->tail=NULL;
    q->current_size=0;
}
//add new node to queue
void enqueue(Sorted_Queue* q, Vertex* vertex){
    Queue_Node* new_node = (Queue_Node*)malloc(sizeof(Queue_Node));
    if(new_node==NULL){
        printf("not enough memory\n");
    }
    new_node->q_vertex=vertex;
    new_node->next=NULL;
    //q empty
    if(q->head==NULL){
        q->head = new_node;
    }
    //q not empty
    if(q->tail!=NULL){
        q->tail->next = new_node;
    }
    //update tail and size regardless since queue is FIFO
    q->tail = new_node;
    (q->current_size)++;
}
//remove a node from the head of queue and return the vertex pointer of the node that was removed
Vertex* dequeue(Sorted_Queue* q){
    Vertex* result = (Vertex*)malloc(sizeof(Vertex));
    //q is empty
    if(q->head==NULL){
        return NULL;
    }
    result = q->head->q_vertex;
    q->head = q->head->next;
    q->current_size--;
    //if q is now empty
    if(q->head==NULL){
        q->tail = NULL;
    }
    return result;
}
//sorts the queue according in ascending order of g_cost
void sort_queue(Sorted_Queue* q){

    int i, j, k, min_index, size, min_cost, curr_cost;
    Vertex* min = (Vertex*)malloc(sizeof(Vertex));
    Vertex* temp = (Vertex*)malloc(sizeof(Vertex));
    Queue_Node* curr = (Queue_Node*)malloc(sizeof(Queue_Node));
    if(q->head!=NULL){
        size=q->current_size;
        //loops through the whole queue
        for(i=0;i<size;i++){
            curr= q->head;
            min = q->head->q_vertex; //just a large number
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
                temp = dequeue(q);
                if(k!=min_index){
                    enqueue(q,temp);
                }
            }
            //enqueue min value at the end
            enqueue(q,min);
        }
    }
}

#endif