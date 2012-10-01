package org.thobe.java.tooling;

import java.lang.reflect.Method;

public final class CallFrame
{
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
                    String.format( "Cannot access local variables of %s, variable table not available.", this ) );
        }
        for ( LocalVariable variable : variables )
        {
            if ( name.equals( variable.name ) )
            {
                synchronized ( this )
                {
                    if ( locals == null )
                    { // this frame is live, get the value from the underlying frame
                        return tools.getLocal( thread, method, height, variable.start, variable.length,
                                               variable.slot, variable.signature.charAt( 0 ) );
                    }
                    else
                    { // this frame has been detached, get the value from the table
                        if ( position < variable.start || position > (variable.start + variable.length) )
                        {
                            throw new IllegalStateException(
                                    String.format( "Local variable '%s' is out of range [%d-%d], current position: %d.",
                                                   name, variable.start, variable.start + variable.length, position ) );
                        }
                        else if ( variable.slot >= locals.length )
                        {
                            throw new IllegalStateException( String.format(
                                    "The slot [%d] for the local variable '%s' has not been captured (%d captured).",
                                    variable.slot, name, locals.length ) );
                        }
                        return locals[variable.slot];
                    }
                }
            }
        }
        throw new IllegalArgumentException( String.format( "No such local variable: '%s'.", name ) );
    }

    public void setLocal( String name, String value )
    {
        if ( name == null )
        {
            throw new IllegalArgumentException( "name is null" );
        }
        if ( variables == null )
        {
            throw new UnsupportedOperationException(
                    String.format( "Cannot access local variables of %s, variable table not available.", this ) );
        }
        for ( LocalVariable variable : variables )
        {
            if ( name.equals( variable.name ) )
            {
                synchronized ( this )
                {
                    if ( locals == null )
                    { // this frame is live, we can set the value in the underlying frame
                        tools.setLocal( thread, method, height, variable.start, variable.length, variable.slot,
                                        variable.signature.charAt( 0 ), value );
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
        this.locals = locals;
    }
}
