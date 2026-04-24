Style a line built from many parts
==================================

**When to reach for this:** you want to print several arguments
separated by a space — the way Node's ``console.log`` works — without
building the final string by hand.

:cpp:class:`polycpp::chalk::Chalk` overloads ``operator()`` for a
variadic argument list. Each argument is streamed to a
``std::ostringstream`` and the results are joined with a single
space, then wrapped in the styler's ANSI codes.

.. code-block:: cpp

   #include <polycpp/chalk/chalk.hpp>
   using namespace polycpp::chalk;

   auto tag = chalk().cyan().bold();

   int retries = 3;
   std::string host = "api.example.com";

   std::cout << tag("retrying", retries, "times against", host) << '\n';
   // -> [cyan bold]retrying 3 times against api.example.com[/]

The argument list can mix any streamable types — strings, integers,
floats, custom types with an ``operator<<`` overload. There is no
per-argument styling; the whole joined line gets the same chain.

For per-argument styling, call the styler once per segment and
concatenate:

.. code-block:: cpp

   auto err = chalk().red().bold();
   auto msg = chalk();
   std::cout << err("ERROR:") << ' ' << msg("disk full on") << ' '
             << err(host) << '\n';
