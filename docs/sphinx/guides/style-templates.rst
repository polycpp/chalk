Cache reusable style templates
==============================

**When to reach for this:** you call the same style on every log line
or every table row, and do not want to pay the cost of reconstructing
the chain each time.

:cpp:class:`polycpp::chalk::Chalk` is already immutable and copy-cheap
— each style layer is ``shared_ptr``-linked, not re-expanded — so the
idiomatic optimisation is simply to store the composed styler in a
variable and reuse it.

.. code-block:: cpp

   // Declare once, near the logger / renderer.
   static const auto SUCCESS = chalk().green().bold();
   static const auto FAIL    = chalk().red().bold().underline();
   static const auto MUTED   = chalk().dim();

   // Reuse per call.
   for (const auto& test : results) {
       const auto& tag = test.passed ? SUCCESS : FAIL;
       std::cout << tag(test.passed ? "PASS" : "FAIL")
                 << ' ' << MUTED(test.name) << '\n';
   }

The ``static`` qualifier ensures the chain is built once, at first
call, and shared across every invocation of the function.

Do not store the *result* of calling the styler if the text changes:
:cpp:func:`Chalk::operator()` returns a ``std::string`` that is only
valid for one specific input. Cache the styler, call it fresh.
