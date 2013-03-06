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
         * Checks whether or not the "heap" array is valid. 
		 * It checks if the sentinels match (in value and sign).
		 * On a mismatch, it returns false.
         */
        bool valid () const {
//            std::cout << std::endl << "starting..." << std::endl;
//            std::cout << "N = " << N << std::endl;
//            std::cout << "a[0] = " << (int)a[0] << std::endl;
//            std::cout << "a[96] = " << (int)a[96] << std::endl; 
			
//            char *p = const_cast<char*>(reinterpret_cast<const char*>(&a));
            size_type left, right;
            int i = 0;
			
            while(i < N-(int)sizeof(size_type)) {
                left = (int)*const_cast<int*>(reinterpret_cast<const int*>(a+i)); //get the sentinel value
                std::cout << "valid()  left: " << left << std::endl;
                if(left < 0)	//if the block is busy
                    right = (int)*const_cast<int*>(reinterpret_cast<const int*>(a + i + (-1*left) + sizeof(size_type)));
                else			//if the block is free
                    right = (int)*const_cast<int*>(reinterpret_cast<const int*>(a + i + left + sizeof(size_type)));
                std::cout << "valid() right: " << right << std::endl;
                //std::cout << "valid()     i: " << i << std::endl;

                if(left != right)
                    return false;
                
                i += std::abs(left) + 2*sizeof(size_type);
                //std::cout << "i at the end of while: " << i << std::endl;
            }
			
            if(i != N) {
                //std::cout << "i: " << i << "; N: " << N << std::endl;
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
		 * Throws bad_alloc if the user tries to create a heap with size that is smaller than
		 * the size of two int sentinels.
         */
         
        Allocator () {
			if(N < 2*sizeof(size_type)) {
				throw std::bad_alloc();
			}
            int* p1 = reinterpret_cast<int*>(&a[0]);
            *p1 = (N-2*(sizeof(size_type)));
            int* p2 = reinterpret_cast<int*>(&a[N-sizeof(size_type)]);
            *p2 = (N-2*(sizeof(size_type)));
            assert(valid());}

        // Default copy, destructor, and copy assignment
        //Allocator  (const Allocator<T>&);
        //~Allocator ();
        //Allocator& operator = (const Allocator&);

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
			std::cout << "in allocate()... " << std::endl;
			//return 0 if the user requests... well, 0 bytes. undefined behavior.
			if(n == 0)
				return 0;
			
			//traverse through the heap to find a free block
			char *p = const_cast<char*>(reinterpret_cast<const char*>(&a));
            size_type left;
			size_type bytes_needed = n * sizeof(value_type);
			//std::cout << "bytes_needed: " << bytes_needed << std::endl;
			
            int i = 0;
			
            while(i < N-(int)sizeof(size_type)) {
				left = (int)*reinterpret_cast<int*>(p+i); //get the sentinel value
				
				//if a free block is found, check the value.
				//if value is more than n but less than n+(size of 2 sentinels), give away the whole damn thing
				//if value is more than n+(size of 2 sentinels), just give out how much was requested
				//else, pass to the next block
				std::cout << "allocate() left: " << left << std::endl;
                if(left >= bytes_needed) {	//if the block is free && big enough for allocation
					//std::cout << "left+2*(int)sizeof(size_type) - (bytes_needed + 2*(int)sizeof(size_type) = " << left+2*(int)sizeof(size_type) << " - (" << bytes_needed << " + 2*" << (int)sizeof(size_type) << ")" << std::endl;
					//std::cout << " = " << (left+2*(int)sizeof(size_type) - (bytes_needed + 2*(int)sizeof(size_type))) << std::endl;
					if(left+2*(int)sizeof(size_type) - (bytes_needed + 2*(int)sizeof(size_type)) <=  2*(int)sizeof(size_type)) {	
						//if the block is less than n+(size of 2 sentinels)
						//give the whole damn thing away!						
						//allocate the block in the internal "heap"
						//since we're not making a new block, just change the sign of the sentinels to negative value
						
						int* p1 = reinterpret_cast<int*>(&a[i]);
						*p1 *= -1;
						p1 = reinterpret_cast<int*>(&a[i] + left + sizeof(size_type));
						*p1 *= -1;
						
						//std::cout << "allocated with extras" << std::endl;
						return reinterpret_cast<pointer>(a+i);
					}
					else {	
						//if the block has enough space to allocate more after allocating this block
						//give out how much was requested
						//first change the left-sentinel to a new negative value (bytes_needed) 
						//then move sizeof(size_type)+bytes_needed indices to the right and create a new right-sentinel w/ same value as above
						//then make a new left-sentinel to the right of the one above with positive value of:
						//		(original sentinal value - bytes_needed - 2*sizeof(size_type))
						//then change the original right-sentinel to the same value as above
						
						int* p1 = reinterpret_cast<int*>(&a[i]);
						*p1 = -1*bytes_needed;
						p1 = reinterpret_cast<int*>(a + i + bytes_needed + sizeof(size_type));
						*p1 = -1*bytes_needed;
						++p1;
						*p1 = (left - bytes_needed - 2*sizeof(size_type));
						p1 = reinterpret_cast<int*>(a + i + left + sizeof(size_type));
						*p1 = (left - bytes_needed - 2*sizeof(size_type));
						
						//std::cout << "allocated just enough" << std::endl;
						return reinterpret_cast<pointer>(a+i);						
					}
				}
                //if the block is busy, just skip
				//skip to the next left-sentinel (no need to check right-sentinels)
				//std::cout << "allocate() i before end of while: " << i << std::endl;
				i += std::abs(left) + 2*sizeof(size_type);
				//std::cout << "allocate() i at the end of while: " << i << std::endl;
			}
			
            assert(valid());
			
			//if there is no free block available, throw bad_alloc
			//std::cout << "throwing party in allocate()" << std::endl;
			throw std::bad_alloc();
            
			return reinterpret_cast<pointer>(a+i);}

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * constructs an allocator at pointer p.
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
			std::cout << "in deallocate()..." << std::endl;
            //get the sentinel value (assert them being negative? is it user error?)
			//turn them into positive values 
			//check neighboring sentinels:
			//	if positive, then merge
			//	if negative, then ignore
			char* lp = const_cast<char*>(reinterpret_cast<const char*>(p));
			char* rp;
			//std::cout << "left: " << (int)*d << std::endl;
			int* d1 = reinterpret_cast<int*>(p);
			//std::cout << "left before: " << *d1 << std::endl;
            *d1 *= -1;
			//std::cout << "left after : " << *d1 << std::endl;
			d1 = reinterpret_cast<int*>(lp + *d1 + sizeof(size_type));
			//std::cout << "right before: " << *d1 << std::endl;
			*d1 *= -1;
			//std::cout << "right after : " << *d1 << std::endl;
			rp = reinterpret_cast<char*>(d1);
			
			int total_bytes = *d1;
			bool merged = false;
			//check neighbors
			//ignore the left if the block to be deallocated is the first block in the heap
			std::cout << "total_bytes (before merges)= " << total_bytes << std::endl;
			if(reinterpret_cast<char*>(p) != a) {	//compare pointers
				std::cout << "sentinel to the left: " << (int)*(lp-sizeof(size_type)) << std::endl;
				if(*(lp - sizeof(size_type)) > 0) {	//if block left of this one is free
					//add the total bytes (plus the 2 sentinels in between)
					total_bytes += (int)*(reinterpret_cast<char*>(lp) - sizeof(size_type)) + 2*sizeof(size_type);
					//change the pointer (bytes of the block to the left + 2 pointer sizes)
					lp -= *(lp - sizeof(size_type)) + 2*sizeof(size_type);
					
					std::cout << "total_bytes (after left merge)= " << total_bytes << std::endl;
					//checks if blocks on both sides are free. could've been taken out into its own
					//if statement, but this requires less computation if both sides are actually free
					if(reinterpret_cast<char*>(d1) != &a[N]-sizeof(size_type)) {	//compare pointers
						std::cout << "sentinel to the right: " << (int)*(reinterpret_cast<char*>(d1) + sizeof(size_type)) << std::endl;
						if(*(d1 + 1) > 0) {
							//add the total bytes (plus the 2 sentinels in between)
							std::cout << "*(d1+1) = " << (int) *(d1+1) << std::endl;
							total_bytes += *(d1+1) + 2*sizeof(size_type);
							std::cout << "total_bytes (after both merge)= " << total_bytes << std::endl;
							//change the pointer (bytes of the block to the right + 2 pointer sizes)
							rp = reinterpret_cast<char*>(d1);
							rp += *(rp + sizeof(size_type)) + 2*sizeof(size_type);
							merged = true;
						}
					}
				}
			}
			//at this point, the value that *d holds is the number of free bytes 
			//(d points to the left-sentinel of the block, whether it merged or not)

			//if both sides have been merged already, skip this
			if(!merged) {
				//ignore the right if the block to be deallocated is the last block in the heap
				if(reinterpret_cast<char*>(d1) != &a[N]-sizeof(size_type)) {	//compare pointers
					std::cout << "sentinel to the right (only): " << (int)*(reinterpret_cast<char*>(d1) + sizeof(size_type)) << std::endl;
					if((int)*(rp + sizeof(size_type)) > 0) {
						std::cout << "*(d1 + sizeof(size_type)) = " << (int)*(rp + sizeof(size_type)) << std::endl;
						//add the total bytes (plus the 2 sentinels in between)
						std::cout << "total_bytes (before right-only merge)= " << total_bytes << std::endl;
						total_bytes += (int)*reinterpret_cast<int*>(reinterpret_cast<char*>(d1) + sizeof(size_type)) + 2*sizeof(size_type);
						std::cout << "total_bytes (after right-only merge)= " << total_bytes << std::endl;
						//std::cout << "total_bytes: " << total_bytes << std::endl;
						//change the pointer (bytes of the block to the right + 2 pointer sizes)
						rp = reinterpret_cast<char*>(d1);
						rp += (int)*(rp + sizeof(size_type)) + 2*sizeof(size_type);
						//std::cout << "*rp: " << (int)*rp << std::endl;
					}
				}
				//else
				//	total_bytes += sizeof(size_type);
			}
			
			//change the values on both sentinels
			std::cout << "total_bytes (before setting)= " << total_bytes << std::endl;
			int* lpi = reinterpret_cast<int*>(lp);
			int* rpi = reinterpret_cast<int*>(rp);
			//std::cout << "before setting lp: " << (int)*lpi << std::endl;
			*lpi = total_bytes;
			std::cout << "after setting lp: " << (int)*lpi << std::endl;
			*rpi = total_bytes;
			std::cout << "after setting rp: " << (int)*rpi << std::endl;
			
			std::cout << "a[0]: " << (int)a[0] << std::endl;
			std::cout << "a[N-4]: " << (int)a[N-4] << std::endl;
			
            assert(valid());
			//std::cout << "passed valid()" << std::endl;
			}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * destroys the allocator at pointer p.
         */
        void destroy (pointer p) {
            p->~T();            // uncomment!
            assert(valid());}
			
		bool isValid() { return valid(); }
		};

#endif // Allocator_h
