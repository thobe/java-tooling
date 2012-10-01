package org.thobe.java.tooling;

class LocalVariable
{
    final long start;
    final int length;
    final String signature;
    final int slot;
    final String name;

    LocalVariable( long start, int length, String signature, int slot, String name )
    {
        this.start = start;
        this.length = length;
        this.signature = signature;
        this.slot = slot;
        this.name = name;
    }
}
