Examples
========

Self-contained programs exercising the main features of chalk. Each
example compiles against the public API only — no private headers, no
non-exported targets.

.. toctree::
   :maxdepth: 1

   rainbow
   probe-terminal
   diff-table

Running an example
------------------

From the repository root:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build --target <example_name>
   ./build/examples/<example_name>

Examples are only built when ``POLYCPP_CHALK_BUILD_EXAMPLES=ON`` is passed to CMake.
