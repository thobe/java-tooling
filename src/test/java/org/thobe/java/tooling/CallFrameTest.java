package org.thobe.java.tooling;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.fail;

public class CallFrameTest
{
    ToolingInterface tools = ToolingInterface.getToolingInterface();

    @Test
    public void shouldAccessLiveCallFrameOfCurrentMethod() throws Exception
    {
        // given
        int a = doNotInline( 5 );
        long b = doNotInline( 666l );
        byte c = doNotInline( (byte) 7 );
        short d = doNotInline( (short) 60 );
        boolean e = doNotInline( false );
        char f = doNotInline( 'x' );
        float g = doNotInline( 3.14f );
        double h = doNotInline( 10.0 );

        // when
        CallFrame frame = tools.getCallFrame( 0 );

        // then
        assertEquals( a, frame.getLocal( "a" ) );
        a = 14;
        assertEquals( a, frame.getLocal( "a" ) );
        assertEquals( b, frame.getLocal( "b" ) );
        assertEquals( c, frame.getLocal( "c" ) );
        assertEquals( d, frame.getLocal( "d" ) );
        assertEquals( e, frame.getLocal( "e" ) );
        assertEquals( f, frame.getLocal( "f" ) );
        assertEquals( g, frame.getLocal( "g" ) );
        assertEquals( h, frame.getLocal( "h" ) );
        assertSame( this, frame.getThis() );
        assertSame( frame, frame.getLocal( "frame" ) );
    }

    @Test
    public void shouldDetachFrameWhenExitingMethod() throws Exception
    {
        // given
        String message = "hello world";
        CallFrame frame = method( message );

        // when
        Object value = frame.getLocal( "value" );

        // then
        assertSame( message, value );
    }

    @Test
    public void shouldSetLocalVariablesOfLiveFrame() throws Exception
    {
        // given
        String local = doNotInline( "pre" );
        CallFrame frame = tools.getCallFrame( 0 );

        // when
        frame.setLocal( "local", "post" );

        // then
        assertEquals( "post", local );
    }

    @Test
    public void shouldThrowExceptionWhenAttemptingToSetLocalVariablesOnDetachedFrame() throws Exception
    {
        // given
        CallFrame frame = method( "fish" );

        // when
        try
        {
            frame.setLocal( "value", "chicken" );
            fail( "expected exception" );
        }
        // then
        catch ( IllegalStateException expected )
        {
            assertEquals( "Cannot set local variable on detached frame.", expected.getMessage() );
        }
    }

    private CallFrame method( String value )
    {
        assertNotNull( value );
        return tools.getCallFrame( 0 );
    }

    private static <T> T doNotInline( T value )
    {
        return value;
    }
}
