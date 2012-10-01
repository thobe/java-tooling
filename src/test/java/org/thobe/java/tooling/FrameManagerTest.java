package org.thobe.java.tooling;

import org.junit.Test;

import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assume.assumeNotNull;

public class FrameManagerTest
{
    @Test
    public void shouldReturnNullWhenPoppingFrameBeforePushing() throws Exception
    {
        // given
        FrameManager manager = new FrameManager();

        // when
        CallFrame frame = manager.popFrame( Thread.currentThread(), 10 );

        // then
        assertNull( frame );
    }

    @Test
    public void shouldReturnPushedFrame() throws Exception
    {
        // given
        FrameManager manager = new FrameManager();
        CallFrame pushed = new CallFrame( null, Thread.currentThread(), null, 10, null, null );
        manager.pushFrame( pushed );

        // when
        CallFrame popped = manager.popFrame( Thread.currentThread(), 10 );

        // then
        assertSame( pushed, popped );
    }

    @Test
    public void shouldReturnNullWhenPoppingASecondTime() throws Exception
    {
        // given
        FrameManager manager = new FrameManager();
        manager.pushFrame( new CallFrame( null, Thread.currentThread(), null, 10, null, null ) );
        assumeNotNull( manager.popFrame( Thread.currentThread(), 10 ) );

        // when
        CallFrame frame = manager.popFrame( Thread.currentThread(), 10 );

        // then
        assertNull( frame );
    }

    @Test
    public void shouldOnlyRetainLastPushedCopyOfFrame() throws Exception
    {
        // given
        FrameManager manager = new FrameManager();
        manager.pushFrame( new CallFrame( null, Thread.currentThread(), null, 10, null, null ) );
        CallFrame pushed = new CallFrame( null, Thread.currentThread(), null, 10, null, null );
        manager.pushFrame( pushed );

        // when
        CallFrame popped = manager.popFrame( Thread.currentThread(), 10 );

        // then
        assertSame( pushed, popped );
        assertNull( manager.popFrame( Thread.currentThread(), 10 ) );
    }
}
