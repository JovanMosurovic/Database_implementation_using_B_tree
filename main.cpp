#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <iomanip>

using namespace std;

const string red = "\033[1;31m";
const string green = "\033[1;32m";
const string resetColor = "\033[0m";
const string underline = "\033[4m";
const string bold = "\033[1m";

struct Customer {
    long long CUSTOMER_ID;
    string CUSTOMER_F_NAME;
    string CUSTOMER_L_NAME;
    string CUSTOMER_EMAIL_1;
    long long CUSTOMER_AD_ID;
};

struct CustomerAccount {
    long long CUSTOMERACCOUNT_ID;
    long long CUSTOMERACCOUT_B_ID;
    long long CUSTOMERACCOUNT_C_ID;
    string CUSTOMERACCOUNT_NAME;
    int CUSTOMERACCOUNT_TAX_STATUS;
    double CUSTOMERACCOUNT_BALANCE;
};

struct KeyValuePair {
    long long key;
    CustomerAccount *account;

    KeyValuePair(long long key, CustomerAccount *account) : key(key), account(account) {}

    KeyValuePair() : key(0), account(nullptr) {}

    bool operator==(const KeyValuePair &keyValPair) const {
        return key == keyValPair.key;
    }

    bool operator!=(const KeyValuePair &keyValPair) const {
        return key != keyValPair.key;
    }

    bool operator<(const KeyValuePair &keyValPair) const {
        return key < keyValPair.key;
    }

    bool operator>(const KeyValuePair &keyValPair) const {
        return key > keyValPair.key;
    }

    bool operator<=(const KeyValuePair &keyValPair) const {
        return key <= keyValPair.key;
    }

    bool operator>=(const KeyValuePair &keyValPair) const {
        return key >= keyValPair.key;
    }

};

class BTreeElem {
    KeyValuePair *accounts;
    bool isLeaf;
    BTreeElem **children;
    int numberOfKeys;
    int maxKeys;
    BTreeElem *parent;

public:

    BTreeElem(int maxKeys_) : maxKeys(maxKeys_), numberOfKeys(0), isLeaf(true), parent(nullptr) {
        accounts = new KeyValuePair[maxKeys];
        children = new BTreeElem*[maxKeys + 2];
        for (int i = 0; i < maxKeys + 2; ++i) {
            children[i] = nullptr;
        }
    }

    ~BTreeElem() {
        for (int i = 0; i < (maxKeys + 2); ++i) {
            delete children[i];
        }
        delete[] children;
    }


    //<editor-fold desc="Getters">

    KeyValuePair *getAccounts() const {
        return accounts;
    }

    CustomerAccount *getAccount(long long accId) {
        for (int i = 0; i < numberOfKeys; ++i) {
            if (accounts[i].key == accId) {
                return accounts[i].account;
            }
        }
        return nullptr;
    }

    bool isIsLeaf() const {
        return isLeaf;
    }

    BTreeElem **getChildren() const {
        return children;
    }

    int getNumberOfKeys() const {
        return numberOfKeys;
    }

    int getMaxKeys() const {
        return maxKeys;
    }

    BTreeElem *getParent() const {
        return parent;
    }

    //</editor-fold>

    //<editor-fold desc="Setters">

    void setAccounts(KeyValuePair *accounts_) {
        this->accounts = accounts_;
    }

    void setAccount(KeyValuePair keyValPair) {
        for (int i = 0; i < numberOfKeys; ++i) {
            if (accounts[i].key == keyValPair.key) {
                accounts[i].account = keyValPair.account;
                return;
            }
        }
        accounts[numberOfKeys++] = keyValPair;
    }

    void setIsLeaf(bool isLeaf_) {
        this->isLeaf = isLeaf_;
    }

    void setChildren(BTreeElem **children_) {
        this->children = children_;
    }

    void setNumberOfKeys(int numberOfKeys_) {
        this->numberOfKeys = numberOfKeys_;
    }

    void setMaxKeys(int maxKeys_) {
        this->maxKeys = maxKeys_;
    }

    void setParent(BTreeElem *parent_) {
        this->parent = parent_;
    }

    //</editor-fold>

};

class BTree {
    int m;
    BTreeElem *root;
    Customer *customers;
    int customerCount;

public:


    BTree(int m, BTreeElem *root = nullptr) : m(m), root(root) {}

    ~BTree() {
        delete root;
    }


    void insertAccount(CustomerAccount *customerAccount);
    void printTree();
    void searchAccount(long long accId);
    void removeAccount(long long accId);
    void removeAllAccounts(long long customerId);
    void loadDataFromFile(const string &fileName);
    void loadCustomersFromFile(const string &fileName);
    void createOutputFile(Customer* customer, CustomerAccount** accounts, int count);


    void splitNode(BTreeElem *node, const KeyValuePair &newPair);
    bool existsAccountWithID(long long accId);
    bool existsAccountWithIDWithSteps(long long accId, int* steps);
    void gatherKeys(BTreeElem* node, vector<KeyValuePair>& keys);
    void insertAccountNoOut(CustomerAccount *account);
    void printCustomerDetails(Customer* customer);
    void splitParent(BTreeElem *node, const KeyValuePair &newPair);
    void rebuildTree(vector<KeyValuePair>& keys);


    //<editor-fold desc="Getters">

    BTreeElem *getRoot() const {
        return root;
    }

    //</editor-fold>

    //<editor-fold desc="Setters">

    void setRoot(BTreeElem *root) {
        this->root = root;
    }

    //</editor-fold>

};

void BTree::insertAccount(CustomerAccount *customerAccount) {
    long long accId = customerAccount->CUSTOMERACCOUNT_ID;
    cout << "->Inserting ID: " << accId << endl;

    if (existsAccountWithID(accId)) {
        cout << red << "Error: An account with ID " << accId << " already exists." << resetColor << endl;
        return;
    }

    if (root == nullptr) {
        root = new BTreeElem(m);
        root->setAccount({accId, customerAccount});
        return;
    }

    BTreeElem *current = root;
    while (!current->isIsLeaf()) {
        int i = 0;
        while (i < current->getNumberOfKeys() && accId > current->getAccounts()[i].key) {
            i++;
        }
        current = current->getChildren()[i];
    }

    if (current->getNumberOfKeys() < m - 1) {
        int i = current->getNumberOfKeys() - 1;
        while (i >= 0 && current->getAccounts()[i].key > accId) {
            current->getAccounts()[i + 1] = current->getAccounts()[i];
            i--;
        }
        current->setAccount({accId, customerAccount});
    } else {
        cout << "Splitting the node" << endl;
        splitNode(current, {accId, customerAccount});
    }
}

void BTree::searchAccount(long long accId) {
    Customer* customer = nullptr;
    for(int i = 0; i < customerCount; i++) {
        if(customers[i].CUSTOMER_ID == accId) {
            customer = &customers[i];
            break;
        }
    }

    if(customer == nullptr) {
        cout << red << "Customer not found." << resetColor << endl;
        return;
    }

    printCustomerDetails(customer);

    CustomerAccount* foundAccounts[5555];
    int foundCount = 0;
    int steps = 0;

    queue<BTreeElem*> nodesQueue;
    nodesQueue.push(root);

    while (!nodesQueue.empty()) {
        BTreeElem* currentNode = nodesQueue.front();
        nodesQueue.pop();

        for (int i = 0; i < currentNode->getNumberOfKeys(); i++) {
            steps++;
            if (currentNode->getAccounts()[i].account->CUSTOMERACCOUNT_C_ID == accId) {
                foundAccounts[foundCount++] = currentNode->getAccounts()[i].account;
            }
        }

        if (!currentNode->isIsLeaf()) {
            for (int i = 0; i <= currentNode->getNumberOfKeys(); i++) {
                if (currentNode->getChildren()[i] != nullptr) {
                    nodesQueue.push(currentNode->getChildren()[i]);
                }
            }
        }
    }

    cout << bold << "Total number of steps: " << steps << endl;
    createOutputFile(customer, foundAccounts, foundCount);
}

void BTree::removeAccount(long long accId) {
    cout << "->Deleting ID: " << accId << endl;
    int numberOfSteps = 0;

    if (!existsAccountWithIDWithSteps(accId, &numberOfSteps)) {
        cout << red << "Error: Account with ID " << accId << " does not exist." << resetColor << endl;
        cout << bold << "\xB3" << "Number of steps for search: " << numberOfSteps << resetColor << endl;
        return;
    }

    vector<KeyValuePair> keys;
    gatherKeys(root, keys);

    vector<KeyValuePair> filteredKeys;
    for (int i = 0; i < keys.size(); ++i) {
        if (keys[i].key != accId) {
            filteredKeys.push_back(keys[i]);
        }
    }

    for (int i = 1; i < filteredKeys.size(); ++i) {
        KeyValuePair key = filteredKeys[i];
        int j = i - 1;

        while (j >= 0 && filteredKeys[j].key > key.key) {
            filteredKeys[j + 1] = filteredKeys[j];
            --j;
        }
        filteredKeys[j + 1] = key;
    }
    rebuildTree(filteredKeys);

    cout << green << "The account with ID " << accId << " has been successfully deleted." << resetColor << endl;

}

void BTree::removeAllAccounts(long long customerId) {
    cout << "\xC4>Deleting all accounts for the user with ID: " << customerId << endl;
    int numberOfSteps = 0;
    int deletedCount = 0;

    vector<KeyValuePair> keys;
    gatherKeys(root, keys);

    vector<KeyValuePair> filteredKeys;
    for (const auto& key : keys) {
        numberOfSteps++;
        if (key.account->CUSTOMERACCOUNT_C_ID == customerId) {
            cout << "Deleted account: " << key.account->CUSTOMERACCOUNT_ID << endl;
            deletedCount++;
        } else {
            filteredKeys.push_back(key);
        }
    }

    rebuildTree(filteredKeys);

    cout << bold << "\xB3Total number of steps to delete all accounts: " << numberOfSteps << resetColor << endl;

    if (deletedCount == 0) {
        cout << "No accounts have been deleted for the user with ID " << customerId << "." << endl;
    } else {
        cout << green << "All accounts for the user with ID " << customerId << " have been successfully deleted." << resetColor << endl;
    }
}



void BTree::splitNode(BTreeElem *node, const KeyValuePair &newPair) {

    KeyValuePair *temp = new KeyValuePair[m + 1];
    int i, j;
    for (i = 0; i < m - 1; i++) {
        temp[i] = node->getAccounts()[i];
    }

    for (i = m - 2; i >= 0 && newPair.key < temp[i].key; i--) {
        temp[i + 1] = temp[i];
    }
    temp[i + 1] = newPair;

    BTreeElem *newNode = new BTreeElem(m);
    newNode->setIsLeaf(node->isIsLeaf());

    int middleIndex = (m - 1) / 2;
    KeyValuePair middleKey = temp[middleIndex];
    node->setNumberOfKeys(middleIndex);
    newNode->setNumberOfKeys(m - 1 - middleIndex);

    for (int i = 0; i < middleIndex; ++i) {
        node->getAccounts()[i] = temp[i];
    }
    for (int i = middleIndex + 1, j = 0; i < m; ++i, ++j) {
        newNode->getAccounts()[j] = temp[i];
    }

    if (!node->isIsLeaf()) {
        for (int i = middleIndex + 1, j = 0; i <= m; ++i, ++j) {
            newNode->getChildren()[j] = node->getChildren()[i];
            if (newNode->getChildren()[j] != nullptr) {
                newNode->getChildren()[j]->setParent(newNode);
            }
            node->getChildren()[i] = nullptr;
        }
    }

    if (node->getParent() == nullptr) {
        BTreeElem *newRoot = new BTreeElem(m);
        newRoot->setIsLeaf(false);
        newRoot->getChildren()[0] = node;
        newRoot->getChildren()[1] = newNode;
        newRoot->setAccount(middleKey);
        root = newRoot;
        node->setParent(newRoot);
        newNode->setParent(newRoot);
    } else {
        BTreeElem *parent = node->getParent();
        int index;
        for (index = parent->getNumberOfKeys() - 1; index >= 0 && parent->getAccounts()[index].key > middleKey.key; index--) {
            parent->getAccounts()[index + 1] = parent->getAccounts()[index];
            parent->getChildren()[index + 2] = parent->getChildren()[index + 1];
        }
        parent->getAccounts()[index + 1] = middleKey;
        parent->getChildren()[index + 1] = node;
        parent->getChildren()[index + 2] = newNode;
        parent->setNumberOfKeys(parent->getNumberOfKeys() + 1);
        newNode->setParent(parent);

        if (parent->getNumberOfKeys() >= m) {
            splitParent(parent, middleKey);
        }
    }

    delete[] temp;
}

bool BTree::existsAccountWithID(long long accId) {
    BTreeElem *current = root;
    while (current != nullptr) {
        for (int i = 0; i < current->getNumberOfKeys(); i++) {
            if (current->getAccounts()[i].key == accId) {
                return true;
            }
        }
        int i = 0;
        while (i < current->getNumberOfKeys() && accId > current->getAccounts()[i].key) {
            i++;
        }
        current = current->getChildren()[i];

    }
    return false;
}

bool BTree::existsAccountWithIDWithSteps(long long accId, int* steps) {
    BTreeElem *current = root;
    while (current != nullptr) {
        (*steps)++;
        for (int i = 0; i < current->getNumberOfKeys(); i++) {
            if (current->getAccounts()[i].key == accId) {
                return true;
            }
        }
        int i = 0;
        while (i < current->getNumberOfKeys() && accId > current->getAccounts()[i].key) {
            i++;
        }
        current = current->getChildren()[i];
    }
    return false;
}

void BTree::gatherKeys(BTreeElem* node, vector<KeyValuePair>& keys) {
    if (node == nullptr) return;

    for (int i = 0; i < node->getNumberOfKeys(); ++i) {
        keys.push_back(node->getAccounts()[i]);
    }

    if (!node->isIsLeaf()) {
        for (int i = 0; i <= node->getNumberOfKeys(); ++i) {
            gatherKeys(node->getChildren()[i], keys);
        }
    }
}

void BTree::insertAccountNoOut(CustomerAccount *customerAccount) {
    long long accId = customerAccount->CUSTOMERACCOUNT_ID;

    if (existsAccountWithID(accId)) {
        cout << red << "Error: Account with ID " << accId << " already exists." << resetColor << endl;
        return;
    }

    if (root == nullptr) {
        root = new BTreeElem(m);
        root->setAccount({accId, customerAccount});
        return;
    }

    BTreeElem *current = root;
    while (!current->isIsLeaf()) {
        int i = 0;
        while (i < current->getNumberOfKeys() && accId > current->getAccounts()[i].key) {
            i++;
        }
        current = current->getChildren()[i];
    }

    if (current->getNumberOfKeys() < m - 1) {
        int i = current->getNumberOfKeys() - 1;
        while (i >= 0 && current->getAccounts()[i].key > accId) {
            current->getAccounts()[i + 1] = current->getAccounts()[i];
            i--;
        }
        current->setAccount({accId, customerAccount});
    } else {
        splitNode(current, {accId, customerAccount});
    }
}

void BTree::printCustomerDetails(Customer* customer) {
    if (customer != nullptr) {
        cout << green << "Customer details:" << resetColor << endl;
        cout << "ID: " << customer->CUSTOMER_ID << endl;
        cout << "First Name: " << customer->CUSTOMER_F_NAME << endl;
        cout << "Last Name: " << customer->CUSTOMER_L_NAME << endl;
        cout << "Email: " << customer->CUSTOMER_EMAIL_1 << endl;
        cout << "Address ID: " << customer->CUSTOMER_AD_ID << endl;
    } else {
        cout << red << "The customer is not found." << resetColor << endl;
    }
}

void BTree::splitParent(BTreeElem *node, const KeyValuePair &newPair) {
    splitNode(node, newPair);
}

void BTree::rebuildTree(vector<KeyValuePair>& keys) {

    int n = keys.size();
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++)
            if (keys[j].key < keys[min_idx].key)
                min_idx = j;

        KeyValuePair temp = keys[i];
        keys[i].key = keys[min_idx].key;
        keys[i].account = keys[min_idx].account;
        keys[min_idx].key = temp.key;
        keys[min_idx].account = temp.account;
    }

    int numberOfSteps = 0;
    delete root;
    root = nullptr;

    for (int i = 0; i < keys.size(); ++i) {
        insertAccountNoOut(keys[i].account);
        numberOfSteps++;
    }
    cout << bold << "\xB3Total number of steps for deletion: " << numberOfSteps/4 << resetColor << endl;
}



// <editor-fold desc="ISPISI">

void BTree::printTree() {
    if (root == nullptr) {
        cout << red << "The tree is empty" << resetColor << endl;
        return;
    }

    cout << endl << underline << "\t\t\tPRINT TREE" << resetColor << '\n' << endl;

    queue<tuple<BTreeElem*, int, string>> nodesQueue;
    nodesQueue.push(make_tuple(root, 0, "R: "));

    int currentLevel = 0;
    while (!nodesQueue.empty()) {
        BTreeElem* currentNode;
        int nodeLevel;
        string position;
        tie(currentNode, nodeLevel, position) = nodesQueue.front();
        nodesQueue.pop();

        if (nodeLevel > currentLevel) {
            cout << endl;
            currentLevel = nodeLevel;
        }

        cout << position << string(4 * nodeLevel, ' ') << "{ ";
        for (int j = 0; j < currentNode->getNumberOfKeys(); j++) {
            if (j > 0) cout << ", ";
            cout << currentNode->getAccounts()[j].key;
        }
        cout << " }";

        if (!currentNode->isIsLeaf()) {
            for (int j = 0; j <= currentNode->getNumberOfKeys(); j++) {
                if (currentNode->getChildren()[j] != nullptr) {
                    string childPosition = (j == 0) ? "L:" : (j == currentNode->getNumberOfKeys()) ? "R:" : "M:";
                    nodesQueue.push(make_tuple(currentNode->getChildren()[j], nodeLevel + 1, childPosition));
                }
            }
        }
        cout << endl;
    }
}

void finish() {
    printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
    printf("\xB3         Exiting the program...       \xB3\n");
    printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
    printf("----------------------------------------");
    printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
    printf("\xB3    Program successfully completed!   \xB3\n");
    printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
    exit(0);
}

// </editor-fold>

// <editor-fold desc="FAJLOVI">

void BTree::loadDataFromFile(const string &fileName) {
    ifstream file(fileName);
    string line;

    if (!file.is_open()) {
        cout << red << "Error opening the file!" << resetColor << endl;
        return;
    }
    else {
        cout << green << "The file has been successfully opened!" << resetColor << '\n' << endl;
    }

    while (getline(file, line)) {
        stringstream ss(line);

        long long customerId, bankId, custAccId;
        string accountName;
        int taxStatus;
        double balance;

        getline(ss, line, '|');
        customerId = stoll(line);
        getline(ss, line, '|');
        bankId = stoll(line);
        getline(ss, line, '|');
        custAccId = stoll(line);
        getline(ss, accountName, '|');
        getline(ss, line, '|');
        taxStatus = stoi(line);
        getline(ss, line);
        balance = stod(line);

        CustomerAccount* account = new CustomerAccount{
                customerId, bankId, custAccId,
                accountName, taxStatus, balance
        };

        this->insertAccount(account);
    }
    cout << '\n' << green << "File loading complete!" << resetColor << endl;
    file.close();
}

void BTree::loadCustomersFromFile(const string &fileName) {
    ifstream file(fileName);
    string line;
    customerCount = 0;
    customers = new Customer[5555];

    while (getline(file, line)) {
        stringstream ss(line);
        long long customerId;
        string firstName, lastName, email;
        long long adId;

        getline(ss, line, '|');
        customerId = stoll(line);
        getline(ss, firstName, '|');
        getline(ss, lastName, '|');
        getline(ss, email, '|');
        getline(ss, line);
        adId = stoll(line);

        customers[customerCount++] = {customerId, firstName, lastName, email, adId};
    }
    file.close();
}

void BTree::createOutputFile(Customer* customer, CustomerAccount** accounts, int count) {
    ofstream outFile("outputFile.txt");
    if (!outFile.is_open()) {
        cout << "Error opening output file!" << endl;
        return;
    }
    outFile << fixed << setprecision(2);

    if (count == 0) {
        outFile << customer->CUSTOMER_ID << "|" << customer->CUSTOMER_F_NAME << "|"
                << customer->CUSTOMER_L_NAME << "|" << customer->CUSTOMER_EMAIL_1
                << "|null|null|null|null" << endl;
    } else {
        for (int i = 0; i < count; i++) {
            outFile << customer->CUSTOMER_ID << "|" << customer->CUSTOMER_F_NAME << "|"
                    << customer->CUSTOMER_L_NAME << "|" << customer->CUSTOMER_EMAIL_1 << "|"
                    << accounts[i]->CUSTOMERACCOUNT_ID << "|" << accounts[i]->CUSTOMERACCOUNT_C_ID << "|"
                    << accounts[i]->CUSTOMERACCOUNT_TAX_STATUS << "|" << accounts[i]->CUSTOMERACCOUNT_BALANCE << endl;
        }
    }

    cout << green << "Output file created successfully!" << resetColor << endl;

    outFile.close();
}

// </editor-fold>



int main() {
    int m;
    cout << bold << "\nEnter m: " << resetColor << endl << "->";
    cin >> m;
    if(m < 3 || m > 10) {
        cout << red << "Error: m must be between 3 and 10." << resetColor << endl;
        return 0;
    }
    BTree *b = nullptr;
    int choice;

    do {
        printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
        printf("\xB3                 MENU                 \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  1. CREATE B TREE FROM FILE          \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  2. SEARCH ALL ACCOUNTS              \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  3. ADD NEW ACCOUNT                  \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  4. DELETE ACCOUNT                   \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  5. DELETE ALL ACCOUNTS              \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  6. PRINT THE TREE                   \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  0. EXIT                             \xB3\n");
        printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
        cout << "\xB3" << "Enter a number to select your desired option: " << endl << "\xC4>";
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "You have selected the option \"CREATE B TREE FROM FILE\"" << endl;
                if(b != nullptr) {
                    cout << red << "\nYou have already created a tree from the file!" << resetColor << endl;
                    break;
                }
                string customerFileName = "Customer.txt";
                string dataFileName = "CustomerAccount20.txt";
                b = new BTree(m);
                b->loadCustomersFromFile(customerFileName);
                b->loadDataFromFile(dataFileName);
                break;
            }

            case 2: {
                cout << "You have selected the option \"SEARCH ALL ACCOUNTS\"" << endl;
                if(!b) {
                    cout << "\nFirst create the tree from the file." << endl;
                    break;
                }
                cout << "\xB3" << "Enter Customer ID: " << endl << "\xC4>";
                long long accId;
                cin >> accId;
                b->searchAccount(accId);

                break;
            }

            case 3: {
                cout << "You have selected the option \"ADD NEW ACCOUNT\"" << endl;
                if(!b) {
                    cout << "\nFirst create the tree from the file." << endl;
                    break;
                }
                printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
                printf("\xB3     ENTER DATA FOR A NEW ACCOUNT     \xB3\n");
                printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");

                long long customerId, bankId, accId;
                string accountName;
                int taxStatus;
                double balance;

                cout << "CUSTOMER ID: ";
                cin >> customerId;
                cout << "BANK ID: ";
                cin >> bankId;
                cout << "ACCOUNT ID: ";
                cin >> accId;
                cout << "ACCOUNT NAME: ";
                cin.ignore();
                getline(cin, accountName);
                cout << "TAX STATUS (0 or 1): ";
                cin >> taxStatus;
                if(!(taxStatus == 0 || taxStatus == 1)) {
                    cout << red << "Error: Tax status must be 0 or 1." << resetColor << endl;
                    break;
                }
                cout << "BALANCE: ";
                cin >> balance;

                CustomerAccount *newAccount = new CustomerAccount{accId, bankId, customerId, accountName, taxStatus, balance};
                b->insertAccount(newAccount);

                break;
            }

            case 4: {
                cout << "You have selected the option \"DELETE ACCOUNT\"" << endl;
                if(!b) {
                    cout << "\nFirst create the tree from the file." << endl;
                    break;
                }
                cout << "\xB3" << "Enter the ID of the account you want to delete: " << endl << "\xC4>";
                long long accId;
                cin >> accId;
                b->removeAccount(accId);
                break;
            }

            case 5: {
                cout << "You have selected the option \"DELETE ALL ACCOUNTS \"" << endl;
                if(!b) {
                    cout << "\nFirst create the tree from the file." << endl;
                    break;
                }
                cout << "\xB3" << "Enter the user ID whose accounts you want to delete: " << endl << "\xC4>";
                long long customerId;
                cin >> customerId;
                b->removeAllAccounts(customerId);
                break;
            }

            case 6: {
                cout << "You have selected the option \"PRINT THE TREE\"" << endl;
                if(!b) {
                    cout << "\nFirst create the tree so you can print it." << endl;
                    break;
                }
                b->printTree();
                break;
            }

            case 0:
                finish();
                break;

            default:
                cout << "Wrong choice, please enter the numbers 1, 2, 3, 4, 5, 6, or 0 to exit." << endl;
        }

    } while (choice != 0);

    delete b;
    return 0;
}

