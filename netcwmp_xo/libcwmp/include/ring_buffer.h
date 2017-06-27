#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__



#include <pthread.h>
typedef unsigned int 	uint32_t;

struct ring_buffer
{
    void         *buffer;     //缓冲区
    uint32_t     size;       //大小
    uint32_t     in;         //入口位置
    uint32_t       out;        //出口位置
    pthread_mutex_t *f_lock;    //互斥锁
};
extern struct ring_buffer* ring_buffer_init(void *buffer, uint32_t size, pthread_mutex_t *f_lock);
extern void ring_buffer_free(struct ring_buffer *ring_buf);
extern uint32_t ring_buffer_len(const struct ring_buffer *ring_buf);
extern uint32_t ring_buffer_get(struct ring_buffer *ring_buf, void *buffer, uint32_t size);
extern uint32_t ring_buffer_put(struct ring_buffer *ring_buf, void *buffer, uint32_t size);



#endif
