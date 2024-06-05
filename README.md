# C template project

## Features
- **CMake**

    Like it or not, it's the [most widely used tool for building C](https://www.jetbrains.com/lp/devecosystem-2021/c).
    Without reinventing the wheel, I followed the project structure recommended by this [book](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html).

- **cmocka**

> cmocka is ...
>
> an elegant unit testing framework for C with support for mock objects.
> It only requires the standard C library, works on a range of computing
> platforms (including embedded) and with different compilers.

    Easy to install, just run `<pkg manager> install libmocka-(dev|devel)` and easy to use, see [official page](https://cmocka.org/).
    Examples [here](https://api.cmocka.org/).

- **clang-tidy**

    Static-analysis tool that catches a good amount of mistakes at copmile-time.
    Unfortunately, it doesn't come preinstalled on Mac, but you can [work it around](#requirements).

- **valgrind, strace, ltrace**

    These tools aren't available on Mac, but you can use them inside the Docker container.
    You may use that Docker image to play around with `gcc` and `gdb` too.

## Requirements

Install these programs on your machine:

- CMake
- Docker
- clang-tidy
- Cmocka

PS: To install `clang-tidy`, install `llvm` with Homebrew first, then add this alias to the `.bashrc`:

```sh
alias clang-tidy='/opt/homebrew/opt/llvm/bin/clang-tidy'
```

## How to use it

Everything should work out of the box: building, running, tests.

```bash
$ docker compose run controloop
/app $ ./build.sh -d
/app $ cd build && sudo ctest -V
```

## FAQ

## Others

- A very good list of compiler warnings you may want to add:
http://fastcompression.blogspot.com/2019/01/compiler-warnings.html

- Here's a tiny program that you can use to try out the Address Sanitizer.
  Copy it into `apps/main.c`, select the `Asan` [variant](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/variants.md), build, and run it.

```c
#include <stdlib.h>

int main()
{
    int *x = malloc(10 * sizeof(int));
    free(x);
    return x[5];
}
```

## TODOs
