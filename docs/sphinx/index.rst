chalk
=====

**ANSI terminal colour and styling**

Chainable, level-aware ANSI escape codes for terminal output. Port of the npm chalk package — ``chalk().red().bold()`` returns a reusable styler, with 16-colour, 256-colour, truecolour, and automatic stream-level detection built in.

.. code-block:: cpp

   #include <polycpp/chalk/chalk.hpp>
   using namespace polycpp::chalk;

   // Use the auto-detected global or build your own styler.
   std::cout << chalk().red().bold()("Error: disk full") << '\n';

   // Compose once, reuse anywhere.
   auto warn = chalk().yellow().underline();
   std::cout << warn("heads up") << '\n';

   // RGB + hex + ANSI-256 all honour the detected colour level.
   std::cout << chalk().hex("#ff8800").bgRgb(20, 20, 40)("branded") << '\n';

.. grid:: 2

   .. grid-item-card:: Drop-in familiarity
      :margin: 1

      Mirrors npm ``chalk``'s chainable fluent API. ``chalk().red().bold()("boom")`` reads the same in C++20 as ``chalk.red.bold('boom')`` does in Node.

   .. grid-item-card:: C++20 native
      :margin: 1

      Header-only where possible, zero-overhead abstractions, ``constexpr``
      and ``std::string_view`` throughout.

   .. grid-item-card:: Tested
      :margin: 1

      81 GoogleTest cases cover ANSI code generation, the style-aggregator pipeline, hex/RGB/ANSI-256 colour conversion, and ``supports-color`` auto-detection.

   .. grid-item-card:: Plays well with polycpp
      :margin: 1

      Uses the same JSON value, error, and typed-event types as the rest of
      the polycpp ecosystem — no impedance mismatch.

Getting started
---------------

.. code-block:: bash

   # With FetchContent (recommended)
   FetchContent_Declare(
       polycpp_chalk
       GIT_REPOSITORY https://github.com/polycpp/chalk.git
       GIT_TAG        master
   )
   FetchContent_MakeAvailable(polycpp_chalk)
   target_link_libraries(my_app PRIVATE polycpp::chalk)

:doc:`Installation <getting-started/installation>` · :doc:`Quickstart <getting-started/quickstart>` · :doc:`Tutorials <tutorials/index>` · :doc:`API reference <api/index>`

.. toctree::
   :hidden:
   :caption: Getting started

   getting-started/installation
   getting-started/quickstart

.. toctree::
   :hidden:
   :caption: Tutorials

   tutorials/index

.. toctree::
   :hidden:
   :caption: How-to guides

   guides/index

.. toctree::
   :hidden:
   :caption: API reference

   api/index

.. toctree::
   :hidden:
   :caption: Examples

   examples/index
