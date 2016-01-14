#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
  _buffer = new float[RING_BUFFER_SIZE];
  _ringPointer = 0;
  _count = 0;
  _ringSize = RING_BUFFER_SIZE;
}

float RingBuffer::average()
{
  float value = 0;
  for (int i = 0; i <= _ringSize; i++)
  {
    value = value + _buffer[i];
  }
  return value / _ringSize;
}

bool RingBuffer::full()
{
  return (_count >= _ringSize);
}

void RingBuffer::addToBuffer(float value)
{
  if (_count < _ringSize) {
    _count++;
  }
  _buffer[_ringPointer] = value;
  _ringPointer = (_ringPointer + 1) % _ringSize;
}

