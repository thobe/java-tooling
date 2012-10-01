package org.thobe.java.tooling;

import org.junit.Test;
import org.thobe.testing.subprocess.SubprocessConfiguration;
import org.thobe.testing.subprocess.SubprocessConfigurator;
import org.thobe.testing.subprocess.Subprocess;
import org.thobe.testing.subprocess.SubprocessTestRunner;

import static org.junit.Assert.assertTrue;

@SubprocessTestRunner.SubprocessConfiguration(ToolingInterfaceTest.SubprocessConfig.class)
public class ToolingInterfaceTest
{
    public static class SubprocessConfig implements SubprocessConfigurator
    {
        @Override
        public <T extends SubprocessConfiguration<T>> void configureProcess( T starter )
        {
            starter.vmArg( String.format( "-agentpath:%s", ToolingInterface.dynlib() ) );
        }
    }

    private final ToolingInterface tools = ToolingInterface.getToolingInterface();

    @Test
    public void canAccessLiveFrame() throws Exception
    {
        assertTrue( tools.canAccessLiveFrame() );
    }

    @Test
    public void canAccessFrameVariables() throws Exception
    {
        assertTrue( tools.canAccessFrameVariables() );
    }

    @Test
    public void canAccessHeap() throws Exception
    {
        assertTrue( tools.canAccessHeap() );
    }
}
