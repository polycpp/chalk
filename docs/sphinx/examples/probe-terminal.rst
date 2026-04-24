Probe the terminal
==================

A diagnostic that prints the detected colour support for stdout and
stderr, then shows a live sample at each capability level the
terminal claims. Handy inside a ``--version`` or a support-ticket
template.

.. literalinclude:: ../../../examples/probe_terminal.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_CHALK_BUILD_EXAMPLES=ON
   cmake --build build --target probe_terminal
   ./build/examples/probe_terminal
   NO_COLOR=1 ./build/examples/probe_terminal
   FORCE_COLOR=3 ./build/examples/probe_terminal
