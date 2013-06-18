A Java interface for JVMTI
==========================

This library provides introspection access for:

* Heap walking.

  This provides the ability to find all instances of a class.
  
* Call frames.

  This provides the ability to access local variables from methods
  invoking the current method.


Security measures
-----------------

The operations provided by this library are inherantly unsafe. To
allow you to guard against misuse of this library, a few measures are
in place.

* Access to an instance of `ToolingInterface` (from where all
  operations are provided) is guarded by the ToolingInterface
  implementing the singleton pattern, with the reference to the sole
  instance being in native code, inaccessible from Java, through
  reflection or otherwise.

* Acquiring the singleton instance of ToolingInterface requires the
  `"org.thobe.java.tooling.ToolingInterface"`
  `RuntimePermission`. Checking for this permission is performed from
  native code where the instance is stored, preventing any means of
  circumventing the checking.

* In order to guard against illicitly created instances of
  `ToolingInterface` (for example created through `sun.misc.Unsafe`),
  each native method verifies the `this` reference against the stored
  instance of `ToolingInterface`, rejecting invocations where the
  reference is not the same.

* The greatest security risk comes from _leaking_ the
  `ToolingInterface` instance. Instances of `CallFrame` hold a private
  reference to the `ToolingInterface` instance, and with access to
  call frames it is also possible to access a `ToolingInterface`
  instance stored in a local variable of that frame, or in a field of
  some other object if not properly guarded. Regular guards against
  reflective access should be sufficient to prevent this though.
