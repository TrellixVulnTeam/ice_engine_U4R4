Ice Engine
--------

[![Build Status](https://travis-ci.org/icebreakersentertainment/ice_engine.svg?branch=master)](https://travis-ci.org/icebreakersentertainment/ice_engine)
[![Build status](https://ci.appveyor.com/api/projects/status/2tqewfhy60mxv429/branch/master?svg=true)](https://ci.appveyor.com/project/IcebreakersEntertainment/ice-engine/branch/master)

**Note**: This project is incredibly young, and isn't in any kind of production ready state. 

Documentation is available at [http://game-engine-ice_engine.readthedocs.io/en/latest/](http://game-engine-ice_engine.readthedocs.io/en/latest/).

To clone:

    git clone https://github.com/icebreakersentertainment/ice_engine.git

Get/build prerequisites:

    python setup.py

To build on Linux:

    mkdir build
    cd build
    cmake ..
    make

To test on Linux:

    mkdir build
    cd build
    cmake -D ICEENGINE_BUILD_AS_LIBRARY=1 -D ICEENGINE_GRAPHICS_BACKEND=noop -D ICEENGINE_BUILD_TESTS=1 ..
    make
    ctest

To build on Windows:

    mkdir build
    cd build
    
    # Workaround for `Release` directory being created as a file in Windows
    mkdir build/Release
    
    cmake -G "Visual Studio 14" ..
    msbuild /p:Configuration=Release ice_engine.sln

To test on Windows:

    mkdir build
    
    # Workaround for `Release` directory being created as a file in Windows
    mkdir build/Release
    mkdir build/tests
    mkdir build/tests/Release
    
    cmake -G "Visual Studio 14" -D BUILD_AS_LIBRARY=1 -D ICEENGINE_GRAPHICS_BACKEND=noop -D ICEENGINE_BUILD_TESTS=1 ..
    msbuild /p:Configuration=Release ice_engine.sln
    ctest
