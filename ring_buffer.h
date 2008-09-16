#ifndef RING_BUFFER_H
#define RING_BUFFER_H
typedef struct _ring_buffer {
	unsigned char idx_start;
	unsigned char idx_end;
	unsigned char data[8];
} ring_buffer;

static inline uint8_t ring_buffer_has_data(ring_buffer* buffer) {
	return (buffer->idx_start != buffer->idx_end);
}

static inline void ring_buffer_push_array(ring_buffer* buffer, uint8_t* array, uint8_t len) {
	int rbp; for(rbp=0;rbp<len;rbp++) { buffer->data[(buffer->idx_end++)&7] = array[rbp]; }
}

static inline void ring_buffer_push(ring_buffer* buffer, uint8_t value) {
	buffer->data[(buffer->idx_end++)&7] = value;
}

static inline uint8_t ring_buffer_pop(ring_buffer* buffer) {
	return buffer->data[(buffer->idx_start++)&7];
}
#endif
