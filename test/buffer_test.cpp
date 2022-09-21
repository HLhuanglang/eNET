#include "buffer.h"

int main() {
    buffer* buf = sigleton<buffer_pool>::get_instance()->get(12345);
    return 0;
}