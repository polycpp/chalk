Coloured diff summary
=====================

Walks a list of ``(marker, text)`` pairs and prints each one with a
``git diff``-style colour: green for additions, red for removals,
dim for context.

.. literalinclude:: ../../../examples/diff_table.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_CHALK_BUILD_EXAMPLES=ON
   cmake --build build --target diff_table
   ./build/examples/diff_table
