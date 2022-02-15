# SCISL (S C Interoperability Scripting Language)
A scripting language derived from ADScript  
The main feature of this language is the ability to use C/C++ functions and variables inside of it.  
Functions and variables on the host process can be called by ADscript once registered.  
  
## Documentation

The syntax is fairly straightforward.  
Every new line signifies a new function.  
Every space signifies a new argument for that function.  

All lines of the script can be abstracted as:  
`FunctionName arg0 arg1 ... argN`  

The language uses a hybrid compilation model.  
  
There are three types, Strings, Integers, and Floats.  
The types are determined at compile time.  
  
Comments can be made by starting a line with ';'  
  
### Interoperability  
  
To signify that a variable or function is an interoperable the character `$` preceeds the symbol.  
There are also macros which get resolved at compile time that are preceeded with `#`.
  
Macros **CANNOT** be used to replace more than a single argument/function name.  
Attempting to do so will result in a compilation error most of the time.  
  
### Using BREAK  
  
Break is a very powerful and very dangerous command.  
Break allows you to stop your program and then restart where you were later.  
This can be disabled by passing  `true` into `program::run`.  

There is nothing stopping you from changing the current instruction during this break.  
This is intentional, as it can be used for several applications.  
But, it is also incredibly dangerous.  

The optimizer will constant fold wherever it can.  
But, the optimizer doesn't expect you to be jumping around manually.  
A simple way to get around this is by using interoperables.  
The optimizer can't fold them, and anything set by them is also unfoldable.  
  
Essentially, most variables aren't safe if you intend on changing the currentInstruction during a break.  
The only time they are guarenteed to be safe is if you change the currentInstruction to 0 or what it was right after the break.  
