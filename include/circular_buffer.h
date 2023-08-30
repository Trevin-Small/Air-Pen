#ifndef CBUFFER_H_INC
#define CBUFFER_H_INC

template <class T>
class circular_buffer {
    T *data;
    unsigned read_pos;
    unsigned write_pos;
    unsigned in_use;
    const unsigned capacity;
public:
    circular_buffer(unsigned size) :
        data((T *)operator new(size * sizeof(T))),
        read_pos(0),
        write_pos(0),
        in_use(0),
        capacity(size)
    {}

    void push(T const &t) {
        // ensure there's room in buffer:
        if (in_use == capacity)
            pop();

        // construct copy of object in-place into buffer
        new(&data[write_pos++]) T(t);
        // keep pointer in bounds.
        write_pos %= capacity;
        ++in_use;
    }

    // return oldest object in queue:
    T front() {
        return data[read_pos];
    }

    // remove and return oldest object from queue:
    T pop() {

        T t = data[read_pos];
        // destroy the object:
        data[read_pos++].~T();

        // keep pointer in bounds.
        read_pos %= capacity;
        --in_use;

        return t;
    }

    bool full() {
        return in_use == capacity;
    }

~circular_buffer() {
    // first destroy any content
    while (in_use != 0)
        pop();

    // then release the buffer.
    operator delete(data);
}

};

#endif