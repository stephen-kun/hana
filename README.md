### The `datasets` branch of [Hana][]

This branch contains the benchmarks associated to the `master` branch of Hana.
These benchmarks are updated automatically by continuous integration scripts.

The benchmarks are JSON files organized in directories with the following
layout:

```
debug/
    compiler1/
        benchmark1.json
        ...
        benchmarkN.json
    ...
    compilerN/
        benchmark1.json
        ...
        benchmarkN.json
release/
    ...
```

The `debug` and `release` directories represent the type of the CMake build
under which the benchmarks were generated. The `compilerN` subdirectories
correspond to the compiler used to build the benchmarks.

[Hana]: https://github.com/boostorg/hana
