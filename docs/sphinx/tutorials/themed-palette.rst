Apply a brand palette with truecolour
=====================================

**You'll build:** a themed CLI splash screen that uses your brand's
hex palette. The same code degrades to ANSI-256 on a 256-colour
terminal and to the nearest basic colour on a 16-colour terminal —
automatically.

**You'll use:**
:cpp:func:`polycpp::chalk::Chalk::hex`,
:cpp:func:`polycpp::chalk::Chalk::rgb`,
:cpp:func:`polycpp::chalk::Chalk::bgHex`,
:cpp:func:`polycpp::chalk::Chalk::bgRgb`.

**Prerequisites:** a terminal that reports ``COLORTERM=truecolor`` if
you want to see the 24-bit result. Any modern iTerm2, Windows Terminal,
or GNOME Terminal will do.

Step 1 — define the palette
---------------------------

Keep the brand colours in one place so designers can update them
without hunting through the codebase.

.. code-block:: cpp

   #include <polycpp/chalk/chalk.hpp>
   using namespace polycpp::chalk;

   namespace palette {
       constexpr const char* primary   = "#2962FF";   // polycpp blue
       constexpr const char* accent    = "#FF8800";   // warm orange
       constexpr const char* success   = "#0CA678";   // green
       constexpr const char* danger    = "#E03131";   // red
       constexpr const char* charcoal  = "#343a40";   // backgrounds
   }

Step 2 — compose the stylers
----------------------------

Each brand role gets a dedicated :cpp:class:`Chalk`. Background fills
use ``bgHex``; emphasis stacks a modifier like ``bold()`` on top.

.. code-block:: cpp

   struct Theme {
       Chalk title;
       Chalk link;
       Chalk success;
       Chalk danger;
       Chalk chrome;   // dark box background for panels
   };

   Theme makeTheme(const Chalk& base) {
       return {
           .title   = base.hex(palette::primary).bold(),
           .link    = base.hex(palette::accent).underline(),
           .success = base.hex(palette::success),
           .danger  = base.hex(palette::danger).bold(),
           .chrome  = base.bgHex(palette::charcoal),
       };
   }

Pass :cpp:func:`polycpp::chalk::chalk` as ``base`` in production or a
forced-level :cpp:class:`Chalk` in tests.

Step 3 — render the splash
--------------------------

.. code-block:: cpp

   void splash(const Theme& t) {
       std::cout << t.chrome("                                      ") << '\n';
       std::cout << t.chrome("  ") << t.title("  polycpp chalk demo  ")
                 << t.chrome("  ") << '\n';
       std::cout << t.chrome("                                      ") << '\n';
       std::cout << "Docs: " << t.link("https://polycpp.github.io/chalk/") << '\n';
       std::cout << t.success("  build ok  ") << ' '
                 << t.danger("  3 warnings  ") << '\n';
   }

Step 4 — verify graceful degradation
------------------------------------

Run the program with each level and watch the output change shape:

.. code-block:: bash

   # truecolour
   FORCE_COLOR=3 ./splash

   # ANSI-256
   FORCE_COLOR=2 ./splash

   # basic 16
   FORCE_COLOR=1 ./splash

   # no colour at all (great for ``grep`` and CI logs)
   NO_COLOR=1 ./splash

At level 1 the ``#2962FF`` blue collapses to the nearest basic colour
(blue, code 34). At level 2 it maps to an ANSI-256 index. At level 3
you get the exact 24-bit triple. You wrote the code once.

What you learned
----------------

- Define the palette as hex constants; the styler does the colour-space
  math at render time.
- Wrap the per-role stylers in a ``Theme`` struct so swapping themes is
  a single assignment.
- The same :cpp:class:`Chalk` chain works at every level because
  :cpp:func:`Chalk::hex` and :cpp:func:`Chalk::rgb` consult the
  instance's level before emitting escape codes.
