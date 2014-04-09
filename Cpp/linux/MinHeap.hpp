
#pragma once

#include <iostream>
using namespace std;
namespace mingspy
{
    /*
    * 获取堆数组左孩子节点下标， 下标0处也存放数据
    */
    inline int Left(int i) { return (i<<2) + 1;}

    /*
    * 获取堆数组右孩子节点下标， 下标0处也存放数据
    */
    inline int Right(int i) {return (i<<2) + 2;}

    /*
    * 获取堆数组父节点下标， 下标0处也存放数据
    */
    inline int Parent(int i) { return (i+1)>>2;}

    /* 
    *最小堆,A[Parent(i)]>=A[i] 
    *  @see http://blog.csdn.net/mingspy/article/details/8582822 
    */  
    template<typename T>  
    class MinHeap{  
    public:  
        MinHeap(int cap = 10):_size(0), _capability(cap){  
            A = new T[cap];  
        }

        MinHeap(const T * arr, int len):_size(len), _capability(len){  
            A = new T[len];  
            memcpy(A, arr, len * sizeof(T));  
        }  
  
        MinHeap(const MinHeap & r):_size(r._size), _capability(r._capability){  
            A = new T[r._capability];  
            memcpy(A, r.A, r._capability * sizeof(T));  
        }
        ~MinHeap(){  
            delete [] A;  
            A = 0;  
        }  
  
        /* 
        *   Max-Heapify(A, i) A[1...n] 
        *   l <- LEFT(i) 
        *   r <- RIGHT(i) 
        *   if l<= heap-size[A] and A[l] > A[i] 
        *       then largest <- l 
        *       else largest <- i 
        *   if r <= heapsize(A) and A[r] > A[largest] 
        *       then largest <- r 
        *   if largest != i 
        *       then exchange A[i]<->A[largest] 
        *           Max-Heapify(A, largest) 
        */  
        void Heapify(int i){  
            int l = Left(i);  
            int r = Right(i);  
            int smallest = i;  
            if ( l < _size && A[l] < A[smallest]){  
                smallest = l;  
            }  
  
            if ( r < _size && A[r] < A[smallest]){  
                smallest = r;  
            }  
  
            if ( i != smallest){  
                Swap(i, smallest);  
                Heapify(smallest);  
            }  
        }  
  
        void HeapifyUp(int i){  
            int parent;  
            while (i > 0 && A[i] < A[(parent=Parent(i))]){  
                Swap(i, parent);  
                i = parent;  
            }  
        }  
  
        /* 
        * Build-Max-Heap(A)  A[1...n] 
        *   heap-size[A] <- length[A] 
        *   for i <- foot[length[A]/2] downto 1 
        *       do Max-Heapify(A,i); 
        */  
        void Build(){  
            if ( _size == 0) return;  
            for (int i = (_size - 1) / 2; i >= 0; i--){  
                Heapify(i);  
            }  
        }  
  
        /*  
        *HeapSort(A) A[1...n] 
        *   Build-Max-Heap(A) 
        *   for i <- length[A] down to 2  //
        *       do exchange A[1] <->A[i]  // 
        *       heap-size[A] <- heap-size[A] - 1 // 
        *       Max-Heapify(A, i)         //
        */  
        void Sort()  
        {  
            if(_size == 0) return;  
            size_t tmpSize = _size;  
            Build();  
            for (int i = --_size; i > 0; i--)  
            {  
                Swap(i, 0);  
                --_size;  
                Heapify(0);  
            }  
  
            _size = tmpSize; 

            // reverse
            int mid = _size / 2;
            for(int i = 0; i < mid ; i++){
                Swap(i, _size  - 1 - i);
            }
        }  
  
        bool Empty() { return _size == 0;}  
        /* 
        * Heap-Maximum(A)   
        *   return A[1] 
        */  
  
        T & GetMin(){  
            assert (_size > 0); // 
            return A[0];  
        }  

        T & Get(int idx){  
            assert (_size > 0); // 
            return A[idx];  
        }
  
        /* 
        *Heap-Extract-Max(A) A[1...n] 
        *   if heap-size[A] < 1 
        *       then error "heap underflow" 
        *   max <- A[1] 
        *   A[1] <-A[heap-size[A]] 
        *   heap-size[A] <- heap-size[A] - 1 
        *   Max-Heapify(A, 1) 
        *   return max; 
        */  
        T RemoveMin(){  
            assert (_size > 0);  
            T min = A[0];  
            A[0] = A[--_size];  
            Heapify(0);  
            return min;  
        }  
  
        /* 
        *Heap-Increase-Key(A, i, key) 
        *   if key < A[i] 
        *       then error "new key is smaller" 
        *   A[i] <- Key 
        *   while i > 1 and A[Parent(i)] < A[i] 
        *       do exchange A[i] <-> A[Parent(i)] 
        *       i <- Parent(i) 
        */  
        void IncreaseKey(int i, const T & key){  
            assert(i>=0 && i < _size);  
            A[i] = key;  
            HeapifyUp(i);  
        }  
  
        /* 
        *Max-Heap-Insert(A,key) 
        *   heap-size[A] <- heap-size[A] + 1 
        *   A[heap-size[A]] <- -INFINITY 
        *   Heap-Increase-Key(A, heap-size[A], key) 
        */  
        void Insert(const T & key){  
            assert(_size < _capability);  
            A[_size] = key;  
            HeapifyUp(_size);  
            _size ++;  
  
        }  

        size_t size() const{
            return _size;
        }

        /*
        * add key only if key is letter than 
        * A[_size] when the heap is full.
        * useful when do NPath.
        */
        void ConditionAdd(const T & key){
            if(_size < _capability){
                A[_size] = key;  
                HeapifyUp(_size);  
                _size ++; 
            }else if(key < A[_size - 1]){
                A[_size - 1] = key;  
                HeapifyUp(_size - 1); 
            }
        }
    private:  
        inline void Swap(int i, int j)
        {
            T t = A[i];
            A[i] = A[j];
            A[j] = t;
        }

        size_t _size; //  
        size_t _capability; //
        T * A;  
    };  
}  

