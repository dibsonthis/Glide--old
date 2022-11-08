# Glide
Documentation for the Glide programming language.

Link to video demo: [Glide reddit post](https://www.reddit.com/r/ProgrammingLanguages/comments/ybjtax/glide_data_transformation_language_documentation/)

## Purpose

The purpose of Glide is simple. I wanted a way to easily read and write data transformation workflows. With this in mind, the language relies heavily on the arrow (injection) operator '->' to feed data long the chain. This documentation will feature examples of how it's used.

The other reason this language exists, is learning. I wanted to learn how languages are made and uncover the black magic of parsers/iterpreters/compilers. Glide is my 15th attempt at a new language, the ones before have been abandoned due to the codebase being an absolute mess, or the language itself offering nothing new and exciting (at least for me). I decided to stick it out with Glide, and hoping to end the cycle of project hopping by continuing to refactor and upgrade instead of deleting and starting from scratch.

# How to compile and start having fun with Glide

Clone and open source code in VSCode. The first thing you want to look at is ```main.cpp```. There, you should see ```Type type = Type::DEV;```. There are three different types here: _DEV_ | _INTERP_ | _REPL_, we'll go through each one below.

### DEV

If DEV is selected, we're in development mode, running the code now will build and debug the compiler. 
The output will be stored in "bin/build/dev". 
To run, make sure to select: ```C/C++: g++.exe build and debug active file``` (Windows) or ```C/C++: clang++ build and debug active file``` (Mac)

DEV will read directly from ```dev_env/source.gl```, so make sure to put your Glide code in there. You can create other Glide files in that folder and import them into ```source.gl``` too.

### INTERP

If INTERP is selected, you're now building an optimised version of the compiler. 
The output will be stored in "bin/build/interp". 
To run, make sure to select: ```C/C++: g++.exe build interpreter``` (Windows) or ```C/C++: clang++ build interpreter``` (Mac)

After compiling the interpreter, you should copy and paste the executable inside ```test_env```. This directory is where you can test your newly created interpreter. You can run the interpreter using ```./glide "main.gl"``` (or any other .gl file in that directory).

### REPL

If REPL is selected, you're now building an optimised version of the REPL (Read Execute Print Loop). 
The output will be stored in "bin/build/repl". 
To run, make sure to select: ```C/C++: g++.exe build repl``` (Windows) or ```C/C++: clang++ build repl``` (Mac)

After compiling the interpreter, you should copy and paste the executable inside ```test_env```. This directory is where you can test your newly created REPL. You can run the REPL using ```./glide-repl```. The REPL is probably the fastest way to prototype Glide code.

## Glide outside of the repo

After you've compiled the interpreter/REPL, feel free to move the executables to any other location. I personally have my own directory that I use as an isolated Glide test environment.

# Current status of Glide

Current status: Work in progress

You _will_ encounter many bugs, maybe some segfaults here and there, and some undefined behaviour.
Errors are currently quite terrible, and it's a known issue that error line/col numbers can be hilariously incorrect. This is being worked on.
Do _NOT_ use Glide in any production environment, for real. It's not at that stage yet.

# Syntax

## Data types

Glide only has a few simple data types: bool, int, float, string, list, object and empty

The language is dynamically typed and therefore there is no need to declare the variable type. (Note: a type system is in the works, and once fully implemented, this documentation will be updated accordingly. I'm still unsure whether I want typing to be a requirement by the compiler, or a choice given to the programmer. Stay tuned, I guess).

Example - Variable declaration:

```
x = 12
y = 3.45
str = "hi there, universe"
ls = [ 1 2 3 "four" 5.0 ]
obj = @{
  name: "John"
  age: 45
}
```

## Arrow (Injection) Operator

As mentioned in the intro, the arrow (->) operator is by far the most important aspect of Glide. It allows the programmer to do a few useful things:
  - Inject a value into an operator to create a partial operator
  - Inject a value into a function to create a partial function (aka curried functions)
  - Feed data down a transformation chain

Example - Simple chaining using the arrow operator

```
x = 4 -> *5 -> +2 -> +(5/2)   // result: 24.5
```

## Partial Operators

Glide gives you the ability to create partial operators. These are operators that are incomplete and only evaluated once the required operands are injected.

Example - Partial Operator

```
mul3 = (*3)
4 -> mul3   // result: 12
3 -> mul3   // result: 9
```

You can even create a fully partial operator (in the case of a binary operator, this means it has no operands at all).

Example - Fully partial operator

```
add = (+)
add5 = 5 -> add
5 -> add5   // result: 10
```

In the example above, we created a variable with a fully partial operator (+), with no operands. We then injected a value of 5 into it, which took the place of its left hand operand, and saved it as another variable. Finally, we injected 5 into the new variable, which took the place of its next empty operand (the right hand operand). Because the operator now has both required operands, it is then evaluated to produce the result of 10.

## Functions

As with any other programming language, Glide gives the ability to create and call functions.

(Note: The previous version of this language had two different ways to create functions, through function definitions and through lambdas. The current implementation only allows lambdas, however this might also change as well.)

Example - Simple Add Function

```
add = [a b] => {
  ret a + b
}
```

All functions are injected with their surrounding scope upon creation, which means closures are also possible.

Example - Closures:

```
outer = [] => {
  x = 12
  inner = [] => {
    ret x
  }
  ret inner
}

f = outer[]
f[]   // result: 12
```

Functions are also injected with a pointer to themselves, which makes recursion possible.

Example - Recursion:

```
fib = [n] => {

   if [n == 0] => {
     ret 0
  }
  if [n <= 2] => {
     ret 1
  }

  ret fib[n-1] + fib[n-2]
  
}
```

### Calling functions

There are multiple ways functions can be called in Glide. The first one is the simple bracket notation:

```
x = 12
y = 8
add[x y]    // result: 20
```

The other way is through injection:

```
add4 = [x] => {
  ret x + 4
}

10 -> add4 -> *2   // result: 28
```

Injecting a value into a function adds that value as an argument. If all the required arguments are present or injected, then the function is called and the value is returned. In the example above, the function 'add4' required only one argument, and once that argument is injected, the function is invoked.

### Partial (Curried) Functions

Let's take another look at the add function we defined previously. It requires two arguments, but what happens if we inject only one?

```
add4 = 4 -> add
5 -> add4   // result: 9
```

As you can see, if the function still requires more arguments after injection, it isn't evaluated. Instead, it returns a curried/partial function that contains the injected value as preloaded argument. This allows us to create curried/partial functions relatively easily.

## Range Operator

An important operator, especially for looping, is the range (..) operator:

```
list_1_10 = 1..10
```

It's an easy way to create lists to iterate through.

## Loops

Glide contains both for and while loops. 

### For loops:

Example - Simple for loop:

```
for [1..10] => {
    print["hi\n"]
}
```

For loops have an interesting implementation detail which seemed like a good idea at the time, however it could cause bugs if the user is unaware of it. If, as with the example above, a for loop was only given a list to iterate through, the body of that loop automatically gets access to two variables: '\_i' and '\_it'. \_i is the current index, and \_it is the current element. This provides a lot of convenience in simple loops, however it introduces complexity in nested loops. It's advisable to only omit the index when the loop is simple and flat. So, how do we get around this issue in less simple loops?

Example - For loop with index:

```
for [1..10 index] => {
  print[index]
}
```

We can directly define a variable for the index. This allows us to access the current index by name, and not by a scoped variable. This means that in a nested loop, we can access any outer index without confusion or error.

We can go one step further and define the item variable.

Example - For loop with index and item

```
for [1..10 index item] => {
  print["Index: " index "\n"]
  print["Item: " item "\n"]
}
```

It is highly recommended that you declare at least the index when creating loops, even if they're basic loops, in the case they need to be refactored to be nested in the future. If you're certain your loop will always remain flat, then feel free to omit the index or item variables.

### While loops

```
while [x <= 10] => {
  x = x + 1
}
```

While loops take a single conditional and loops until that conditional stops being met.

## If Statements and If Blocks

In Glide, you can write one off if statements, or if blocks which act as an if/else if(s)/else statement.

Example - If Statement:

```
if [x != 3] => {
  x = 3
}
```

Example - If block:

```
if => {
  x < 3: {
    x = 0
  }
  x > 3: {
    x = 10
  }
  default: {
    x = 100
  }
}
```

## Lists

Lists can contain any type of element. They can be constructed using the range (..) operator, as seen in previous examples, or by explicitly building them. Notice that in Glide, list items are not comma separated. 

List items can be accessed by index using the dot-bracket notation.

Example - Lists:

```
x = [ 1 2 3 "hello" "goodbye" ]
x.[3]   // result: "hello"
x.[0] = 100   // result: [ 100 2 3 "hello" "goodbye" ]
```

We can also add lists together:

```
x = [1 2 3] + [4 5 6]   // result: [1 2 3 4 5 6]
```

### List built-in properties:

length: ``` x.length ```: retrieves the length of the list

## Objects

Objects are containers consisting of key/value pairs.

Example - Objects:

```
obj = @{
  first: "James"
  last: "Smith"
  get_name: [] => {
    ret first + " " + last
  }
}
  
obj.last          // returns: "Smith"
obj.get_name[]    // returns "James Smith"
```

An interesting feature of Glide objects is that properties can access any previously defined property in the object (as per the get_name function above).

Objects can also be added together, which merges the right object's properties with the left's. If the left and right object share the same property, that property is replaced with the right's.

Example - Adding objects:

```
x = 
  @{ name: "Irene" score: 12 } 
  + 
  @{ name: "Irene" score: 29 grade: "B" }
  
// returns @{ name: "Irene" score: 29 grade: "B" }
```

## Chaining

As touched upon previously, Glide's entire reason for existing is to be able to easily read and write data transformation. In the context of Glide, this is called chaining. 

Example - Chaining:

```
x =
(10, 20) -> add
-> +2
-> add12
-> mul4
-> some_function
-> *12
-> (y =)
-> [x] => { ret x * (3+2) }
```

Some interesting points on the above:

- You can inject directly into a lambda, which makes chaining a really powerful way to transform data without too much pre-meditation.
- You can assign variables directly inside a chain (y =), which can act as a way of "saving state" in the middle of a transformation.
- You can inject multiple values into a function/operator using a comma-separated list, as shown in the first line of the transformation.

## Standard Lib

As it stands currently, the standard lib has some useful functionality primed at data transformation.

### Map

_map_ creates a copy of the list provided and applies the given function/operator to each element:

```
1..100 -> std.map[*2]
1..100 -> std.map[([x] => { ret x * 2 })]
```

### Filter

_filter_ filters the provided list based on the given condition: 

```
1..100 -> std.filter[>10] -> std.filter[<20]
1..100 -> std.filter[([x] => { ret x < 20 && x > 10 })]
```

### Reduce

_reduce_ reduces the provided list based on the given function/operation:

```
1..100 -> std.reduce[+]
1..100 -> std.reduce[([a b] => { ret a + b })]
```

### Zip

_zip_ takes two lists and applies the given function to each element of the same index:

```
(1..100, 1..100) -> std.zip[+]
(1..100, 1..100) -> std.zip[([a b] => { ret a + b })]
```

## Implementation

Glide is written in C++. The compiler produces bytecode (kind of, it's not really a byte) instructions that are evaluated using a virtual machine also built in C++. As it currently stands, the performance is not exactly optimal and work is being done to improve it on this front.

## Closing note

Glide is very much a work in progress, with many _many_ bugs that are being identified and fixed daily. This is not intended to be a language used in any production environment (yet), and can be classed as a toy language until all issues are ironed out.
