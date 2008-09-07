#ifndef RING_BUFFER_H
#define RING_BUFFER_H
typedef struct _ring_buffer {
	unsigned char idx_start;
	unsigned char idx_end;
	unsigned char data[8];
} ring_buffer;

#define ring_buffer_push(buffer, value) buffer.data[buffer.idx_end++%8] = value
#define ring_buffer_pop(buffer) buffer.data[buffer.idx_start++%8]
#define ring_buffer_has_data(buffer) buffer.idx_start != buffer.idx_end
#define ring_buffer_push_array(buffer, array, len) { int rbp; for(rbp=0;rbp<len;rbp++) { buffer.data[buffer.idx_end++%8] = array[rbp]; } }

#endif
