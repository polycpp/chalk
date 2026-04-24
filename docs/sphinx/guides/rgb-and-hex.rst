Use RGB, hex, or ANSI-256 colours
=================================

**When to reach for this:** your brand or design system specifies a
colour as ``#1abc9c`` / ``rgb(26, 188, 156)`` / ANSI index ``36``,
and you want it on the terminal without manually computing escape
codes.

Three equivalent entry points on :cpp:class:`polycpp::chalk::Chalk`:

.. code-block:: cpp

   #include <polycpp/chalk/chalk.hpp>
   using namespace polycpp::chalk;

   Chalk c(Options{.level = 3});   // force truecolour for the demo

   // Hex — accepts #RRGGBB, RRGGBB, #RGB, and RGB forms.
   std::cout << c.hex("#1abc9c")("mint foreground") << '\n';
   std::cout << c.bgHex("#2962FF")("blue background") << '\n';

   // RGB components.
   std::cout << c.rgb(26, 188, 156)("same mint, spelled decimal") << '\n';

   // ANSI-256 palette index.
   std::cout << c.ansi256(75)("cornflower") << '\n';
   std::cout << c.bgAnsi256(236)("near-black panel") << '\n';

All three are **level-aware**: at level 1 they collapse to the nearest
basic colour, at level 2 they map to ANSI-256, and at level 3 they
emit exact 24-bit codes. You write the code once; the level decides
what bytes actually hit the wire.

If you need the raw numbers (e.g., to build a colour-picker UI),
reach for the low-level helpers in
:cpp:any:`polycpp::chalk::ansi` —
:cpp:func:`polycpp::chalk::ansi::hexToRgb`,
:cpp:func:`polycpp::chalk::ansi::rgbToAnsi256`,
:cpp:func:`polycpp::chalk::ansi::ansi256ToAnsi`.
