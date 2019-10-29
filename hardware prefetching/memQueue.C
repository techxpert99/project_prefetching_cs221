/**
 * @author Ritik Jain
 * @file memQueue.C
 * @date 24/10/2019
 * @brief Implements the Memory Read/ Write Buffers
 */

#include "memQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

memQueue::memQueue(u_int32_t _capacity, Cache* _c, u_int32_t _latency, bool _pipelined, bool _write, char _name)
{
    capacity = _capacity;
    source = _c;
    latency = _latency;
    pipelined = _pipelined;
    write = _write;
    id = _name;

    front = 0;
    rear = 0;
    size = 0;

    queue = (Request*) calloc(capacity,sizeof(Request));
    readyTime = (u_int32_t*) calloc(capacity,sizeof(u_int32_t));
    tags = (u_int32_t*) calloc(capacity,sizeof(u_int32_t));
    indexes = (u_int32_t*) calloc(capacity,sizeof(u_int32_t));

    printf("Created a Memory Buffer with the following parameters :\n");
    printf("\tType: %s",(write)?"Write\n" : "Read\n");
    printf("\tCapacity : %d\n",capacity);
    printf("\tPipelined : %s", (pipelined)?"True" : "False");
    printf("\tLatency : %u cycles",latency);
    printf("\tSize : %d\n\n",size);
}

memQueue::~memQueue()
{
    free(queue);
    free(readyTime);
    free(tags);
    free(indexes);    
}

void memQueue::printQueue()
{
    printf("Memory Buffer :\n");
    printf("\tType: %s",(write)?"Write\n" : "Read\n");
    printf("\tCapacity : %d\n",capacity);
    printf("\tPipelined : %s", (pipelined)?"True" : "False");
    printf("\tLatency : %u cycles",latency);
    printf("\tSize : %d\n\n",size);

    int i = front;

    while(i != rear)
    {
        printf("Address : %x , ready at : %u\n", queue[i].addr, readyTime[i]);
        i++;

        if(i==capacity) i=0;
    }
}

bool memQueue::push(Request req, u_int32_t cycle)
{
    bool success = true;

    if(size==capacity) return false;

    u_int32_t index,tag;

    //Checking if the request is a duplicate.
    tag = source->getTag(req.addr);
    index = source->getIndex(req.addr);

    int dup = findDuplicate(tag, index);

    if(dup==-1 || queue[dup].fromCPU)
    {
        //If no duplicate

        queue[rear] = req;
        tags[rear] = tag;
        indexes[rear] = index;

        //If the Memory is Pipelined, The request will be ready, Latency cycles after the last item in the queue
        if(pipelined || size==0)
            readyTime[rear] = cycle + latency;

        else
        {
            //If the memory is not pipelined :
            int i = rear-1;
            if(i<0) i = capacity-1;
            readyTime[rear] = latency + readyTime[i];
        }
        
        printf("Adding to %c: readyTime is %u\n",id,readyTime[rear]);
        size++;
        rear++;
        if(rear==capacity)  rear = 0;
    }
    else if(req.fromCPU)
        queue[dup] = req; //Replace the duplicate if the request is issued by the CPU
}

int memQueue::findDuplicate(u_int32_t tag, u_int32_t index)
{
    int dup = -1;
    int i = front;

    while(i != rear)
    {
        if(tags[i] == tag && indexes[i] == index) dup = i;

        i++;
        if(i==capacity) i=0;
    }

    return dup;
}

bool memQueue::pop()
{
    if(size==0) return false;

    size--;
    front++;
    if(front == capacity) front = 0;

    return true;
    
}

u_int32_t memQueue::getSize()
{
    return size;
}

bool memQueue::frontReady(u_int32_t cycle) { 
	if(size > 0 && readyTime[front] <= cycle) return true;
	else return false;
}