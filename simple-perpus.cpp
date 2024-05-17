#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdlib>

#ifdef _WIN32 // Memeriksa apakah sistem operasi yang digunakan adalah Windows
#include <windows.h> 
void delay(int milliseconds) { // Fungsi untuk memberikan delay dalam milidetik
    Sleep(milliseconds);
}
#else
#include <unistd.h>
void delay(int milliseconds) {
    usleep(milliseconds * 1000);
}
#endif

#ifdef _WIN32 // Memeriksa apakah sistem operasi yang digunakan adalah Windows
#include <cstdlib>
void clearScreen() { // Fungsi untuk membersihkan layar konsol
    system("cls");
}
#else
#include <cstdio>
void clearScreen() {
    system("clear");
}
#endif

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

char getch() {
    char buf = 0;
    struct termios old;
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}
#endif

using namespace std;

// Struktur untuk menyimpan informasi buku
struct Book {
    int id;
    string title;
    int volume;
    string author;
    string publisher;
    string category;
    int year;
};

// Struktur untuk menyimpan informasi peminjaman buku
struct BookLoan {
    int bookId;
    string borrowerName;
    int durationDays;
};

const int MAX_LOANS = 100; // Jumlah maksimum peminjaman buku yang dapat disimpan
BookLoan loans[MAX_LOANS]; // Array untuk menyimpan informasi peminjaman buku
int loanCount = 0; // Jumlah peminjaman buku saat ini
const int MAX_BOOKS = 100; // Jumlah maksimum buku yang dapat disimpan
const int BOOKS_PER_PAGE = 10; // Jumlah maksimum buku yang ditampilkan per halaman
const int LOANS_PER_PAGE = 5; // Jumlah maksimum peminjaman yang ditampilkan per halaman


// Fungsi hash untuk mengonversi string menjadi nilai integer
size_t hashString(const string& str) {
    size_t hash = 0;
    for (char c : str) {
        hash = hash * 31 + c;
    }
    return hash;
}

// Fungsi untuk memberikan delay dalam milidetik
/*void delay(int milliseconds) {
//    Sleep(milliseconds);
//}*/

// Fungsi untuk membersihkan layar konsol
/*void clearScreen() {
    system("cls");
}*/

// Fungsi untuk menginput informasi buku dan memperbarui hash table
void inputBook(Book books[], int& bookCount, unordered_map<string, vector<int>>& titleIndex, unordered_map<int, int>& idIndex) {
    Book newBook;
    newBook.id = bookCount + 1; // Mengatur ID buku sesuai dengan bookCount
    cout << "Masukkan judul buku: ";
    getline(cin, newBook.title);
    cout << "Masukkan volume buku: ";
    cin >> newBook.volume;
    cin.ignore(); // Mengabaikan karakter newline yang tersisa di buffer
    cout << "Masukkan nama pengarang: ";
    getline(cin, newBook.author);
    cout << "Masukkan nama publisher: ";
    getline(cin, newBook.publisher);
    cout << "Masukkan kategori buku: ";
    getline(cin, newBook.category);
    cout << "Masukkan tahun terbit: ";
    cin >> newBook.year;
    cin.ignore(); // Mengabaikan karakter newline yang tersisa di buffer

    if (bookCount < MAX_BOOKS) {
        books[bookCount] = newBook;
        titleIndex[newBook.title].push_back(bookCount);
        idIndex[newBook.id] = bookCount; // Menyimpan indeks buku berdasarkan ID
        bookCount++;
        clearScreen();
        cout << "\nBuku dengan judul \"" << newBook.title << " Volume " << newBook.volume << "\" berhasil ditambahkan ke dalam database.\n";
        delay(4000);
        clearScreen();
    } else {
        cout << "Kapasitas penyimpanan buku sudah penuh.\n";
        delay(3000);
        clearScreen();
    }
}

// Fungsi untuk menampilkan informasi buku
void displayBook(Book book) {
    cout << "\nInformasi Buku:\n";
    cout << "ID Buku: " << book.id << endl;
    cout << "Judul: " << book.title << endl;
    cout << "Volume: " << book.volume << endl;
    cout << "Pengarang: " << book.author << endl;
    cout << "Publisher: " << book.publisher << endl;
    cout << "Kategori: " << book.category << endl;
    cout << "Tahun Terbit: " << book.year << endl;

    // Memeriksa status peminjaman buku
    bool isOnLoan = false;
    for (int i = 0; i < loanCount; i++) {
        if (loans[i].bookId == book.id) {
            isOnLoan = true;
            cout << "Status: Sedang Dipinjam oleh " << loans[i].borrowerName << " (" << loans[i].durationDays << " hari)\n";
            break;
        }
    }

    if (!isOnLoan) {
        cout << "Status: Tersedia\n";
    }
}

// Fungsi untuk menampilkan daftar buku
void displayBookList(Book books[], int bookCount, int& currentPage) {
    int startIndex = (currentPage - 1) * BOOKS_PER_PAGE;
    int endIndex = min(startIndex + BOOKS_PER_PAGE, bookCount);

    cout << "\n\n================================================\n";
    cout << "Daftar Buku (Halaman " << currentPage << "):\n";
    for (int i = startIndex; i < endIndex; i++) {
        cout << "\n" << (i + 1) << ". ";
        displayBook(books[i]);
    }

    char choice;
    do {
        cout << "\n\n================================================\n";
        cout << "Tekan 'n' untuk halaman selanjutnya, 'p' untuk halaman sebelumnya, atau 'q' untuk keluar: ";
        choice = getch();

        if (choice == 'n') {
            if (endIndex == bookCount) {
                clearScreen();
                cout << "\nAnda sudah berada di halaman terakhir.\n";
            } else {
                clearScreen();
                currentPage++;
                return displayBookList(books, bookCount, currentPage);
            }
        } else if (choice == 'p') {
            if (currentPage > 1) {
                clearScreen();
                currentPage--;
                return displayBookList(books, bookCount, currentPage);
            } else {
                clearScreen();
                cout << "\nAnda sudah berada di halaman pertama.\n";
            }
        } else if (choice != 'q') {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (choice != 'q');
    clearScreen();
}

// Fungsi untuk mencari buku berdasarkan judul (tidak case-sensitive)
void searchBooks(Book books[], unordered_map<string, vector<int>>& titleIndex) {
    string searchString;
    cout << "Masukkan judul buku: ";
    getline(cin, searchString);

    // Mengonversi searchString menjadi huruf kecil
    transform(searchString.begin(), searchString.end(), searchString.begin(), ::tolower);

    vector<int> foundIndices;
    for (const auto& pair : titleIndex) {
        string lowercaseTitle = pair.first;
        transform(lowercaseTitle.begin(), lowercaseTitle.end(), lowercaseTitle.begin(), ::tolower);

        if (lowercaseTitle.find(searchString) != string::npos) {
            for (int index : pair.second) {
                foundIndices.push_back(index);
            }
        }
    }

    if (foundIndices.empty()) {
        clearScreen();
        cout << "Tidak ada buku yang ditemukan dengan judul tersebut.\n";
        delay(3000);
        clearScreen();
    } else {
        int currentPage = 1;
        int totalPages = (foundIndices.size() + BOOKS_PER_PAGE - 1) / BOOKS_PER_PAGE;

        char choice;
        do {
            int startIndex = (currentPage - 1) * BOOKS_PER_PAGE;
            int endIndex = min(startIndex + BOOKS_PER_PAGE, static_cast<int>(foundIndices.size()));

            cout << "\nHasil Pencarian (Halaman " << currentPage << " dari " << totalPages << "):\n";
            for (int i = startIndex; i < endIndex; i++) {
                cout << "\n" << (i + 1) << ". ";
                displayBook(books[foundIndices[i]]);
            }

            if (currentPage < totalPages) {
                cout << "\nTekan 'n' untuk halaman selanjutnya, 'p' untuk halaman sebelumnya, atau 'q' untuk keluar: ";
                choice = getch();

                if (choice == 'n') {
                    clearScreen();
                    currentPage++;
                } else if (choice == 'p') {
                    if (currentPage > 1) {
                        clearScreen();
                        currentPage--;
                    } else {
                        clearScreen();
                        cout << "Anda sudah berada di halaman pertama.\n";
                    }
                } else if (choice != 'q') {
                    cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                }
            } else {
                cout << "\nTekan 'q' untuk keluar: ";
                choice = getch();
            }
        } while (choice != 'q');
    }
    clearScreen();
}

// Fungsi untuk menyimpan data buku ke dalam file CSV
void saveBookData(Book books[], int bookCount) {
    ofstream outputFile("book_data.csv"); // Membuka file dalam mode tulis

    if (outputFile.is_open()) {
        // Menulis header file
        outputFile << "ID,Judul,Volume,Pengarang,Publisher,Kategori,Tahun\n";

        // Menulis data setiap buku ke dalam file
        for (int i = 0; i < bookCount; i++) {
            outputFile << books[i].id << "," << books[i].title << "," << books[i].volume << "," << books[i].author << "," << books[i].publisher << "," << books[i].category << "," << books[i].year << "\n";
        }

        outputFile.close(); // Menutup file
        cout << "Data buku berhasil disimpan ke dalam file book_data.csv.\n";
    } else {
        cout << "Gagal membuka file book_data.csv untuk menyimpan data buku.\n";
    }
}


// Fungsi untuk menyimpan data peminjaman ke dalam file CSV
void saveLoanData() {
    ofstream outputFile("loan_data.csv"); // Membuka file dalam mode tulis

    if (outputFile.is_open()) {
        // Menulis header file
        outputFile << "ID Buku,Nama Peminjam,Durasi (hari)\n";

        // Menulis data setiap peminjaman ke dalam file
        for (int i = 0; i < loanCount; i++) {
            outputFile << loans[i].bookId << "," << loans[i].borrowerName << "," << loans[i].durationDays << "\n";
        }

        outputFile.close(); // Menutup file
        cout << "Data peminjaman berhasil disimpan ke dalam file loan_data.csv.\n";
    } else {
        cout << "Gagal membuka file loan_data.csv untuk menyimpan data peminjaman.\n";
    }
}

// Fungsi untuk membaca data buku dari file CSV
void loadBookData(Book books[], int& bookCount, unordered_map<string, vector<int>>& titleIndex, unordered_map<int, int>& idIndex) {
    ifstream inputFile("book_data.csv"); // Membuka file dalam mode baca

    if (inputFile.is_open()) {
        string line;
        getline(inputFile, line); // Mengabaikan baris header

        bookCount = 0;
        while (getline(inputFile, line)) {
            if (bookCount >= MAX_BOOKS) {
                cout << "Kapasitas penyimpanan buku sudah penuh. Data tidak dapat dimuat seluruhnya.\n";
                break;
            }

            // Memisahkan setiap nilai dalam baris menggunakan koma sebagai pemisah
            size_t pos = 0;
            string token;
            vector<string> values;
            while ((pos = line.find(',')) != string::npos) {
                token = line.substr(0, pos);
                values.push_back(token);
                line.erase(0, pos + 1);
            }
            values.push_back(line); // Menambahkan nilai terakhir

            // Mengisi data buku ke dalam array
            books[bookCount].id = stoi(values[0]); // Mengisi nilai id dari file
            books[bookCount].title = values[1];
            books[bookCount].volume = stoi(values[2]);
            books[bookCount].author = values[3];
            books[bookCount].publisher = values[4];
            books[bookCount].category = values[5];
            books[bookCount].year = stoi(values[6]);

            // Memperbarui hash table
            titleIndex[books[bookCount].title].push_back(bookCount);
            idIndex[books[bookCount].id] = bookCount;

            bookCount++;
        }

        inputFile.close(); // Menutup file
        cout << "Data buku berhasil dimuat dari file book_data.csv.\n";
    } else {
        cout << "File book_data.csv tidak ditemukan. Data buku tidak dapat dimuat.\n";
    }
}

// Fungsi untuk membaca data peminjaman dari file CSV
void loadLoanData(unordered_map<int, int>& idIndex) {
    ifstream inputFile("loan_data.csv"); // Membuka file dalam mode baca

    if (inputFile.is_open()) {
        string line;
        getline(inputFile, line); // Mengabaikan baris header

        loanCount = 0;
        while (getline(inputFile, line)) {
            if (loanCount >= MAX_LOANS) {
                cout << "Kapasitas penyimpanan peminjaman sudah penuh. Data tidak dapat dimuat seluruhnya.\n";
                break;
            }

            // Memisahkan setiap nilai dalam baris menggunakan koma sebagai pemisah
            size_t pos = 0;
            string token;
            vector<string> values;
            while ((pos = line.find(',')) != string::npos) {
                token = line.substr(0, pos);
                values.push_back(token);
                line.erase(0, pos + 1);
            }
            values.push_back(line); // Menambahkan nilai terakhir

            int bookId = stoi(values[0]);
            string borrowerName = values[1];
            int durationDays = stoi(values[2]);

            // Memeriksa apakah buku dengan ID tersebut ada dalam database
            if (idIndex.count(bookId) > 0) {
                BookLoan newLoan = { bookId, borrowerName, durationDays };
                loans[loanCount++] = newLoan;
            } else {
                cout << "Buku dengan ID " << bookId << " tidak ditemukan dalam database. Data peminjaman tidak dapat dimuat.\n";
            }
        }

        inputFile.close(); // Menutup file
        cout << "Data peminjaman berhasil dimuat dari file loan_data.csv.\n";
    } else {
        cout << "File loan_data.csv tidak ditemukan. Data peminjaman tidak dapat dimuat.\n";
    }
}


// Fungsi untuk mengurutkan data peminjaman menggunakan algoritma insertion sort
void insertionSortLoans(BookLoan loans[], int n) {
    if (n <= 1) {
        return;  // Basis: Array dengan 0 atau 1 elemen sudah terurut
    }

    // Urutkan elemen pertama dari loans[1...n]
    insertionSortLoans(loans, n - 1);

    // Masukkan loans[n] ke dalam posisi yang tepat di antara loans[0...n-1]
    BookLoan lastElement = loans[n - 1];
    int j = n - 2;

    // Geser elemen yang lebih besar dari lastElement ke kanan
    while (j >= 0 && loans[j].durationDays > lastElement.durationDays) {
        loans[j + 1] = loans[j];
        j--;
    }

    loans[j + 1] = lastElement;
}

// Fungsi untuk meminjam buku
void borrowBook(Book books[], unordered_map<int, int>& idIndex) {
    int bookId;
    cout << "Masukkan ID buku yang ingin dipinjam: ";
    cin >> bookId;
    cin.ignore();

    if (idIndex.count(bookId) == 0) {
        cout << "Buku dengan ID tersebut tidak ditemukan.\n";
        delay(3000);
        clearScreen();
        return;
    }

    int bookIndex = idIndex[bookId];

    // Memeriksa apakah buku sedang dipinjam
    bool isOnLoan = false;
    for (int i = 0; i < loanCount; i++) {
        if (loans[i].bookId == bookId) {
            isOnLoan = true;
            break;
        }
    }

    if (isOnLoan) {
        cout << "Tidak dapat meminjam buku \"" << books[bookIndex].title << " Volume " << books[bookIndex].volume << "\" karena sedang dipinjam oleh orang lain.\n";
        delay(3000);
        clearScreen();
        return;
    }

    if (loanCount < MAX_LOANS) {
        string borrowerName;
        int durationDays;
        cout << "Masukkan nama peminjam: ";
        getline(cin, borrowerName);
        cout << "Masukkan durasi peminjaman (maksimal 7 hari): ";
        cin >> durationDays;
        cin.ignore();

        if (durationDays > 7) {
            cout << "Durasi peminjaman melebihi batas maksimum (7 hari).\n";
            delay(3000);
            clearScreen();
            return;
        }

        BookLoan newLoan = { bookId, borrowerName, durationDays };
        loans[loanCount++] = newLoan;

        // Mengurutkan data peminjaman berdasarkan durasi (hari) secara menaik
        insertionSortLoans(loans, loanCount);

        clearScreen();
        cout << "Buku dengan judul \"" << books[bookIndex].title << " Volume " << books[bookIndex].volume << "\" berhasil dipinjam oleh " << borrowerName << " selama " << durationDays << " hari.\n";
        delay(4000);
        clearScreen();
    } else {
        cout << "Kapasitas peminjaman buku sudah penuh.\n";
        delay(3000);
        clearScreen();
    }
}

// Fungsi untuk mengembalikan buku
void returnBook(Book books[], unordered_map<int, int>& idIndex) {
    int bookId;
    cout << "Masukkan ID buku yang ingin dikembalikan: ";
    cin >> bookId;
    cin.ignore();

    if (idIndex.count(bookId) == 0) {
        cout << "Buku dengan ID tersebut tidak ditemukan.\n";
        delay(3000);
        clearScreen();
        return;
    }

    int bookIndex = idIndex[bookId];
    bool isOnLoan = false;
    int loanIndex = -1;

    for (int i = 0; i < loanCount; i++) {
        if (loans[i].bookId == bookId) {
            isOnLoan = true;
            loanIndex = i;
            break;
        }
    }

    if (isOnLoan) {
        string borrowerName = loans[loanIndex].borrowerName;
        cout << "Buku dengan judul \"" << books[bookIndex].title << " Volume " << books[bookIndex].volume << "\" berhasil dikembalikan oleh " << borrowerName << ".\n";
        delay(4000);
        clearScreen();

        // Menghapus informasi peminjaman dari array loans
        for (int i = loanIndex; i < loanCount - 1; i++) {
            loans[i] = loans[i + 1];
        }
        loanCount--;
    } else {
        cout << "Buku dengan ID tersebut tidak sedang dipinjam.\n";
        delay(3000);
        clearScreen();
    }
}

// Fungsi untuk menampilkan daftar peminjam buku
void displayLoanList(unordered_map<int, int>& idIndex, Book books[]) {
    if (loanCount == 0) {
        cout << "Tidak ada buku yang sedang dipinjam.\n";
        delay(3000);
        clearScreen();
        return;
    }

    int currentPage = 1;
    char choice;

    do {
        int startIndex = (currentPage - 1) * LOANS_PER_PAGE;
        int endIndex = min(startIndex + LOANS_PER_PAGE, loanCount);

        cout << "\n\n================================================\n";
        cout << "\nDaftar Peminjam Buku (Halaman " << currentPage << "):\n";
        for (int i = startIndex; i < endIndex; i++) {
            int bookIndex = idIndex[loans[i].bookId];
            Book book = books[bookIndex];

            cout << "ID Buku: " << book.id << "\n";
            cout << "Peminjam: " << loans[i].borrowerName << "\n";
            cout << "Judul Buku: " << book.title << " Volume " << book.volume << "\n";
            cout << "Durasi Peminjaman: " << loans[i].durationDays << " hari\n\n";
        }

        if (endIndex < loanCount) {
            cout << "================================================\n";
            cout << "Tekan 'n' untuk halaman selanjutnya atau 'q' untuk keluar: ";
            choice = getch();

            if (choice == 'n') {
                clearScreen();
                currentPage++;
            } else if (choice != 'q') {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            }
        } else {
            cout << "================================================\n";
            cout << "Tekan 'q' untuk keluar: ";
            choice = getch();
        }
    } while (choice != 'q');
    clearScreen();
}

int main() {
    int choice;
    Book books[MAX_BOOKS]; // Array untuk menyimpan kumpulan buku
    int bookCount = 0; // Jumlah buku yang telah ditambahkan
    int currentPage = 1; // Indeks buku yang sedang ditampilkan

    unordered_map<string, vector<int>> titleIndex; // Hash table untuk menyimpan indeks buku berdasarkan judul
    unordered_map<int, int> idIndex; // Hash table untuk menyimpan indeks buku berdasarkan ID

    loadBookData(books, bookCount, titleIndex, idIndex); // Membaca data buku dari file CSV
    loadLoanData(idIndex); // Membaca data peminjaman dari file CSV

    do {
        cout << "\n===== APLIKASI MANAJEMEN PERPUSTAKAAN =====\n";
        cout << "Jumlah Buku: " << bookCount << endl << endl;
        cout << "1. Tambah Buku Baru\n";
        cout << "2. Cari Buku\n";
        cout << "3. Daftar Buku\n";
        cout << "4. Peminjaman Buku\n";
        cout << "5. Pengembalian Buku\n";
        cout << "6. Daftar Peminjam Buku\n";
        cout << "7. Simpan Data Buku\n";
        cout << "8. Keluar\n";
        cout << "Pilihan Anda: ";
        choice = getch();

        if (choice >= '1' && choice <= '8') { // Memeriksa apakah input berupa angka 1-8
            choice = choice - '0'; // Konversi dari nilai ASCII ke nilai numerik
        }
        cout << choice << endl;

        switch (choice) {
            case 1:
                clearScreen();
                inputBook(books, bookCount, titleIndex, idIndex); // Menambahkan buku baru ke dalam database
                break;
            case 2:
                clearScreen();
                if (bookCount > 0) {
                    searchBooks(books, titleIndex); // Mencari buku berdasarkan judul
                } else {
                    cout << "Belum ada buku yang ditambahkan.\n";
                    delay(3000);
                    clearScreen();
                }
                break;
            case 3:
                clearScreen();
                if (bookCount > 0) {
                    displayBookList(books, bookCount, currentPage); // Menampilkan daftar buku mulai dari halaman 1
                } else {
                    cout << "Belum ada buku yang ditambahkan.\n";
                    delay(3000);
                    clearScreen();
                }
                break;
            case 4:
                clearScreen();
                if (bookCount > 0) {
                    borrowBook(books, idIndex);
                } else {
                    cout << "Belum ada buku yang ditambahkan.\n";
                    delay(3000);
                    clearScreen();
                }
                break;
            case 5:
                clearScreen();
                if (bookCount > 0) {
                    returnBook(books, idIndex);
                } else {
                    cout << "Belum ada buku yang ditambahkan.\n";
                    delay(3000);
                    clearScreen();
                }
                break;
            case 6:
                clearScreen();
                displayLoanList(idIndex, books); // Menampilkan daftar peminjaman buku
                break;
            case 7:
                clearScreen();
                saveBookData(books, bookCount); // Menyimpan data buku ke dalam file CSV
                saveLoanData(); // Menyimpan data peminjaman ke dalam file CSV
                delay(3000);
                clearScreen();
                break;
            case 8:
                clearScreen();
                cout << "================================================\n";
                cout << "Terima kasih telah menggunakan aplikasi ini.\n";
                cout << "================================================\n";
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (choice != 8);

    return 0;
}
