#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>

using namespace std;

const int MAX_SLOTS = 10;
const int HASH_SIZE = 20;

struct Ticket {
    int    ticketID;
    string plateNumber;
    string vehicleType;
    int    slotNumber;
    string timeIn;
    string timeOut;
    bool   isParked;
    Ticket* next;
};

struct HashEntry {
    string plateNumber;
    int    slotNumber;
    bool   isUsed;
};

struct Vehicle {
    string plateNumber;
    string vehicleType;
    string arrivalTime;
};

struct ParkingSlot {
    int    slotNumber;
    bool   isOccupied;
    string plateNumber;
};

vector<ParkingSlot> parkingSlots;   
queue<Vehicle>      arrivalQueue;   
Ticket*             ticketHead = NULL; 
int                 nextTicketID = 1001;
HashEntry           hashTable[HASH_SIZE];

string getTime() {
    time_t now = time(0);
    tm* t = localtime(&now);
    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return string(buf);
}

void printLine() {
    cout << "  ======================================================" << endl;
}
void printThin() {
    cout << "  ------------------------------------------------------" << endl;
}

int hashFunction(string plate) {
    int sum = 0;
    for (int i = 0; i < (int)plate.length(); i++)
        sum += (int)plate[i];
    return sum % HASH_SIZE;
}

void hashInsert(string plate, int slot) {
    int index = hashFunction(plate);
    int start = index;
    while (hashTable[index].isUsed) {
        index = (index + 1) % HASH_SIZE;
        if (index == start) return;
    }
    hashTable[index].plateNumber = plate;
    hashTable[index].slotNumber  = slot;
    hashTable[index].isUsed      = true;
}

int hashSearch(string plate) {
    int index = hashFunction(plate);
    int start = index;
    while (hashTable[index].isUsed) {
        if (hashTable[index].plateNumber == plate)
            return hashTable[index].slotNumber;
        index = (index + 1) % HASH_SIZE;
        if (index == start) break;
    }
    return -1;
}

void hashRemove(string plate) {
    int index = hashFunction(plate);
    int start = index;
    while (hashTable[index].isUsed) {
        if (hashTable[index].plateNumber == plate) {
            hashTable[index].plateNumber = "";
            hashTable[index].slotNumber  = 0;
            hashTable[index].isUsed      = false;
            return;
        }
        index = (index + 1) % HASH_SIZE;
        if (index == start) break;
    }
}

void addTicket(string plate, string type, int slot, string timeIn) {
    Ticket* newTicket    = new Ticket;
    newTicket->ticketID  = nextTicketID++;
    newTicket->plateNumber = plate;
    newTicket->vehicleType = type;
    newTicket->slotNumber  = slot;
    newTicket->timeIn      = timeIn;
    newTicket->timeOut     = "";
    newTicket->isParked    = true;
    newTicket->next        = ticketHead;
    ticketHead             = newTicket;

    cout << endl;
    printLine();
    cout << "         * PARKTRACK PARKING TICKET *" << endl;
    printLine();
    cout << "  Ticket ID    : TKT-" << newTicket->ticketID << endl;
    cout << "  Plate No.    : " << newTicket->plateNumber << endl;
    cout << "  Vehicle Type : " << newTicket->vehicleType << endl;
    cout << "  Slot Assigned: SLOT " << newTicket->slotNumber << endl;
    cout << "  Time In      : " << newTicket->timeIn << endl;
    printThin();
    cout << "    Keep this ticket. Present upon exit." << endl;
    printLine();
    cout << endl;
}

Ticket* findTicketByPlate(string plate) {
    Ticket* current = ticketHead;
    while (current != NULL) {
        if (current->plateNumber == plate && current->isParked)
            return current;
        current = current->next;
    }
    return NULL;
}

void printAllTickets() {
    if (ticketHead == NULL) {
        cout << "  No tickets issued yet." << endl;
        return;
    }
    cout << "  TKT-ID    PLATE         TYPE          SLOT   STATUS" << endl;
    printThin();
    Ticket* current = ticketHead;
    while (current != NULL) {
        cout << "  TKT-" << current->ticketID;
        cout << "    " << current->plateNumber;
        int p1 = 13 - (int)current->plateNumber.length();
        for (int i = 0; i < p1; i++) cout << " ";
        cout << current->vehicleType;
        int p2 = 13 - (int)current->vehicleType.length();
        for (int i = 0; i < p2; i++) cout << " ";
        cout << current->slotNumber;
        cout << "      ";
        cout << (current->isParked ? "PARKED" : "EXITED") << endl;
        current = current->next;
    }
}

void initSlots() {
    parkingSlots.clear();
    for (int i = 1; i <= MAX_SLOTS; i++) {
        ParkingSlot s;
        s.slotNumber  = i;
        s.isOccupied  = false;
        s.plateNumber = "";
        parkingSlots.push_back(s);
    }
}

int findFreeSlot() {
    for (int i = 0; i < (int)parkingSlots.size(); i++) {
        if (!parkingSlots[i].isOccupied)
            return parkingSlots[i].slotNumber;
    }
    return -1;
}

void occupySlot(int slotNum, string plate) {
    parkingSlots[slotNum - 1].isOccupied  = true;
    parkingSlots[slotNum - 1].plateNumber = plate;
}

void freeSlot(int slotNum) {
    parkingSlots[slotNum - 1].isOccupied  = false;
    parkingSlots[slotNum - 1].plateNumber = "";
}

void displaySlots() {
    cout << endl;
    cout << "  -- PARKING SLOT STATUS --" << endl;
    printLine();
    int freeCount = 0;
    for (int i = 0; i < (int)parkingSlots.size(); i++) {
        cout << "  Slot [" << parkingSlots[i].slotNumber << "]   ";
        if (parkingSlots[i].isOccupied)
            cout << "[OCCUPIED]   Plate: " << parkingSlots[i].plateNumber << endl;
        else {
            cout << "[ FREE ]" << endl;
            freeCount++;
        }
    }
    printThin();
    cout << "  Available: " << freeCount
         << "  |  Occupied: " << (MAX_SLOTS - freeCount)
         << "  |  Total: " << MAX_SLOTS << endl;
    printLine();
}

void processQueue();

void enqueueVehicle() {
    string plate;
    int choice;

    cout << endl;
    cout << "  -- VEHICLE ARRIVAL --" << endl;
    cout << "  Enter plate number   : ";
    cin  >> plate;

    for (int i = 0; i < (int)plate.length(); i++)
        plate[i] = toupper(plate[i]);

    if (hashSearch(plate) != -1) {
        cout << "  [!] " << plate << " is already parked at Slot "
             << hashSearch(plate) << "." << endl;
        return;
    }

    cout << "  Select vehicle type  :" << endl;
    cout << "    1 - Car" << endl;
    cout << "    2 - Motorcycle" << endl;
    cout << "    3 - Truck" << endl;
    cout << "  Choice: ";
    cin  >> choice;

    string vehicleType;
    if      (choice == 1) vehicleType = "Car";
    else if (choice == 2) vehicleType = "Motorcycle";
    else if (choice == 3) vehicleType = "Truck";
    else                  vehicleType = "Unknown";

    Vehicle v;
    v.plateNumber = plate;
    v.vehicleType = vehicleType;
    v.arrivalTime = getTime();

    arrivalQueue.push(v);
    cout << "  [+] " << plate << " (" << vehicleType << ") added to arrival queue." << endl;

    processQueue();
}

void processQueue() {
    if (arrivalQueue.empty()) {
        cout << "  [i] No vehicles in the arrival queue." << endl;
        return;
    }

    cout << "  Processing arrival queue..." << endl;

    while (!arrivalQueue.empty()) {
        int slot = findFreeSlot();
        if (slot == -1) {
            cout << "  [!] Parking lot is FULL. "
                 << arrivalQueue.size() << " vehicle(s) still waiting." << endl;
            break;
        }
        Vehicle v = arrivalQueue.front();
        arrivalQueue.pop();

        occupySlot(slot, v.plateNumber);
        hashInsert(v.plateNumber, slot);
        addTicket(v.plateNumber, v.vehicleType, slot, v.arrivalTime);
    }
}

void vehicleExit() {
    string plate;
    cout << endl;
    cout << "  -- VEHICLE EXIT --" << endl;
    cout << "  Enter plate number : ";
    cin  >> plate;

    for (int i = 0; i < (int)plate.length(); i++)
        plate[i] = toupper(plate[i]);

    int slot = hashSearch(plate);
    if (slot == -1) {
        cout << "  [!] No parking record found for: " << plate << endl;
        return;
    }

    Ticket* t = findTicketByPlate(plate);
    string timeOut = getTime();
    if (t != NULL) {
        t->isParked = false;
        t->timeOut  = timeOut;
    }

    freeSlot(slot);
    hashRemove(plate);

    cout << endl;
    printLine();
    cout << "          * PARKTRACK EXIT RECEIPT *" << endl;
    printLine();
    if (t != NULL) {
        cout << "  Ticket ID    : TKT-" << t->ticketID << endl;
        cout << "  Plate No.    : " << t->plateNumber << endl;
        cout << "  Vehicle Type : " << t->vehicleType << endl;
        cout << "  Slot Number  : " << t->slotNumber << endl;
        cout << "  Time In      : " << t->timeIn << endl;
        cout << "  Time Out     : " << t->timeOut << endl;
    }
    printThin();
    cout << "      Thank you for using ParkTrack!" << endl;
    printLine();
    cout << endl;

    if (!arrivalQueue.empty()) {
        cout << "  [i] Processing waiting vehicles..." << endl;
        processQueue();
    }
}

void searchVehicle() {
    string plate;
    cout << endl;
    cout << "  -- SEARCH VEHICLE --" << endl;
    cout << "  Enter plate number : ";
    cin  >> plate;

    for (int i = 0; i < (int)plate.length(); i++)
        plate[i] = toupper(plate[i]);

    int slot = hashSearch(plate);
    if (slot == -1) {
        cout << "  [!] No active record found for: " << plate << endl;
        return;
    }

    Ticket* t = findTicketByPlate(plate);
    printLine();
    cout << "  VEHICLE FOUND" << endl;
    cout << "  Plate No.    : " << plate << endl;
    cout << "  Slot Number  : " << slot << endl;
    if (t != NULL) {
        cout << "  Vehicle Type : " << t->vehicleType << endl;
        cout << "  Ticket ID    : TKT-" << t->ticketID << endl;
        cout << "  Time In      : " << t->timeIn << endl;
    }
    printLine();
}

// =============================================================
//  OPERATION: Update / Reassign Slot
// =============================================================
void updateSlot() {
    string plate;
    int newSlot;

    cout << endl;
    cout << "  -- UPDATE / REASSIGN SLOT --" << endl;
    cout << "  Enter plate number to reassign : ";
    cin  >> plate;

    for (int i = 0; i < (int)plate.length(); i++)
        plate[i] = toupper(plate[i]);

    int currentSlot = hashSearch(plate);
    if (currentSlot == -1) {
        cout << "  [!] No active record for: " << plate << endl;
        return;
    }

    cout << "  Current slot : " << currentSlot << endl;
    cout << "  Enter new slot number (1-" << MAX_SLOTS << ") : ";
    cin  >> newSlot;

    if (newSlot < 1 || newSlot > MAX_SLOTS) {
        cout << "  [!] Invalid slot number." << endl;
        return;
    }

    if (parkingSlots[newSlot - 1].isOccupied) {
        cout << "  [!] Slot " << newSlot << " is already taken by "
             << parkingSlots[newSlot - 1].plateNumber << "." << endl;
        return;
    }

    freeSlot(currentSlot);
    occupySlot(newSlot, plate);
    hashRemove(plate);
    hashInsert(plate, newSlot);

    Ticket* t = findTicketByPlate(plate);
    if (t != NULL) t->slotNumber = newSlot;

    cout << "  [+] " << plate << " reassigned from Slot "
         << currentSlot << " to Slot " << newSlot << "." << endl;
}

void viewTickets() {
    cout << endl;
    cout << "  -- TICKET RECORDS (Linked List) --" << endl;
    printThin();
    printAllTickets();
    printLine();
}

void viewQueue() {
    if (arrivalQueue.empty()) {
        cout << "  [i] Arrival queue is empty." << endl;
        return;
    }
    queue<Vehicle> temp = arrivalQueue;
    int pos = 1;
    cout << endl;
    cout << "  -- VEHICLES WAITING IN QUEUE --" << endl;
    printThin();
    while (!temp.empty()) {
        Vehicle v = temp.front();
        temp.pop();
        cout << "  #" << pos++ << "  " << v.plateNumber
             << "  (" << v.vehicleType << ")  " << v.arrivalTime << endl;
    }
    printLine();
}

void systemSummary() {
    int occupied = 0;
    for (int i = 0; i < (int)parkingSlots.size(); i++)
        if (parkingSlots[i].isOccupied) occupied++;

    int totalTickets = 0, activeTickets = 0;
    Ticket* cur = ticketHead;
    while (cur != NULL) {
        totalTickets++;
        if (cur->isParked) activeTickets++;
        cur = cur->next;
    }

    cout << endl;
    printLine();
    cout << "           PARKTRACK SYSTEM SUMMARY" << endl;
    printLine();
    cout << "  Total Slots    : " << MAX_SLOTS << endl;
    cout << "  Occupied       : " << occupied << endl;
    cout << "  Available      : " << (MAX_SLOTS - occupied) << endl;
    cout << "  Queue Size     : " << arrivalQueue.size() << endl;
    cout << "  Tickets Issued : " << totalTickets << endl;
    cout << "  Active Tickets : " << activeTickets << endl;
    printLine();
}

int main() {

    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].plateNumber = "";
        hashTable[i].slotNumber  = 0;
        hashTable[i].isUsed      = false;
    }
    
    initSlots();

    cout << endl;
    printLine();
    cout << "        __              _     ___                  _    " << endl;
    cout << "       |  _ \\ _ _  _ __| | __|   |__ _  ___  ____ | | __" << endl;
    cout << "       | |_) / _  | '__| |/ / | || '__/ _  ||  __|| |/ /" << endl;
    cout << "       |  _/ (_|  | |  |   <  | || | | (_| || |_  |   < " << endl;
    cout << "       |_|   \\__,_|_|  |_|\\_\\ |_||_|  \\__,_||____||_|\\_\\" << endl;
    cout << endl;
    cout << "         A Parking Management System  v1.0" << endl;
    printLine();
    cout << endl;

    int choice;

    do {
        cout << "  MAIN MENU" << endl;
        printThin();
        cout << "  [1] Vehicle Arrival" << endl;
        cout << "  [2] Process Arrival Queue" << endl;
        cout << "  [3] Vehicle Exit" << endl;
        cout << "  [4] View Parking Slots" << endl;
        cout << "  [5] Search Vehicle" << endl;
        cout << "  [6] Update / Reassign Slot" << endl;
        cout << "  [7] View All Ticket Records" << endl;
        cout << "  [8] View Arrival Queue" << endl;
        cout << "  [9] System Summary" << endl;
        cout << "  [0] Exit" << endl;
        printThin();
        cout << "  Choice: ";
        cin  >> choice;
        cout << endl;

        if      (choice == 1) enqueueVehicle();
        else if (choice == 2) processQueue();
        else if (choice == 3) vehicleExit();
        else if (choice == 4) displaySlots();
        else if (choice == 5) searchVehicle();
        else if (choice == 6) updateSlot();
        else if (choice == 7) viewTickets();
        else if (choice == 8) viewQueue();
        else if (choice == 9) systemSummary();
        else if (choice == 0) cout << "  Exiting ParkTrack. Goodbye!" << endl;
        else                  cout << "  [!] Invalid choice. Please try again." << endl;

        cout << endl;

    } while (choice != 0);

    return 0;
}
