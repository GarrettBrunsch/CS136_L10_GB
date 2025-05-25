// Garrett Brunsch
// Lab #10 Priority Queue/Heap S25
// CS_136_14728SP25P
// Due 5/18/25 with extension due 5/25

// To simplify testing I have included a few key spots to easily modify testing data:
    // input and output files are used as const strings at the top of the file
    // class RushInfo allows for easy modificationof rush names and percentages
    // processOrders has a couple optional lines of code to clear (or keep) the backorder log from previous runs of this program

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

const string IN_INVENTORY = "in_inventory.txt";
const string IN_ORDERS = "in_orders.txt";
const string OUT_BACKORDER = "out_backorder.txt";

enum MenuChoice
{
    DISPLAY_INVENTORY = 1,
    UNPROCESSED_ORDERS,
    PROCESS_ORDERS,
    EXIT
};

class RushInfo
{
private:
    const string RUSH_NAMES[4] = { "", "extreme", "expedite", "standard" };
    const double MARKUP_PERCENTAGES[4] = { 0.0, 0.68, 0.45, 0.38 };

public:
    string getRushName(int rushCode) const;
    double getMarkupPercentage(int rushCode) const;
};

class Item
{
private:
    string itemId;
    string itemName;
    int quantity;
    double price;

public:
    Item();
    Item(const string& id, const string& name, int qty, double p);

    string getItemId() const { return itemId; }
    string getItemName() const { return itemName; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }

    void setItemId(const string& id) { itemId = id; }
    void setItemName(const string& name) { itemName = name; }
    void setQuantity(int qty) { quantity = qty; }
    void setPrice(double p) { price = p; }

    bool reduceQuantity(int qty);
    void display() const;
};

class Inventory
{
private:
    static const int MAX_ITEMS = 5;
    Item items[MAX_ITEMS];
    int itemCount;

public:
    Inventory();
    bool loadFromFile(const string& filename);
    Item* findItem(const string& itemId);
    void display() const;
    int getItemCount() const { return itemCount; }
};

class Order
{
private:
    int orderNumber;
    string itemId;
    int quantity;
    int rushStatus;

public:
    Order();
    Order(int num, const string& id, int qty, int rush);

    int getOrderNumber() const { return orderNumber; }
    string getItemId() const { return itemId; }
    int getQuantity() const { return quantity; }
    int getRushStatus() const { return rushStatus; }

    void setOrderNumber(int num) { orderNumber = num; }
    void setItemId(const string& id) { itemId = id; }
    void setQuantity(int qty) { quantity = qty; }
    void setRushStatus(int rush) { rushStatus = rush; }

    bool operator<(const Order& other) const;

    void display(const RushInfo& rushInfo) const;
};


class ProcessedOrder
{
private:
    int orderNumber;
    string itemName;
    int rushStatus;
    int quantity;
    double markupPercentage;
    double costToWarehouse;
    double markup;
    double costToCustomer;

public:
    ProcessedOrder();
    ProcessedOrder(int num, const string& name, int rush, int qty,
        double markupPct, double cost);

    int getOrderNumber() const { return orderNumber; }
    string getItemName() const { return itemName; }
    int getRushStatus() const { return rushStatus; }
    int getQuantity() const { return quantity; }
    double getMarkupPercentage() const { return markupPercentage; }
    double getCostToWarehouse() const { return costToWarehouse; }
    double getMarkup() const { return markup; }
    double getCostToCustomer() const { return costToCustomer; }

    void display(const RushInfo& rushInfo) const;
};

class BackOrder
{
private:
    int orderNumber;
    string itemName;
    int rushStatus;
    int quantity;

public:
    BackOrder();
    BackOrder(int num, const string& name, int rush, int qty);

    void display(const RushInfo& rushInfo) const;
};

class Heap
{
private:
    static const int MAX_SIZE = 100;
    Order data[MAX_SIZE];
    int size;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void siftUp(int i);
    void siftDown(int i);

public:
    Heap();
    bool isEmpty() const { return size == 0; }
    int getSize() const { return size; }
    void insert(const Order& order);
    Order extractMax();
    void display(const RushInfo& rushInfo) const;
};

class PriorityQueue
{
private:
    Heap heap;

public:
    void enqueue(const Order& order);
    Order dequeue();

    void display(const RushInfo& rushInfo) const;

    bool isEmpty() const { return heap.isEmpty(); }
    int getSize() const { return heap.getSize(); }
};

class OrderProcessor
{
private:
    Inventory inventory;
    PriorityQueue orderQueue;
    RushInfo rushInfo;

    static const int MAX_PROCESSED = 100;
    ProcessedOrder processedOrders[MAX_PROCESSED];
    int processedCount;

    static const int MAX_BACKORDERS = 100;
    BackOrder backOrders[MAX_BACKORDERS];
    int backOrderCount;

public:
    OrderProcessor();
    bool loadInventory(const string& filename);
    bool loadOrders(const string& filename);
    void processOrders();

    void displayInventory() const;
    void displayOrders() const;
    void displayProcessedOrders() const;
    void displayBackOrders() const;

    int getSize() const;
};


void displayMenu();
void clearInputError(string& error);
bool initializeSystem(OrderProcessor* pProcessor);

int main()
{
    int choice = 0;
    int returnValue = 0;
    OrderProcessor* pProcessor = new OrderProcessor();
    bool systemInitialized = initializeSystem(pProcessor);

    // Only proceeds when both inventory and orders were loaded successfully
    if (systemInitialized)
    {
        do
        {
            displayMenu();
            cin >> choice;

            switch (choice)
            {
            case DISPLAY_INVENTORY:
                pProcessor->displayInventory();
                break;
            case UNPROCESSED_ORDERS:
                pProcessor->displayOrders();
                break;
            case PROCESS_ORDERS:
                pProcessor->processOrders();
                break;
            case EXIT:
                cout << "Now exiting program...\n";
                break;
            default:
                string error = "Invalid option. Please enter an option between 1 and 4 \n";
                clearInputError(error);
                break;
            }
        } while (choice != EXIT);
    }
    else
    {
       returnValue = 1;
       cout << "Failed to intiailize system. Now exiting program...\n";
    }

    delete pProcessor;
    return returnValue;
}

void displayMenu()
{
    cout << "\n\n=== Main Menu ===\n"
        "1. Display Inventory\n"
        "2. Display Unprocessed Orders\n"
        "3. Process Orders\n"
        "4. Exit\n"
        "Enter your choice: ";
}

void clearInputError(string& error)
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << error;
}

bool initializeSystem(OrderProcessor* pProcessor)
{
    bool success = true;

    cout << "=== Order Processing System ===\n"
        "Loading inventory from " << IN_INVENTORY << "\n";

    bool inventoryLoaded = pProcessor->loadInventory(IN_INVENTORY);
    if (!inventoryLoaded)
    {
        cout << "Failed to load inventory\n";
        success = false;
    }

    if (success)
    {
        cout << "Loading orders from " << IN_ORDERS << "\n";
        bool ordersLoaded = pProcessor->loadOrders(IN_ORDERS);
        if (!ordersLoaded)
        {
            cout << "Failed to load orders\n";
            success = false;
        }
    }

    if (success)
    {
        cout << "System initialized successfully\n";
    }

    return success;
}

string RushInfo::getRushName(int rushCode) const
{
    return (rushCode >= 1 && rushCode <= 3) ? RUSH_NAMES[rushCode] : "Unknown";
}

double RushInfo::getMarkupPercentage(int rushCode) const
{
    return (rushCode >= 1 && rushCode <= 3) ? MARKUP_PERCENTAGES[rushCode] : 0.0;
}

Item::Item()
{
    quantity = 0;
    price = 0.0;
}

Item::Item(const string& id, const string& name, int qty, double p)
{
    itemId = id;
    itemName = name;
    quantity = qty;
    price = p;
}

bool Item::reduceQuantity(int qty)
{
    bool success = false;
    if (quantity >= qty)
    {
        quantity -= qty;
        success = true;
    }
    return success;
}

void Item::display() const
{
    cout << left << setw(10) << itemId
        << left << setw(20) << itemName
        << right << setw(6) << quantity
        << fixed << setprecision(2) << right << setw(10) << price << "\n";
}

Inventory::Inventory()
{
    itemCount = 0;
}

bool Inventory::loadFromFile(const string& filename)
{
    bool success = false;
    ifstream file(filename);
    if (file)
    {
        string itemId, itemName;
        int quantity;
        double price;

        itemCount = 0;
        while (file >> itemId >> itemName >> quantity >> price && itemCount < MAX_ITEMS)
        {
            items[itemCount].setItemId(itemId);
            items[itemCount].setItemName(itemName);
            items[itemCount].setQuantity(quantity);
            items[itemCount].setPrice(price);
            itemCount++;
        }

        file.close();
        success = true;
    }
    else
    {
        cout << "Error: Cannot open inventory file\n";
    }
    return success;
}

Item* Inventory::findItem(const string& itemId)
{
    Item* result = nullptr;
    for (int i = 0; i < itemCount; i++)
    {
        if (items[i].getItemId() == itemId)
        {
            result = &items[i];
        }
    }
    return result;
}

void Inventory::display() const
{
    cout << "\n===== Inventory =====\n"
        << left << setw(10) << "Item ID"
        << left << setw(20) << "Item Name"
        << right << setw(6) << "Qty"
        << right << setw(10) << "Price" << "\n";

    cout << string(46, '-') << "\n";

    for (int i = 0; i < itemCount; i++)
    {
        items[i].display();
    }
    cout << "\n";
}

Order::Order()
{
    orderNumber = 0;
    quantity = 0;
    rushStatus = 0;
}

Order::Order(int num, const string& id, int qty, int rush)
{
    orderNumber = num;
    itemId = id;
    quantity = qty;
    rushStatus = rush;
}

bool Order::operator<(const Order& other) const
{
    return rushStatus > other.rushStatus;
}

void Order::display(const RushInfo& rushInfo) const
{
    cout << left << setw(8) << orderNumber
        << left << setw(10) << itemId
        << right << setw(6) << quantity
        << "  " << left << setw(10) << rushInfo.getRushName(rushStatus) << "\n";
}

ProcessedOrder::ProcessedOrder()
{
    orderNumber = 0;
    rushStatus = 0;
    quantity = 0;
    markupPercentage = 0.0;
    costToWarehouse = 0.0;
    markup = 0.0;
    costToCustomer = 0.0;
}

ProcessedOrder::ProcessedOrder(int num, const string& name, int rush, int qty,
    double markupPct, double cost)
{
    orderNumber = num;
    itemName = name;
    rushStatus = rush;
    quantity = qty;
    markupPercentage = markupPct;
    costToWarehouse = cost;
    markup = costToWarehouse * markupPercentage;
    costToCustomer = costToWarehouse + markup;
}

void ProcessedOrder::display(const RushInfo& rushInfo) const
{
    cout << left << setw(8) << orderNumber
        << left << setw(20) << itemName
        << left << setw(10) << rushInfo.getRushName(rushStatus)
        << right << setw(6) << quantity
        << right << setw(8) << fixed << setprecision(0) << markupPercentage * 100 << "%"
        << right << setw(10) << fixed << setprecision(2) << markup
        << right << setw(12) << fixed << setprecision(2) << costToCustomer
        << right << setw(12) << fixed << setprecision(2) << costToWarehouse << "\n";
}

BackOrder::BackOrder()
{
    orderNumber = 0;
    rushStatus = 0;
    quantity = 0;
}

BackOrder::BackOrder(int num, const string& name, int rush, int qty)
{
    orderNumber = num;
    itemName = name;
    rushStatus = rush;
    quantity = qty;
}

void BackOrder::display(const RushInfo& rushInfo) const
{
    cout << left << setw(8) << orderNumber
        << left << setw(20) << itemName
        << left << setw(10) << rushInfo.getRushName(rushStatus)
        << right << setw(6) << quantity << "\n";
}

void Heap::siftUp(int i)
{
    int current = i;
    while (current > 0 && data[parent(current)] < data[current])
    {
        swap(data[current], data[parent(current)]);
        current = parent(current);
    }
}

void Heap::siftDown(int i)
{
    int maxIndex = i;
    int l = leftChild(i);

    if (l < size && !(data[l] < data[maxIndex]))
    {
        maxIndex = l;
    }

    int r = rightChild(i);

    if (r < size && !(data[r] < data[maxIndex]))
    {
        maxIndex = r;
    }

    if (i != maxIndex)
    {
        swap(data[i], data[maxIndex]);
        siftDown(maxIndex);
    }
}

Heap::Heap()
{
    size = 0;
}

void Heap::insert(const Order& order)
{
    bool success = true;
    if (size == MAX_SIZE)
    {
        cout << "Heap is full\n";
        success = false;
    }

    if (success)
    {
        data[size] = order;
        siftUp(size);
        size++;
    }
}

Order Heap::extractMax()
{
    Order result = data[0];
    data[0] = data[size - 1];
    size--;
    if (size > 0)
    {
        siftDown(0);
    }
    return result;
}

void Heap::display(const RushInfo& rushInfo) const
{
    cout << "\n===== Orders in Queue =====\n"
        << left << setw(8) << "Order #"
        << left << setw(10) << "Item ID"
        << right << setw(6) << "Qty"
        << "  " << left << setw(10) << "Status" << "\n";
    cout << string(38, '-') << "\n";

    for (int i = 0; i < size; i++)
    {
        data[i].display(rushInfo);
    }
    cout << "\n";
}

void PriorityQueue::enqueue(const Order& order)
{
    heap.insert(order);
}

Order PriorityQueue::dequeue()
{
    return heap.extractMax();
}

void PriorityQueue::display(const RushInfo& rushInfo) const
{
    heap.display(rushInfo);
}

OrderProcessor::OrderProcessor()
{
    processedCount = 0;
    backOrderCount = 0;
}

bool OrderProcessor::loadInventory(const string& filename)
{
    return inventory.loadFromFile(filename);
}

bool OrderProcessor::loadOrders(const string& filename)
{
    bool success = false;
    ifstream file(filename);
    if (file)
    {
        string itemId;
        int orderNumber, quantity, rushStatus;

        while (file >> itemId >> quantity >> rushStatus)
        {
            orderNumber = getSize() + 1;
            Order order(orderNumber, itemId, quantity, rushStatus);
            orderQueue.enqueue(order);
        }

        file.close();
        success = true;
    }
    else
    {
        cout << "Error: Cannot open orders file\n";
    }
    return success;
}

void OrderProcessor::processOrders()
{
    // These two lines are kept for easy testing. Without them the backorder file will always contain the content from previous runs.
    // These are kept because the intended design would likely prefer to keep track of backorders, but I left the option
    // to use either implementation in case the other method is preferred
    
    ofstream clearFile(OUT_BACKORDER, ios::trunc);
    clearFile.close();

    double totalCostToWarehouse = 0.0;
    double totalMarkup = 0.0;
    double totalCostToCustomer = 0.0;

    cout << "\nProcessing orders...\n";

    while (!orderQueue.isEmpty())
    {
        Order order = orderQueue.dequeue();

        Item* item = inventory.findItem(order.getItemId());

        if (item != nullptr)
        {
            if (item->getQuantity() >= order.getQuantity())
            {
                double markupPct = rushInfo.getMarkupPercentage(order.getRushStatus());
                double costToWarehouse = item->getPrice() * order.getQuantity();

                processedOrders[processedCount] = ProcessedOrder(
                    order.getOrderNumber(),
                    item->getItemName(),
                    order.getRushStatus(),
                    order.getQuantity(),
                    markupPct,
                    costToWarehouse
                );

                totalCostToWarehouse += costToWarehouse;
                totalMarkup += processedOrders[processedCount].getMarkup();
                totalCostToCustomer += processedOrders[processedCount].getCostToCustomer();

                processedCount++;

                // Update inventory
                item->reduceQuantity(order.getQuantity());
            }
            else
            {
                
                ofstream backOrderFile(OUT_BACKORDER, ios::app); // Append so it doens't overwrite previous entries
                if (backOrderFile)
                {
                    backOrderFile << order.getOrderNumber() << " "
                        << order.getItemId() << " "
                        << item->getItemName() << " "
                        << order.getQuantity() << " "
                        << order.getRushStatus() << "\n";
                    backOrderFile.close();
                    backOrderCount++;
                }
            }
        }
        else
        {
            cout << "Error: Item " << order.getItemId() << " not found in inventory\n";
        }
    }
    displayProcessedOrders();
    displayBackOrders();
}

void OrderProcessor::displayInventory() const
{
    inventory.display();
}

void OrderProcessor::displayOrders() const
{
    orderQueue.display(rushInfo);
}

void OrderProcessor::displayProcessedOrders() const
{
    cout << "\n===== Processed Orders =====\n"
        << left << setw(8) << "Order #"
        << left << setw(20) << "Item Name"
        << left << setw(10) << "Status"
        << right << setw(6) << "Qty"
        << right << setw(8) << "Markup"
        << right << setw(10) << "Markup($)"
        << right << setw(12) << "Cust. Cost"
        << right << setw(12) << "WH Cost" << "\n";

    cout << string(86, '-') << "\n";

    double totalCostToWarehouse = 0.0;
    double totalMarkup = 0.0;
    double totalCostToCustomer = 0.0;

    for (int i = 0; i < processedCount; i++)
    {
        processedOrders[i].display(rushInfo);
        totalCostToWarehouse += processedOrders[i].getCostToWarehouse();
        totalMarkup += processedOrders[i].getMarkup();
        totalCostToCustomer += processedOrders[i].getCostToCustomer();
    }

    cout << "\n===== Processing Summary =====\n"
        "Total orders processed: " << processedCount << "\n"
        "Total cost to warehouse: $" << fixed << setprecision(2) << totalCostToWarehouse << "\n"
        "Total markup: $" << fixed << setprecision(2) << totalMarkup << "\n"
        "Total cost to customers: $" << fixed << setprecision(2) << totalCostToCustomer << "\n";
}

void OrderProcessor::displayBackOrders() const
{
    cout << "\n===== Back Orders =====\n"
        << left << setw(8) << "Order #"
        << left << setw(20) << "Item Name"
        << left << setw(10) << "Status"
        << right << setw(6) << "Qty" << "\n";
    cout << string(44, '-') << "\n";

    ifstream backOrderFile(OUT_BACKORDER);
    if (backOrderFile)
    {
        int orderNum, qty, rushStatus;
        string itemId, itemName;

        while (backOrderFile >> orderNum >> itemId >> itemName >> qty >> rushStatus)
        {
            cout << left << setw(8) << orderNum
                << left << setw(20) << itemName
                << left << setw(10) << rushInfo.getRushName(rushStatus)
                << right << setw(6) << qty << "\n";
        }
        backOrderFile.close();
    }
    cout << "\n";
}

int OrderProcessor::getSize() const
{
    return orderQueue.getSize() + processedCount + backOrderCount;
}

/*
=== Order Processing System ===
Loading inventory from in_inventory.txt
Loading orders from in_orders.txt
System initialized successfully


=== Main Menu ===
1. Display Inventory
2. Display Unprocessed Orders
3. Process Orders
4. Exit
Enter your choice: g
Invalid option. Please enter an option between 1 and 4


=== Main Menu ===
1. Display Inventory
2. Display Unprocessed Orders
3. Process Orders
4. Exit
Enter your choice: 1

===== Inventory =====
Item ID   Item Name              Qty     Price
----------------------------------------------
AS34DF    Stroller                25    138.79
BC56GH    Crib                    15    249.99
DE78IJ    Highchair               30     89.95
FG90KL    CarSeat                 20    179.50
HI12MN    BabyMonitor             40     65.25



=== Main Menu ===
1. Display Inventory
2. Display Unprocessed Orders
3. Process Orders
4. Exit
Enter your choice: 2

===== Orders in Queue =====
Order # Item ID      Qty  Status
--------------------------------------
2       BC56GH         2  extreme
4       FG90KL         3  extreme
3       DE78IJ         8  expedite
8       DE78IJ        25  extreme
10      HI12MN         5  expedite
6       AS34DF        10  expedite
7       BC56GH         5  standard
1       AS34DF         5  standard
9       FG90KL        30  standard
5       HI12MN        12  standard



=== Main Menu ===
1. Display Inventory
2. Display Unprocessed Orders
3. Process Orders
4. Exit
Enter your choice: 3

Processing orders...

===== Processed Orders =====
Order # Item Name           Status       Qty  Markup Markup($)  Cust. Cost     WH Cost
--------------------------------------------------------------------------------------
2       Crib                extreme        2      68%    339.99      839.97      499.98
4       CarSeat             extreme        3      68%    366.18      904.68      538.50
8       Highchair           extreme       25      68%   1529.15     3777.90     2248.75
6       Stroller            expedite      10      45%    624.55     2012.45     1387.90
10      BabyMonitor         expedite       5      45%    146.81      473.06      326.25
1       Stroller            standard       5      38%    263.70      957.65      693.95
7       Crib                standard       5      38%    474.98     1724.93     1249.95
5       BabyMonitor         standard      12      38%    297.54     1080.54      783.00

===== Processing Summary =====
Total orders processed: 8
Total cost to warehouse: $7728.28
Total markup: $4042.91
Total cost to customers: $11771.19

===== Back Orders =====
Order # Item Name           Status       Qty
--------------------------------------------
3       Highchair           expedite       8
9       CarSeat             standard      30



=== Main Menu ===
1. Display Inventory
2. Display Unprocessed Orders
3. Process Orders
4. Exit
Enter your choice: 4
Now exiting program...

=== Order Processing System ===
Loading inventory from MISS_INV_EXAMPLE.txt
Error: Cannot open inventory file
Failed to load inventory
Failed to intiailize system. Now exiting program...
*/
