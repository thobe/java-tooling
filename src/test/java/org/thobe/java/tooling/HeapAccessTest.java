package org.thobe.java.tooling;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.model.Statement;
import org.thobe.testing.subprocess.SubprocessTestRunner;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

@RunWith(SubprocessTestRunner.class)
@SubprocessTestRunner.SubprocessConfiguration(RunWithToolingAgent.class)
public class HeapAccessTest
{
    private ToolingInterface tools = ToolingInterface.getToolingInterface();
    {
        System.out.println(tools);
    }

    @Test
    public void shouldAccessAllLiveInstancesOfGivenClass() throws Exception
    {
        // when
        HeapAccessTest[] instances = tools.getLiveInstancesOf( HeapAccessTest.class );

        // then
        assertEquals( 1, instances.length );
        assertEquals( this, instances[0] );
    }

    @Test
    public void shouldAccessLiveInstanceOfInterface() throws Exception
    {
        // given
        Runnable runnable = new Runnable()
        {
            @Override
            public void run()
            {
            }
        };

        // when
        Runnable[] instances = tools.getLiveInstancesOf( Runnable.class );

        // then
        assertTrue( "size=" + instances.length, instances.length != 0 );
        boolean found = false;
        for ( Runnable instance : instances )
        {
            if ( instance == runnable )
            {
                found = true;
            }
        }
        assertTrue( "Should have found known instance.", found );
    }

    @Test
    public void shouldHaveSameSizeAsRetainedSizeForObject() throws Exception
    {
        // given
        Object object = new Object();
        // when
        long size = tools.sizeOf( object );
        long retained = tools.retainedSize( object );

        // then
        assertEquals( size, retained );
    }

    @Test
    public void shouldNotReturnZeroAsRetainedSizeOfObject() throws Exception
    {
        // given
        Object object = new Object();

        // when
        long size = tools.retainedSize( object );

        // then
        assertTrue( "size=" + size, size != 0 );
    }
}
