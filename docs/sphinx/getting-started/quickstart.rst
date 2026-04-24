Quickstart
==========

This page walks through a minimal chalk program end-to-end. Copy the
snippet, run it, then jump to :doc:`../tutorials/index` for task-oriented
walkthroughs or :doc:`../api/index` for the full reference.

The example below builds a tiny log formatter: three severity levels —
INFO, WARN, ERROR — each with its own colour and weight. We force
``level = 2`` (ANSI-256) so the output looks the same whether you run it
in a pipe, a CI log, or a modern terminal; in a real program you would
leave that to :cpp:func:`polycpp::chalk::chalk`, which inspects the TTY
for you.

Full example
------------

.. code-block:: cpp

   #include <iostream>
   #include <string>
   #include <polycpp/chalk/chalk.hpp>

   using namespace polycpp::chalk;

   // Build a log line with a coloured severity tag.
   static std::string log_line(const Chalk& tag,
                               const std::string& label,
                               const std::string& message) {
       return tag("[" + label + "]") + " " + message;
   }

   int main() {
       // Force level 2 so the snippet is reproducible even without a TTY.
       Chalk c(Options{.level = 2});

       // Compose once, reuse.
       auto info  = c.cyan().bold();
       auto warn  = c.yellow().bold();
       auto error = c.red().bold();

       std::cout << log_line(info,  "INFO",  "starting up")          << '\n';
       std::cout << log_line(warn,  "WARN",  "disk 82% full")        << '\n';
       std::cout << log_line(error, "ERROR", "upstream unreachable") << '\n';

       // Ad-hoc highlight inside prose.
       auto hi = c.hex("#ff8800").underline();
       std::cout << "build " << hi("succeeded") << " in 3.4s\n";

       return 0;
   }

Compile it with the same CMake wiring from :doc:`installation`:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build
   ./build/my_app

Expected output (with ANSI escapes rendered — in a real terminal the
bracketed tags and the word "succeeded" are coloured):

.. code-block:: text

   [INFO] starting up
   [WARN] disk 82% full
   [ERROR] upstream unreachable
   build succeeded in 3.4s

What just happened
------------------

A :cpp:class:`polycpp::chalk::Chalk` instance is an immutable styler — every
modifier (``red()``, ``bold()``, ``hex("#ff8800")``, …) returns a **new**
``Chalk`` that remembers the previous styles. Calling the result like a
function (``info("[INFO]")``) wraps the text in the accumulated ANSI
open/close codes and returns a ``std::string``.

Because styler instances are cheap to copy, the idiomatic pattern is:
**build it once, keep the variable, reuse it**. ``info``, ``warn``, and
``error`` above are three distinct stylers that share no mutable state.

The ``level`` field caps the colour space: ``0`` disables colour
entirely (the styler returns plain text), ``1`` maps RGB down to the 16
basic colours, ``2`` uses the ANSI 256 palette, and ``3`` emits 24-bit
truecolor. If you construct ``Chalk()`` with no options, level defaults
to ``0`` — call :cpp:func:`polycpp::chalk::chalk` instead to get a global
styler that inherits the auto-detected level for stdout.

Next steps
----------

- :doc:`../tutorials/index` — step-by-step walkthroughs of common tasks.
- :doc:`../guides/index` — short how-tos for specific problems.
- :doc:`../api/index` — every public type, function, and option.
- :doc:`../examples/index` — runnable programs you can drop into a sandbox.
