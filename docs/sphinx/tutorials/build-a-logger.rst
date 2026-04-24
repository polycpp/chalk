Build a level-aware logger
==========================

**You'll build:** a tiny ``Logger`` class that prints INFO / WARN /
ERROR lines with per-severity colour, falling back gracefully to plain
text when stdout is not a terminal.

**You'll use:**
:cpp:class:`polycpp::chalk::Chalk`,
:cpp:func:`polycpp::chalk::chalk`,
:cpp:func:`polycpp::chalk::chalkStderr`.

**Prerequisites:** a CMake project that links ``polycpp::chalk``. See
:doc:`../getting-started/installation` if you haven't wired that up yet.

Step 1 — pick the right global styler
-------------------------------------

Every process gets two free stylers: one for stdout, one for stderr.
Both auto-detect the colour support of their respective streams, so
you do not have to probe ``isatty`` yourself.

.. code-block:: cpp

   #include <polycpp/chalk/chalk.hpp>
   using namespace polycpp::chalk;

   Chalk& out = chalk();         // stdout, level = supportsColor()
   Chalk& err = chalkStderr();   // stderr, level = supportsColorStderr()

The returned references are process-wide singletons: do not delete
them, do not move from them, and do not mutate ``level()`` unless you
really want every caller in the binary to see the change.

Step 2 — derive styled sub-loggers once
---------------------------------------

A fresh ``Chalk`` per call would be wasteful; cache the three stylers
you need at construction time.

.. code-block:: cpp

   class Logger {
   public:
       Logger()
           : info_(chalk().cyan().bold())
           , warn_(chalk().yellow().bold())
           , error_(chalkStderr().red().bold()) {}

       void info(const std::string& msg)  { emit(std::cout, info_,  "INFO",  msg); }
       void warn(const std::string& msg)  { emit(std::cout, warn_,  "WARN",  msg); }
       void error(const std::string& msg) { emit(std::cerr, error_, "ERROR", msg); }

   private:
       static void emit(std::ostream& os, const Chalk& tag,
                        const std::string& label, const std::string& msg) {
           os << tag("[" + label + "]") << ' ' << msg << '\n';
       }

       Chalk info_, warn_, error_;
   };

Note that ``error_`` is built from ``chalkStderr()``, not ``chalk()`` —
the level detection is per-stream. If stdout is redirected to a file
but stderr is still a TTY (a very common CI pattern), errors still
come out coloured on the terminal while info/warn degrade to plain
text on disk.

Step 3 — use it like an ordinary logger
---------------------------------------

.. code-block:: cpp

   int main() {
       Logger log;
       log.info("starting up");
       log.warn("disk 82% full");
       log.error("upstream unreachable");
   }

Run it twice — once in a terminal, once piped to ``cat`` — and confirm
that the coloured escapes disappear in the second run. No code change
required.

Step 4 — make it overridable for testing
----------------------------------------

Auto-detection is great for production, terrible for tests that want
deterministic output. Accept an override at construction:

.. code-block:: cpp

   class Logger {
   public:
       explicit Logger(std::optional<int> level = std::nullopt) {
           Chalk c = level
               ? Chalk(Options{.level = *level})
               : chalk();
           info_  = c.cyan().bold();
           warn_  = c.yellow().bold();
           error_ = c.red().bold();
       }
       // …
   };

In tests, pass ``Logger(0)`` to get plain strings you can
``EXPECT_EQ`` against. In production, pass nothing and inherit the
auto-detected level.

What you learned
----------------

- :cpp:func:`polycpp::chalk::chalk` and
  :cpp:func:`polycpp::chalk::chalkStderr` give you two pre-configured
  stylers — use the one that matches the stream you're writing to.
- ``Chalk`` instances are immutable and cheap to copy; cache the
  composed styler rather than rebuilding it per message.
- For tests, bypass auto-detection by constructing
  ``Chalk(Options{.level = …})`` directly.
