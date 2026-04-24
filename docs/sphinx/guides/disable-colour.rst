Disable colour output
=====================

**When to reach for this:** unit tests, logs that will be grepped, or a
``--no-color`` flag for end users.

Three options, in increasing order of scope.

.. code-block:: cpp

   // 1. Construct a styler pinned to level 0 — local scope only.
   Chalk plain(Options{.level = 0});
   std::cout << plain.red().bold()("still plain text\n");

   // 2. Mutate the process-wide stdout styler — affects every caller.
   chalk().setLevel(0);

   // 3. Let the user decide via environment variable. supportsColor()
   //    honours NO_COLOR=1 automatically, so this is zero-code:
   //      NO_COLOR=1 ./my_app

Prefer option 3 whenever possible — it is the contract users expect
from any CLI that touches a terminal (see `no-color.org
<https://no-color.org>`_). Options 1 and 2 are useful when you want
deterministic strings inside a test, or a programmatic opt-out
triggered by a flag parser instead of an env var.

Do not try to post-process the output by stripping escape codes
yourself; ``level = 0`` makes :cpp:func:`Chalk::operator()` emit the
plain text in the first place, saving a pass.
