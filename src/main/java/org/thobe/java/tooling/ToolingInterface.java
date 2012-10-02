package org.thobe.java.tooling;

import java.io.File;
import java.lang.UnsatisfiedLinkError;
import java.lang.reflect.Method;
import java.net.URL;
import java.security.Permission;
import java.util.EnumSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import static org.thobe.java.tooling.Capability.can_access_local_variables;
import static org.thobe.java.tooling.Capability.can_generate_frame_pop_events;

public class ToolingInterface
{
    private static final Logger log = Logger.getLogger( ToolingInterface.class.getName() );
    private static final Permission PERMISSION = new RuntimePermission( ToolingInterface.class.getName() );
    private static boolean dynlibLoaded = false;
    private final Set<Capability> capabilities;
    private final FrameManager frameManager = new FrameManager();
    static final String LIB_SEARCH_PATH = ToolingInterface.class.getName() + ".LIB_SEARCH_PATH";

    @SuppressWarnings("unused"/*initialized from native code*/)
    private ToolingInterface( int version, Set<Capability> capabilities )
    {
        verifyPermission();
        this.capabilities = capabilities;
    }

    @Override
    public String toString()
    {
        return "ToolingInterface" + capabilities;
    }

    public static synchronized ToolingInterface getToolingInterface() throws SecurityException
    {
        verifyPermission();
        ToolingInterface tools = null;
        if ( !dynlibLoaded )
        {
            try
            { // try initializing before loading the library, it might have been loaded as an agent
                tools = initialize0( EnumSet.noneOf( Capability.class ) );
                if ( tools == null )
                { // the library is loaded, but we didn't get instance, fail fast to avoid re-attempt
                    throw new IllegalStateException( "Failed to initialize ToolingInterface." );
                }
            }
            catch ( UnsatisfiedLinkError error )
            { // the library was not loaded as an agent, load it as a regular dynamic library
                System.load( dynlib() );
            }
            finally
            {
                dynlibLoaded = true;
            }
        }
        if ( tools == null )
        {
            tools = initialize0( EnumSet.noneOf( Capability.class ) );
        }
        if ( tools == null )
        {
            throw new IllegalStateException( "Failed to initialize ToolingInterface." );
        }
        return tools;
    }

    static String dynlib()
    {
        String lib = System.mapLibraryName( "javatooling" );
        URL resource = ToolingInterface.class.getResource( '/' + lib );
        if ( resource == null )
        {
            String path = System.getProperty( LIB_SEARCH_PATH, null );
            if ( path != null )
            {
                File file = new File( path, lib );
                if ( file.isFile() )
                {
                    return file.getAbsolutePath();
                }
            }
            throw new IllegalStateException( "Cannot locate native library: " + lib );
        }
        // TODO: if the library is in a jar, copy it to a temporary location where it can be loaded
        return resource.getPath();
    }

    private static native ToolingInterface initialize0( Set<Capability> capabilities );

    public boolean canAccessLiveFrame()
    {
        return capabilities.contains( can_generate_frame_pop_events );
    }

    public boolean canAccessFrameVariables()
    {
        return capabilities.contains( can_access_local_variables );
    }

    public boolean canAccessHeap()
    {
        return capabilities.contains( Capability.can_tag_objects );
    }

    private void require( Capability capability, String action )
    {
        if ( !capabilities.contains( capability ) )
        {
            throw new UnsupportedOperationException( String.format( "Cannot %s, JVMTI capability %s is not available.",
                                                                    action, capability.name() ) );
        }
    }

    public CallFrame getCallFrame( int depth )
    {
        if ( depth < 0 )
        {
            throw new IllegalArgumentException(
                    String.format( "Illegal depth [%d] must be greater than or equal to 0.", depth ) );
        }
        CallFrame frame = getCallFrame0( Thread.currentThread(), depth + 2,
                                         capabilities.contains( can_access_local_variables ),
                                         capabilities.contains( can_generate_frame_pop_events ) );
        if ( frame == null )
        {
            throw new IllegalArgumentException(
                    String.format( "Specified depth [%s] is deeper than the current call stack.", depth ) );
        }
        return frame;
    }

    private native CallFrame getCallFrame0( Thread thread, int depth, boolean locals, boolean live );

    native Object getLocal( Thread thread, Method method, int height, long start, int length, int slot, char type );

    native void setLocal( Thread thread, Method method, int height, long start, int length, int slot, char type,
                          Object value );

    CallFrame createFrame( Thread thread, Method method, int height, Object that, long position,
                           LocalVariable[] variables, Object[] detachedValues/*null if frame is live*/ )
    {
        if ( thread == null )
        {
            throw new Error( "Internal calls did not propagate thread properly!" );
        }
        CallFrame frame = new CallFrame( this, thread, method, height, that, variables );
        if ( detachedValues != null )
        { // the frame is a detached snapshot
            frame.detach( position, detachedValues );
        }
        else
        { // the frame is live, add it to the manager
            frameManager.pushFrame( frame );
        }
        return frame;
    }

    LocalVariable createLocal( long start, int length, String signature, int slot, String name )
    {
        return new LocalVariable( start, length, signature, slot, name );
    }

    void popFrame( Thread thread, int height, long position, Object[] locals )
    {
        CallFrame frame = frameManager.popFrame( thread, height );
        if ( frame != null )
        {
            frame.detach( position, locals );
        }
        else
        {
            log.log( Level.SEVERE, String.format( "Thread[%s] does not have a registered call frame at height=%d.",
                                                  thread.getName(), height ) );
        }
    }

    @SuppressWarnings("unchecked")
    public <T> T[] getLiveInstancesOf( Class<T> type )
    {
        require( Capability.can_tag_objects, "getLiveInstances" );
        if ( type == null || type.isPrimitive() )
        {
            throw new IllegalArgumentException( String.format( "%s cannot have any instance.", type ) );
        }
        return (T[]) getLiveInstances0( type, Thread.currentThread().getId() );
    }

    public native long sizeOf( Object object );

    public long retainedSize( Object object )
    {
        require( Capability.can_tag_objects, "retainedSize" );
        return retainedSize0( object );
    }

    private native long retainedSize0( Object object );

    private native Object[] getLiveInstances0( Class<?> type, long tag );

    static void verifyPermission()
    {
        SecurityManager sm = System.getSecurityManager();
        if ( sm != null )
        {
            sm.checkPermission( PERMISSION );
        }
    }

    Object boxBool( boolean value )
    {
        return value;
    }

    boolean unboxBool( Boolean value )
    {
        return value;
    }

    Object boxByte( byte value )
    {
        return value;
    }

    byte unboxByte( Byte value )
    {
        return value;
    }

    Object boxChar( char value )
    {
        return value;
    }

    char unboxChar( Character value )
    {
        return value;
    }

    Object boxShort( short value )
    {
        return value;
    }

    short unboxShort( Short value )
    {
        return value;
    }

    Object boxInt( int value )
    {
        return value;
    }

    int unboxInt( Integer value )
    {
        return value;
    }

    Object boxLong( long value )
    {
        return value;
    }

    long unboxLong( Long value )
    {
        return value;
    }

    Object boxFloat( float value )
    {
        return value;
    }

    float unboxFloat( Float value )
    {
        return value;
    }

    Object boxDouble( double value )
    {
        return value;
    }

    double unboxDouble( Double value )
    {
        return value;
    }
}
