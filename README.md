Examples for COSEC students
===========================

It is assumed you have made yourself familiar with `ntc-cmake`, what it automates and how, please start there (see dependencies below for the link).

- Serialization:
  - `schema-serial`: usage of Flatbuffers as a serialization library with external schema specification and code generator.
  - `interprocess-copy`: benchmark of various interprocess data copy implementations.
- Asynchrony:
  - `thread-pool`: simple thread pool and executor usage.
  - `asio-basic`: basic asynchronous tcp server with logging and error handling.
  - `asio-advanced`: multi-threaded tcp server with continuations that has operation limits.
  - `asio-shared`: server with shared state which uses strands for synchronization.
  - `generator`: synchronous generator based on C++20 coroutines.
- Qt:
  - `layouts-painting`: basic use signals/slots, Qt Designer, layouts and mouse/paint events for custom-looking widgets.
  - `item-models`: usage of Qt Item Models and Views/Widgets.
  - `block-scene`: usage of `QGraphicsScene`, items and views.
  - `gui-progress`: offloading lengthy tasks to a non-gui QThread and communicating with it using queued signals/slots.
  - `locale-resources`: usage of resources and translations in Qt.
  - `geohash`: qtpositioning + simple use of `QNetworkAccessManager` from qtnetwork.
  - `delegates-sql`: use of queries and item models with qtsql and custom item delegates.
  - `treeview`: tree view item model example.
- Miscellaneous:
  - `et`: use of expression templates to optimize evaluation of expressions.

Dependencies:
- C++ compiler and standard library with sufficient support for C++20.
- \>=ntc-cmake-1.1.0 - see https://github.com/palebedev/ntc-cmake
- \>=Boost-1.74.0
- \>=Qt-5.15 (LinguistTools,Network,Positioning,Sql,Svg,WebEngineWidgets modules and their dependencies)
- \>=google-benchmark-1.5.0
- \>=FlatBuffers-1.12.0
