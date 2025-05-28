# Report: Building a search engine like Google 
**Grup:** Martí Gallego, Marc Bermudo, Marçal Bosch  

## C4 Component Diagram

> Volàtil (Memòria): Fa servir el color blau<br>
> Persistent (arxiu en el disc): Fa servir el color verd

```mermaid
flowchart TD
    %% Volatile (Memory) - blue
    classDef volatile fill:#cce6ff,stroke:#3399ff,color:#003366;
    %% Persistent (Disk) - green
    classDef persistent fill:#d6f5d6,stroke:#33cc33,color:#145214;

    subgraph Volatile_Memory["Volatile (Memory)"]
        DocumentsList["Documents List"]
        ReverseIndex["Reverse Index (Hashmap)"]
        QueryList["Query Linked List"]
        DocumentGraph["Document Graph"]
        RecentQueries["Queue (Last 3 Queries)"]
    end

    subgraph Persistent_Storage["Persistent (Disk)"]
        DatasetFiles["Dataset Files"]
        ReverseIndexFile["Reverse Index File"]
        RelevanceCache["Relevance Score Cache"]
    end

    %% Relations
    DatasetFiles --> DocumentsList
    DocumentsList --> ReverseIndex
    DocumentsList --> DocumentGraph
    QueryList --> ReverseIndex
    ReverseIndex --> DocumentsList
    DocumentGraph --> DocumentsList
    ReverseIndexFile --> ReverseIndex
    RelevanceCache --> DocumentGraph

    %% Apply styles
    class DocumentsList,ReverseIndex,QueryList,DocumentGraph,RecentQueries volatile;
    class DatasetFiles,ReverseIndexFile,RelevanceCache persistent;
```


## Runtime Complexity Analysis (taula)


## Search Time Analysis: With/Without Reverse Index (gràfica)


## Initialization Time vs Hashmap Slot Count (gràfica)


## Search Time vs Hashmap Slot Count (gràfica)

## Reverse Index Improvement Proposal