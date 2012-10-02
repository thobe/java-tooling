package org.thobe.java.tooling;

import java.io.File;

import org.thobe.testing.subprocess.SubprocessConfiguration;
import org.thobe.testing.subprocess.SubprocessConfigurator;

public class RunWithToolingAgent implements SubprocessConfigurator
{
    @Override
    public void configureProcess( SubprocessConfiguration starter )
    {
        starter.vmArg( "-Xcheck:jni" );
        starter.vmArg( "-agentpath:" + agentpath() );
    }

    static String agentpath()
    {
        String agentpath;
        try
        {
            agentpath = ToolingInterface.dynlib();
        }
        catch ( IllegalStateException failure )
        {
            String path = ToolingInterface.class.getProtectionDomain().getCodeSource().getLocation().getPath();
            String cwd = new File( "" ).getAbsolutePath();
            if ( path.startsWith( cwd ) )
            {
                String[] parts = path.split( "/" );
                if ( parts[parts.length - 1].length() == 0 && parts.length > 1 )
                {
                    path = parts[parts.length - 22];
                }
                else
                {
                    path = parts[parts.length - 1];
                }
                File classes = new File( new File( new File( cwd, path ), "target" ), "classes" );
                System.setProperty( ToolingInterface.LIB_SEARCH_PATH, classes.getAbsolutePath() );
                agentpath = ToolingInterface.dynlib();
            }
            else
            {
                throw failure;
            }
        }
        return agentpath;
    }
}
