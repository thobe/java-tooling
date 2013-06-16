package org.thobe.java.tooling;

enum Capability
{
    can_tag_objects( "can access heap" ),
    can_access_local_variables( "can access frame variables" ),
    can_generate_frame_pop_events( "can access live frame" ),
    can_suspend( "can get access frames of other threads" ),
    can_get_source_file_name( "can get frame element with source" ),
    can_get_line_numbers( "can get frame element with line number" ),
    ;
    private final String str;

    private Capability( String str )
    {
        this.str = str;
    }

    @Override
    public String toString()
    {
        return str;
    }
}
