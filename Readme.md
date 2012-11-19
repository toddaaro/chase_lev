
This is a basic implementation of the Chase-Lev workstealing deque in
C. This is a lock-free data structure.

The current status is that it mostly works, but not entirely. There is
an issue with O2 level optimizations breaking multi-socket machine
functionality. The fix for this is to mark a few things as 'volatile',
but I have not yet done so. Hopefully I'll get around to it, but this
was a toy project so maybe not.


