// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
To test the program:
    % ls /usr/include/cppunit/
    ...
    TestFixture.h
    ...
    % locate libcppunit.a
    /usr/lib/libcppunit.a
    % g++ -pedantic -std=c++0x -Wall Allocator.c++ TestAllocator.c++ -o TestAllocator -lcppunit -ldl
    % valgrind TestAllocator >& TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_one
    // --------

    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}

    // --------
    // test_ten
    // --------

    void test_ten () {
        A x;
        const difference_type s = 10;
        const value_type      v = 2;
        const pointer         b = x.allocate(s);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}
   
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

template <typename B>
struct TestAllocator2 : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename B::value_type      value_type;
    typedef typename B::difference_type difference_type;
    typedef typename B::pointer         pointer;

	// ----------
    // test_valid
    // ----------
    
    void test_valid_1 () {
		Allocator<int, 8> x;
    }    
	
	void test_valid_2 () {
		try {
			Allocator<int, 4> x;} 
		catch(std::bad_alloc e){
//			std::cout << "bad_alloc" << std::endl;
		}
    }    
	
	void test_valid_3 () {
		try {
			Allocator<int, 0> x;} 
		catch(std::bad_alloc e){
//			std::cout << "bad_alloc" << std::endl;
		}
    }    
	
	void test_valid_4 () {
		Allocator<bool, 8> x;
    }   
	
	void test_valid_5 () {
		Allocator<char, 8> x;
    }  
	
	void test_valid_6 () {
		Allocator<double, 8> x;
    }  
	
	void test_valid_7 () {
		Allocator<int, 8> x;
    }  
	
	void test_valid_8 () {
		Allocator<bool, 100> x;
    }
	
	void test_valid_9 () {
		Allocator<char, 100> x;
    }
	
	void test_valid_10 () {
		Allocator<double, 100> x;
    }
	
	void test_valid_11 () {
		Allocator<int, 100> x;
    }
	
	void test_valid_12 () {
		Allocator<int, 321> x;
    }
	
	void test_valid_13 () {
		Allocator<int, 500> x;
    }
	
	void test_valid_14 () {
		Allocator<int, 1000> x;
    }
	
	
   
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator2);
    CPPUNIT_TEST(test_valid_1);
//    CPPUNIT_TEST(test_valid_2);
//    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_4);
    CPPUNIT_TEST(test_valid_5);
    CPPUNIT_TEST(test_valid_6);
    CPPUNIT_TEST(test_valid_7);
    CPPUNIT_TEST(test_valid_8);
    CPPUNIT_TEST(test_valid_9);
    CPPUNIT_TEST(test_valid_10);
    CPPUNIT_TEST(test_valid_11);
    CPPUNIT_TEST(test_valid_12);
    CPPUNIT_TEST(test_valid_13);
    CPPUNIT_TEST(test_valid_14);
    CPPUNIT_TEST_SUITE_END();};

    
// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

//    tr.addTest(TestAllocator< std::allocator<int> >::suite());
//    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!
    
//    tr.addTest(TestAllocator< std::allocator<double> >::suite());
//    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

	tr.addTest(TestAllocator2<Allocator<int, 100>>::suite());
	
    tr.run();

    cout << "Done." << endl;
    return 0;}
