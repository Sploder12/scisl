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
