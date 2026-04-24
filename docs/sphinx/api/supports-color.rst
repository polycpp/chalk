Colour-support detection
========================

Runtime detection of what the current terminal (or an arbitrary file
descriptor) can render. Respects ``NO_COLOR`` and ``FORCE_COLOR``
exactly like the npm ``supports-color`` package.

Declared in ``polycpp/chalk/supports_color.hpp``.

ColorSupport
------------

.. doxygenstruct:: polycpp::chalk::ColorSupport
   :members:
   :undoc-members:

Functions
---------

.. doxygenfunction:: polycpp::chalk::detectColorSupport
.. doxygenfunction:: polycpp::chalk::supportsColor
.. doxygenfunction:: polycpp::chalk::supportsColorStderr
