# gcc内置原子操作

## __sync系列函数

[__sync系列函数官方文档](https://gcc.gnu.org/onlinedocs/gcc/_005f_005fsync-Builtins.html)

从GCC4.1.版本之后就引入了内置的原子操作函数，可对x86_64架构（除此之外还有其他类型）1、2、4、8字节的integer scalar或pointer使用，可有效减少对锁机制的使用进一步而提升效率，这些函数以__sync开头，而在GCC4.7之后的版本，这些函数被替换成了以__atomic开头的一系列函数。

将__sync_系列17个函数声明整理简化如下：

```cpp
type __sync_fetch_and_add (type *ptr, type value, ...)
// 将value加到*ptr上，结果更新到*ptr，并返回操作之前*ptr的值

type __sync_fetch_and_sub (type *ptr, type value, ...)
// 从*ptr减去value，结果更新到*ptr，并返回操作之前*ptr的值

type __sync_fetch_and_or (type *ptr, type value, ...)
// 将*ptr与value相或，结果更新到*ptr， 并返回操作之前*ptr的值

type __sync_fetch_and_and (type *ptr, type value, ...)
// 将*ptr与value相与，结果更新到*ptr，并返回操作之前*ptr的值

type __sync_fetch_and_xor (type *ptr, type value, ...)
// 将*ptr与value异或，结果更新到*ptr，并返回操作之前*ptr的值

type __sync_fetch_and_nand (type *ptr, type value, ...)
// 将*ptr取反后，与value相与，结果更新到*ptr，并返回操作之前*ptr的值

type __sync_add_and_fetch (type *ptr, type value, ...)
// 将value加到*ptr上，结果更新到*ptr，并返回操作之后新*ptr的值

type __sync_sub_and_fetch (type *ptr, type value, ...)
// 从*ptr减去value，结果更新到*ptr，并返回操作之后新*ptr的值

type __sync_or_and_fetch (type *ptr, type value, ...)
// 将*ptr与value相或， 结果更新到*ptr，并返回操作之后新*ptr的值

type __sync_and_and_fetch (type *ptr, type value, ...)
// 将*ptr与value相与，结果更新到*ptr，并返回操作之后新*ptr的值

type __sync_xor_and_fetch (type *ptr, type value, ...)
// 将*ptr与value异或，结果更新到*ptr，并返回操作之后新*ptr的值

type __sync_nand_and_fetch (type *ptr, type value, ...)
// 将*ptr取反后，与value相与，结果更新到*ptr，并返回操作之后新*ptr的值

bool __sync_bool_compare_and_swap (type *ptr, type oldval, type newval, ...)
// 比较*ptr与oldval的值，如果两者相等，则将newval更新到*ptr并返回true

type __sync_val_compare_and_swap (type *ptr, type oldval, type newval, ...)
// 比较*ptr与oldval的值，如果两者相等，则将newval更新到*ptr并返回操作之前*ptr的值

__sync_synchronize (...)
// 发出完整内存栅栏

type __sync_lock_test_and_set (type *ptr, type value, ...)
// 将value写入*ptr，对*ptr加锁，并返回操作之前*ptr的值。即，try spinlock语义

void __sync_lock_release (type *ptr, ...)
// 将0写入到*ptr，并对*ptr解锁。即，unlock spinlock语义
```

## __atomic系列函数

[__atomic系列函数官方文档](https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html)