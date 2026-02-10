# POLHACK

![alt text](image.png)

![alt text](image-1.png)

![alt text](image-2.png)

# V2 : Added decompilation and explorer & save instance (doesnt work in solo mode !!)
# THE KEY TO OPEN OR CLOSE THE UI IS "DELETE"

#1 Open Source Polytoria Executor

Written by @ElCapor on github

Notice: I am open to any work opportunities, I am specialized in reverse engineering and protection systems.

Communicate with me and let's make your security better

# Tree

```
.
├── .github/
│   └── workflows/
│       └── build.yml
├── src/ ----> Source code of the dll script runner
│   ├── main.cpp
│   └── unity.hpp  ----> Unity Resolve wrapper with an additional feature specially for polytoria
├── UI/   ----------> The UI of the executor
│   ├── App.xaml
│   ├── App.xaml.cs
│   ├── DllInjector.cs
│   ├── MainWindow.xaml
│   ├── MainWindow.xaml.cs
│   ├── NamedPipes.cs
│   └── PolyHack.csproj
├── DumpGlobals.lua ---> A script to dump globals
├── PolyHack.sln ----> UI Solution file
├── Readme.md
└── xmake.lua
```

# Build

- Visual Studio 2022
- Download xmake from [https://xmake.Io](https://xmake.io)

### Download

#### Build from source

```sh
git clone https://github.com/ElCapor/polytoria-executor.git

cd polytoria-executor

xmake

Open PolyHack.sln in visual studio and press build

```

#### Download standalone dll
Download latest DLL from Github Actions

#### Download Pre-Built
[Polytoria.Executor.by.ElCapor.7z](https://github.com/ElCapor/polytoria-executor/blob/master/Release/Polytoria%20Executor%20by%20ElCapor.7z)



# Community
https://discord.gg/vscvprztP3
