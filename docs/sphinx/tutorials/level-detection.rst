Colour-level detection end-to-end
=================================

**You'll build:** a probe utility that prints the detected colour
support for stdout and stderr, plus a sample of each supported level.
This is the same logic chalk uses internally, surfaced so you can call
it directly from config screens and ``--version``-style commands.

**You'll use:**
:cpp:func:`polycpp::chalk::supportsColor`,
:cpp:func:`polycpp::chalk::supportsColorStderr`,
:cpp:func:`polycpp::chalk::detectColorSupport`,
:cpp:struct:`polycpp::chalk::ColorSupport`.

**Prerequisites:** none — the detection functions are pure and do
not touch the escape-code generator.

Step 1 — read the cached results for standard streams
-----------------------------------------------------

The two stream-level helpers are cached on first call, so hitting them
is free after startup.

.. code-block:: cpp

   #include <polycpp/chalk/supports_color.hpp>
   using namespace polycpp::chalk;

   const ColorSupport& out = supportsColor();
   const ColorSupport& err = supportsColorStderr();

   std::cout << "stdout level: " << out.level << '\n';
   std::cout << "stdout has 256? " << out.has256 << '\n';
   std::cout << "stderr level: " << err.level << '\n';

The :cpp:struct:`ColorSupport` struct exposes three boolean convenience
fields — ``hasBasic``, ``has256``, ``has16m`` — so you can avoid the
magic numbers 1, 2, 3 in conditionals.

Step 2 — detect an arbitrary file descriptor
--------------------------------------------

If you are writing to a pipe, a ``popen`` child, or a log file that is
occasionally a TTY, query the fd directly:

.. code-block:: cpp

   int fd = fileno(log_file);
   ColorSupport s = detectColorSupport(fd);
   if (s.level > 0) {
       // safe to emit escape codes here
   }

:cpp:func:`detectColorSupport` is **not** cached. Call it once per
logical stream and keep the result.

Step 3 — print a level banner
-----------------------------

Users love ``--probe`` flags. Here is one you can paste verbatim:

.. code-block:: cpp

   static const char* levelName(int l) {
       switch (l) {
           case 0: return "none (plain text)";
           case 1: return "basic 16 colours";
           case 2: return "ANSI-256 palette";
           case 3: return "truecolor (24-bit)";
           default: return "unknown";
       }
   }

   void probe() {
       const auto& s = supportsColor();
       Chalk c(Options{.level = s.level});
       std::cout << "colour support: " << levelName(s.level) << '\n';
       std::cout << "  basic 16: " << c.red()("red")     << " "
                                   << c.green()("green") << " "
                                   << c.blue()("blue")   << '\n';
       if (s.has256) {
           std::cout << "  256:      " << c.ansi256(196)("196") << " "
                                       << c.ansi256( 46)( "46") << " "
                                       << c.ansi256( 21)( "21") << '\n';
       }
       if (s.has16m) {
           std::cout << "  truecolour: " << c.hex("#ff0066")("#ff0066") << " "
                                         << c.hex("#00cc99")("#00cc99") << '\n';
       }
   }

Step 4 — respect NO_COLOR and FORCE_COLOR
-----------------------------------------

Both environment variables are honoured automatically by
:cpp:func:`supportsColor`. You do not need to check them yourself.
``NO_COLOR=1`` clamps the level to 0; ``FORCE_COLOR=N`` (with ``N`` in
``0..3``) overrides the detected level. This is the same contract the
npm ``supports-color`` package defines, so users switching from Node
already know how it behaves.

Verify by running:

.. code-block:: bash

   ./probe              # whatever the terminal supports
   NO_COLOR=1 ./probe   # level 0
   FORCE_COLOR=3 ./probe  # level 3, even inside a pipe

What you learned
----------------

- :cpp:func:`supportsColor` / :cpp:func:`supportsColorStderr` are the
  usual entry points; they are cached and respect ``NO_COLOR`` /
  ``FORCE_COLOR``.
- :cpp:func:`detectColorSupport` handles arbitrary file descriptors
  and is not cached.
- The :cpp:struct:`ColorSupport` fields map 1:1 onto the four
  rendering tiers you can emit.
