//
//  Copyright Mathieu Champlon 2009
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <turtle/mock.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/equal.hpp>

#include <boost/test/auto_unit_test.hpp>
#define BOOST_LIB_NAME boost_unit_test_framework
#include <boost/config/auto_link.hpp>

#ifdef _MSC_VER
#   pragma warning( disable : 4355 )
#endif

namespace
{
    void f1();
    int f2( float );

    BOOST_STATIC_ASSERT( (boost::mpl::equal< mock::expectation< void() >,
                                             mock::expectation< BOOST_TYPEOF( f1 ) >
                          >::value) );
    BOOST_STATIC_ASSERT( (boost::mpl::equal< mock::expectation< int( float ) >,
                                             mock::expectation< BOOST_TYPEOF( f2 ) >
                          >::value) );

    struct example
    {
        void method1();
        float method2( int );
    };

    BOOST_STATIC_ASSERT(
        (boost::mpl::equal<
            mock::expectation< void() >,
            mock::expectation<
                mock::detail::signature<
                    BOOST_TYPEOF( &example::method1 )
                >::type
            >
         >::value) );
    BOOST_STATIC_ASSERT(
        (boost::mpl::equal<
            mock::expectation< float( int ) >,
            mock::expectation<
                mock::detail::signature<
                    BOOST_TYPEOF( &example::method2 )
                >::type
            >
         >::value) );
}

BOOST_AUTO_TEST_CASE( ptr_uniformizes_reference_and_pointer )
{
    int i = 0;
    BOOST_CHECK_EQUAL( mock::detail::ref( i ), mock::detail::ref( &i ) );
}

BOOST_AUTO_TEST_CASE( ptr_accesses_inner_pointer_from_auto_ptr )
{
    std::auto_ptr< int > i( new int( 0 ) );
    BOOST_CHECK_EQUAL( *i, mock::detail::ref( i ) );
}

BOOST_AUTO_TEST_CASE( ptr_accesses_inner_pointer_from_shared_ptr )
{
    boost::shared_ptr< int > i( new int( 0 ) );
    BOOST_CHECK_EQUAL( *i, mock::detail::ref( i ) );
}

namespace
{
    template< typename T >
    void my_function( T& t )
    {
        t.my_method( "some parameter" );
    }
    MOCK_CLASS( mock_class )
    {
        MOCK_METHOD_EXT( my_method, 1, void( const std::string& ), my_method )
    };
}

BOOST_AUTO_TEST_CASE( mock_object_for_static_polymorphism )
{
    const mock_class mock;
    MOCK_EXPECT( mock, my_method ).once().with( "some parameter" );
    my_function( mock );
}

namespace
{
    MOCK_CLASS( mock_class_with_operator )
    {
        MOCK_CONST_METHOD_EXT( operator+=, 1, mock_class_with_operator&( int ), addition )
    };
}

BOOST_AUTO_TEST_CASE( mock_addition_operator )
{
    mock_class_with_operator mock;
    MOCK_EXPECT( mock, addition ).once().returns( boost::ref( mock ) );
    mock += 1;
}

namespace
{
    MOCK_CLASS( my_mock )
    {
        MOCK_METHOD_EXT( my_method, 1, void( int ), my_method )
    };
}

BOOST_AUTO_TEST_CASE( MOCK_METHOD_EXT_macro_defines_a_bindable_method )
{
    my_mock m;
    boost::bind( &my_mock::my_method, &m, 42 );
}

BOOST_AUTO_TEST_CASE( MOCK_VERIFY_macro )
{
    my_mock m;
    MOCK_VERIFY( m, my_method );
}

BOOST_AUTO_TEST_CASE( MOCK_RESET_macro )
{
    my_mock m;
    MOCK_RESET( m, my_method );
}

BOOST_AUTO_TEST_CASE( MOCK_EXPECT_macro )
{
    my_mock m;
    MOCK_EXPECT( m, my_method ).once().with( 42 );
    m.my_method( 42 );
}