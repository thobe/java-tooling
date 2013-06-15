package org.thobe.java.tooling;

import java.lang.reflect.Method;
import java.util.NoSuchElementException;

import static java.lang.String.format;

public final class CallFrame
{
    private static final Object[] NO_OBJECTS = new Object[0];
    private final ToolingInterface tools;
    final Thread thread;
    private final Method method;
    final int height;
    private final Object that;
    private final LocalVariable[] variables;
    // detached
    private Object[] locals;
    private long position = -1;

    CallFrame( ToolingInterface tools, Thread thread, Method method, int height, Object that,
               LocalVariable[] variables )
    {
        this.tools = tools;
        this.thread = thread;
        this.method = method;
        this.height = height;
        this.that = that;
        this.variables = variables;
    }

    public Object getLocal( String name )
    {
        if ( name == null )
        {
            throw new IllegalArgumentException( "name is null" );
        }
        if ( variables == null )
        {
            throw new UnsupportedOperationException(
                    format( "Cannot access local variables of %s, variable table not available.", this ) );
        }
        String notInRange = null;
        for ( LocalVariable variable : variables )
        {
            if ( name.equals( variable.name ) )
            {
                synchronized ( this )
                {
                    if ( locals == null )
                    { // this frame is live, get the value from the underlying frame
                        try
                        {
                            return tools.getLocal( thread, method, height, variable.start, variable.length,
                                                   variable.slot, variable.signature.charAt( 0 ) );
                        }
                        catch ( LocalNotInRangeException e )
                        {
                            notInRange = e.getMessage();
                        }
                    }
                    else
                    { // this frame has been detached, get the value from the table
                        if ( position < variable.start || position > (variable.start + variable.length) )
                        {
                            notInRange = format( "[%d-%d] (position in frame: %d).",
                                                 variable.start, variable.start + variable.length, position );
                        }
                        else if ( variable.slot >= locals.length )
                        {
                            throw new IllegalStateException( format(
                                    "The slot [%d] for the local variable '%s' has not been captured (%d captured).",
                                    variable.slot, name, locals.length ) );
                        }
                        else
                        {
                            return locals[variable.slot];
                        }
                    }
                }
            }
        }
        throw new NoSuchElementException(
                format( notInRange == null ? "No such local variable: '%s'."
                                           : "The local variable '%s' is out of range " + notInRange,
                        name ) );
    }

    public void setLocal( String name, Object value )
    {
        if ( name == null )
        {
            throw new IllegalArgumentException( "name is null" );
        }
        if ( variables == null )
        {
            throw new UnsupportedOperationException(
                    format( "Cannot access local variables of %s, variable table not available.", this ) );
        }
        for ( LocalVariable variable : variables )
        {
            if ( name.equals( variable.name ) )
            {
                synchronized ( this )
                {
                    if ( locals == null )
                    { // this frame is live, we can set the value in the underlying frame
                        try
                        {
                            tools.setLocal( thread, method, height, variable.start, variable.length, variable.slot,
                                            variable.signature.charAt( 0 ), value );
                        }
                        catch ( NullPointerException e )
                        {
                            throw new NullPointerException( name );
                        }
                        catch ( ClassCastException e )
                        {
                            String type = e.getMessage();
                            if ( type == null || type.isEmpty() )
                            {
                                type = variable.signature.substring( 1, variable.signature.length() - 1 )
                                               .replace( '/', '.' );
                            }
                            throw new ClassCastException( '"' + name + "\" may not be " + value.getClass().getName() +
                                                          ", must be instance of " + type );
                        }
                    }
                    else
                    {
                        throw new IllegalStateException( "Cannot set local variable on detached frame." );
                    }
                }
            }
        }
    }

    public Object getThis()
    {
        return that;
    }

    synchronized void detach( long position, Object[] locals )
    {
        this.position = position;
        this.locals = locals == null ? NO_OBJECTS : locals;
    }
}
