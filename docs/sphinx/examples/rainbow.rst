Rainbow text
============

Print a phrase where each character cycles through the HSV colour
wheel, demonstrating truecolour (24-bit) output.

.. literalinclude:: ../../../examples/rainbow.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_CHALK_BUILD_EXAMPLES=ON
   cmake --build build --target rainbow
   ./build/examples/rainbow "polycpp chalk"
