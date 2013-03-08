// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h


//uncomment to see debug messages
//#define _DEBUG

#ifdef _DEBUG
#define DBG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DBG(str) do { } while ( false )
#endif

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
           DBG(std::endl << "valid() -- starting...");
           DBG("valid() -- N = " << N);
           DBG("valid() -- a[0] = " << (int)a[0]);
           DBG("valid() -- a[N-4] = " << (int)a[N-4]); 
			
//            char *p = const_cast<char*>(reinterpret_cast<const char*>(&a));
            size_type left, right;
            int i = 0;
			
            while(i < N-(int)sizeof(size_type)) {
                left = (int)*const_cast<int*>(reinterpret_cast<const int*>(a+i)); //get the sentinel value
                DBG("valid() --  left: " << left);
                if(left < 0)	//if the block is busy
                    right = (int)*const_cast<int*>(reinterpret_cast<const int*>(a + i + (-1*left) + sizeof(size_type)));
                else			//if the block is free
                    right = (int)*const_cast<int*>(reinterpret_cast<const int*>(a + i + left + sizeof(size_type)));
                DBG("valid() -- right: " << right);
                DBG("valid() --     i: " << i);

                if(left != right)
                    return false;
                
                i += std::abs(left) + 2*sizeof(size_type);
                DBG("valid() -- i at the end of while: " << i);
            }
			
			
			assert(i == N);
            // if(i != N) {
                // DBG("valid() -- i: " << i << "; N: " << N);
                // return false;
            // }
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
         * allocates the requested amount of space to give out to the user.
	 * returns the pointer to the first element of the block given out.
	 * gives out extra space if the space that is to be left over can't 
	 * be used once this block is allocated. otherwise, gives out exactly
	 * how much was asked for. 
	 * throws bad_alloc if there is no space to be given out. 
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
		 
        pointer allocate (size_type n) {
			DBG("allocate() -- in allocate()... ");
			//return 0 if the user requests... well, 0 bytes. undefined behavior.
			if(n == 0)
				return 0;
			
			//traverse through the heap to find a free block
			char *p = const_cast<char*>(reinterpret_cast<const char*>(&a));
            size_type left;
			size_type bytes_needed = n * sizeof(value_type);
			DBG("allocate() -- bytes_needed: " << bytes_needed);
			
            int i = 0;
			
            while(i < N-(int)sizeof(size_type)) {
				left = (int)*reinterpret_cast<int*>(p+i); //get the sentinel value
				
				//if a free block is found, check the value.
				//if value is more than n but less than n+(size of 2 sentinels), give away the whole damn thing
				//if value is more than n+(size of 2 sentinels), just give out how much was requested
				//else, pass to the next block
				DBG("allocate() left: " << left);
                if(left >= bytes_needed) {	//if the block is free && big enough for allocation
					DBG("allocate() -- left+2*(int)sizeof(size_type) - (bytes_needed + 2*(int)sizeof(size_type) = " << left+2*(int)sizeof(size_type) << " - (" << bytes_needed << " + 2*" << (int)sizeof(size_type) << ")");
					DBG("allocate() --  = " << (left+2*(int)sizeof(size_type) - (bytes_needed + 2*(int)sizeof(size_type))));
					if(left+2*(int)sizeof(size_type) - (bytes_needed + 2*(int)sizeof(size_type)) <=  2*(int)sizeof(size_type)) {	
						//if the block is less than n+(size of 2 sentinels)
						//give the whole damn thing away!						
						//allocate the block in the internal "heap"
						//since we're not making a new block, just change the sign of the sentinels to negative value
						
						int* p1 = reinterpret_cast<int*>(&a[i]);
						*p1 *= -1;
						p1 = reinterpret_cast<int*>(&a[i] + left + sizeof(size_type));
						*p1 *= -1;
						
						DBG("allocate() -- allocated with extras");
						return reinterpret_cast<pointer>(a+i+sizeof(size_type));
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
						
						DBG("allocate() -- allocated just enough");
						DBG("allocate() -- a[0] = " << (int)*a);
						DBG("allocate() -- a[8] = " << (int)*(a+8));
						DBG("allocate() -- a[12] = " << (int)*(a+12));
						return reinterpret_cast<pointer>(a+i+sizeof(size_type));						
					}
				}
                //if the block is busy, just skip
				//skip to the next left-sentinel (no need to check right-sentinels)
				DBG("allocate() -- i before end of while: " << i);
				i += std::abs(left) + 2*sizeof(size_type);
				DBG("allocate() -- i at the end of while: " << i);
			}
			
            assert(valid());
			
			//if there is no free block available, throw bad_alloc
			DBG("allocate() -- throwing party in allocate()");
			throw std::bad_alloc();
            
			return reinterpret_cast<pointer>(a+i+sizeof(size_type));}

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * constructs an allocator at pointer p.
         */
        void construct (pointer p, const_reference v) {
			DBG("construct() -- constructin a sentry");
            new (p) T(v);                            // uncomment!
            assert(valid());}

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * deallocates the block pointed by the argument p. 
	 * frees up the block to be used by other requests; if there
	 * are any free blocks adjacent to the block that was just 
	 * freed, then those blocks are merged with the block that is
	 * currently being deallocated. 
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type = 0) {
			DBG("deallocate() -- in deallocate()...");
            //get the sentinel value (assert them being negative? is it user error?)
			//turn them into positive values 
			//check neighboring sentinels:
			//	if positive, then merge
			//	if negative, then ignore
			char* lp = const_cast<char*>(reinterpret_cast<const char*>(p)-4); //pointer to the sentinel
			char* rp;
			int* d1 = reinterpret_cast<int*>(lp); //pointer to the sentinel
			DBG("deallocate() -- left before: " << *d1);
            *d1 *= -1;
			DBG("deallocate() -- left after : " << *d1);
			d1 = reinterpret_cast<int*>(lp + *d1 + sizeof(size_type));
			DBG("deallocate() -- right before: " << *d1);
			*d1 *= -1;
			DBG("deallocate() -- right after : " << *d1);
			rp = reinterpret_cast<char*>(d1); //pointer to the right sentinel
			
			DBG("deallocate() -- *lp: " << (int)*lp);
			DBG("deallocate() -- *rp: " << (int)*rp);
			assert(*lp == *rp);
			
			int total_bytes = *d1;
			bool merged = false;
			
			//check neighbors
			//ignore the left if the block to be deallocated is the first block in the heap
			DBG("deallocate() -- total_bytes (before merges)= " << total_bytes);
			if(reinterpret_cast<char*>(lp) != a) {	//compare pointers
				DBG("deallocate() -- sentinel to the left: " << (int)*(lp-sizeof(size_type)));
				if(*(lp - sizeof(size_type)) > 0) {	//if block left of this one is free
					//add the total bytes (plus the 2 sentinels in between)
					total_bytes += (int)*(reinterpret_cast<char*>(lp) - sizeof(size_type)) + 2*sizeof(size_type);
					//change the pointer (bytes of the block to the left + 2 pointer sizes)
					lp -= *(lp - sizeof(size_type)) + 2*sizeof(size_type);
					
					DBG("deallocate() -- total_bytes (after left merge)= " << total_bytes);
					//checks if blocks on both sides are free. could've been taken out into its own
					//if statement, but this requires less computation if both sides are actually free
					if(reinterpret_cast<char*>(d1) != &a[N]-sizeof(size_type)) {	//compare pointers
						DBG("deallocate() -- sentinel to the right: " << (int)*(reinterpret_cast<char*>(d1) + sizeof(size_type)));
						if(*(d1 + 1) > 0) {
							//add the total bytes (plus the 2 sentinels in between)
							DBG("deallocate() -- *(d1+1) = " << (int) *(d1+1));
							total_bytes += *(d1+1) + 2*sizeof(size_type);
							DBG("deallocate() -- total_bytes (after both merge)= " << total_bytes);
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
					DBG("deallocate() -- sentinel to the right (only): " << (int)*(reinterpret_cast<char*>(d1) + sizeof(size_type)));
					if((int)*(rp + sizeof(size_type)) > 0) {
						DBG("deallocate() -- *(d1 + sizeof(size_type)) = " << (int)*(rp + sizeof(size_type)));
						//add the total bytes (plus the 2 sentinels in between)
						DBG("deallocate() -- total_bytes (before right-only merge)= " << total_bytes);
						total_bytes += (int)*reinterpret_cast<int*>(reinterpret_cast<char*>(d1) + sizeof(size_type)) + 2*sizeof(size_type);
						DBG("deallocate() -- total_bytes (after right-only merge)= " << total_bytes);
						DBG("deallocate() -- total_bytes: " << total_bytes);
						//change the pointer (bytes of the block to the right + 2 pointer sizes)
						rp = reinterpret_cast<char*>(d1);
						rp += (int)*(rp + sizeof(size_type)) + 2*sizeof(size_type);
						DBG("deallocate() -- *rp: " << (int)*rp);
					}
				}
				//else
				//	total_bytes += sizeof(size_type);
			}
			
			//change the values on both sentinels
			DBG("deallocate() -- total_bytes (before setting)= " << total_bytes);
			int* lpi = reinterpret_cast<int*>(lp);
			int* rpi = reinterpret_cast<int*>(rp);
			
			DBG("deallocate() -- before setting lp: " << (int)*lpi);
			*lpi = total_bytes;
			DBG("deallocate() -- after setting lp: " << (int)*lpi);
			*rpi = total_bytes;
			DBG("deallocate() -- after setting rp: " << (int)*rpi);
			assert(*lpi == *rpi);
			
			DBG("deallocate() -- a[0]: " << (int)a[0]);
			DBG("deallocate() -- a[N-4]: " << (int)a[N-4]);
			
            assert(valid());
			DBG("deallocate() -- passed valid()");
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
