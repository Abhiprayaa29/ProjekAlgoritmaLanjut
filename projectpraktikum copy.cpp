#include <bits/stdc++.h>
#include <ctime>
using namespace std;

const int MAX = 100;
string usernames[MAX];
string passwords[MAX];
string roles[MAX];
int jumlahuser = 0;
int jumlahbarang = 0;

struct barang
{
    int id_barang;
    string nama;
    string kategori;
    int harga;
    int stok;
};

struct barang barang[100];

struct ItemKeranjang
{
    int id_barang;
    string nama;
    int harga;
    int jumlah;
};

vector<ItemKeranjang> keranjang;

void menuManajer();
void menuKasir();
void tambahbarang();
void lihatbarang();
void caribarang();
void hapusbarang();
void registerUser();
void login();
void cariBarangID();
void simpanBarangKeFile();
void bacaBarangDariFile();
void simpanloginkefile();
void bacaLoginDariFile();
void tambahKeKeranjang(int id, int jumlah);
void lihatKeranjang();
void checkout();
string blocking(string teks);
void buatNota(int total, int bayar, int kembalian);
string getCurrentDateTime();

string getCurrentDateTime()
{
    time_t now = time(0);
    char *dt = ctime(&now);
    string datetime(dt);

    if (!datetime.empty() && datetime.back() == '\n')
    {
        datetime.pop_back();
    }
    return datetime;
}

// Fungsi untuk membuat nota pembelian
void buatNota(int total, int bayar, int kembalian)
{
    // Buat nama file nota memakai timestamp
    time_t now = time(0);
    tm *ltm = localtime(&now);

    char filename[100];
    sprintf(filename, "nota_%02d%02d%04d_%02d%02d%02d.txt",
            ltm->tm_mday, ltm->tm_mon + 1, 1900 + ltm->tm_year,
            ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    ofstream nota(filename);
    if (!nota.is_open())
    {
        cout << "Error: Tidak dapat membuat file nota!\n";
        return;
    }

    // Header nota
    nota << "========================================\n";
    nota << "           TOKO MUGI BERKAH            \n";
    nota << "        NOTA PEMBELIAN                 \n";
    nota << "========================================\n";
    nota << "Tanggal: " << getCurrentDateTime() << "\n";
    nota << "========================================\n\n";

    // Detail barang
    nota << "DETAIL PEMBELIAN:\n";
    nota << "----------------------------------------\n";
    nota << left << setw(15) << "NAMA BARANG"
         << setw(8) << "QTY"
         << setw(10) << "HARGA"
         << setw(12) << "SUBTOTAL" << "\n";
    nota << "----------------------------------------\n";

    for (const auto &item : keranjang)
    {
        int subtotal = item.harga * item.jumlah;
        nota << left << setw(15) << item.nama.substr(0, 14)
             << setw(8) << item.jumlah
             << setw(10) << item.harga
             << setw(12) << subtotal << "\n";
    }

    nota << "----------------------------------------\n";
    nota << right << setw(35) << "TOTAL: " << total << "\n";
    nota << right << setw(35) << "BAYAR: " << bayar << "\n";
    nota << right << setw(35) << "KEMBALI: " << kembalian << "\n";
    nota << "----------------------------------------\n\n";
    nota << "        TERIMA KASIH ATAS             \n";
    nota << "         KUNJUNGAN ANDA!              \n";
    nota << "    BEKERJALAH DENGAN JUJUR           \n";
    nota << "========================================\n";

    nota.close();
    cout << "Nota berhasil disimpan sebagai: " << filename << "\n";
}

// Fitur keranjang & checkout
void tambahKeKeranjang(int id, int jumlah)
{
    int idx = -1;
    for (int i = 0; i < jumlahbarang; i++)
    {
        if (barang[i].id_barang == id)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        cout << "Barang tidak ditemukan!\n";
        return;
    }
    if (jumlah <= 0)
    {
        cout << "Jumlah tidak valid!\n";
        return;
    }
    if (barang[idx].stok < jumlah)
    {
        cout << "Stok tidak cukup, tersedia: " << barang[idx].stok << endl;
        return;
    }

    // Cek apakah barang sudah ada di keranjang
    bool found = false;
    for (auto &item : keranjang)
    {
        if (item.id_barang == id)
        {
            if (barang[idx].stok < (item.jumlah + jumlah))
            {
                cout << "Stok tidak cukup untuk total di keranjang, tersedia: " << barang[idx].stok << endl;
                return;
            }
            item.jumlah += jumlah;
            found = true;
            break;
        }
    }
    if (!found)
    {
        keranjang.push_back({id, barang[idx].nama, barang[idx].harga, jumlah});
    }
    cout << "Berhasil tambah ke keranjang.\n";
}

void lihatKeranjang()
{
    if (keranjang.empty())
    {
        cout << "Keranjang kosong.\n";
        return;
    }
    int total = 0;
    cout << "ID\tNama\t\tJumlah\tHarga\tSubtotal\n";
    cout << "================================================\n";
    for (const auto &item : keranjang)
    {
        int subtotal = item.harga * item.jumlah;
        total += subtotal;
        cout << item.id_barang << "\t" << item.nama << "\t\t" << item.jumlah << "\t" << item.harga << "\t" << subtotal << "\n";
    }
    cout << "================================================\n";
    cout << "Total harga: " << total << "\n";
}

void checkout()
{
    if (keranjang.empty())
    {
        cout << "Keranjang kosong. Tidak bisa checkout.\n";
        return;
    }

    // Validasi stok sekali lagi sebelum checkout
    for (const auto &item : keranjang)
    {
        for (int i = 0; i < jumlahbarang; i++)
        {
            if (barang[i].id_barang == item.id_barang)
            {
                if (barang[i].stok < item.jumlah)
                {
                    cout << "Stok " << barang[i].nama << " tidak cukup. Checkout dibatalkan.\n";
                    return;
                }
                break;
            }
        }
    }

    int total = 0;
    for (const auto &item : keranjang)
        total += item.harga * item.jumlah;

    cout << "Total bayar: " << total << "\n";
    cout << "Masukkan uang pembayaran: ";
    int bayar;
    cin >> bayar;

    if (bayar < total)
    {
        cout << "Uang tidak cukup. Checkout dibatalkan.\n";
        return;
    }

    int kembalian = bayar - total;

    // Kurangi stok
    for (const auto &item : keranjang)
    {
        for (int i = 0; i < jumlahbarang; i++)
        {
            if (barang[i].id_barang == item.id_barang)
            {
                barang[i].stok -= item.jumlah;
                break;
            }
        }
    }

    // Simpan perubahan stok ke file
    simpanBarangKeFile();

    // Buat nota pembelian
    buatNota(total, bayar, kembalian);

    cout << "Transaksi berhasil. Kembalian: " << kembalian << "\n";
    keranjang.clear();
}

void menuManajer()
{
    int pilihan;
    do
    {
        system("cls");
        cout << "========================================\n";
        cout << "||          MENU MANAJER TOKO         ||\n";
        cout << "||          MUGI BERKAH JAYA          ||\n";
        cout << "========================================\n";
        cout << "||  1. Tambah Barang                  ||\n";
        cout << "||  2. Lihat Barang                   ||\n";
        cout << "||  3. Hapus Barang                   ||\n";
        cout << "||  4. Register Pengguna Baru         ||\n";
        cout << "||  0. Logout                         ||\n";
        cout << "========================================\n";
        cout << "Masukkan pilihan: ";
        cin >> pilihan; // PERBAIKAN: Hapus duplikasi cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            tambahbarang();
            break;
        case 2:
            lihatbarang();
            break;
        case 3:
            hapusbarang();
            break;
        case 4:
            registerUser();
            break;
        case 0:
            cout << "Logout...\n";
            break;
        default:
            cout << "Pilihan tidak valid!\n";
            break;
        }
        if (pilihan != 0)
            system("pause");
    } while (pilihan != 0);
}

void menuKasir()
{
    int pilihan;
    do
    {
        system("cls"); // PERBAIKAN: Tambah system("cls") untuk membersihkan layar
        cout << "========================================\n";
        cout << "||            MENU KASIR              ||\n";
        cout << "||          TOKO MUGI BERKAH          ||\n";
        cout << "========================================\n";
        cout << "||  1. Lihat Barang                   ||\n";
        cout << "||  2. Cari Barang Berdasarkan Nama  ||\n";
        cout << "||  3. Cari Barang Berdasarkan ID    ||\n";
        cout << "||  4. Lihat Keranjang               ||\n";
        cout << "||  5. Checkout                      ||\n";
        cout << "||  0. Logout                        ||\n";
        cout << "========================================\n";
        cout << "Masukkan pilihan: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            lihatbarang();
            break;
        case 2:
            caribarang();
            break;
        case 3:
            cariBarangID();
            break;
        case 4:
            lihatKeranjang();
            break;
        case 5:
            checkout();
            break;
        case 0:
            cout << "Logout berhasil.\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
            break;
        }

        if (pilihan != 0)
        {
            system("pause");
        }
    } while (pilihan != 0);
}

void simpanBarangKeFile()
{
    ofstream file("data_barang.txt");
    if (!file.is_open())
    {
        cout << "Error: Tidak dapat membuka file data_barang.txt\n";
        return;
    }
    for (int i = 0; i < jumlahbarang; i++)
    {
        file << barang[i].id_barang << "," << barang[i].nama << ","
             << barang[i].kategori << "," << barang[i].harga << ","
             << barang[i].stok << "\n";
    }
    file.close();
}

void bacaBarangDariFile()
{
    ifstream file("data_barang.txt");
    if (!file.is_open())
    {
        return; // Tidak perlu menampilkan pesan error
    }

    string line;
    jumlahbarang = 0;

    while (getline(file, line) && jumlahbarang < 100)
    {
        stringstream ss(line);
        string id, nama, kategori, harga, stok;

        if (getline(ss, id, ',') && getline(ss, nama, ',') &&
            getline(ss, kategori, ',') && getline(ss, harga, ',') &&
            getline(ss, stok, ','))
        {

            try
            {
                barang[jumlahbarang].id_barang = stoi(id);
                barang[jumlahbarang].nama = nama;
                barang[jumlahbarang].kategori = kategori;
                barang[jumlahbarang].harga = stoi(harga);
                barang[jumlahbarang].stok = stoi(stok);
                jumlahbarang++;
            }
            catch (const exception &e)
            {
                cout << "Error parsing line: " << line << endl;
            }
        }
    }
    file.close();
}

void simpanloginkefile()
{
    ofstream file("data_login.txt");
    if (!file.is_open())
    {
        cout << "Error: Tidak dapat membuka file data_login.txt\n";
        return;
    }
    for (int i = 0; i < jumlahuser; i++)
    {
        file << usernames[i] << '|' << passwords[i] << '|' << roles[i] << '\n';
    }
    file.close();
}

void bacaLoginDariFile()
{
    ifstream file("data_login.txt");
    if (!file.is_open())
    {
        return; // Tidak perlu menampilkan pesan error
    }

    string line;
    jumlahuser = 0;
    while (getline(file, line) && jumlahuser < MAX)
    {
        stringstream ss(line);
        string user, pass, role_temp;
        if (getline(ss, user, '|') &&
            getline(ss, pass, '|') &&
            getline(ss, role_temp, '|'))
        {

            // Bersihkan whitespace dan karakter tersembunyi
            user.erase(remove_if(user.begin(), user.end(), ::isspace), user.end());
            pass.erase(remove_if(pass.begin(), pass.end(), ::isspace), pass.end());
            role_temp.erase(remove_if(role_temp.begin(), role_temp.end(), ::isspace), role_temp.end());

            usernames[jumlahuser] = user;
            passwords[jumlahuser] = pass;
            roles[jumlahuser] = role_temp;
            jumlahuser++;
        }
    }
    file.close();
}

void tambahbarang()
{
    system("cls");
    cout << "=== TAMBAH BARANG ===\n";

    int id_input;
    cout << "Masukkan ID Barang: ";
    cin >> id_input;

    bool ditemukan = false;
    for (int i = 0; i < jumlahbarang; i++)
    {
        if (barang[i].id_barang == id_input)
        {
            // ID ditemukan, tambahkan stok saja
            int tambah_stok;
            cout << "ID ditemukan: " << barang[i].nama << " (Stok saat ini: " << barang[i].stok << ")\n";
            do
            {
                cout << "Masukkan jumlah stok yang ingin ditambahkan: ";
                cin >> tambah_stok;
                if (tambah_stok < 0)
                {
                    cout << "Jumlah tidak boleh negatif!\n";
                }
            } while (tambah_stok < 0);

            barang[i].stok += tambah_stok;
            cout << "Stok berhasil diperbarui! Stok sekarang: " << barang[i].stok << "\n";
            simpanBarangKeFile();
            return;
        }
    }

    // Jika tidak ditemukan, input sebagai barang baru
    barang[jumlahbarang].id_barang = id_input;

    cout << "Masukkan nama barang: ";
    cin >> ws;
    getline(cin, barang[jumlahbarang].nama);

    cout << "Masukkan kategori barang: ";
    getline(cin, barang[jumlahbarang].kategori);

    do
    {
        cout << "Masukkan harga barang: ";
        cin >> barang[jumlahbarang].harga;
        if (barang[jumlahbarang].harga < 0)
        {
            cout << "Harga tidak boleh negatif!\n";
        }
    } while (barang[jumlahbarang].harga < 0);

    do
    {
        cout << "Masukkan stok barang: ";
        cin >> barang[jumlahbarang].stok;
        if (barang[jumlahbarang].stok < 0)
        {
            cout << "Stok tidak boleh negatif!\n";
        }
    } while (barang[jumlahbarang].stok < 0);

    jumlahbarang++;
    simpanBarangKeFile();
    cout << "Barang baru berhasil ditambahkan!\n";
}

void lihatbarang()
{
    system("cls");
    cout << "========================================\n";
    cout << "||            DAFTAR BARANG           ||\n";
    cout << "||         TOKO MUGI BERKAH           ||\n";
    cout << "========================================\n";

    if (jumlahbarang == 0)
    {
        cout << "||         Tidak ada barang!          ||\n";
        cout << "========================================\n";
        return;
    }

    // Pilihan pengurutan
    cout << "||         PILIH PENGURUTAN:          ||\n";
    cout << "========================================\n";
    cout << "|| 1. Urut berdasarkan ID (asc)       ||\n";
    cout << "|| 2. Urut berdasarkan Stok (asc)     ||\n";
    cout << "|| 3. Urut berdasarkan Stok (desc)    ||\n";
    cout << "|| 4. Tampilkan tanpa urutan          ||\n";
    cout << "========================================\n";
    cout << "|| Masukkan pilihan: ";

    int pilihan_urut;
    cin >> pilihan_urut;
    cout << "========================================\n";

    // Buat array sementara untuk pengurutan
    struct barang temp_barang[100];
    for (int i = 0; i < jumlahbarang; i++)
    {
        temp_barang[i] = barang[i];
    }

    // Lakukan pengurutan berdasarkan pilihan
    switch (pilihan_urut)
    {
    case 1: // Urut berdasarkan ID (ascending)
        for (int i = 0; i < jumlahbarang - 1; i++)
        {
            for (int j = 0; j < jumlahbarang - i - 1; j++)
            {
                if (temp_barang[j].id_barang > temp_barang[j + 1].id_barang)
                {
                    struct barang temp = temp_barang[j];
                    temp_barang[j] = temp_barang[j + 1];
                    temp_barang[j + 1] = temp;
                }
            }
        }
        cout << "||     DIURUTKAN BERDASARKAN ID       ||\n";
        break;

    case 2: // Urut berdasarkan Stok (ascending)
        for (int i = 0; i < jumlahbarang - 1; i++)
        {
            for (int j = 0; j < jumlahbarang - i - 1; j++)
            {
                if (temp_barang[j].stok > temp_barang[j + 1].stok)
                {
                    struct barang temp = temp_barang[j];
                    temp_barang[j] = temp_barang[j + 1];
                    temp_barang[j + 1] = temp;
                }
            }
        }
        cout << "||   DIURUTKAN BERDASARKAN STOK ASC   ||\n";
        break;

    case 3: // Urut berdasarkan Stok (descending)
        for (int i = 0; i < jumlahbarang - 1; i++)
        {
            for (int j = 0; j < jumlahbarang - i - 1; j++)
            {
                if (temp_barang[j].stok < temp_barang[j + 1].stok)
                {
                    struct barang temp = temp_barang[j];
                    temp_barang[j] = temp_barang[j + 1];
                    temp_barang[j + 1] = temp;
                }
            }
        }
        cout << "||  DIURUTKAN BERDASARKAN STOK DESC   ||\n";
        break;

    case 4: // Tanpa urutan
        cout << "||        TAMPILAN TANPA URUTAN       ||\n";
        break;

    default:
        cout << "||    PILIHAN TIDAK VALID! DEFAULT    ||\n";
        break;
    }

    cout << "========================================\n";
    cout << "ID\tNama\t\tKategori\tHarga\tStok\n";
    cout << "================================================\n";
    for (int i = 0; i < jumlahbarang; i++)
    {
        cout << temp_barang[i].id_barang << "\t" << temp_barang[i].nama << "\t\t"
             << temp_barang[i].kategori << "\t\t" << temp_barang[i].harga << "\t"
             << temp_barang[i].stok << endl;
    }
    cout << "================================================\n";
}

void caribarang()
{
    system("cls");
    cout << "=== CARI BARANG ===\n";
    string nama_barang;
    cout << "Masukkan nama barang yang dicari: ";
    cin >> ws;
    getline(cin, nama_barang);

    bool found = false;
    for (int i = 0; i < jumlahbarang; i++)
    {
        // Pencarian case-insensitive
        string nama_lower = barang[i].nama;
        string cari_lower = nama_barang;
        transform(nama_lower.begin(), nama_lower.end(), nama_lower.begin(), ::tolower);
        transform(cari_lower.begin(), cari_lower.end(), cari_lower.begin(), ::tolower);

        if (nama_lower.find(cari_lower) != string::npos)
        {
            cout << "ID: " << barang[i].id_barang << endl;
            cout << "Nama Barang: " << barang[i].nama << endl;
            cout << "Kategori: " << barang[i].kategori << endl;
            cout << "Harga: " << barang[i].harga << endl;
            cout << "Stok: " << barang[i].stok << endl;

            // Tanya mau tambah ke keranjang
            char tambah;
            cout << "Tambah ke keranjang? (y/n): ";
            cin >> tambah;
            if (tambah == 'y' || tambah == 'Y')
            {
                int jumlah;
                do
                {
                    cout << "Masukkan jumlah: ";
                    cin >> jumlah;
                    if (jumlah <= 0)
                    {
                        cout << "Jumlah harus lebih dari 0!\n";
                    }
                } while (jumlah <= 0);
                tambahKeKeranjang(barang[i].id_barang, jumlah);
            }
            cout << "------------------------\n";
            found = true;
        }
    }
    if (!found)
    {
        cout << "Barang tidak ditemukan!\n";
    }
}

void hapusbarang()
{
    system("cls");
    cout << "=== HAPUS BARANG ===\n";
    int id;
    cout << "Masukkan ID barang yang akan dihapus: ";
    cin >> id;

    int idx = -1;
    for (int i = 0; i < jumlahbarang; i++)
    {
        if (barang[i].id_barang == id)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        cout << "Barang tidak ditemukan.\n";
    }
    else
    {
        cout << "Barang yang akan dihapus: " << barang[idx].nama << endl;
        char konfirmasi;
        cout << "Yakin ingin menghapus? (y/n): ";
        cin >> konfirmasi;

        if (konfirmasi == 'y' || konfirmasi == 'Y')
        {
            for (int i = idx; i < jumlahbarang - 1; i++)
            {
                barang[i] = barang[i + 1];
            }
            jumlahbarang--;
            simpanBarangKeFile();
            cout << "Barang berhasil dihapus.\n";
        }
        else
        {
            cout << "Penghapusan dibatalkan.\n";
        }
    }
}

string blocking(string teks)
{
    int panjang = teks.length();
    string balok[4] = {"", "", "", ""};
    for (int i = 0; i < panjang; i++)
    {
        balok[i % 4] += teks[i];
    }
    return balok[0] + balok[1] + balok[2] + balok[3];
}

void registerUser()
{
    system("cls");
    cout << "=== REGISTER USER BARU ===\n";

    string username, role;

    // Validasi username dan tidak kosong
    bool username_exists;
    do
    {
        cout << "Masukkan username: ";
        cin >> username;

        if (username.empty())
        {
            cout << "Username tidak boleh kosong!\n";
            continue;
        }

        username_exists = false;
        for (int i = 0; i < jumlahuser; i++)
        {
            if (usernames[i] == username)
            {
                cout << "Username sudah ada! Gunakan username yang berbeda.\n";
                username_exists = true;
                break;
            }
        }
    } while (username_exists || username.empty());

    // Validasi role
    do
    {
        cout << "Masukkan role (manajer/kasir): ";
        cin >> role;
        transform(role.begin(), role.end(), role.begin(), ::tolower);
        if (role != "manajer" && role != "kasir")
        {
            cout << "Role harus 'manajer' atau 'kasir'!\n";
        }
    } while (role != "manajer" && role != "kasir");

    usernames[jumlahuser] = username;
    // Password otomatis dienkripsi dari username menggunakan blocking
    passwords[jumlahuser] = blocking(username);
    roles[jumlahuser] = role;
    jumlahuser++;

    simpanloginkefile();
    cout << "User berhasil didaftarkan!\n";
    cout << "Username: " << username << "\n";
    cout << "Password: " << blocking(username) << " (otomatis dari username)\n";
    cout << "Password telah dienkripsi untuk keamanan.\n";
}

void cariBarangID()
{
    system("cls");
    cout << "=== CARI BARANG BERDASARKAN ID ===\n";
    int id;
    cout << "Masukkan ID barang: ";
    cin >> id;

    bool found = false;
    for (int i = 0; i < jumlahbarang; i++)
    {
        if (barang[i].id_barang == id)
        {
            cout << "ID: " << barang[i].id_barang << endl;
            cout << "Nama Barang: " << barang[i].nama << endl;
            cout << "Kategori: " << barang[i].kategori << endl;
            cout << "Harga: " << barang[i].harga << endl;
            cout << "Stok: " << barang[i].stok << endl;

            // Tanya mau tambah ke keranjang
            char tambah;
            cout << "Tambah ke keranjang? (y/n): ";
            cin >> tambah;
            if (tambah == 'y' || tambah == 'Y')
            {
                int jumlah;
                do
                {
                    cout << "Masukkan jumlah: ";
                    cin >> jumlah;
                    if (jumlah <= 0)
                    {
                        cout << "Jumlah harus lebih dari 0!\n";
                    }
                } while (jumlah <= 0);
                tambahKeKeranjang(id, jumlah);
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        cout << "Barang tidak ditemukan!\n";
    }
}

int main()
{
    // Baca data dari file
    bacaLoginDariFile();
    bacaBarangDariFile();

    int pilihan;
    bool isRunning = true;

    while (isRunning)
    {
        system("cls");
        cout << "========================================\n";
        cout << "||      MANAJEMEN TOKO MUGI BERKAH     ||\n";
        cout << "========================================\n";
        cout << "||       BEKERJALAH DENGAN JUJUR       ||\n";
        cout << "========================================\n";
        cout << "||  1. Login                           ||\n";
        cout << "||  2. Register                        ||\n";
        cout << "||  3. Keluar                          ||\n";
        cout << "========================================\n";
        cout << "Masukkan pilihan: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        {
            if (jumlahuser == 0)
            {
                cout << "Belum ada user yang terdaftar. Silakan register terlebih dahulu.\n";
                system("pause");
                break;
            }

            string username, password;
            system("cls");
            cout << "========================================\n";
            cout << "||              LOGIN                 ||\n";
            cout << "||         TOKO MUGI BERKAH           ||\n";
            cout << "========================================\n";
            cout << "|| Username : ";
            cin >> username;
            cout << "|| Password : ";
            cin >> password;
            cout << "========================================\n";

            int index = -1;
            for (int i = 0; i < jumlahuser; i++)
            {

                if (usernames[i] == username && passwords[i] == blocking(username))
                {
                    index = i;
                    break;
                }
            }

            if (index != -1)
            {
                cout << "\nLogin berhasil sebagai " << roles[index] << "!\n";
                system("pause");

                if (roles[index] == "manajer")
                {
                    menuManajer();
                }
                else if (roles[index] == "kasir")
                {
                    menuKasir();
                }
                else
                {
                    cout << "Role tidak dikenali.\n";
                    system("pause");
                }
            }
            else
            {
                cout << "========================================\n";
                cout << "||          LOGIN GAGAL!              ||\n";
                cout << "|| Username atau password salah.     ||\n";
                cout << "========================================\n";

                cout << "|| DEBUG INFO:\n";
                cout << "|| Input username   : '" << username << "'\n";
                cout << "|| Input password   : '" << password << "'\n";
                cout << "|| Encrypted input  : '" << blocking(username) << "'\n";

                cout << "|| Daftar user tersimpan:\n";
                for (int i = 0; i < jumlahuser; i++)
                {
                    cout << "|| " << i + 1 << ". Username: '" << usernames[i] << "' | Password: '" << passwords[i] << "' | Role: '" << roles[i] << "'\n";
                }

                cout << "========================================\n";
                system("pause");
            }
            break;
        }

        case 2:
            registerUser();
            system("pause");
            break;

        case 3:
            system("cls");
            cout << "========================================\n";
            cout << "Terima kasih telah menggunakan aplikasi toko.\n";
            cout << "Semoga hari Anda menyenangkan!\n";
            cout << "========================================\n";
            isRunning = false;
            break;

        default:
            cout << "Pilihan tidak valid.\n";
            system("pause");
        }
    }

    return 0;
}