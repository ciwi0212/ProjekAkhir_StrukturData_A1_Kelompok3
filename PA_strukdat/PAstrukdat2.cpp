#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;

const int max_tugas = 100;

struct tugas {
    string id;
    string judul;
    string deskripsi;
    string matkul;
    enum Level { penting_mendesak = 1, penting_tidak_mendesak = 2, tidak_penting_mendesak = 3, tidak_penting_tidak_mendesak = 4 } skala_prioritas;
};

tugas arrtugas[max_tugas];
int jmlh_tugas = 0;

struct node {
    tugas tugasharian;
    node *next;
};

struct user {
    string username;
    string password;
    node *head;
    node* stack = nullptr;
    node* queue = nullptr;
    int counter = 1;
};

map<string, user> users;
user* currentuser = nullptr;

string generateID(user* currentuser) {
    string prefix = "CQ";
    stringstream ss;
    ss << prefix << setw(3) << setfill('0') << currentuser->counter; 
    currentuser->counter++; 
    return ss.str();
}

void merge(tugas arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<tugas> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].id >= R[j].id) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(tugas arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int partition(tugas arr[], int low, int high) {
    int pivot = arr[high].skala_prioritas;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j].skala_prioritas < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(tugas arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

vector<tugas> fibonacciSearch(tugas arr[], int n, tugas::Level skalaPrioritas) {
    int fibMMm2 = 0;
    int fibMMm1 = 1;
    int fibM = fibMMm2 + fibMMm1;

    while (fibM < n) {
        fibMMm2 = fibMMm1;
        fibMMm1 = fibM;
        fibM = fibMMm2 + fibMMm1;
    }

    int offset = -1;
    vector<tugas> foundData;
    
    while (fibM > 1) {
        int i = min(offset + fibMMm2, n - 1);

        if (arr[i].skala_prioritas < skalaPrioritas) {
            fibM = fibMMm1;
            fibMMm1 = fibMMm2;
            fibMMm2 = fibM - fibMMm1;
            offset = i;
        } else if (arr[i].skala_prioritas > skalaPrioritas) {
            fibM = fibMMm2;
            fibMMm2 = fibMMm1 - fibMMm2;
            fibMMm1 = fibM - fibMMm2;
        } else {
            bool alreadyExists = false;
            for (const auto& tugasFound : foundData) {
                if (tugasFound.id == arr[i].id) {
                    alreadyExists = true;
                    break;
                }
            }
            if (!alreadyExists) {
                foundData.push_back(arr[i]);
            }

            if (fibMMm1 > 1) {
                fibM = fibMMm1;
                fibMMm1 = fibMMm2;
                fibMMm2 = fibM - fibMMm1;
            } else {
                break;
            }
        }
    }

    if (fibMMm1 && arr[offset + 1].skala_prioritas == skalaPrioritas) {
        bool alreadyExists = false;
        for (const auto& tugasFound : foundData) {
            if (tugasFound.id == arr[offset + 1].id) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists) {
            foundData.push_back(arr[offset + 1]);
        }
    }

    return foundData;
}



vector<tugas> jumpSearch(tugas arr[], int n, const string& id) {
    int step = sqrt(n);
    int prev = 0;

    vector<tugas> foundData;

    while (prev < n && arr[min(step, n) - 1].id > id) {
        prev = step;
        step += sqrt(n);
        if (prev >= n) return foundData;
    }

    for (int i = prev; i < min(step, n); i++) {
        if (arr[i].id == id) {
            foundData.push_back(arr[i]);
        }
    }

    return foundData; 
}

bool boyerMooreSearch(const string& text, const string& pattern) {
    int m = pattern.size();
    int n = text.size();
    vector<int> badChar(256, -1);

    for (int i = 0; i < m; i++) badChar[(int)pattern[i]] = i;

    int shift = 0;
    while (shift <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[shift + j]) j--;

        if (j < 0) return true;
        else shift += max(1, j - badChar[text[shift + j]]);
    }
    return false;
}

node* createNode(const tugas& tugasbaru) {
    node* newNode = new node;
    newNode->tugasharian = tugasbaru;
    newNode->next = nullptr;
    return newNode;
}

void addToLL(node*& head, node* newNode) {
    if (head == nullptr) {
        head = newNode;
    } else {
        node* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void tambahTugas() {
    if (currentuser == nullptr) {
        cout << "Tidak ada pengguna yang login!" << endl;
        return;
    }

    if (jmlh_tugas >= max_tugas) {
        cout << "Daftar tugas penuh!" << endl;
        return;
    }

    tugas tugasbaru;

    cout << "Masukkan Judul tugas: ";
    getline(cin, tugasbaru.judul);
    if (tugasbaru.judul.empty()) {
        cout << "Judul tugas tidak boleh kosong!" << endl;
        return;
    }

    cout << "Masukkan Deskripsi tugas: ";
    getline(cin, tugasbaru.deskripsi);
    if (tugasbaru.deskripsi.empty()) {
        cout << "Deskripsi tugas tidak boleh kosong!" << endl;
        return;
    }

    cout << "Masukkan Mata Kuliah: ";
    getline(cin, tugasbaru.matkul);
    if (tugasbaru.matkul.empty()) {
        cout << "Mata kuliah tidak boleh kosong!" << endl;
        return;
    }

    cout << "Masukkan Level Prioritas (1: Penting Mendesak, 2: Penting Tidak Mendesak, 3: Tidak Penting Mendesak, 4: Tidak Penting Tidak Mendesak): ";
    int level;
    cin >> level;
    cin.ignore();
    if (level < 1 || level > 4) {
        cout << "Input level prioritas tidak valid!" << endl;
        return;
    }
    tugasbaru.skala_prioritas = static_cast<tugas::Level>(level);

    tugasbaru.id = generateID(currentuser);

    arrtugas[jmlh_tugas++] = tugasbaru;

    node* newNode = createNode(tugasbaru);
    addToLL(currentuser->head, newNode);
    node* queueNode = createNode(tugasbaru);
    addToLL(currentuser->queue, queueNode);
    
    cout << "Tugas berhasil ditambahkan!" << endl;
}

void tampilkanSemuaTugas() {
    if (currentuser == nullptr) {
        cout << "Tidak ada pengguna yang login!" << endl;
        return;
    }

    if (currentuser->head == nullptr) {
        cout << "Tidak ada tugas!" << endl;
        return;
    }
    
    node *temp = currentuser->head;

    cout << "\n=== Daftar Tugas yang Belum Selesai ===\n";
    temp = currentuser->queue;
    if (!temp) {
        cout << "Tidak ada tugas yang sedang berlangsung!" << endl;
    } else {
        while (temp) {
            cout << "ID: " << temp->tugasharian.id << "\n"
                << "Judul: " << temp->tugasharian.judul << "\n"
                << "Deskripsi: " << temp->tugasharian.deskripsi << "\n"
                << "Mata Kuliah: " << temp->tugasharian.matkul << "\n"
                << "Prioritas: ";
            switch (temp->tugasharian.skala_prioritas) {
                case tugas::penting_mendesak: cout << "1 - Penting Mendesak"; break;
                case tugas::penting_tidak_mendesak: cout << "2 - Penting Tidak Mendesak"; break;
                case tugas::tidak_penting_mendesak: cout << "3 - Tidak Penting Mendesak"; break;
                case tugas::tidak_penting_tidak_mendesak: cout << "4 - Tidak Penting Tidak Mendesak"; break;
                default: cout << "Prioritas tidak valid"; break;
            }
            cout << endl << endl;
            temp = temp->next;
        }
    }

    cout << "\n=== Daftar Tugas yang Sudah Selesai ===\n";
    temp = currentuser->stack;
    if (!temp) {
        cout << "Belum ada tugas yang diselesaikan!" << endl;
    } else {
        while (temp) {
            cout << "ID: " << temp->tugasharian.id << "\n"
                << ", Judul: " << temp->tugasharian.judul << "\n"
                << ", Deskripsi: " << temp->tugasharian.deskripsi << "\n"
                << ", Mata Kuliah: " << temp->tugasharian.matkul << "\n"
                << ", Prioritas: ";
            switch (temp->tugasharian.skala_prioritas) {
                case tugas::penting_mendesak: cout << "1 - Penting Mendesak"; break;
                case tugas::penting_tidak_mendesak: cout << "2 - Penting Tidak Mendesak"; break;
                case tugas::tidak_penting_mendesak: cout << "3 - Tidak Penting Mendesak"; break;
                case tugas::tidak_penting_tidak_mendesak: cout << "4 - Tidak Penting Tidak Mendesak"; break;
                default: cout << "Prioritas tidak valid"; break;
            }
            cout << endl <<endl;
            temp = temp->next;
        }
    }

    cout << "------------------------------------\n";

    int pilihan_sort, pilihan_search;
    string search_key;

    cout << "Pilih metode sorting:\n";
    cout << "1. Sorting Menurut ID Secara Descending\n";
    cout << "2. Sorting Menurut Skala Prioritas Secara Ascending\n";
    cout << "Pilih opsi: ";

    if (!(cin >> pilihan_sort) || (pilihan_sort != 1 && pilihan_sort != 2)) {
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  
        cout << "Input tidak valid untuk pilihan sorting!" << endl;
        return;
    }

    try {
        if (pilihan_sort == 1) mergeSort(arrtugas, 0, jmlh_tugas - 1);
        else if (pilihan_sort == 2) quickSort(arrtugas, 0, jmlh_tugas - 1);
    } catch (const std::exception &e) {
        cout << "Terjadi kesalahan saat sorting: " << e.what() << endl;
        return;
    }
    cout << "\n=== Daftar Tugas Setelah Sorting ===\n";
    for (int i = 0; i < jmlh_tugas; i++) {
        cout << "ID: " << arrtugas[i].id << "\n"
            << "Judul: " << arrtugas[i].judul << "\n"
            << "Deskripsi: " << arrtugas[i].deskripsi << "\n"
            << "Mata Kuliah: " << arrtugas[i].matkul << "\n"
            << "Prioritas: ";
        switch (arrtugas[i].skala_prioritas) {
            case tugas::penting_mendesak: cout << "1 - Penting Mendesak"; break;
            case tugas::penting_tidak_mendesak: cout << "2 - Penting Tidak Mendesak"; break;
            case tugas::tidak_penting_mendesak: cout << "3 - Tidak Penting Mendesak"; break;
            case tugas::tidak_penting_tidak_mendesak: cout << "4 - Tidak Penting Tidak Mendesak"; break;
            default: cout << "Prioritas tidak valid"; break;
        }
        cout << endl << endl;
    }

    cout << "Pilih metode searching:\n";
    cout << "1. Fibonacci Search (Skala Prioritas)\n";
    cout << "2. Jump Search (ID)\n";
    cout << "3. Boyer-Moore Search (Judul)\n";
    cout << "Pilih opsi: ";
    if (!(cin >> pilihan_search) || (pilihan_search < 1 || pilihan_search > 3)) {
        cin.clear();  
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "Input tidak valid untuk pilihan searching!" << endl;
        
    }

    cin.ignore(); 

    try {
        if (pilihan_search == 1) {
            cout << "Masukkan kata kunci pencarian (skala): ";
   			getline(cin, search_key);
            quickSort(arrtugas, 0, jmlh_tugas - 1);
            tugas::Level skalaPrioritas;
            if (search_key == "penting_mendesak") skalaPrioritas = tugas::penting_mendesak;
            else if (search_key == "penting_tidak_mendesak") skalaPrioritas = tugas::penting_tidak_mendesak;
            else if (search_key == "tidak_penting_mendesak") skalaPrioritas = tugas::tidak_penting_mendesak;
            else if (search_key == "tidak_penting_tidak_mendesak") skalaPrioritas = tugas::tidak_penting_tidak_mendesak;
            else {
                cout << "Skala prioritas tidak valid!" << endl;
            }

            vector<tugas> hasilFibonacci = fibonacciSearch(arrtugas, jmlh_tugas, skalaPrioritas);
            if (hasilFibonacci.empty()) {
                cout << "Tugas dengan skala prioritas tersebut tidak ditemukan!" << endl;
            } else {
                for (auto& t : hasilFibonacci) {
                    cout << "ID: " << t.id 
					<< "\nJudul: " << t.judul 
					<< "\nDeskripsi : " << t.deskripsi
					<< "\nMata Kuliah : " << t.matkul
					<< "\nPrioritas: ";
                    switch (t.skala_prioritas) {
                        case tugas::penting_mendesak: cout << "Penting Mendesak"; break;
                        case tugas::penting_tidak_mendesak: cout << "Penting Tidak Mendesak"; break;
                        case tugas::tidak_penting_mendesak: cout << "Tidak Penting Mendesak"; break;
                        case tugas::tidak_penting_tidak_mendesak: cout << "Tidak Penting Tidak Mendesak"; break;
                        default: cout << "Prioritas tidak valid"; break;
                    }
                    cout << endl <<endl;
                }
            }
        }
        else if (pilihan_search == 2) {
            cout << "Masukkan kata kunci pencarian (ID): ";
    		getline(cin, search_key);
            mergeSort(arrtugas, 0, jmlh_tugas - 1);

            vector<tugas> hasilJump = jumpSearch(arrtugas, jmlh_tugas, search_key);
            if (hasilJump.empty()) {
                cout << "Tugas dengan skala prioritas tersebut tidak ditemukan!" << endl;
            } else {
                for (auto& t : hasilJump) {
                    cout << "ID: " << t.id 
					<< "\nJudul: " << t.judul 
					<< "\nDeskripsi : " << t.deskripsi
					<< "\nMata Kuliah : " << t.matkul
					<< "\nPrioritas: ";
                    switch (t.skala_prioritas) {
                        case tugas::penting_mendesak: cout << "Penting Mendesak"; break;
                        case tugas::penting_tidak_mendesak: cout << "Penting Tidak Mendesak"; break;
                        case tugas::tidak_penting_mendesak: cout << "Tidak Penting Mendesak"; break;
                        case tugas::tidak_penting_tidak_mendesak: cout << "Tidak Penting Tidak Mendesak"; break;
                        default: cout << "Prioritas tidak valid"; break;
                    }
                    cout << endl <<endl;
                }
            }
        }
        else if (pilihan_search == 3) {
            cout << "Masukkan kata kunci pencarian (judul): ";
   			getline(cin, search_key);
            bool ditemukan = false;
            for (int i = 0; i < jmlh_tugas; i++) {
                if (boyerMooreSearch(arrtugas[i].judul, search_key)) {
                    cout << "Ditemukan tugas dengan ID: " << arrtugas[i].id << endl;
                    cout << "Judul: " << arrtugas[i].judul << "\n"
                         << "Deskripsi: " << arrtugas[i].deskripsi << "\n"
                         << "Mata Kuliah: " << arrtugas[i].matkul << "\n"
                         << "Prioritas: ";
                    switch (arrtugas[i].skala_prioritas) {
                        case tugas::penting_mendesak: cout << "1 - Penting Mendesak"; break;
                        case tugas::penting_tidak_mendesak: cout << "2 - Penting Tidak Mendesak"; break;
                        case tugas::tidak_penting_mendesak: cout << "3 - Tidak Penting Mendesak"; break;
                        case tugas::tidak_penting_tidak_mendesak: cout << "4 - Tidak Penting Tidak Mendesak"; break;
                        default: cout << "Prioritas tidak valid"; break;
                    }
                    cout << endl << endl;
                    ditemukan = true;
                }
            }
            if (!ditemukan) cout << "Tugas tidak ditemukan!" << endl;
        }
    } catch (const std::exception& e) {
        cout << "Terjadi kesalahan saat pencarian: " << e.what() << endl;
    }


}

   

void selesaikanTugas() {
    if (currentuser == nullptr) {
        cout << "Tidak ada pengguna yang login!" << endl;
        return;
    }

    if (!currentuser->queue) {
        cout << "Tidak ada tugas yang sedang berlangsung!" << endl;
        return;
    }

    if (currentuser->queue == nullptr) {
        cout << "Antrean tugas sedang kosong, tidak ada tugas untuk diselesaikan!" << endl;
        return;
    }

    node *tugas_selesai = currentuser->queue;
    if (tugas_selesai == nullptr) {
        cout << "Terjadi kesalahan: tugas yang diambil dari antrean null!" << endl;
        return;
    }

    currentuser->queue = currentuser->queue->next;
    tugas_selesai->next = currentuser->stack;
    currentuser->stack = tugas_selesai;

    if (currentuser->stack == nullptr) {
        cout << "Terjadi kesalahan: stack tidak valid setelah menyelesaikan tugas!" << endl;
        return;
    }

    cout << "Tugas " << tugas_selesai->tugasharian.judul << " berhasil diselesaikan!" << endl;
}

void updateTugas(const string &id) {
    if (currentuser == nullptr) {
        cout << "Tidak ada pengguna yang login!" << endl;
        return;
    }

    bool ditemukan = false;
    node* temp = currentuser->head;

    while (temp != nullptr) {
        if (temp->tugasharian.id == id) {
            ditemukan = true;
            cout << "Tugas ditemukan! Masukkan data baru untuk tugas ID " << id << endl;

            cout << "Masukkan Judul tugas: ";
            string judul;
            cin.ignore();
            getline(cin, judul);
            if (judul.empty()) {
                cout << "Judul tugas tidak boleh kosong!" << endl;
                return;
            }
            temp->tugasharian.judul = judul;

            cout << "Masukkan Deskripsi tugas: ";
            string deskripsi;
            getline(cin, deskripsi);
            if (deskripsi.empty()) {
                cout << "Deskripsi tugas tidak boleh kosong!" << endl;
                return;
            }
            temp->tugasharian.deskripsi = deskripsi;

            cout << "Masukkan Mata Kuliah: ";
            string matkul;
            getline(cin, matkul);
            if (matkul.empty()) {
                cout << "Mata kuliah tidak boleh kosong!" << endl;
                return;
            }
            temp->tugasharian.matkul = matkul;

            cout << "Masukkan Level Prioritas (1: Penting Mendesak, 2: Penting Tidak Mendesak, 3: Tidak Penting Mendesak, 4: Tidak Penting Tidak Mendesak): ";
            int level;
            cin >> level;
            cin.ignore();
            if (level < 1 || level > 4) {
                cout << "Input level prioritas tidak valid!" << endl;
                return;
            }
            temp->tugasharian.skala_prioritas = static_cast<tugas::Level>(level);

            for (int i = 0; i < jmlh_tugas; i++) {
                if (arrtugas[i].id == id) {
                    arrtugas[i] = temp->tugasharian;  
                    break;
                }
            }

            node* queueTemp = currentuser->queue;
            while (queueTemp != nullptr) {
                if (queueTemp->tugasharian.id == id) {
                    queueTemp->tugasharian = temp->tugasharian;
                    break;
                }
                queueTemp = queueTemp->next;
            }

            cout << "Tugas berhasil diperbarui!" << endl;
            return;
        }
        temp = temp->next;
    }

    if (!ditemukan) {
        cout << "Tugas dengan ID " << id << " tidak ditemukan!" << endl;
    }
}

void deleteTugas(const string &id) {
    if (currentuser == nullptr) {
        cout << "Tidak ada pengguna yang login!" << endl;
        return;
    }

    node* prev = nullptr;
    node* curr = currentuser->queue;

    while (curr != nullptr) {
        if (curr->tugasharian.id == id) {
            if (prev == nullptr) {
                currentuser->queue = curr->next;
            } else {
                prev->next = curr->next;
            }

            delete curr;
            cout << "Tugas dengan ID " << id << " berhasil dihapus dari queue!" << endl;

            node* prevLinkedList = nullptr;
            node* tempLinkedList = currentuser->head;
            bool ditemukan = false;

            while (tempLinkedList != nullptr) {
                if (tempLinkedList->tugasharian.id == id) {
                    ditemukan = true;
                    if (prevLinkedList == nullptr) {  
                        currentuser->head = tempLinkedList->next;
                    } else { 
                        prevLinkedList->next = tempLinkedList->next;
                    }
                    delete tempLinkedList;
                    cout << "Tugas dengan ID " << id << " berhasil dihapus dari linked list!" << endl;
                    break;
                }
                prevLinkedList = tempLinkedList;
                tempLinkedList = tempLinkedList->next;
            }

            if (!ditemukan) {
                cout << "Tugas dengan ID " << id << " tidak ditemukan di linked list!" << endl;
            }

            if (currentuser->stack != nullptr && currentuser->stack->tugasharian.id == id) {
                currentuser->stack = currentuser->stack->next;
                cout << "Tugas dengan ID " << id << " berhasil dihapus dari stack!" << endl;
            } else {
                node* prevStack = nullptr;
                node* tempStack = currentuser->stack;
                while (tempStack != nullptr) {
                    if (tempStack->tugasharian.id == id) {
                        if (prevStack == nullptr) {
                            currentuser->stack = tempStack->next;
                        } else {
                            prevStack->next = tempStack->next;
                        }
                        delete tempStack;
                        cout << "Tugas dengan ID " << id << " berhasil dihapus dari stack!" << endl;
                        break;
                    }
                    prevStack = tempStack;
                    tempStack = tempStack->next;
                }
            }

            bool ditemukanDiArray = false;
            for (int i = 0; i < jmlh_tugas; i++) {
                if (arrtugas[i].id == id) {
                    ditemukanDiArray = true;

                    for (int j = i; j < jmlh_tugas - 1; j++) {
                        arrtugas[j] = arrtugas[j + 1];  
                    }
                    jmlh_tugas--;  
                    cout << "Tugas dengan ID " << id << " berhasil dihapus dari array arrtugas!" << endl;
                    break;
                }
            }

            if (!ditemukanDiArray) {
                cout << "Tugas dengan ID " << id << " tidak ditemukan di array arrtugas!" << endl;
            }

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    cout << "Tugas dengan ID " << id << " tidak ditemukan di queue!" << endl;
}

void registerUser() {
    string username, password;
    cout << "==============================" << endl;
    cout << "     R E G I S T R A S I" << endl;
    cout << "==============================" << endl;
    
    do {
        cout << "Masukkan username: ";
        getline(cin, username);
        if (users.find(username) != users.end()) {
            cout << "Username sudah terdaftar! Coba username lain." << endl;
            return;
        }
    } while (username.empty());
    
    do {
        cout << "Masukkan password (minimal 8 karakter): ";
        getline(cin, password);
        if (password.length() < 8) {
            cout << "Password terlalu pendek! Password minimal 8 karakter" << endl;
        }
    } while (password.length() < 8);
    
    user newuser = {username, password, nullptr};
    users[username] = newuser;

    cout << "Registrasi berhasil! Silakan login." << endl;
}

bool loginUser() {
    string username, password;
    cout << "==============================" << endl;
    cout << "           L O G I N          " << endl;
    cout << "==============================" << endl;
    cout << "Masukkan username: ";
    getline(cin, username);
    cout << "Masukkan password: ";
    getline(cin, password);

    if (users.find(username) != users.end() && users[username].password == password) {
        currentuser = &users[username];
        cout << "Login berhasil!" << endl;
        return true;
    } else {
        cout << "Username atau password salah!" << endl;
        return false;
    }
}


int main() {
    int pilihan;

        do {
        cout << "\n==============================" << endl;
        cout << "      C A M P  Q U E S T" << endl;
        cout << "==============================" << endl;
        cout << "1. Registrasi" << endl;
        cout << "2. Login" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilihan: ";
        if (!(cin >> pilihan) || (pilihan != 1 && pilihan != 2 && pilihan != 0)) {
            cout << "Input tidak valid! Silakan masukkan pilihan yang benar." << endl;
            cin.clear();  
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  
            continue;
        }
        
        cin.ignore();

        switch (pilihan) {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginUser()) {
                    int menuPilihan;
                    do {
                        cout << "\n==============================" << endl;
                        cout << "     C A M P  Q U E S T" << endl;
                        cout << "==============================" << endl;
                        cout << "1. Tambah Tugas\n";
                        cout << "2. Tampilkan Semua Tugas\n";
                        cout << "3. Selesaikan Tugas\n";
                        cout << "4. Perbarui Tugas\n";
                        cout << "5. Hapus Tugas\n";
                        cout << "6. Keluar\n";
                        cout << "Pilih opsi: ";
                        if (!(cin >> menuPilihan)) {
                            cout << "Input tidak valid! Masukkan angka." << endl;
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            continue;
                        }
                        cin.ignore();

                        if (menuPilihan < 1 || menuPilihan > 6) {
                            cout << "Pilihan tidak valid! Silakan masukkan pilihan yang benar." << endl;
                            continue;
                        }

                        switch (menuPilihan) {
                            case 1:
                                tambahTugas();
                                break;
                            case 2:
                                tampilkanSemuaTugas();
                                break;
                            case 3:
                                selesaikanTugas();
                                break;
                            case 4: {
                                string id;
                                cout << "Masukkan ID tugas yang ingin diperbarui: ";
                                cin >> id;
                                if (id.empty() || id.find_first_not_of(' ') == string::npos) {
                                    cout << "ID tidak boleh kosong atau hanya berisi spasi!" << endl;
                                    break;
                                }
                                updateTugas(id);
                                break;
                            }
                            case 5: {
                                string id;
                                cout << "Masukkan ID tugas yang ingin dihapus: ";
                                cin >> id;
                                if (id.empty() || id.find_first_not_of(' ') == string::npos) {
                                    cout << "ID tidak boleh kosong atau hanya berisi spasi!" << endl;
                                    break;
                                }
                                deleteTugas(id);
                                break;
                            }
                            case 6:
                                cout << "Keluar dari menu manajemen tugas.\n";
                                break;
                            default:
                                cout << "Pilihan tidak valid, coba lagi.\n";
                                break;
                        }
                    } while (menuPilihan != 6);
                } else {
                    cout << "Login gagal! Silakan coba lagi." << endl;
                }
                break;
            case 0:
                cout << "Terima kasih telah menggunakan CampQuest!" << endl;
                break;
            default:
                cout << "Pilihan tidak valid!" << endl;
        }
    } while (pilihan != 0);

    return 0;
}