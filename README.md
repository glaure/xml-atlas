# xml-atlas
XML viewer editor project


# Discussion

## Available GUI toolkits

Currently there is no native GUI toolkit avalable for Rust.
What is available are bindings to toolkits written in another language. This 
is a compromise we have to live with.

These are GUI toolkits that are available and we discussed using it:


* fltk https://www.fltk.org/
  * Works, small and relative easy to use. But does not look modern.
* Qt
  * Few bindings available
  * Personally having years of experience (C++ though)
* egui https://github.com/emilk/egui
  * no tree view


# State of Qt in combination with Rust

## Comparison to other Rust Qt bindings
(from https://github.com/KDAB/cxx-qt)

| Project | Integrate into C++ codebase  | Safe Rust | QML | QWidgets | Maintained<sup>1</sup> | Binding mechanism |
|-------- | ---------------------------- | --------- | --- | -------- | ---------------------- | ----------------- |
| CXX-Qt  |  ✔                           | ✔         | ✔ | limited<sup>2</sup> | ✔       | [cxx](https://cxx.rs) plus additional code generation to implement QObject subclasses in Rust and bind them to C++ |
| [qmetaobject](https://github.com/woboq/qmetaobject-rs/) | ✗ | ✔ | ✔ | ✗ | ✔ | [cpp](https://github.com/mystor/rust-cpp) macro to write C++ inline in Rust, plus Rust macros to create QObject subclasses from Rust structs |
| [Rust Qt Binding Generator](https://invent.kde.org/sdk/rust-qt-binding-generator) | ✔ | ✔ | ✔ | limited<sup>2</sup> | ✗ | generates Rust traits and C++ bindings from JSON description of QObject subclass |
| [rust-qt](https://rust-qt.github.io/) | ✗ | ✗ | ✔ | ✔ | ✗ | [ritual](https://rust-qt.github.io/ritual/) to generate unsafe Rust bindings from C++ headers |
| [qml-rust](https://github.com/White-Oak/qml-rust) | ✗ | ✔ | ✔ | ✗ | ✗ | [DOtherSide](https://github.com/filcuc/DOtherSide) C wrapper for QML C++ classes |
| [qmlrs](https://github.com/flanfly/qmlrs) | ✗ | ✔ | ✔ | ✗ | ✗ | own C++ library to bind QQmlApplicationEngine |
| [qmlrsng](https://github.com/nbigaouette/qmlrsng) | ✗ | ✔ | ✔ | ✗ | ✗ | [libqmlbind](https://github.com/seanchas116/libqmlbind) with [bindgen](https://rust-lang.github.io/rust-bindgen/) |
| [rust-qml](https://github.com/florianjacob/rust-qml) | ✗ | ✔ | ✔ | ✗ | ✗ | [libqmlbind](https://github.com/seanchas116/libqmlbind) |

<sup>1</sup>: maintained: supports Qt6 and repository has had nontrivial commits within last year as of August 2022

<sup>2</sup>: CXX-Qt and Rust Qt Binding Generator can be used to implement custom QObjects subclasses in Rust. C++
bindings for these QObject subclasses can be used in QWidgets applications, but these projects do not provide Rust
bindings for QWidgets APIs.


# Rust toolchain
Rust has to be installed:

https://www.rust-lang.org/tools/install


# Build, Test and Run


cargo build

cargo test