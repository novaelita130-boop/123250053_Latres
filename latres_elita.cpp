#include <iostream>
#include <string>

using namespace std;

struct NodeQueue {
    string namaPelanggan;
    NodeQueue* next;
};

struct MenuNode {
    string nama;
    string kategori;
    int harga;
    string status; 
    
    NodeQueue* front;
    NodeQueue* rear;
    
    MenuNode* kiri;
    MenuNode* kanan;
};


struct HistoryNode {
    int jenisAksi; 
    string namaMenu;
    string namaPelanggan;
    string statusLama;
    HistoryNode* next;
};

MenuNode* rootMenu = NULL;
HistoryNode* topUndo = NULL;


void enqueue(MenuNode* menu, string namaPelanggan) {
    NodeQueue* baru = new NodeQueue{namaPelanggan, NULL};
    if (menu->rear == NULL) {
        menu->front = menu->rear = baru;
    } else {
        menu->rear->next = baru;
        menu->rear = baru;
    }
}

string dequeue(MenuNode* menu) {
    if (menu->front == NULL) return "";
    NodeQueue* hapus = menu->front;
    string nama = hapus->namaPelanggan;
    menu->front = menu->front->next;
    if (menu->front == NULL) menu->rear = NULL;
    delete hapus;
    return nama;
}

void pushUndo(int jenis, string menu, string pelanggan = "", string statusLama = "") {
    HistoryNode* baru = new HistoryNode{jenis, menu, pelanggan, statusLama, topUndo};
    topUndo = baru;
}

HistoryNode popUndo() {
    if (topUndo == NULL) return {0, "", "", "", NULL};
    HistoryNode* hapus = topUndo;
    HistoryNode data = *hapus;
    topUndo = topUndo->next;
    delete hapus;
    return data;
}

MenuNode* insertMenu(MenuNode* root, string nama, string kategori, int harga) {
    if (root == NULL) {
        return new MenuNode{nama, kategori, harga, "Tersedia", NULL, NULL, NULL, NULL};
    }
    if (nama < root->nama) {
        root->kiri = insertMenu(root->kiri, nama, kategori, harga);
    } else if (nama > root->nama) {
        root->kanan = insertMenu(root->kanan, nama, kategori, harga);
    }
    return root;
}

MenuNode* cariMenu(MenuNode* root, string nama) {
    if (root == NULL || root->nama == nama) return root;
    if (nama < root->nama) return cariMenu(root->kiri, nama);
    return cariMenu(root->kanan, nama);
}

void tampilkanMenuInorder(MenuNode* root) {
    if (root != NULL) {
        tampilkanMenuInorder(root->kiri);
        cout << "Nama     : " << root->nama << "\n";
        cout << "Kategori : " << root->kategori << "\n";
        cout << "Harga    : " << root->harga << "\n";
        cout << "Status   : " << root->status << "\n";
        cout << "-------------------------\n";
        tampilkanMenuInorder(root->kanan);
    }
}

MenuNode* cariMinimum(MenuNode* root) {
    while (root->kiri != NULL) root = root->kiri;
    return root;
}

MenuNode* hapusMenuNode(MenuNode* root, string nama) {
    if (root == NULL) return NULL;

    if (nama < root->nama) {
        root->kiri = hapusMenuNode(root->kiri, nama);
    } else if (nama > root->nama) {
        root->kanan = hapusMenuNode(root->kanan, nama);
    } else {
        // Kosongkan antrean jika menu dihapus
        while (root->front != NULL) {
            dequeue(root);
        }

        if (root->kiri == NULL) {
            MenuNode* temp = root->kanan;
            delete root;
            return temp;
        } else if (root->kanan == NULL) {
            MenuNode* temp = root->kiri;
            delete root;
            return temp;
        }

        MenuNode* temp = cariMinimum(root->kanan);
        root->nama = temp->nama;
        root->kategori = temp->kategori;
        root->harga = temp->harga;
        root->status = temp->status;
        root->front = temp->front;
        root->rear = temp->rear;
        root->kanan = hapusMenuNode(root->kanan, temp->nama);
    }
    return root;
}


int main() {
    int pilihan;
    string nama, kategori, pelanggan;
    int harga;

    do {
        cout << "\n SISTEM RESTORAN \n";
        cout << "1. Tambah Menu Makanan\n";
        cout << "2. Lihat Daftar Menu\n";
        cout << "3. Catat Pesanan Pelanggan\n";
        cout << "4. Restock Menu (Tersedia Kembali)\n";
        cout << "5. Hapus Menu\n";
        cout << "6. Undo Aksi Terakhir\n";
        cout << "7. Lihat Antrian Pelanggan (Waiting List)\n";
        cout << "8. Keluar Program\n";
        cout << "\nPilihan: ";
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1: {
                cout << "Masukkan Nama Menu: ";
                getline(cin, nama);
                cout << "Masukkan Kategori Makanan: ";
                getline(cin, kategori);
                cout << "Masukkan Harga: ";
                cin >> harga;
                cin.ignore();

                rootMenu = insertMenu(rootMenu, nama, kategori, harga);
                cout << "Menu berhasil ditambahkan!!\n";
                break;
            }
            case 2: {
                if (rootMenu == NULL) {
                    cout << "Belum ada menu di restoran.\n";
                } else {
                    cout << "\nDAFTAR MENU RESTORAN\n";
                    tampilkanMenuInorder(rootMenu);
                }
                break;
            }
            case 3: {
                cout << "Masukkan Nama Pelanggan: ";
                getline(cin, pelanggan);
                cout << "Masukkan Nama Menu yang Dipesan: ";
                getline(cin, nama);

                MenuNode* menu = cariMenu(rootMenu, nama);
                if (menu == NULL) {
                    cout << "Menu tidak ditemukan!\n";
                } else if (menu->status == "Tersedia") {
                    cout << "Pesanan " << pelanggan << " berhasil dibuat\n";
                    
                    menu->status = "Habis";
                    
                    pushUndo(1, nama, pelanggan);
                } else {
                    cout << "Menu sedang habis, " << pelanggan << " masuk ke dalam waiting list\n";
                    enqueue(menu, pelanggan);
                    pushUndo(1, nama, pelanggan);
                }
                break;
            }
            case 4: {
                cout << "Masukkan Nama Menu yang Tersedia Kembali: ";
                getline(cin, nama);

                MenuNode* menu = cariMenu(rootMenu, nama);
                if (menu == NULL) {
                    cout << "Menu tidak ditemukan!\n";
                } else {
                    string statusLama = menu->status;
                    menu->status = "Tersedia";
                    cout << "Menu tersedia kembali\n";
                    pushUndo(2, nama, "", statusLama);

                    if (menu->front != NULL) {
                        string pelangganAntrian = dequeue(menu);
                        cout << "Pesanan langsung diberikan kepada " << pelangganAntrian << " dari waiting list\n";
                    }
                }
                break;
            }
            case 5: {
                cout << "Masukkan Nama Menu yang Ingin Dihapus: ";
                getline(cin, nama);
                MenuNode* menu = cariMenu(rootMenu, nama);
                if (menu == NULL) {
                    cout << "Menu tidak ditemukan!\n";
                } else {
                    rootMenu = hapusMenuNode(rootMenu, nama);
                    cout << "Menu berhasil dihapus\n";
                }
                break;
            }
            case 6: {
                if (topUndo == NULL) {
                    cout << "Tidak ada aksi untuk di-undo\n";
                } else {
                    HistoryNode aksi = popUndo();
                    if (aksi.jenisAksi == 1) {
                        cout << "Undo: Pesanan menu " << aksi.namaMenu << " dibatalkan\n";
                    } else if (aksi.jenisAksi == 2) {
                        MenuNode* menu = cariMenu(rootMenu, aksi.namaMenu);
                        if (menu != NULL) {
                            menu->status = aksi.statusLama;
                        }
                        cout << "Undo: Menu " << aksi.namaMenu << " kembali habis\n";
                    }
                }
                break;
            }
            case 7: {
                cout << "Masukkan Nama Menu untuk Cek Antrian: ";
                getline(cin, nama);
                MenuNode* menu = cariMenu(rootMenu, nama);
                if (menu == NULL) {
                    cout << "Menu tidak ditemukan!\n";
                } else if (menu->front == NULL) {
                    cout << "Antrian kosong\n";
                } else {
                    NodeQueue* bantu = menu->front;
                    while (bantu != NULL) {
                        cout << bantu->namaPelanggan << " menunggu menu " << nama << "\n";
                        bantu = bantu->next;
                    }
                }
                break;
            }
            case 0:
                cout << "Keluar dari program. Terima kasih!\n";
                break;
            default:
                cout << "Pilihan menu tidak valid!\n";
        }
    } while (pilihan != 0);

    return 0;
}