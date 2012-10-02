package org.thobe.java.tooling;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.thobe.testing.subprocess.SubprocessConfiguration;
import org.thobe.testing.subprocess.SubprocessConfigurator;
import org.thobe.testing.subprocess.Subprocess;
import org.thobe.testing.subprocess.SubprocessTestRunner;

import static org.junit.Assert.assertTrue;

@RunWith(SubprocessTestRunner.class)
@SubprocessTestRunner.SubprocessConfiguration(RunWithToolingAgent.class)
public class ToolingInterfaceTest
{
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
