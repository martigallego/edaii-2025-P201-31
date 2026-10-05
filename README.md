# EDA II — Search Engine in C

A search-engine project developed for the Data Structures and Algorithms II (EDA II) coursework at Universitat Pompeu Fabra (UPF).

The project implements a document search system in C, focusing on data structures, hashing, graph representation, reverse indexing, testing and performance analysis.

## Project overview

The program processes a collection of documents and supports keyword-based searches over their content.

The implementation includes:

- Document parsing and serialization
- Query parsing
- Dynamic data structures
- A document graph based on links between documents
- Hash tables for efficient lookup
- A reverse index mapping keywords to document IDs
- Keyword intersection for multi-term queries
- Unit testing
- Runtime and memory analysis
- Performance comparison between indexed and non-indexed search

## Technical highlights

### Reverse index

A hash-based reverse index maps each keyword to the documents containing it. This avoids scanning every document for each query and improves keyword lookup for larger datasets.

### Document graph

Documents are represented as nodes connected by links. The implementation keeps track of graph relationships and document degrees.

### Performance analysis

The project includes experiments comparing search with and without a reverse index, different hash-table sizes, initialization time and search time.

The detailed analysis is available in REPORT.md.

## Technologies

- C
- Data structures and algorithms
- Hash tables
- Graphs
- Dynamic memory management
- File I/O
- Unit testing
- Make
- GCC and GDB
- Valgrind
- Clang-format
- GitHub Actions

## Repository structure

- .github/workflows/ — continuous integration
- datasets/ — document datasets
- img/ — analysis figures
- src/ — application source code
- test/ — unit tests
- Makefile — build and development commands
- REPORT.md — design, complexity and performance analysis

## Build and run

Run the application:

    make r

Run unit tests:

    make t

Check formatting:

    make f

Check memory usage:

    make v

Debug with GDB:

    make d

Debug the test suite:

    make dt

## Continuous integration

GitHub Actions automatically runs the unit-test workflow when changes are pushed to the repository.

## Complexity and design analysis

The accompanying report documents the complexity of document parsing, graph operations, reverse-index lookup and multi-keyword search. It also discusses possible improvements, including a trie-based reverse index.

## Academic context

Course: Data Structures and Algorithms II (EDA II)
Group: P201-31
Institution: Universitat Pompeu Fabra
