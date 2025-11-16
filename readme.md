# File System Hash Table Project

This project implements a simplified **File System** using a **hash table with open addressing**. It was created as a class assignment to demonstrate hash table design, probing strategies, dynamic resizing, and incremental rehashing.

---

## The FileSys class supports:

- Inserting files  
- Searching for files  
- Removing files  
- Updating disk blocks  
- Automatic rehashing when the table gets too full  
- Automatic rehashing when the deleted count becomes too large  
- Incremental rehashing (moving 25% of the old table at a time)

Project components:

- **driver.cpp** — Demonstration program  
- **mytest.cpp** — Tests to validating file system behavior  

---

## What FileSys (filesys.cpp) Does

`FileSys` is a dynamic hash-based file storage system.

### Supported Operations
- `insert(File)`
- `remove(File)`
- `getFile(name, block)`
- `updateDiskBlock(File, newBlock)`

### Collision Resolution
Uses open addressing with selectable probing styles:
- **LINEAR**
- **QUADRATIC**
- **DOUBLEHASH** (default)

### Automatic Rehashing Rules
Rehashing is triggered when:
- **Load factor > 0.5**
- **Deleted ratio > 0.8**

Rehashing behavior:
- Moves the current table into `m_oldTable`
- Allocates a new table using the next prime ≥ `4 × (# live entries)`
- Gradually reinserts **25%** of the old table during each insertion  
  (incremental migration)
