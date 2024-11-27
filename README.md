# Repository naming
To get information of how this user is naming the repositories go [here](https://github.com/DysonParra#repository-naming)

# Information of the project.
  Cli lib that receive an undetermined number of arguments, analyze if are correctly formed and if yes parse it into structs or else print the specific error in console.    
The flags are from two types (you can use the two at same time):  
  * With value: an alphanumeric string started with '-' and the next argument must be the value of the flag.  
  Example:  -downloadPath documents   -sourceFile myFile.xml   -rootDir C:/project  
  * Withouth value: an alphanumerirc string started with '--'.  
  Example:  --useDefault  --notUseIncognito  --generateLogFile  --preserveTempFiles  

    For use the library you need to specify in source code a sets of flags:
    * Required: The flags that if are not included in the arguments the lib return an error.  
    * Optional: The flags that can or not be included in the arguments withouth problem.  
    * Default: The flags that are used if the cli arguments not specify any flag.  

The required and the optional flags are a matrix (can be different size), that in each row defined a set of flags that are just like a XOR, only one of these flags must be included, and the default flags are a vector, example:

    required = [ "-name"  "--notUseName"           ]    optional = [ "-loadPageTimeOut"                ]
               [ "-chromeDriverPath"               ]               [ "-outputPath" "-notGenerateFiles" ]
               [ "-account"  "--notLogin" "-token" ]                                                     
    
    default = [ "-name"  "dev"  "-chromeDriverPath"  "/opt/driver"  "--notLogin"  "-outputPath"  "/out"]

In the example:  
  - One and only one of the flags "-name" and "--notUseName" must be included.  
  - The flag "-chromeDriverPath" must be included.  
  - One and only one of the flags "-account", "--notLogin" and "-token" must be included.  
  - The flag "-loadPageTimeOut" can be or not included.  
  - Can include one of the flags "-outputPath" or "-notGenerateFiles", but not the two.
  - If the cli argument list is void the default flags are used, so the default flags must be defined in such way that all required flags are specified.

The lib also validate that a flag is not defined as required and as optional at same time.  
For use the library use as example the code in "tester.c" (the main source file), the escense of the code is call the method "convertArgsToFlags" and if not return null the flags are OK, else in the console you can see exactly which is the error.

# Project struct
- header  
  Contains all header files (.h) used in some source file (.c).  
- icon  
  Contains the icon file (.ico) that the source file will have, the name of the file must be iqual to the main source file except the extension.
- object  
  Temp folder that is created while the source files are compiled.
- project root dir  
  The root of the project contains all source files (.c), dynamic libs (.dll) and binary file (.exe)

# Binary and library
The operation is equal for all c repositories of this user.  
Use the command: "make all" and all files are created  

There's two type of libraries:
- Static:  
  Is a static lib file (.a) that include a header file (.h) and that will be merged with the main source file in only one binary file (.exe).
- Dynamic:  
  There's two types
    * Local:  
      Is a shared lib file (.dll) that include a header file (.h) and a source file (.c) different to the main source file.
    * External:  
      Is a shared lib file (.dll) that include a header file (.h), is external because the current project has not the source file (.c), so the libray was created in another project and used in current.

# Makefile
There's some important variables:
  - appName  
    The name of the source file with main method withouth extension.
  - prefferDynamic  
    If there are a dynamic lib (.dll) and a static lib (.a) for the same source file (.c), when create the binary file it need to decide which use to generate the binary, so if the value is 0 use the static, and else use the dynamic.

Generally the only one task used is 'all', but these task call another tasks, and the more importants are:
  - make clean  
    Erase the content of the folder object, the binary file (.exe), static libs (.a) and local dynamic libs (.dll).
  - make libName$(dLibMakeFormat)  
    Generate a dynamic lib (local) using the file with name libName.c
  - make libName$(sLibMakeFormat)  
     Generate a static lib using the file with name libName.c
  - make app  
    Generate the binary file (.exe), but the source file needs to include all headers that will use in the source code, for example:  
    #include "flag.h"  
    #include "flag_processor.h"  
    No need to specify that are in the folder header, the task read the file, identify all user included libraries in the source file (in this example two) and generate the command to link the libraries with the binary file considering the value of "prefferDynamic", so if you include more libraries no need to modify the MakeFile  
  - make appR  
    It executes make app and then run the app.
  - make appN
    Generate or try to generate the binary file (.exe) withouth identify the libraries to link.
  - make appNR
    It executes make appN and then run the app.
