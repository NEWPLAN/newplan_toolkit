# Collections of Queues

- **```blocking_queue```**: a blocking queue for multi-producer-multi-consumer systems
  -  *usage*: 
        ```cpp
        auto* bq = new newplan_toolkit::BlockingQueue();
        ```

- **```spsc_queue```**: an asynchronious queue for single-producer-single-consumer systems [*high-performance*]
  -  *usage*: 
     -  *create*
        ```cpp
        auto *spsc_q = new newplan_toolkit::SPSCQueue(102400);
        ```

     - *insert*
        ```cpp 
        spsc_q->push(data); 
        ```

     - *pop*
        ```cpp 
        new_data = spsc_q->blocking_pop();//blocking

        do {
            recv_ptr = spsc_q->front();//non-blocking
        } while (recv_ptr == nullptr); 
        spsc_q->pop();
        ```

## Other Useful Implementations
 - *readerwriterqueue* : single-produce-single-consumer queue, https://github.com/cameron314/readerwriterqueue
 - *concurrentqueue* : multiple-produce-multiple-consumer queue, https://github.com/cameron314/concurrentqueue
