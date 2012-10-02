package org.thobe.java.tooling;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.junit.runner.RunWith;
import org.thobe.testing.subprocess.SubprocessTestRunner;
import org.thobe.testing.subprocess.Task;
import org.thobe.testing.subprocess.TestProcesses;

import static org.junit.Assert.assertTrue;

public class DynamicLoadedVsAgentLoadedTest
{
    @Rule
    public final TestName test = new TestName();
    @Rule
    public final TestProcesses subprocess = new TestProcesses();
    private static final Task<Object, String> TOOLING_INTERFACE_TO_STRING = new Task<Object, String>()
    {
        @Override
        protected String run( Object o ) throws Exception
        {
            return ToolingInterface.getToolingInterface().toString();
        }
    };

    @Test
    public void capabilitiesWhenDynamicallyLoaded() throws Exception
    {
        // given
        RunWithToolingAgent.agentpath();
        // when
        log( ToolingInterface.getToolingInterface() );
    }

    @Test
    public void capabilitiesWhenAgentLoaded() throws Exception
    {
        log( subprocess.taskRunner( null ).config( new RunWithToolingAgent() ).start()
                       .run( TOOLING_INTERFACE_TO_STRING ) );
    }

    private void log( Object ti )
    {
        System.out.printf( "%s: %s%n", test.getMethodName(), ti );
    }
}
