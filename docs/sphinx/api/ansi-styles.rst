ANSI codes and colour conversion
================================

Low-level helpers in the ``polycpp::chalk::ansi`` namespace. Most
callers use the :cpp:class:`polycpp::chalk::Chalk` interface, but the
escape-code formatters and colour-space converters are public so you
can build your own rendering pipeline.

Declared in ``polycpp/chalk/ansi_styles.hpp``.

AnsiCode
--------

.. doxygenstruct:: polycpp::chalk::ansi::AnsiCode
   :members:
   :undoc-members:

Escape-code formatters
----------------------

.. doxygenfunction:: polycpp::chalk::ansi::wrapAnsi16
.. doxygenfunction:: polycpp::chalk::ansi::wrapAnsi256
.. doxygenfunction:: polycpp::chalk::ansi::wrapAnsi16m

Colour-space conversion
-----------------------

.. doxygenfunction:: polycpp::chalk::ansi::rgbToAnsi256
.. doxygenfunction:: polycpp::chalk::ansi::hexToRgb
.. doxygenfunction:: polycpp::chalk::ansi::ansi256ToAnsi
.. doxygenfunction:: polycpp::chalk::ansi::hexToAnsi256
.. doxygenfunction:: polycpp::chalk::ansi::rgbToAnsi

Level-aware code generators
---------------------------

.. doxygenfunction:: polycpp::chalk::ansi::makeColorCode(int, int, int, int, bool)
.. doxygenfunction:: polycpp::chalk::ansi::makeColorCode(int, const std::string&, bool)
.. doxygenfunction:: polycpp::chalk::ansi::makeColorCode256

Constants
---------

The header also defines ``constexpr AnsiCode`` values for every basic
modifier and colour (``RESET``, ``BOLD``, ``RED``, ``BG_BLUE``, …) and
two group-level close strings (``FG_CLOSE``, ``BG_CLOSE``). Browse
them inline — the full list is documented on the namespace overview:

.. doxygennamespace:: polycpp::chalk::ansi
   :desc-only:
