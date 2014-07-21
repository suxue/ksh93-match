# Introduction

`match` is a builtin for at&t ksh93 for matching strings to regexp.

## Setup

    make
    make test
    make install

To use `match` as a builtin command, load it from its dynamic library:

    builtin ~/.local/lib/ksh/libmatch.so match

Or, wrap inside a function, which will then load the builtin before first
call.

    function match {
        builtin -f \${.sh.fun} \${.sh.fun}
        command \${.sh.fun} "\$@"
    }


## Usage

    match [ options ] regexp subject [capture]

Here `[capture]` is the name of variable you want the captured results
to be put in.

For basic usage and examples, see the directory `tests/` and `test.sh`.

To get the explanation of all options, invoke `match` without arguments.

As ksh93 has dynamic scope, which is weird to most programmers. `match`
follows a simple rule to find which variable the captured groups will be
put in.

1. if variable named `[capture]` exists in local scope, (over)write it.
2. If the builtin is wrapped in a function exactly named `match`, check
   this function's scope as well.
3. If both 1 and 2 fails, create a variable named `[capture]` in global
   scope.

## License

Copyright (C) 2014 SuXue

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
