#ifndef RINGBUFFER
#define RINGBUFFER

#define RING_BUFFER_SIZE 10

class RingBuffer
{
  public:
    RingBuffer();
    void addToBuffer(float value);
    float average();
    bool full();

  private:
    float *_buffer;
    int _ringPointer;
    int _count;
    int _ringSize;
};

#endif
