package org.thobe.java.tooling;

class LocalNotInRangeException extends Exception
{
    @SuppressWarnings("unused"/*initialized from native code*/)
    LocalNotInRangeException( String message )
    {
        super( message );
    }

    @Override
    public synchronized Throwable fillInStackTrace()
    {
        return this;
    }
}
