# heureka
... is a heuristic backtracking solver for abstract argumentation

## 1 Installation

Run build, make sure that eigen3 is installed.

## 2 Usage

Run e.g. 'heureka -p EE-ST -f example.tgf' or 'heureka -p DC-ST -a a12 -f example.tgf'

### 2.1 Mandatory Parameters
* **-p** the problem in the form (EE|SE|DS|DC)-(CO|PR|ST|GR), where
  * **EE** enumerate all extensions
  * **SE** find single extension
  * **DS** decide sceptically
  * **DC** decide credulously
  * **CO** complete
  * **PR** preferred
  * **ST** stable
  * **GR** grounded
* **-f** source file either in .tgf or .apx format
* **-a** the argument which shall be justified (only in case of DS and DC)

### 2.2 Optional Parameters
* **-H** the used heuristic
* **-d** debug information, either short or verbose, lets the solver print debug information
* **-fo** the file format, either apx or tgf (is otherwise infered from the file ending)

### 2.3 Custom Heuristics
If the paramter -H is used, a custom heuristic is constructed from it, e.g.
  heureka -EE-CO -f ex.apx -H "/ + outdeg 1 + indeg 1"
uses a heuristic h(x) = (outdeg(x)+1)/(indeg(x)+1).
Heuristics can be constructed out of the following components:
* **+ a b**, **- a b**, __* a b__, **/ a b** and **^ a b** are mathematical operations in Polish notation
* **deg in out**, **indeg** and **outdeg** are indegree and outdegree respectively
* **dynindeg** is the number of attackers not defeated by the current partial extension
* **dynoutdeg** is the number of attacks on attackers of the current partial solution
* **path indepth inalpha outdepth outalpha** is based on paths, **inpath** and **outpath** have preset parameters
* **scc** sorts arguments according to its strongly connected component, a component is sorted after all compnents it depends on

