package org.thobe.java.tooling;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.thobe.testing.subprocess.SubprocessTestRunner;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

@RunWith(SubprocessTestRunner.class)
@SubprocessTestRunner.SubprocessConfiguration(RunWithToolingAgent.class)
public class CallStackTest
{
    ToolingInterface tools = ToolingInterface.getToolingInterface();

    @Test
    public void shouldGetStackOfCallFramesForCurrentTest() throws Exception
    {
        // when
        CallFrame[] stack = tools.getCallStack();

        // then
        assertTrue( "length should be longer than 1: " + stack.length, stack.length > 1 );
        assertEquals( "frame height on stack", tools.getCallFrame( 0 ).height, stack[0].height );
    }

    @Test
    public void examplePyFrameBuilding() throws Exception
    {
        class PyFrame
        {
            final PyFrame f_prev;
            final CallFrame frame;

            private PyFrame( PyFrame f_prev, CallFrame frame )
            {
                this.f_prev = f_prev;
                this.frame = frame;
            }
        }

        CallFrame[] stack = tools.getCallStack();
        PyFrame frame = null;
        for ( int i = stack.length-1; i >=0; i-- )
        {
            frame = new PyFrame( frame, stack[i] );
        }
        assertNotNull( frame );
        assertEquals( tools.getCallFrame( 0 ).toStackTraceElement(), frame.frame.toStackTraceElement() );
        /*
        while ( frame != null )
        {
            System.out.println("at " + frame.frame.toStackTraceElement());
            frame = frame.f_prev;
        }
        //*/
    }
}
