# Report: Building a search engine like Google 
**Grup:** Martí Gallego, Marc Bermudo, Marçal Bosch  

## Diagrama C4

A continuació es mostren els continguts del diagrama de components (C4):

- **Components volàtils (memòria RAM)**  
    - **CLI Interface**: gestiona la interacció per línia de comandes.  
    - **Document Loader**: parseja els fitxers i construeix les llistes de documents i enllaços.  
    - **Query Processor**: inicialitza i processa les consultes de l’usuari.  
    - **Reverse Index Manager**: construeix i manté en memòria l’índex invers (hashmap).  
    - **Graph Analyzer**: construeix en memòria el grafs de documents i calcula l’índex de rellevància.

- **Components persistents (disc)**
    - **Datasets**: ftixers de documents de la wikipedia (format .txt) llegits Document Loader.



--

## Diagrama conexions (C4)
```mermaid
---
config:
  layout: fixed
---
flowchart TD
 subgraph s1["Untitled subgraph"]
        n3["Untitled Node"]
        n4["Untitled Node"]
        n5["Untitled Node"]
  end
    A["Christmas"] -- Get money --> B("Go shopping")
    B --> C{"Let me think"}
    C -- One --> D["Laptop"]
    C -- Two --> E["iPhone"]
    C -- Three --> F["fa:fa-car Car"]
    D --> n1["cpu"]
    n1 --> n2["Untitled Node"]
    n2 --> E
    E --> D & n3
    n3 --> n4
    n4 --> n5 & n7["Untitled Node"]
    n10[" "] --> n11[" "]
    n6["Cylinder"]
    n8>"Odd"]
    n9["Sample Label"]
    n12["Cylinder"]
    n10@{ shape: anchor}
    n11@{ shape: anchor}
    n6@{ shape: cyl}
    n9@{ icon: "mc:default", pos: "b"}
    n12@{ shape: cyl}
