package org.thobe.java.tooling;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

class FrameManager
{
    private final Map<FrameLocation, CallFrame> frames = new ConcurrentHashMap<FrameLocation, CallFrame>();

    void pushFrame( CallFrame frame )
    {
        frames.put( new FrameLocation( frame.thread, frame.height ), frame );
    }

    CallFrame popFrame( Thread thread, int height )
    {
        return frames.remove( new FrameLocation( thread, height ) );
    }

    private static final class FrameLocation
    {
        private final Thread thread;
        private final int height;

        FrameLocation( Thread thread, int height )
        {
            this.thread = thread;
            this.height = height;
        }

        @Override
        public int hashCode()
        {
            return 31 * thread.hashCode() + height;
        }

        @Override
        public boolean equals( Object obj )
        {
            if ( this == obj )
            {
                return true;
            }
            if ( obj instanceof FrameLocation )
            {
                FrameLocation that = (FrameLocation) obj;
                return this.height == that.height && this.thread == that.thread;
            }
            return false;
        }
    }
}
