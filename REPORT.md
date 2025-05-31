# Report: Building a search engine like Google 
**Grup:** Martí Gallego, Marc Bermudo, Marçal Bosch  

## C4 Component Diagram

> Volàtil (Memòria): Fa servir el color blau<br>
> Persistent (arxiu en el disc): Fa servir el color verd

```mermaid
graph TD
    %%──────────────────────────────────────
    %% 1) In-Memory (Volatile) subgraph (light blue)
    %%──────────────────────────────────────
    subgraph InMemory["In Memory (Volatile)"]
        direction TB
        CLI["CLI Interface main.c"]
        Query["Query Processor query.c"]
        DocumentList["Document Linked List document.h / document.c"]
        HashMap["Reverse Index hashmap.h / hashmap.c"]
        Graph["Document Graph graph.h / graph.c"]
        RecentQueries["Recent Queries Queue static array in query.c"]
    end

    %%──────────────────────────────────────
    %% 2) On-Disk (Persistent) subgraph (light green)
    %%──────────────────────────────────────
    subgraph Persistent["On Disk (Persistent)"]
        direction TB
        DocumentFiles["Document Files .txt in datasets"]
        IndexCache["Serialized Index Not implemented"]
        ScoresCache["Relevance Scores Not implemented"]
    end

    %%──────────────────────────────────────
    %% 3) Data-flow arrows (labels simplified to avoid parse errors)
    %%──────────────────────────────────────
    DocumentFiles -->|"loadDocs"| DocumentList
    DocumentList -->|"buildIndex"| HashMap
    DocumentList -->|"buildGraph"| Graph

    CLI -->|"querySearch"| Query
    Query -->|"startQuery"| RecentQueries
    Query -->|"searchWithIndex"| HashMap
    HashMap -->|"findKeyword"| DocumentList
    DocumentList -->|"scoreRelevance"| Graph
    Graph -->|"calcScores"| ScoresCache
    HashMap -->|"serializeIndex"| IndexCache
    RecentQueries -->|"showRecent"| CLI

    %%──────────────────────────────────────
    %% 4) Class definitions + assignments
    %%──────────────────────────────────────
    classDef volatile fill:#ADD8E6,stroke:#333;
    classDef persistent fill:#90EE90,stroke:#333;

    class CLI,Query,DocumentList,HashMap,Graph,RecentQueries volatile;
    class DocumentFiles,IndexCache,ScoresCache persistent;


```


## Runtime Complexity Analysis (taula)
| Description                                                                 | Big-O           | Justification                                                                                                                                                                                                                                                                                              |
|-----------------------------------------------------------------------------|-----------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Parsing a document into the struct (including adding links)                 | O(N + L²)       | **N**: Total characters in document (file read). **L**: Number of links. Each link insertion traverses existing list (O(L) per insertion → Σ(1 to L) = O(L²). Body processing O(N).                                                                                                                       |
| Parsing a query into the struct                                             | O(m)            | **m**: Query string length. Single pass for tokenization (strtok) O(m) + O(1) per word insertion (maintains tail pointer). K words processed but tokenization dominates.                                                                                                                                |
| Counting neighbours in the graph (total edges)                              | O(1)            | Direct access to precomputed `graf->totalEnllaços` field. No traversal needed.                                                                                                                                                                                                                              |
| Counting neighbours of a specific document                                  | O(1) avg        | Hash lookup (O(1) avg) + direct access to `grauEntrant`/`grauSortint` in NodeGrau. No adjacency list traversal.                                                                                                                                                                                          |
| Finding docs containing a keyword (reverse-index)                           | O(W + C)        | **W**: Keyword length (normalization+hashing). **C**: Collision chain length. Hashing O(W), bucket access O(1) avg, chain scan O(C). Returns pointer without traversing document list.                                                                                                                     |
| Finding docs matching all keywords in query                                 | O(K·D²)         | **K**: Keywords. **D**: Max document list size. First keyword: O(d₁). Subsequent keywords: Nested loop intersection - for each of R docs in current result, scan S docs in new list (O(R·S)). Worst-case R=S=D → O(D²) per keyword → O(K·D²). First term O(D) dominated by O(D²). |
>Variables:
>>D = número total de caracteres o tokens en un documento<br>
>>L = número de enlaces en ese documento<br>
>>K = número de palabras (keywords) en la consulta<br>
>>V = número de documentos (nodos del grafo)<br>
>>E = número total de enlaces (aristas) en el grafo<br>
>>m, m_i = tamaño de la lista de documentos para cada palabra<br>
>> M = número de documentos que coinciden con la consulta (≤ 5 en la salida)<br>




## Search Time Analysis: With/Without Reverse Index (gràfica)
Per avaluar l'eficiència del motor de cerca, s'ha implementat una funció (`comparaMetodesBusqueda`) que mesura el temps d'execució (en mil·lisegons) de dues estratègies:

- **Cerca sense índex invertit**: es fa una cerca lineal per tots els documents, comprovant si cada paraula clau apareix en el títol o el cos del document.
- **Cerca amb índex invertit**: es consulta un `hashmap` que relaciona cada paraula amb els documents que la contenen, millorant notablement el temps de resposta.


## Initialization Time vs Hashmap Slot Count (gràfica)


## Search Time vs Hashmap Slot Count (gràfica)

## Reverse Index Improvement Proposal