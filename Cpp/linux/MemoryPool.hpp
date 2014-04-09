/**
*    Copyright (C) 2013 mingspy@163.com
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

///////////////////////////////////////////////////////////////////////////
// CLAS_PARSE_ERROR

#pragma once
#include "MemLeaksCheck.h"

#if defined(CLAS_NO_EXCEPTIONS)

#define CLAS_PARSE_ERROR(what, where) { parse_error_handler(what, where); assert(0); }

namespace mingspy
{
// When exceptions are disabled by defining CLAS_NO_EXCEPTIONS,
// this function is called to notify user about the error.
// It must be defined by the user.
// <br><br>
// This function cannot return. If it does, the results are undefined.
// <br><br>
// A very simple definition might look like that:
// <pre>
// void %claslib::%parse_error_handler(const char *what, void *where)
// {
//     std::cout << "Parse error: " << what << "\n";
//     std::abort();
// }
// </pre>
// \param what Human readable description of the error.
// \param where Pointer to character data where error was detected.
void parse_error_handler( const char *what, void *where );
}

#else

#include <exception>    // For std::exception

#define CLAS_PARSE_ERROR(what, where) throw parse_error(what, where)

namespace mingspy
{

// Parse error exception.
// This exception is thrown by the parser when an error occurs.
// Use what() function to get human-readable error message.
// Use where() function to get a pointer to position within source text where error was detected.
// <br><br>
// If throwing exceptions by the parser is undesirable,
// it can be disabled by defining CLAS_NO_EXCEPTIONS macro before rapidxml.hpp is included.
// This will cause the parser to call rapidxml::parse_error_handler() function instead of throwing an exception.
// This function must be defined by the user.
// <br><br>
// This class derives from <code>std::exception</code> class.
class parse_error: public std::exception
{

public:

    // Constructs parse error
    parse_error( const char *what, void *where )
        : m_what( what )
        , m_where( where ) {}

    // Gets human readable description of error.
    // \return Pointer to null terminated description of the error.
    virtual const char *what() const throw()
    {
        return m_what;
    }

    // Gets pointer to character data where error happened.
    // Ch should be the same as char type of MemoryPool that produced the error.
    // \return Pointer to location within the parsed string where error occurred.
    template <class Ch>
    Ch *where() const
    {
        return reinterpret_cast<Ch *>( m_where );
    }

private:

    const char *m_what;
    void *m_where;

};
}

#endif


///////////////////////////////////////////////////////////////////////////
// Pool sizes

#ifndef POOL_SIZE_512
// Size of static memory block of memory_pool.
// Define CLAS_STATIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// No dynamic memory allocations are performed by memory_pool until static memory is exhausted.
#define POOL_SIZE_512 (512)
#endif

#ifndef POOL_SIZE_1K
// Size of dynamic memory block of memory_pool.
// Define CLAS_DYNAMIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// After the static block is exhausted, dynamic blocks with approximately this size are allocated by memory_pool.
#define POOL_SIZE_1K (1024)
#endif

#ifndef POOL_SIZE_128K
// Size of static memory block of memory_pool.
// Define CLAS_STATIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// No dynamic memory allocations are performed by memory_pool until static memory is exhausted.
#define POOL_SIZE_128K (128 * 1024)
#endif

#ifndef POOL_SIZE_64K
// Size of dynamic memory block of memory_pool.
// Define CLAS_DYNAMIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// After the static block is exhausted, dynamic blocks with approximately this size are allocated by memory_pool.
#define POOL_SIZE_64K (64 * 1024)
#endif

#ifndef CLAS_ALIGNMENT
// Memory allocation alignment.
// Define CLAS_ALIGNMENT before including rapidxml.hpp if you want to override the default value, which is the size of pointer.
// All memory allocations for nodes, attributes and strings will be aligned to this value.
// This must be a power of 2 and at least 1, otherwise memory_pool will not work.
#define CLAS_ALIGNMENT sizeof(void *)
#endif
namespace mingspy
{
namespace internal
{

// Struct that contains lookup tables for the parser
// It must be a template to allow correct linking (because it has static data members, which are defined in a header file).
template <int Dummy>
struct lookup_tables {
    static const unsigned char lookup_whitespace[ 256 ];       // Whitespace table
    static const unsigned char lookup_upcase[ 256 ];           // To uppercase conversion table for ASCII characters
};

// Detect whitespace character
template <class Ch>
struct whitespace_pred {
    static unsigned char test( Ch ch )
    {
        return internal::lookup_tables<0>::lookup_whitespace[ static_cast<unsigned char>( ch ) ];
    }
};

// Find length of the string
template <class Ch>
inline std::size_t Measure( const Ch *p )
{
    const Ch * tmp = p;
    while ( *tmp )
        ++tmp;
    return tmp - p;
}

}

namespace internal
{

template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_whitespace[ 256 ] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,            // 0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 1
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // A
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // B
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // C
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // D
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,            // E
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0             // F
};


// Upper case conversion
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_upcase[ 256 ] = {
    // 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A   B   C   D   E   F
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,             // 0
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,             // 1
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,             // 2
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,             // 3
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,             // 4
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,             // 5
    96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,             // 6
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127,        // 7
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,            // 8
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,            // 9
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,            // A
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,            // B
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,            // C
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,            // D
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,            // E
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255         // F
};

}
}
namespace mingspy
{
///////////////////////////////////////////////////////////////////////
// Memory pool
// This class is used by the clas to create new nodes and attributes, without overheads of dynamic memory allocation.
// In most cases, you will not need to use this class directly.
// However, if you need to create nodes manually or modify names/values of nodes,
// you are encouraged to use memory_pool of relevant xml_document to allocate the memory.
// Not only is this faster than allocating them by using <code>new</code> operator,
// but also their lifetime will be tied to the lifetime of document,
// possibly simplyfing memory management.
// <br><br>
// Call allocate_node() or allocate_attribute() functions to obtain new nodes or attributes from the pool.
// You can also call allocate_string() function to allocate strings.
// Such strings can then be used as names or values of nodes without worrying about their lifetime.
// Note that there is no <code>free()</code> function -- all allocations are freed at once when clear() function is called,
// or when the pool is destroyed.
// <br><br>
// It is also possible to create a standalone memory_pool, and use it
// to allocate nodes, whose lifetime will not be tied to any document.
// <br><br>
// Pool maintains <code>CLAS_STATIC_POOL_SIZE</code> bytes of statically allocated memory.
// Until static memory is exhausted, no dynamic memory allocations are done.
// When static memory is exhausted, pool allocates additional blocks of memory of size <code>CLAS_DYNAMIC_POOL_SIZE</code> each,
// by using global <code>new[]</code> and <code>delete[]</code> operators.
// This behaviour can be changed by setting custom allocation routines.
// Use set_allocator() function to set them.
// <br><br>
// Allocations for nodes, attributes and strings are aligned at <code>CLAS_ALIGNMENT</code> bytes.
// This value defaults to the size of pointer on target architecture.
// <br><br>
// To obtain absolutely top performance from the parser,
// it is important that all nodes are allocated from a single, contiguous block of memory.
// Otherwise, cache misses when jumping between two (or more) disjoint blocks of memory can slow down parsing quite considerably.
// If required, you can tweak <code>CLAS_STATIC_POOL_SIZE</code>, <code>CLAS_DYNAMIC_POOL_SIZE</code> and <code>CLAS_ALIGNMENT</code>
// to obtain best wasted memory to performance compromise.
// To do it, define their values before rapidxml.hpp file is included.
// \param Ch Character type of created nodes.

template <class Ch = char>
class MemoryPool
{

public:

    // \cond internal
    typedef void *( alloc_func ) ( std::size_t );       // Type of user-defined function used to allocate memory
    typedef void ( free_func ) ( void * );              // Type of user-defined function used to free memory
    // \endcond

    // Constructs empty pool with default allocator functions.
    MemoryPool( std::size_t staticSize = POOL_SIZE_1K, std::size_t dynamicSize = POOL_SIZE_128K )
        : m_staticSize( staticSize )
        , m_dynamicSize( dynamicSize )
        , m_alloc_func( 0 )
        , m_free_func( 0 )
    {
        m_static_memory = new char[ m_staticSize ];
        init();
    }

    // Destroys pool and frees all the memory.
    // This causes memory occupied by nodes allocated by the pool to be freed.
    // Nodes allocated from the pool are no longer valid.
    ~MemoryPool()
    {
        clear();
        delete [] m_static_memory;
    }

    // Allocates a char array of given size from the pool, and optionally copies a given string to it.
    // If the allocation request cannot be accommodated, this function will throw <code>std::bad_alloc</code>.
    // If exceptions are disabled by defining CLAS_NO_EXCEPTIONS, this function
    // will call rapidxml::parse_error_handler() function.
    // \param source String to initialize the allocated memory with, or 0 to not initialize it.
    // \param size Number of characters to allocate, or zero to calculate it automatically from source string length; if size is 0, source string must be specified and null terminated.
    // \return Pointer to allocated char array. This pointer will never be NULL.
    Ch *allocStr( const Ch *source = 0, std::size_t size = 0 )
    {
        assert( source || size );     // Either source or size (or both) must be specified
        if ( size == 0 )
            size = internal::Measure( source ) + 1;
        Ch *result = static_cast<Ch *>( allocAligned( size * sizeof( Ch ) ) );
        if ( source )
            for ( std::size_t i = 0; i < size; ++i )
                result[ i ] = source[ i ];
        return result;
    }

    void *allocAligned( std::size_t size )
    {
        // Calculate aligned pointer
        char * result = align( m_ptr );

        // If not enough memory left in current pool, allocate a new pool
        if ( result + size > m_end ) {
            // Calculate required pool size (may be bigger than CLAS_DYNAMIC_POOL_SIZE)
            std::size_t pool_size = m_dynamicSize;
            if ( pool_size < size )
                pool_size = size;

            // Allocate
            // 2 alignments required in worst case: one for header, one for actual allocation
            std::size_t alloc_size = sizeof( header ) + ( 2 * CLAS_ALIGNMENT - 2 ) + pool_size;
            char *raw_memory = allocRaw( alloc_size );

            // Setup new pool in allocated memory
            char *pool = align( raw_memory );
            header *new_header = reinterpret_cast<header *>( pool );
            new_header->previous_begin = m_begin;
            m_begin = raw_memory;
            m_ptr = pool + sizeof( header );
            m_end = raw_memory + alloc_size;

            // Calculate aligned pointer again using new pool
            result = align( m_ptr );
        }

        // Update pool and return aligned pointer
        m_ptr = result + size;
        return result;
    }

    // Clears the pool.
    // This causes memory occupied by nodes allocated by the pool to be freed.
    // Any nodes or strings allocated from the pool will no longer be valid.
    void clear()
    {
        while ( m_begin != m_static_memory ) {
            char * previous_begin = reinterpret_cast<header *>( align( m_begin ) ) ->previous_begin;
            if ( m_free_func )
                m_free_func( m_begin );
            else
                delete[] m_begin;
            m_begin = previous_begin;
        }

        init();
    }

    // Sets or resets the user-defined memory allocation functions for the pool.
    // This can only be called when no memory is allocated from the pool yet, otherwise results are undefined.
    // Allocation function must not return invalid pointer on failure. It should either throw,
    // stop the program, or use <code>longjmp()</code> function to pass control to other place of program.
    // If it returns invalid pointer, results are undefined.
    // <br><br>
    // User defined allocation functions must have the following forms:
    // <br><code>
    // <br>void *allocate(std::size_t size);
    // <br>void free(void *pointer);
    // </code><br>
    // \param af Allocation function, or 0 to restore default function
    // \param ff Free function, or 0 to restore default function
    void setAllocator( alloc_func *af, free_func *ff )
    {
        assert( m_begin == m_static_memory && m_ptr == align( m_begin ) );    // Verify that no memory is allocated yet
        m_alloc_func = af;
        m_free_func = ff;
    }

private:

    struct header {
        char *previous_begin;
    };

    void init()
    {
        m_begin = m_static_memory;
        m_ptr = align( m_begin );
        m_end = m_static_memory + m_staticSize;
    }

    char *allocRaw( std::size_t size )
    {
        // Allocate
        void * memory;
        if ( m_alloc_func ) {  // Allocate memory using either user-specified allocation function or global operator new[]
            memory = m_alloc_func( size );
            assert( memory ); // Allocator is not allowed to return 0, on failure it must either throw, stop the program or use longjmp
        } else {
            memory = new char[ size ];
#ifdef CLAS_NO_EXCEPTIONS

            if ( !memory )             // If exceptions are disabled, verify memory allocation, because new will not be able to throw bad_alloc
                CLAS_PARSE_ERROR( "out of memory", 0 );
#endif

        }
        return static_cast<char *>( memory );
    }

    char *align( char *ptr )
    {
        std::size_t alignment = ( ( CLAS_ALIGNMENT - ( std::size_t( ptr ) & ( CLAS_ALIGNMENT - 1 ) ) ) & ( CLAS_ALIGNMENT - 1 ) );
        return ptr + alignment;
    }

private:
    char *m_begin;                        // Start of raw memory making up current pool
    char *m_ptr;                          // First free byte in current pool
    char *m_end;                          // One past last available byte in current pool
    char *m_static_memory;                // Static raw memory
    alloc_func *m_alloc_func;             // Allocator function, or 0 if default is to be used
    free_func *m_free_func;               // Free function, or 0 if default is to be used

    const std::size_t m_staticSize;
    const std::size_t m_dynamicSize;
};

}

