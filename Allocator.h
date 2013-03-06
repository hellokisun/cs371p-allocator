// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <new>       // new
#include <stdexcept> // invalid_argument

// ---------
// Allocator
// ---------

template <typename T, int N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef int               size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
//            std::cout << std::endl << "starting..." << std::endl;
//            std::cout << "N = " << N << std::endl;
//            std::cout << "a[0] = " << (int)a[0] << std::endl;
//            std::cout << "a[96] = " << (int)a[96] << std::endl; 

            char *p = const_cast<char*>(reinterpret_cast<const char*>(&a));
            int left, right;
            int i = 0;
			
            while(i < N-(int)sizeof(int)) {
                left = (int)*reinterpret_cast<int*>(p); //get the sentinel value
//                std::cout << "left: " << left << std::endl;
//                std::cout << "i: " << i+left << std::endl;
                if(left < 0)	//if the block is busy
                    right = (int)*reinterpret_cast<int*>(p + (-1*left) + sizeof(int));
                else			//if the block is free
                    right = (int)*reinterpret_cast<int*>(p + left + sizeof(int));
//                std::cout << "right: " << right << std::endl;

                if(left != right)
                    return false;
                
                i += left + sizeof(int);
//                std::cout << "i at the end of while: " << i << std::endl;
            }
            if(i != N-sizeof(int)) {
                std::cout << "i: " << i << "; N: " << N << std::endl;
                return false;
            }
            return true;}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * Initializes the "heap" array with sentinels on both ends of the array.
		 * The sentinel values (int) indicate the number of blocks that are in between the two sentinels. 
		 * The sign of these sentinel values dictate the status of these blocks:
		 * 		positive value indicates the block is free to be given out,
		 * 		negative value indicates the block is has been given out to some other requestor.
         */
         
        Allocator () {
			if(N < 2*sizeof(int)) {
				throw std::bad_alloc();
			}
            int* p1 = reinterpret_cast<int*>(&a[0]);
            *p1 = (N-2*(sizeof(int)));
//			std::cout << "p1: " << *p1 << std::endl;
            int* p2 = reinterpret_cast<int*>(&a[N-sizeof(int)]);
            *p2 = (N-2*(sizeof(int)));
//			std::cout << "p2: " << *p2 << std::endl;
            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator<T>&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
		 
        pointer allocate (size_type n) {
            // <your code>
            assert(valid());
            return 0;}                   // replace!

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                            // uncomment!
            assert(valid());}

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type = 0) {
            // <your code>
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void destroy (pointer p) {
            // p->~T();            // uncomment!
            assert(valid());}
			
		bool isValid() { return valid(); }
		};

#endif // Allocator_h
