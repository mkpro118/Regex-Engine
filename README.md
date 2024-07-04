<h1 align="center">
  Simple Regex Engine in C
</h1>

<p align="center">

  <a href="https://github.com/mkpro118/Regex-Engine/actions/workflows/lint.yaml">
    <img alt="MyPy" src="https://github.com/mkpro118/Regex-Engine/actions/workflows/lint.yaml/badge.svg">
  </a>
  
  <a href="https://github.com/mkpro118/Regex-Engine/actions/workflows/tests.yaml">
    <img alt="Tests" src="https://github.com/mkpro118/Regex-Engine/actions/workflows/tests.yaml/badge.svg">
  </a>

  <a href="https://github.com/mkpro118/CredentialsManager/blob/main/LICENSE">
    <img alt="MIT LICENSE" src="https://img.shields.io/badge/License-MIT-blue.svg"/>
  </a>
</p>

## Project Description

This project is a simple regex engine implemented in C.
The primary purpose of this project is educational, focusing on learning and
implementing key components of a regex compiler and matcher, including:

- Lexer
- Parser
- Abstract Syntax Tree (AST)
- Converting ASTs to NFAs (Non-deterministic Finite Automata)
- Regex compiler
- Regex matcher

## Technologies Used

- C programming language
- GNU Make for build automation
- Address Sanitizer for memory checks

No external libraries are used in this project.

## Prerequisites

To build and run this project, you need:

- A C compiler
- GNU Make

Optional:
- Valgrind (for running tests in a memory-checking environment)

## Installation and Setup

1. Clone the repository:
   ```
   git clone https://github.com/mkpro118/Regex-Engine.git
   ```

2. Navigate to the project directory:
   ```
   cd Regex-Engine
   ```

3. Build the project:
   ```
   make
   ```
   or
   ```
   make build
   ```

## Usage

To use the regex engine in your C program:

1. Include the necessary headers:
   ```c
   #include "regex.h"
   ```

2. Create a regex object:
   ```c
   Regex* regex = regex_create("your_pattern_here");
   ```

3. Match a string against the regex:
   ```c
   bool match = regex_match(regex, "string_to_match");
   ```

4. Free the regex object when done:
   ```c
   regex_free(regex);
   free(regex);
   ```

Example:
```c
#include <stdio.h>
#include "regex.h"

int main() {
    Regex* regex = regex_create("a(b|c)*d");
    
    if (regex == NULL) {
        printf("Failed to create regex\n");
        return 1;
    }
    
    printf("Match: %s\n", regex_match(regex, "abbbcd") ? "true" : "false");
    printf("Match: %s\n", regex_match(regex, "ad") ? "true" : "false");
    printf("Match: %s\n", regex_match(regex, "abba") ? "true" : "false");
    
    regex_free(regex);
    return 0;
}
```

## Features

1. **Basic Regex Operations**: Supports basic regex operations including:
   - Character matching
   - Alternation (`|`)
   - Kleene star (`*`)
   - Kleene Plus (`+`)
   - Optional (`?`)
   - Grouping with parentheses

2. **NFA-based Matching**: Uses Non-deterministic Finite Automata (NFA) for pattern matching, allowing for efficient and flexible regex processing.

3. **AST Representation**: Builds an Abstract Syntax Tree (AST) representation of the regex pattern, which is then converted to an NFA.

4. **Epsilon Closure**: Implements epsilon closure for NFA transitions, enabling proper handling of epsilon (empty) transitions in the regex.

5. **Memory Management**: Careful memory management with proper initialization and cleanup functions for all major components (Lexer, Parser, AST, NFA).

6. **Portability**: Includes portability considerations for different operating systems (Windows, Unix-like systems).

## Contributing

Contributions to this project are welcome. You can contribute by:

1. [Submitting issues](https://github.com/mkpro118/Regex-Engine/issues/new) for bugs or feature requests
2. [Creating pull requests](https://github.com/mkpro118/Regex-Engine/pulls) for proposed changes or improvements

Please ensure that your contributions align with the project's goals and coding standards.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Testing

A comprehensive test suite for the regex engine can be found in the [tests](tests) directory.
Tests can be run using either ASan or Valgrind to monitor program memory.

Running

```bash
make test
```
will run the tests twice, first with ASan, then with valgrind.

Optionally, you can run the tests only using one of those tool using the following commands
```bash
make test_asan  # Only run under ASan
make test_valgrind  # Only run under Valgrind
```

## Development Environment

This project was developed using a Ubuntu 22.04 LTS environment within a Docker container.\
The image used was [mkpro118/mk-sandbox:latest](https://hub.docker.com/repository/docker/mkpro118/mk-sandbox/tags),
the Dockerfile for that can be found at the [mkpro118/mk-sandbox](https://github.com/mkpro118/mk-sandbox) repository.\
While it should work on other Unix-like systems, it has been primarily tested in this environment.

Note: The development was done within a Docker container for convenience, but Docker is not required to build or run this project.

---

For any questions or further information about this project, please open an issue in the project repository.
