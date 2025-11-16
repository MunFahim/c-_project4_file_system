// CMSC 341 - Fall 2024 - Project 4

/*
    Name: Muntasir Fahim
    Date: 11/28/24
    File: mytest.cpp

*/


#include "filesys.h"
FileSys::FileSys(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    // get a valid size for the hash table
    if (size < MINPRIME){
        m_currentCap = MINPRIME;
    }else if (size > MAXPRIME){
        m_currentCap = MAXPRIME;
    }else if (isPrime(size)){
        m_currentCap = size;
    }else{
        m_currentCap = findNextPrime(size);
    }

    // set current size and make new table
    m_currentSize = 0;
    m_currentTable = new File*[m_currentCap];

    // create the new table
    for (int i = 0; i < m_currentCap; i++){
        m_currentTable[i] = nullptr;
    }
    // set defaults
    m_newPolicy = probing;
    m_hash = hash;
    m_currNumDeleted = 0;
    m_transferIndex = 0;
    m_currProbing = probing;
    m_oldTable = nullptr;
    
}

FileSys::~FileSys(){
    clear(m_currentTable, m_currentCap);
    clear(m_oldTable, m_oldCap);
}

void FileSys::clear(File** &table, int size){
    // loop through the table, clearning each File at a time
    if (table != nullptr){
        for(int i = 0; i < size; i++){
            delete table[i];
            table[i] = nullptr;
        }
        delete[] table;
        // table is now null
        table = nullptr;
    }
}


void FileSys::changeProbPolicy(prob_t policy){
    m_newPolicy = policy;
}

bool FileSys::insert(File file){
    // insert with helper
    return doInsert(true, file);
}



bool FileSys::doInsert(bool checkO, File file){
    // if file is empty, dont add it
    if (file.m_used == false){
        return false;
    }
    // check if disk is valid
    if (file.m_diskBlock > DISKMAX || file.m_diskBlock < DISKMIN){
        return false;
    }
    // get inital hash
    int ind = m_hash(file.m_name) % m_currentCap;
    // check if index of hash is not empty
    if (m_currentTable[ind] == nullptr){
        // if not empty, then create a new file and add it in
        File* f = new File(file.m_name, file.m_diskBlock, file.m_used);
        m_currentTable[ind] = f;
        m_currentSize++;
        // if need to check (load balance or deleted ration or old_table)
        if (checkO){
            check();
        }
        return true;
    }else if (m_currentTable[ind]->m_used == false){
        // index position is used, so you can remove the current file and add in the new one
        delete m_currentTable[ind];
        File* f = new File(file.m_name, file.m_diskBlock, file.m_used);
        m_currentTable[ind] = f;
        m_currentSize++;
        // check for load balance, etc...
        if (checkO){
            check();
        }
        return true;
    }
    // if position is taken and is valid file, then get next avaliable position
    int newInd = nextPos(file);
    // if none avaliable from hash probing then file not added
    if (newInd == -1){
        return false;
    }else{
        // else add the file into the table, and do check if necessary
        File* f = new File(file.m_name, file.m_diskBlock, file.m_used);
        m_currentTable[newInd] = f;
        m_currentSize++;
        if (checkO){
            check();
        }
        return true;
    }

    return false;
}

void FileSys::check(){
    // used to check for load balance, deleted ratio, or if 25% still needs to be added into current table
    if (m_oldTable != nullptr){
        rehashNext();
    }else if (lambda() > 0.5){
        transferTable();
    }else if (deletedRatio() > 0.8){
        transferTable();
    }
}


int FileSys::nextPos(File file){
    // used to get the next position in the table using the probing method
    int ind = m_hash(file.m_name) % m_currentCap;
    // index at 1, then continue -> 2, 3, 4 ...
    bool notFound = true;
    int i = 1;
    // gets the hash index position
    int newIndex = getFileHelper(file.m_name, i, m_currProbing, m_currentCap);
    while(notFound){
        // if avaliable then return the index position
        if (m_currentTable[newIndex] == nullptr){
            notFound = false;
            return newIndex;
        }else if (m_currentTable[newIndex]->m_used == false){
            // if index position has empty / deleted file then delete the file and return index
            delete m_currentTable[newIndex];
            notFound = false;
            return newIndex;
        }else if (newIndex == ind){
            // if table has been looped, then file not added
            notFound = false;
        }
        // else continue to next i'th value and get next probing position
        i++;
        newIndex = getFileHelper(file.m_name, i, m_currProbing, m_currentCap);
    }
    return -1;
}


void FileSys::rehashNext(){
    // rehashing the next 25%
    // get amount needed for 25%
    int getPercent = (m_oldCap*25)/100;
    // set the ending after added 25% to the current pointer
    int end = m_transferIndex+getPercent;
    if (end > m_oldCap){
        end = m_oldCap;
    }
    // loop through the 25%, adding in live Files
    for (int i = m_transferIndex; i < end; i++){
        if (m_oldTable[i] != nullptr && m_oldTable[i]->m_used == true){
            doInsert(false, File(m_oldTable[i]->m_name, m_oldTable[i]->m_diskBlock, m_oldTable[i]->m_used));
        }
    }
    // update the new pointer
    m_transferIndex = end;
    if (m_transferIndex >= m_oldCap){
        // clear old table / pointer
        clear(m_oldTable, m_oldCap);
        m_transferIndex = 0;
    }

}


void FileSys::transferTable(){
    if (m_oldTable != nullptr){
        // clear table
        clear(m_oldTable, m_oldCap);
    }


    // set the values from curr table to old table
    m_oldCap = m_currentCap;
    m_oldSize = m_currentSize;
    m_oldNumDeleted = m_currNumDeleted;
    m_oldProbing = m_currProbing;
    // make old table
    m_oldTable = new File*[m_currentCap];
    // trasfering files from current table to old table
    for (int i = 0; i < m_currentCap; i++){
        if (m_currentTable[i] != nullptr){
            File* newF = new File(m_currentTable[i]->m_name, m_currentTable[i]->m_diskBlock, m_currentTable[i]->m_used);
            m_oldTable[i] = newF;
        }else{
            m_oldTable[i] = nullptr;
        }
    }
    
    // clear curr table
    clear(m_currentTable, m_currentCap);
    // reset current table by getting the new size, and making an empty table of nullptrs
    int newSize = findNextPrime(4*(m_currentSize-m_currNumDeleted));
    m_currentTable = new File*[newSize];
    m_currentCap = newSize;
    for (int i = 0; i < m_currentCap; i++){
        m_currentTable[i] = nullptr;
    }
    // reset the values of current table
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_currProbing = m_newPolicy;
}


bool FileSys::remove(File file){
    // check if valid
    if (file.m_diskBlock > DISKMAX || file.m_diskBlock < DISKMIN){
        return false;
    }
    // removes from both current and old tables
    // remove from current first if possible
    bool checkCurr = removeHelper(m_currentTable, m_currentCap, file, m_currProbing);
    bool checkOld = false;
    // if old table active, then remove from old table is possible
    if (m_oldTable != nullptr){
        checkOld = removeHelper(m_oldTable, m_oldCap, file, m_oldProbing);
    }
    // returning if removed from any one
    return checkCurr || checkOld;

}

bool FileSys::removeHelper(File** &table, int size, File file, prob_t method){
    // used to help remove file
    // find the index position of the file
    int index = m_hash(file.m_name) % size;
    if (table[index] != nullptr && table[index]->m_name == file.m_name && table[index]->m_diskBlock == file.m_diskBlock){
        if (table[index]->m_used == false){
            return false;
        }
        // found then update used member and return
        table[index]->setUsed(false);
        m_currNumDeleted++;
        check();
        return true;
    }
    // if not found at initial hash position, then keep probing until found
    int i = 1;
    bool notFound = true;
    int next = getFileHelper(file.m_name, i, method, size);
    while(notFound){
        if (table[next] && table[next]->m_diskBlock == file.m_diskBlock && table[next]->m_name == file.m_name){
            if (table[next]->m_used == false){
                return false;
            }
            // if found then updated its m_used member, and return true
            table[next]->setUsed(false);
            m_currNumDeleted++;
            notFound = false;
            check();
            return true;
        }else if (next == index){
            notFound = false;
        }else{
            i++;
            next = getFileHelper(file.m_name, i, method, size);
        }
    }
    return false;
}




const File FileSys::getFile(string name, int block) const{
    // check if file is valid
    if (block > DISKMAX || block < DISKMIN){
        return File();
    }
    if (name == ""){return File();}

    // look for file in current table
    File gotF = findHelper(m_currentTable, m_currentCap, name, block, m_currProbing);

    // if file not found in current table, and old table is active, then look in old table
    if (m_oldTable != nullptr && (gotF.m_name != name || gotF.m_diskBlock != block)){
        return findHelper(m_oldTable, m_oldCap, name, block, m_oldProbing);
    }

    return gotF;

}

File FileSys::findHelper(File** table, int size, string name, int block, prob_t method) const {
    // helper function used to find find the file
    // look for file in inital hash
    int index = m_hash(name) % size;
    if (table[index] != nullptr && table[index]->m_name == name && table[index]->m_diskBlock == block){
        // if empty, return empty file, if not empty then return the file object
        if (table[index]->m_used == false){
            return File();
        }
        return *table[index];
    }
    // if not found in inital hash, then keep probing untill found
    // if not found from probing then returning empty file
    int i = 1;
    bool notFound = true;
    int next = getFileHelper(name, i, method, size);
    while(notFound){
        if (table[next] && table[next]->m_diskBlock == block && table[next]->m_name == name){
            if (table[next]->m_used == false){
                return File();
            }
            return *table[next];
        }else if (next == index){
            notFound = false;
        }else{
            i++;
            next = getFileHelper(name, i, method, size);
        }
    }
    return File();
}




int FileSys::getFileHelper(string name, int i, prob_t method, int size) const {
    // helper for probing
    if (method == LINEAR){
        // liner -> hash + i mod the size
        return (m_hash(name)+i) % size;
    }else if (method == DOUBLEHASH){
        // double hash -> hash mod size + i times 11 - (hash mod 11) mod size
        return ((m_hash(name) % size) + i * (11-(m_hash(name) % 11))) % size;
    }else{
        // quad -> i*i -> hash mod size + i squared mod size
        return ((m_hash(name)%size)+(i*i)) % size;
    }
}


bool FileSys::updateDiskBlock(File file, int block){
    if (block > DISKMAX || block < DISKMIN){
        return false;
    }
    // updating the disk block of the current table
    int fileIndex = -1;
    // first look for the location of the needed file based on the disk block and name
    // if the new value of the block is found while we are searching then we will have a duplicate file
    int check = m_hash(file.m_name) % m_currentCap;
    if (m_currentTable[check] != nullptr && m_currentTable[check]->m_name == file.m_name && m_currentTable[check]->m_diskBlock == file.m_diskBlock){
        fileIndex = check;
    }
    // keep looping for all the files of that name, and try to look for any matching files
    int i = 1;
    bool notFound = true;
    int next = getFileHelper(file.m_name, i, m_currProbing, m_currentCap);
    while(notFound){
        if (m_currentTable[next] != nullptr && m_currentTable[next]->m_name == file.m_name && m_currentTable[next]->m_diskBlock == file.m_diskBlock){
            fileIndex = next;
        }else if (m_currentTable[next] != nullptr && m_currentTable[next]->m_diskBlock == block){
            // if duplicate file is found, then exit the loop
            fileIndex = -1;
            notFound = false;
        }else if (next == check){
            notFound = false;
        }else{
            i++;
            next = getFileHelper(file.m_name, i, m_currProbing, m_currentCap);
        }
    }
    // updated the block based on the if not dupliated and is found
    if (fileIndex == -1){
        return false;
    }else{
        m_currentTable[fileIndex]->setDiskBlock(block);
        return true;
    }
}

float FileSys::lambda() const {
    // current size of the table divided by the current capacity of the table
    return (float)m_currentSize/(float)m_currentCap;
}

float FileSys::deletedRatio() const {
    // number of deleted files compared to number to modified elements
    if (m_currentSize == 0){
        return 0;
    }
    return (float)m_currNumDeleted/(float)m_currentSize;
}

void FileSys::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool FileSys::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int FileSys::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}
