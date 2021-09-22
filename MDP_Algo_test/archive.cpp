
/* COMPONENT.CPP
//leave as int for now, once the sort function works, change int id to struct Vertex* vertex -> hcost+gcost
class QueueNode{
    public:
        Vertex* q_vertex;
        QueueNode* next;
};

//used in the search
class SortedQueue{
    QueueNode* head;
    QueueNode* tail;
    int current_size;
    public:
        //----Queue functions----
        SortedQueue(){
            head= 0;
            tail= 0;
            current_size=0;
        }
        //add new node to queue
        void enqueue(Vertex* vertex){
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
        Vertex* dequeue(){
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
        void sort_queue(){

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
};
*/

/* COMPONENT.H
//leave as int for now, once the sort function works, change int id to struct Vertex* vertex -> hcost+gcost
class QueueNode{
    public:
        Vertex* q_vertex;
        QueueNode* next;
};

//used in the search
class SortedQueue{
    public:
        QueueNode* head;
        QueueNode* tail;
        int current_size;
        SortedQueue();
        //----Queue functions----
        //add new node to queue
        void enqueue(Vertex* vertex);
        //remove a node from the head of queue and return the vertex pointer of the node that was removed
        Vertex* dequeue();
        //sorts the queue according in ascending order of g_cost
        void sort_queue();
};
*/