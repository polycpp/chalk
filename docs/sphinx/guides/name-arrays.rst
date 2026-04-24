Iterate over every colour or modifier by name
=============================================

**When to reach for this:** you are building a ``--list-colours``
command, a style picker, or a test that exercises every supported
modifier / colour.

The library exposes four named-style arrays you can iterate over at
runtime. They map 1:1 onto the chainable methods on
:cpp:class:`polycpp::chalk::Chalk`.

.. code-block:: cpp

   #include <polycpp/chalk/chalk.hpp>
   using namespace polycpp::chalk;

   for (const auto& name : modifierNames()) {
       // "reset", "bold", "dim", "italic", "underline", "overline",
       // "inverse", "hidden", "strikethrough"
       std::cout << name << '\n';
   }

   for (const auto& name : foregroundColorNames()) {
       // "black", "red", …, "whiteBright", "gray", "grey"
   }

   for (const auto& name : backgroundColorNames()) {
       // "bgBlack", "bgRed", …, "bgWhiteBright", "bgGray", "bgGrey"
   }

   for (const auto& name : colorNames()) {
       // Union of foreground + background.
   }

To go from a name string back to an ANSI code, cross-reference the
arrays against the ``constexpr AnsiCode`` constants in
:cpp:any:`polycpp::chalk::ansi` — the names are listed in the same
order as the codes are declared, so a parallel-index lookup works.

The arrays are ``const std::vector<std::string>&`` — the backing
storage is static, so capturing a reference is safe for the life of
the process.
