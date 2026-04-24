Chalk styler
============

The chainable styler that wraps text in accumulated ANSI codes.
Declared in ``polycpp/chalk/chalk.hpp``.

Chalk
-----

.. doxygenclass:: polycpp::chalk::Chalk
   :members:
   :undoc-members:

Options
-------

.. doxygenstruct:: polycpp::chalk::Options
   :members:
   :undoc-members:

Styler
------

Linked-list node that holds the accumulated open/close codes for one
layer of styling. Exposed for advanced use (custom renderers, snapshot
testing); most code should never need to touch it.

.. doxygenstruct:: polycpp::chalk::Styler
   :members:
   :undoc-members:

Global stylers
--------------

Two process-wide instances with auto-detected colour levels — one per
standard stream.

.. doxygenfunction:: polycpp::chalk::chalk
.. doxygenfunction:: polycpp::chalk::chalkStderr

Name arrays
-----------

Runtime-iterable lists of modifier and colour names that match the
methods on :cpp:class:`polycpp::chalk::Chalk`. See
:doc:`../guides/name-arrays` for usage.

.. doxygenfunction:: polycpp::chalk::modifierNames
.. doxygenfunction:: polycpp::chalk::foregroundColorNames
.. doxygenfunction:: polycpp::chalk::backgroundColorNames
.. doxygenfunction:: polycpp::chalk::colorNames
