#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define MAKSIMUM_ISIM 50
#define MAKSIMUM_ISBN 14
#define MAKSIMUM_TARIH 11

typedef struct Yazar {
    int yazarID;
    char ad[MAKSIMUM_ISIM];
    char soyad[MAKSIMUM_ISIM];
    struct Yazar* next;
} Yazar;

typedef struct Ogrenci {
    char ad[MAKSIMUM_ISIM];
    char soyad[MAKSIMUM_ISIM];
    char ogrenciNo[9];
    int puan;
    struct Ogrenci* next;
    struct Ogrenci* prev;
} Ogrenci;

typedef struct KitapOrnegi {
    char etiketNo[20];
    char durum[20];
    struct KitapOrnegi* next;
} KitapOrnegi;

typedef struct Kitap {
    char kitapAdi[MAKSIMUM_ISIM];
    char isbn[MAKSIMUM_ISBN];
    int adet;
    KitapOrnegi* ornekler;
    struct Kitap* next;
} Kitap;

typedef struct Hareket {
    char ogrenciNo[9];
    char etiketNo[20];
    int islemTuru;
    char tarih[MAKSIMUM_TARIH];
} Hareket;

//fonksiyonlar
void menu();
void yazar_menu();
void ogrenci_menu();
void kitap_menu();
void yazar_dosya_okuma();
void yazar_dosya_yaz();
void yazar_ekle();
void yazar_sil();
void yazar_guncelle();
void yazarlari_yazdir();
void ogrenci_dosya_oku();
void ogrenci_dosya_yaz();
void ogrenci_ekle();
void ogrenci_sil();
void ogrenci_guncelle();
void ogrencileri_yazdir();
void ogrenci_detay();
void cezali_ogrenciler();
void teslim_etmeyenler();
void kitap_dosya_oku();
void kitap_dosya_yaz();
void kitap_ekle();
void kitap_sil();
void kitap_guncelle();
void kitaplari_yazdir();
void kitap_detay();
void raftaki_kitaplar();
void kitap_yazar_esle();
void kitap_yazar_guncelle();
void kitap_odunc();
void kitap_teslim();
void tum_hareketler();
int gun_farki(const char* tarih1, const char* tarih2);
void bugunun_tarihi(char* buffer);

Yazar* yazarListesi = NULL;
Ogrenci* ogrenciListesi = NULL;
Kitap* kitapListesi = NULL;

//ana fonksiyon
int main() {
    menu();
    return 0;
}

//gun farki icin
int gun_farki(const char* tarih1, const char* tarih2) {
    struct tm zaman1 = {0}, zaman2 = {0};
    sscanf(tarih1, "%d-%d-%d", &zaman1.tm_year, &zaman1.tm_mon, &zaman1.tm_mday);
    sscanf(tarih2, "%d-%d-%d", &zaman2.tm_year, &zaman2.tm_mon, &zaman2.tm_mday);
    zaman1.tm_year -= 1900; zaman1.tm_mon -= 1;
    zaman2.tm_year -= 1900; zaman2.tm_mon -= 1;
    time_t zaman_1 = mktime(&zaman1);
    time_t zaman_2 = mktime(&zaman2);
    double fark = difftime(zaman_2, zaman_1);
    return (int)(fark / (60 * 60 * 24));
}

void bugunun_tarihi(char* buffer) {
    time_t zaman = time(NULL);
    struct tm yerelZaman = *localtime(&zaman);
    sprintf(buffer, "%04d-%02d-%02d", yerelZaman.tm_year + 1900, yerelZaman.tm_mon + 1, yerelZaman.tm_mday);
}

// menu fonksiyonu
void menu() {
    yazar_dosya_okuma();
    ogrenci_dosya_oku();
    kitap_dosya_oku();

    int secim;
    do {
        printf("\n--- KUTUPHANE SISTEMI ---\n");
        printf("1. Yazar Islemleri\n");
        printf("2. Ogrenci Islemleri\n");
        printf("3. Kitap Islemleri\n");
        printf("4. Kitap ve Yazar Eslestirme\n");
        printf("5. Kitap Odunc Al\n");
        printf("6. Kitap Teslim Etme\n");
        printf("7. Tum Kitap Hareketlerini Yazdir\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        //fflush(stdout);
        scanf("%d", &secim);

        switch (secim) {
            case 1: yazar_menu(); break;
            case 2: ogrenci_menu(); break;
            case 3: kitap_menu(); break;
            case 4: kitap_yazar_esle(); break;
            case 5: kitap_odunc(); break;
            case 6: kitap_teslim(); break;
            case 7: tum_hareketler(); break;
            case 0: printf("Programdan cikiliyor...\n"); break;
            default: printf("Gecersiz secim.\n");
        }
        fflush(stdout);
    } while (secim != 0);
}

//yazar kontrol ediyoruz
int yazarVarMi(int id) {
    Yazar* yazar = yazarListesi;
    while (yazar) {
        if (yazar->yazarID == id)
            return 1;
        yazar = yazar->next;
    }
    return 0;
}

typedef struct KitapYazar {
    char isbn[14];
    int* yazarKimlikListesi;
    int yazarSayisi;
} KitapYazar;

void kitap_sil() {
    char isbn[14];
    printf("ISBN numarasi(silmek icin): ");
    scanf("%s", isbn);

    Kitap* iter = kitapListesi;
    Kitap* onceki = NULL;

    while (iter && strcmp(iter->isbn, isbn) != 0) {
        onceki = iter;
        iter = iter->next;
    }

    if (!iter) {
        printf("Boyle bir kitap yok\n");
        return;
    }

    if (onceki == NULL)
        kitapListesi = iter->next;
    else
        onceki->next = iter->next;

    KitapOrnegi* ornek = iter->ornekler;
    while (ornek) {
        KitapOrnegi* silinecek = ornek;
        ornek = ornek->next;
        free(silinecek);
    }

    free(iter);

    char dosyaAdi[50];
    sprintf(dosyaAdi, "KitapOrnekleri_%s.csv", isbn);
    if (remove(dosyaAdi) == 0)
        printf("Kitap ornek dosyasi silindi.\n");

    kitap_dosya_yaz();
    printf("Kitap silindi.\n");
}

int ogrenciNumarasiVarMi(const char* ogrenciNumarasi) {
    Ogrenci* iter = ogrenciListesi;
    while (iter) {
        if (strcmp(iter->ogrenciNo, ogrenciNumarasi) == 0)
            return 1; 
        iter = iter->next;
    }
    return 0; 
}

//ogrenci eklemek icin yer acan ve islemleri uygulayan fonksiyon
void ogrenci_ekle() {
    Ogrenci* yeniOgrenci = (Ogrenci*)malloc(sizeof(Ogrenci));
    printf("Ad: ");
    scanf("%s", yeniOgrenci->ad);
    printf("Soyad: ");
    scanf("%s", yeniOgrenci->soyad);

    char ogrenciNumarasi[9];
    do {
        printf("Ogrenci Numarasi(8 hane): ");
        scanf("%s", ogrenciNumarasi);
        if (strlen(ogrenciNumarasi) != 8)
            printf("Numara 8 haneli olmalidir\n");
        else if (ogrenciNumarasiVarMi(ogrenciNumarasi))
            printf("Bu numara zaten var\n");
    } while (strlen(ogrenciNumarasi) != 8 || ogrenciNumarasiVarMi(ogrenciNumarasi));

    strcpy(yeniOgrenci->ogrenciNo, ogrenciNumarasi);
    yeniOgrenci->puan = 100;
    yeniOgrenci->next = NULL;
    yeniOgrenci->prev = NULL;

    
    if (ogrenciListesi == NULL) {
        ogrenciListesi = yeniOgrenci;
    } else {
        Ogrenci* iter = ogrenciListesi;
        while (iter->next)
            iter = iter->next;
        iter->next = yeniOgrenci;
        yeniOgrenci->prev = iter;
    }

    ogrenci_dosya_yaz();
    printf("Ogrenci eklendi.\n");
}

//cezali ogrencileri bulur ve yazar
void cezali_ogrenciler() {
    FILE* dosya = fopen("Hareketler.csv", "r");
    if (!dosya) {
        printf("Hareketler.csv bulunamadi.\n");
        return;
    }

    Hareket hareketler[1000]; 
    int sayac = 0;
    char satir[200];

    while (fgets(satir, sizeof(satir), dosya)) {
        sscanf(satir, "%[^,],%[^,],%d,%s",
               hareketler[sayac].ogrenciNo,
               hareketler[sayac].etiketNo,
               &hareketler[sayac].islemTuru,
               hareketler[sayac].tarih);
        sayac++;
    }
    fclose(dosya);

    int bulundu = 0;

    for (int i = 0; i < sayac; i++) {
        if (hareketler[i].islemTuru == 0) { 
            for (int j = i + 1; j < sayac; j++) {
                if (hareketler[j].islemTuru == 1 &&
                    strcmp(hareketler[i].ogrenciNo, hareketler[j].ogrenciNo) == 0 &&
                    strcmp(hareketler[i].etiketNo, hareketler[j].etiketNo) == 0) {

                    int fark = gun_farki(hareketler[i].tarih, hareketler[j].tarih);
                    if (fark > 15) {
                        
                        Ogrenci* ogrenci = ogrenciListesi;
                        while (ogrenci) {
                            if (strcmp(ogrenci->ogrenciNo, hareketler[i].ogrenciNo) == 0) {
                                printf("Cezali Ogrenci: %s %s | No: %s | Kitap: %s | Gecikme: %d gun\n",
                                       ogrenci->ad, ogrenci->soyad, ogrenci->ogrenciNo, hareketler[i].etiketNo, fark);
                                bulundu = 1;
                                break;
                            }
                            ogrenci = ogrenci->next;
                        }
                    }

                    break;
                }
            }
        }
    }

    if (!bulundu)
        printf("Cezali ogrenci bulunamadi.\n");
}

//kitabi arar, bulamazsa hata verir, bulursa adini, numarasini ve adedini yazdirir
void kitap_detay() {
    if (!kitapListesi) {
        printf("Kutuphanede kitap yok\n");
        return;
    }

    char arananAd[50];
    printf("Aranacak kitabin adi: ");
    scanf(" %[^\n]", arananAd);

    Kitap* kitap = kitapListesi;
    int bulundu = 0;

    while (kitap) {
        if (strcmp(kitap->kitapAdi, arananAd) == 0) {
            printf("\nKitap Bulundu:\n");
            printf("   Kitap Adi: %s\n", kitap->kitapAdi);
            printf("   ISBN: %s\n", kitap->isbn);
            printf("   Adet: %d\n", kitap->adet);

            KitapOrnegi* ornek = kitap->ornekler;
            int siraNumarasi = 1;
            while (ornek) {
                printf("      %d) Etiket: %s | Durum: %s\n", siraNumarasi++, ornek->etiketNo, ornek->durum);
                ornek = ornek->next;
            }

            bulundu = 1;
            break;
        }
        kitap = kitap->next;
    }

    if (!bulundu) {
        printf("'%s' adli kitap bulunamadi.\n", arananAd);
    }
}

//dosyadan okur
void kitap_dosya_oku() {
    FILE* dosya = fopen("Kitaplar.csv", "r");
    if (!dosya) return;

    char satir[200];
    while (fgets(satir, sizeof(satir), dosya)) {
        Kitap* yeniKitap = (Kitap*)malloc(sizeof(Kitap));
        sscanf(satir, "%[^,],%[^,],%d", yeniKitap->kitapAdi, yeniKitap->isbn, &yeniKitap->adet);
        yeniKitap->next = NULL;
        yeniKitap->ornekler = NULL;

        
        char ornekDosyaAdi[50];
        sprintf(ornekDosyaAdi, "KitapOrnekleri_%s.csv", yeniKitap->isbn);
        FILE* ornekDosyasi = fopen(ornekDosyaAdi, "r");
        if (ornekDosyasi) {
            KitapOrnegi* sonOrnek = NULL;
            char ornekSatiri[100];
            while (fgets(ornekSatiri, sizeof(ornekSatiri), ornekDosyasi)) {
                KitapOrnegi* kitapOrnegi = (KitapOrnegi*)malloc(sizeof(KitapOrnegi));
                sscanf(ornekSatiri, "%[^,],%[^\n]", kitapOrnegi->etiketNo, kitapOrnegi->durum);
                kitapOrnegi->next = NULL;
                if (!yeniKitap->ornekler)
                    yeniKitap->ornekler = kitapOrnegi;
                else
                    sonOrnek->next = kitapOrnegi;
                sonOrnek = kitapOrnegi;
            }
            fclose(ornekDosyasi);
        }
        yeniKitap->next = kitapListesi;
        kitapListesi = yeniKitap;
    }

    fclose(dosya);
}

//dosyaya yazar
void kitap_dosya_yaz() {
    FILE* dosya = fopen("Kitaplar.csv", "w");
    if (!dosya) return;

    Kitap* kitap = kitapListesi;
    while (kitap) {
        fprintf(dosya, "%s,%s,%d\n", kitap->kitapAdi, kitap->isbn, kitap->adet);

        char ornekDosyaAdi[50];
        sprintf(ornekDosyaAdi, "KitapOrnekleri_%s.csv", kitap->isbn);
        FILE* ornekDosyasi = fopen(ornekDosyaAdi, "w");
        KitapOrnegi* ornek = kitap->ornekler;
        while (ornek) {
            fprintf(ornekDosyasi, "%s,%s\n", ornek->etiketNo, ornek->durum);
            ornek = ornek->next;
        }
        fclose(ornekDosyasi);

        kitap = kitap->next;
    }

    fclose(dosya);
}

//kitabi eklemek icin adini numarasini ve adedini ister ve ekler
void kitap_ekle() {
    Kitap* yeniKitap = (Kitap*)malloc(sizeof(Kitap));
    printf("Kitap Adi: ");
    scanf(" %[^\n]", yeniKitap->kitapAdi);
    printf("13 haneli ISBN: ");
    scanf("%s", yeniKitap->isbn);
    printf("Adet (kac ornek): ");
    scanf("%d", &yeniKitap->adet);

    yeniKitap->ornekler = NULL;
    yeniKitap->next = NULL;

    for (int i = 1; i <= yeniKitap->adet; i++) {
        KitapOrnegi* kitapOrnegi = (KitapOrnegi*)malloc(sizeof(KitapOrnegi));
        sprintf(kitapOrnegi->etiketNo, "%s_%d", yeniKitap->isbn, i);
        strcpy(kitapOrnegi->durum, "Rafta");
        kitapOrnegi->next = NULL;

        if (!yeniKitap->ornekler) {
            yeniKitap->ornekler = kitapOrnegi;
        } else {
            KitapOrnegi* temp = yeniKitap->ornekler;
            while (temp->next)
                temp = temp->next;
            temp->next = kitapOrnegi;
        }
    }

    yeniKitap->next = kitapListesi;
    kitapListesi = yeniKitap;

    kitap_dosya_yaz();
    printf("Kitap ve ornekleri basariyla eklendi.\n");
}

//kitabin numarasi uzerinden arar ve gunceller
void kitap_guncelle() {
    char isbn[14];
    printf("ISBN numarasi(guncellemek icin): ");
    scanf("%s", isbn);

    Kitap* kitap = kitapListesi;
    while (kitap && strcmp(kitap->isbn, isbn) != 0)
        kitap = kitap->next;

    if (!kitap) {
        printf("Boyle bir kitap yok\n");
        return;
    }

    printf("Yeni Kitap Adi (onceki: %s): ", kitap->kitapAdi);
    scanf(" %[^\n]", kitap->kitapAdi);

    printf("Yeni Adet Bilgisi (onceki: %d): ", kitap->adet);
    int yeniAdet;
    scanf("%d", &yeniAdet);

    if (yeniAdet < kitap->adet) {
        printf("HATA: Mevcut ornek sayisi %d. Daha az sayiya dusulemez.\n", kitap->adet);
        return;
    }

    for (int i = kitap->adet + 1; i <= yeniAdet; i++) {
        KitapOrnegi* yeniOrnek = (KitapOrnegi*)malloc(sizeof(KitapOrnegi));
        sprintf(yeniOrnek->etiketNo, "%s_%d", kitap->isbn, i);
        strcpy(yeniOrnek->durum, "Rafta");
        yeniOrnek->next = NULL;

        KitapOrnegi* iter = kitap->ornekler;
        while (iter->next)
            iter = iter->next;
        iter->next = yeniOrnek;
    }

    kitap->adet = yeniAdet;

    kitap_dosya_yaz();
    printf("Kitap bilgileri guncellendi\n");
}

//kitaplari yazdirir
void kitaplari_yazdir() {
    if (kitapListesi == NULL) {
        printf("Kayitli kitap yok\n");
        return;
    }

    printf("\n<<<<< KUTUPHANEDEKI KITAPLAR >>>>>>\n");

    Kitap* kitap = kitapListesi;
    while (kitap) {
        printf("\nKitap Adi: %s\n", kitap->kitapAdi);
        printf("   ISBN: %s\n", kitap->isbn);
        printf("   Adet: %d\n", kitap->adet);

        KitapOrnegi* ornek = kitap->ornekler;
        int siraNumarasi = 1;
        while (ornek) {
            printf("      %d) Etiket: %s | Durum: %s\n", siraNumarasi++, ornek->etiketNo, ornek->durum);
            ornek = ornek->next;
        }

        kitap = kitap->next;
    }
}

//odunc alma fonksiyonu ogrenci no uzerinden calisiyor
void kitap_odunc() {
    char ogrenciNumarasi[9];
    char kitapAdi[50];
    printf("Ogrenci Numarasi(odunc almak icin): ");
    scanf("%s", ogrenciNumarasi);

    Ogrenci* ogrenci = ogrenciListesi;
    while (ogrenci && strcmp(ogrenci->ogrenciNo, ogrenciNumarasi) != 0) 
        ogrenci = ogrenci->next;

    if (!ogrenci) {
        printf("Ogrenci bulunamadi.\n");
        return;
    }

    if (ogrenci->puan <= 0) {
        printf("Ogrencinin puani yetersiz! Kitap alamaz.\n");
        return;
    }

    printf("Kitap Adi: ");
    scanf(" %[^\n]", kitapAdi);

    Kitap* kitap = kitapListesi;
    while (kitap && strcmp(kitap->kitapAdi, kitapAdi) != 0) 
        kitap = kitap->next;

    if (!kitap) {
        printf("Kitap bulunamadi.\n");
        return;
    }

    KitapOrnegi* kitapOrnegi = kitap->ornekler;
    while (kitapOrnegi && strcmp(kitapOrnegi->durum, "Rafta") != 0) 
        kitapOrnegi = kitapOrnegi->next;

    if (!kitapOrnegi) {
        printf("Tum ornekler baska ogrencide. Islem basarisiz.\n");
        return;
    }

    strcpy(kitapOrnegi->durum, ogrenciNumarasi);

    FILE* dosya = fopen("Hareketler.csv", "a");
    if (!dosya) {
        printf("Hareketler.csv dosyasi acilamadi!\n");
        return;
    }

    char tarih[11];
    bugunun_tarihi(tarih);

    fprintf(dosya, "%s,%s,0,%s\n", ogrenciNumarasi, kitapOrnegi->etiketNo, tarih);
    fclose(dosya);

    kitap_dosya_yaz();
    ogrenci_dosya_yaz();
    printf("Kitap odunc verildi. Etiket: %s\n", kitapOrnegi->etiketNo);
}

//kitap no uzerinen teslim alir ve ogrenciden siler
void kitap_teslim() {
    char etiketNumarasi[20];
    printf("Teslim edilecek kitap etiket numarasi: ");
    scanf("%s", etiketNumarasi);

    Kitap* kitap = kitapListesi;
    KitapOrnegi* kitapOrnegi = NULL;

    while (kitap && !kitapOrnegi) {
        KitapOrnegi* ornek = kitap->ornekler;
        while (ornek) {
            if (strcmp(ornek->etiketNo, etiketNumarasi) == 0) {
                kitapOrnegi = ornek;
                break;
            }
            ornek = ornek->next;
        }
        if (!kitapOrnegi) kitap = kitap->next;
    }

    if (!kitapOrnegi || strcmp(kitapOrnegi->durum, "Rafta") == 0) {
        printf("Bu kitap zaten rafta.\n");
        return;
    }

    char ogrenciNumarasi[9];
    strcpy(ogrenciNumarasi, kitapOrnegi->durum);

    FILE* dosya = fopen("Hareketler.csv", "r");
    Hareket oduncHareket = {0};
    char satir[200];
    while (fgets(satir, sizeof(satir), dosya)) {
        Hareket hareket;
        sscanf(satir, "%[^,],%[^,],%d,%s", hareket.ogrenciNo, hareket.etiketNo, &hareket.islemTuru, hareket.tarih);
        if (hareket.islemTuru == 0 &&
            strcmp(hareket.ogrenciNo, ogrenciNumarasi) == 0 &&
            strcmp(hareket.etiketNo, etiketNumarasi) == 0) {
            oduncHareket = hareket;
        }
    }
    fclose(dosya);

    char tarih[11];
    bugunun_tarihi(tarih);

    int fark = gun_farki(oduncHareket.tarih, tarih);
    Ogrenci* ogrenci = ogrenciListesi;
    while (ogrenci && strcmp(ogrenci->ogrenciNo, ogrenciNumarasi) != 0) 
        ogrenci = ogrenci->next;

    if (fark > 15 && ogrenci) {
        ogrenci->puan -= 10;
        printf("Kitap gec teslim edildi. Ogrenciye -10 ceza puani verildi.\n");
    }

    strcpy(kitapOrnegi->durum, "Rafta");

    dosya = fopen("Hareketler.csv", "a");
    fprintf(dosya, "%s,%s,1,%s\n", ogrenciNumarasi, etiketNumarasi, tarih);
    fclose(dosya);

    kitap_dosya_yaz();
    ogrenci_dosya_yaz();
    printf("Kitap teslim edildi.\n");
}

//kitap ile yazari eslestirmek icin kitabin nosu ile yazar nosunu ister ve esler
void kitap_yazar_esle() {
    char isbn[14];
    printf("Eslestirilecek kitabin ISBN numarasi: ");
    scanf("%s", isbn);

    Kitap* kitap = kitapListesi;
    while (kitap && strcmp(kitap->isbn, isbn) != 0)
        kitap = kitap->next;

    if (!kitap) {
        printf("Bu ISBN numarasina sahip bir kitap yok\n");
        return;
    }

    int yazarAdedi;
    printf("Kac yazar ile eslestirme yapilacak?\n ");
    printf(">>");
    scanf("%d", &yazarAdedi);

    if (yazarAdedi <= 0) {
        printf("negatif sayi girilemez\n");
        return;
    }

    int* yazarlar = (int*)malloc(sizeof(int) * yazarAdedi);

    for (int i = 0; i < yazarAdedi; i++) {
        printf("%d. Yazar ID: ", i + 1);
        scanf("%d", &yazarlar[i]);

        if (!yazarVarMi(yazarlar[i])) {
            printf("Yazar ID %d sistemde yok. Eslestirme iptal edildi.\n", yazarlar[i]);
            free(yazarlar);
            return;
        }
    }

    FILE* dosya = fopen("KitapYazar.csv", "a");
    if (!dosya) {
        printf("KitapYazar.csv dosyasi acilamadi!\n");
        free(yazarlar);
        return;
    }

    fprintf(dosya, "%s", isbn);
    for (int i = 0; i < yazarAdedi; i++) {
        fprintf(dosya, ",%d", yazarlar[i]);
    }
    fprintf(dosya, "\n");
    fclose(dosya);

    free(yazarlar);
    printf("Kitap-yazar eslestirmesi kaydedildi\n");
}

//kitabin yazarini guncellemek uzere calisir
void kitap_yazar_guncelle() {
    char isbn[14];
    printf("ISBN numarasi(yazari guncellemek icin): ");
    scanf("%s", isbn);

    Kitap* kitap = kitapListesi;
    while (kitap && strcmp(kitap->isbn, isbn) != 0)
        kitap = kitap->next;

    if (!kitap) {
        printf("Bu ISBN numarasina sahip bir kitap yok\n");
        return;
    }

    FILE* dosya = fopen("KitapYazar.csv", "r");
    if (!dosya) {
        printf("KitapYazar.csv bulunamadi.\n");
        return;
    }

    char** satirlar = NULL;
    int satirSayisi = 0;
    char satir[256];

    while (fgets(satir, sizeof(satir), dosya)) {
        satirlar = realloc(satirlar, sizeof(char*) * (satirSayisi + 1));
        satirlar[satirSayisi] = strdup(satir);
        satirSayisi++;
    }

    fclose(dosya);

    int yazarSayisi;
    printf("Yeni kac yazar eslestirilecek?\n ");
    printf(">>");
    scanf("%d", &yazarSayisi);

    if (yazarSayisi <= 0) {
        printf("negatif sayi girilemez\n");
        return;
    }

    int* yazarlar = malloc(sizeof(int) * yazarSayisi);
    for (int i = 0; i < yazarSayisi; i++) {
        printf("%d. Yazar ID: ", i + 1);
        scanf("%d", &yazarlar[i]);

        if (!yazarVarMi(yazarlar[i])) {
            printf("Yazar ID %d sistemde yok. Guncelleme iptal edildi.\n", yazarlar[i]);
            for (int satirIndeksi = 0; satirIndeksi < satirSayisi; satirIndeksi++) 
                free(satirlar[satirIndeksi]);
            free(satirlar);
            return;
        }
    }

    
    char yeniSatir[256];
    snprintf(yeniSatir, sizeof(yeniSatir), "%s", isbn);
    for (int i = 0; i < yazarSayisi; i++) {
        char ekleme[20];
        sprintf(ekleme, ",%d", yazarlar[i]);
        strcat(yeniSatir, ekleme);
    }
    strcat(yeniSatir, "\n");

    
    FILE* yeniDosya = fopen("KitapYazar.csv", "w");
    for (int i = 0; i < satirSayisi; i++) {
        if (strncmp(satirlar[i], isbn, strlen(isbn)) == 0 &&
            satirlar[i][strlen(isbn)] == ',') {
            fputs(yeniSatir, yeniDosya);
        } else {
            fputs(satirlar[i], yeniDosya);
        }
        free(satirlar[i]);
    }

    fclose(yeniDosya);
    free(satirlar);
    free(yazarlar);

    printf("Kitap-yazar iliskisi guncellendi.\n");
}

//ogrencinin adi veya nosu ile arama yapar, hareket dokumunu yazdirir
void ogrenci_detay() {
    char ogrenciNumarasi[9];
    char ad[MAKSIMUM_ISIM], soyad[MAKSIMUM_ISIM];
    Ogrenci* bulunanOgrenci = NULL;

    printf("1. Numara ile arama\n");
    printf("2. Ad ve soyad ile arama\n");
    int secim;
    printf("Seciminiz: ");
    scanf("%d", &secim);

    if (secim == 1) {
        printf("Ogrenci Numarasi: ");
        scanf("%s", ogrenciNumarasi);
        Ogrenci* iter = ogrenciListesi;
        while (iter) {
            if (strcmp(iter->ogrenciNo, ogrenciNumarasi) == 0) {
                bulunanOgrenci = iter;
                break;
            }
            iter = iter->next;
        }
    } else if (secim == 2) {
        printf("Ad: ");
        scanf("%s", ad);
        printf("Soyad: ");
        scanf("%s", soyad);
        Ogrenci* iter = ogrenciListesi;
        while (iter) {
            if (strcmp(iter->ad, ad) == 0 && strcmp(iter->soyad, soyad) == 0) {
                bulunanOgrenci = iter;
                break;
            }
            iter = iter->next;
        }
    }

    if (!bulunanOgrenci) {
        printf("Ogrenci bulunamadi.\n");
        return;
    }

   
    printf("\n--- OGRENCI BILGISI ---\n");
    printf("Ad: %s\nSoyad: %s\nNumara: %s\nPuan: %d\n",
           bulunanOgrenci->ad, bulunanOgrenci->soyad, bulunanOgrenci->ogrenciNo, bulunanOgrenci->puan);

    
    FILE* dosya = fopen("Hareketler.csv", "r");
    if (!dosya) {
        printf("Hareketler.csv dosyasi bulunamadi.\n");
        return;
    }

    char satir[200];
    int bulundu = 0;

    printf("\n--- KITAP HAREKETLERI ---\n");
    while (fgets(satir, sizeof(satir), dosya)) {
        Hareket hareket;
        sscanf(satir, "%[^,],%[^,],%d,%s", hareket.ogrenciNo, hareket.etiketNo, &hareket.islemTuru, hareket.tarih);
        if (strcmp(hareket.ogrenciNo, bulunanOgrenci->ogrenciNo) == 0) {
            printf("Etiket: %s | Islem: %s | Tarih: %s\n",
                   hareket.etiketNo,
                   hareket.islemTuru == 0 ? "ODUNC ALMA" : "TESLIM",
                   hareket.tarih);
            bulundu = 1;
        }
    }

    if (!bulundu) {
        printf("Bu ogrenciye ait kayit bulunamadi.\n");
    }

    fclose(dosya);
}

//ogrencinin dosyasinin okunmasi icindir
void ogrenci_dosya_oku() {
    FILE* dosya = fopen("Ogrenciler.csv", "r");
    if (!dosya) {
        printf("Ogrenciler.csv dosyasi acilamadi.\n");
        return;
    }

    char satir[200];
    while (fgets(satir, sizeof(satir), dosya)) {
        Ogrenci* yeniOgrenci = (Ogrenci*)malloc(sizeof(Ogrenci));
        sscanf(satir, "%[^,],%[^,],%[^,],%d",
               yeniOgrenci->ad, yeniOgrenci->soyad, yeniOgrenci->ogrenciNo, &yeniOgrenci->puan);
        
        yeniOgrenci->next = NULL;
        yeniOgrenci->prev = NULL;

    
        if (!ogrenciListesi) {
            ogrenciListesi = yeniOgrenci;
        } else {
            Ogrenci* iter = ogrenciListesi;
            while (iter->next)
                iter = iter->next;
            iter->next = yeniOgrenci;
            yeniOgrenci->prev = iter;
        }
    }

    fclose(dosya);
}

//yazarin dosyasinin okunmasi icindir
void yazar_dosya_okuma() {
    FILE* dosya = fopen("yazarlar.csv", "r");
    if (!dosya) {
        printf("yazarlar.csv dosyasi acilamadi.\n");
        return;
    }

    char satir[200];
    while (fgets(satir, sizeof(satir), dosya)) {
        Yazar* yeniYazar = (Yazar*)malloc(sizeof(Yazar));
        sscanf(satir, "%d,%[^,],%[^\n]",
               &yeniYazar->yazarID, yeniYazar->ad, yeniYazar->soyad);
        
        yeniYazar->next = NULL;

        yeniYazar->next = yazarListesi;
        yazarListesi = yeniYazar;
    }

    fclose(dosya);
}

//yazar islemleri icin menu
void yazar_menu() {
    int secim;
    do {
        printf("\n--- YAZAR ISLEMLERI ---\n");
        printf("1. Yazar Ekle\n");
        printf("2. Yazar Sil\n");
        printf("3. Yazar Guncelle\n");
        printf("4. Yazarlari Listele\n");
        printf("0. Ana Menu\n");
        printf("Seciminiz: ");
        scanf("%d", &secim);

        switch (secim) {
            case 1: yazar_ekle(); break;
            case 2: yazar_sil(); break;
            case 3: yazar_guncelle(); break;
            case 4: yazarlari_yazdir(); break;
            case 0: printf("Ana menuye donuluyor...\n"); break;
            default: printf("Gecersiz secim!\n");
        }
    } while (secim != 0);
}

//ogrenci islemleri icin menu
void ogrenci_menu() {
    int secim;
    do {
        printf("\n--- OGRENCI ISLEMLERI ---\n");
        printf("1. Ogrenci Ekle\n");
        printf("2. Ogrenci Sil\n");
        printf("3. Ogrenci Guncelle\n");
        printf("4. Ogrencileri Listele\n");
        printf("5. Ogrenci Detay Goruntule\n");
        printf("6. Cezali Ogrencileri Listele\n");
        printf("7. Kitap Teslim Etmeyen Ogrenciler\n");
        printf("0. Ana Menu\n");
        printf("Seciminiz: ");
        scanf("%d", &secim);

        switch (secim) {
            case 1: ogrenci_ekle(); break;
            case 2: ogrenci_sil(); break;
            case 3: ogrenci_guncelle(); break;
            case 4: ogrencileri_yazdir(); break;
            case 5: ogrenci_detay(); break;
            case 6: cezali_ogrenciler(); break;
            case 7: teslim_etmeyenler(); break;
            case 0: printf("Ana menuye donuluyor...\n"); break;
            default: printf("Gecersiz secim!\n");
        }
    } while (secim != 0);
}

//kitap islemleri icin menu
void kitap_menu() {
    int secim;
    do {
        printf("\n--- KITAP ISLEMLERI ---\n");
        printf("1. Kitap Ekle\n");
        printf("2. Kitap Sil\n");
        printf("3. Kitap Guncelle\n");
        printf("4. Kitaplari Listele\n");
        printf("5. Raftaki Kitaplari Listele\n");
        printf("6. Kitap Detay Goruntule\n");
        printf("0. Ana Menu\n");
        printf("Seciminiz: ");
        scanf("%d", &secim);

        switch (secim) {
            case 1: kitap_ekle(); break;
            case 2: kitap_sil(); break;
            case 3: kitap_guncelle(); break;
            case 4: kitaplari_yazdir(); break;
            case 5: raftaki_kitaplar(); break;
            case 6: kitap_detay(); break;
            case 0: printf("Ana menuye donuluyor...\n"); break;
            default: printf("Gecersiz secim!\n");
        }
    } while (secim != 0);
}

//ogrenci dosyaya yazdirma fonksiyonu
void ogrenci_dosya_yaz() {
    FILE* dosya = fopen("Ogrenciler.csv", "w");
    if (!dosya) {
        printf("Ogrenciler.csv dosyasi acilamadi!\n");
        return;
    }

    Ogrenci* iter = ogrenciListesi;
    while (iter) {
        fprintf(dosya, "%s,%s,%s,%d\n",
                iter->ad, iter->soyad, iter->ogrenciNo, iter->puan);
        iter = iter->next;
    }

    fclose(dosya);
}

//yazar ekleme fonkisyonu
void yazar_ekle() {
    Yazar* yeniYazar = (Yazar*)malloc(sizeof(Yazar));
    printf("Yazar ID: ");
    scanf("%d", &yeniYazar->yazarID);
    
    if (yazarVarMi(yeniYazar->yazarID)) {
        printf("Bu ID'ye sahip yazar zaten var!\n");
        free(yeniYazar);
        return;
    }
    
    printf("Ad: ");
    scanf("%s", yeniYazar->ad);
    printf("Soyad: ");
    scanf("%s", yeniYazar->soyad);
    
    yeniYazar->next = yazarListesi;
    yazarListesi = yeniYazar;
    
    yazar_dosya_yaz();
    printf("Yazar basariyla eklendi.\n");
}

//yazar silme
void yazar_sil() {
    int id;
    printf("Silinecek yazar ID: ");
    scanf("%d", &id);
    
    Yazar* iter = yazarListesi;
    Yazar* onceki = NULL;
    
    while (iter && iter->yazarID != id) {
        onceki = iter;
        iter = iter->next;
    }
    
    if (!iter) {
        printf("Yazar bulunamadi.\n");
        return;
    }
    
    if (!onceki)
        yazarListesi = iter->next;
    else
        onceki->next = iter->next;
    
    free(iter);
    yazar_dosya_yaz();
    printf("Yazar silindi.\n");
}

//yazari guncelleme
void yazar_guncelle() {
    int id;
    printf("Guncellenecek yazar ID: ");
    scanf("%d", &id);
    
    Yazar* yazar = yazarListesi;
    while (yazar && yazar->yazarID != id)
        yazar = yazar->next;
    
    if (!yazar) {
        printf("Yazar bulunamadi.\n");
        return;
    }
    
    printf("Yeni Ad (onceki: %s): ", yazar->ad);
    scanf("%s", yazar->ad);
    printf("Yeni Soyad (onceki: %s): ", yazar->soyad);
    scanf("%s", yazar->soyad);
    
    yazar_dosya_yaz();
    printf("Yazar bilgileri guncellendi.\n");
}

//yazarlarin bilgilerini yazdirma fonksiyoun
void yazarlari_yazdir() {
    if (!yazarListesi) {
        printf("Kayitli yazar yok.\n");
        return;
    }
    
    printf("\n--- YAZARLAR ---\n");
    Yazar* iter = yazarListesi;
    while (iter) {
        printf("ID: %d | %s %s\n", iter->yazarID, iter->ad, iter->soyad);
        iter = iter->next;
    }
}

//yazar dosyasina yazma
void yazar_dosya_yaz() {
    FILE* dosya = fopen("yazarlar.csv", "w");
    if (!dosya) {
        printf("yazarlar.csv dosyasi acilamadi!\n");
        return;
    }
    
    Yazar* iter = yazarListesi;
    while (iter) {
        fprintf(dosya, "%d,%s,%s\n", iter->yazarID, iter->ad, iter->soyad);
        iter = iter->next;
    }
    
    fclose(dosya);
}

//ogrencinin nosunu alir ve guncelleme islemlerini yapar
void ogrenci_guncelle() {
    char numara[9];
    printf("Guncellenecek ogrenci no: ");
    scanf("%s", numara);
    
    Ogrenci* ogrenci = ogrenciListesi;
    while (ogrenci && strcmp(ogrenci->ogrenciNo, numara) != 0)
        ogrenci = ogrenci->next;
    
    if (!ogrenci) {
        printf("Ogrenci bulunamadi.\n");
        return;
    }
    
    printf("Yeni Ad (onceki: %s): ", ogrenci->ad);
    scanf("%s", ogrenci->ad);
    printf("Yeni Soyad (onceki: %s): ", ogrenci->soyad);
    scanf("%s", ogrenci->soyad);
    
    ogrenci_dosya_yaz();
    printf("Ogrenci bilgileri guncellendi.\n");
}

//ogrencilerin isimlerini nolarini ve puanlarini listeler
void ogrencileri_yazdir() {
    if (!ogrenciListesi) {
        printf("Kayitli ogrenci yok.\n");
        return;
    }
    
    printf("\n--- OGRENCILER ---\n");
    Ogrenci* iter = ogrenciListesi;
    while (iter) {
        printf("%s %s | No: %s | Puan: %d\n",
               iter->ad, iter->soyad, iter->ogrenciNo, iter->puan);
        iter = iter->next;
    }
}

//elde olmayan kitaplari odunc alanlari yazdirir
void teslim_etmeyenler() {
    FILE* dosya = fopen("Hareketler.csv", "r");
    if (!dosya) {
        printf("Hareketler.csv dosyasi bulunamadi.\n");
        return;
    }
    
    char satir[200];
    int bulundu = 0;
    
    while (fgets(satir, sizeof(satir), dosya)) {
        Hareket hareket;
        sscanf(satir, "%[^,],%[^,],%d,%s",
               hareket.ogrenciNo, hareket.etiketNo, &hareket.islemTuru, hareket.tarih);
        
        if (hareket.islemTuru == 0) { 
            FILE* dosya2 = fopen("Hareketler.csv", "r");
            char satir2[200];
            int teslimEdildi = 0;
            
            while (fgets(satir2, sizeof(satir2), dosya2)) {
                Hareket hareket2;
                sscanf(satir2, "%[^,],%[^,],%d,%s",
                       hareket2.ogrenciNo, hareket2.etiketNo, &hareket2.islemTuru, hareket2.tarih);
                
                if (strcmp(hareket.ogrenciNo, hareket2.ogrenciNo) == 0 &&
                    strcmp(hareket.etiketNo, hareket2.etiketNo) == 0 &&
                    hareket2.islemTuru == 1) {
                    teslimEdildi = 1;
                    break;
                }
            }
            fclose(dosya2);
            
            if (!teslimEdildi) {
                Ogrenci* ogrenci = ogrenciListesi;
                while (ogrenci && strcmp(ogrenci->ogrenciNo, hareket.ogrenciNo) != 0)
                    ogrenci = ogrenci->next;
                
                if (ogrenci) {
                    printf("%s %s | No: %s | Kitap: %s | Odunc Tarihi: %s\n",
                           ogrenci->ad, ogrenci->soyad, ogrenci->ogrenciNo, hareket.etiketNo, hareket.tarih);
                    bulundu = 1;
                }
            }
        }
    }
    
    fclose(dosya);
    
    if (!bulundu)
        printf("Teslim edilmeyen kitap yok.\n");
}

//eldeki kitaplari yazdirir
void raftaki_kitaplar() {
    if (!kitapListesi) {
        printf("Kayitli kitap yok.\n");
        return;
    }
    
    printf("\n--- RAFTAKI KITAPLAR ---\n");
    Kitap* kitap = kitapListesi;
    int bulundu = 0;
    
    while (kitap) {
        KitapOrnegi* ornek = kitap->ornekler;
        while (ornek) {
            if (strcmp(ornek->durum, "Rafta") == 0) {
                printf("Kitap: %s | ISBN: %s | Etiket: %s\n",
                       kitap->kitapAdi, kitap->isbn, ornek->etiketNo);
                bulundu = 1;
            }
            ornek = ornek->next;
        }
        kitap = kitap->next;
    }
    
    if (!bulundu)
        printf("Rafta kitap yok.\n");
}

//hareketlerin tamamini yazdirir
void tum_hareketler() {
    FILE* dosya = fopen("Hareketler.csv", "r");
    if (!dosya) {
        printf("Hareketler.csv dosyasi bulunamadi.\n");
        return;
    }
    
    printf("\n--- TUM KITAP HAREKETLERI ---\n");
    char satir[200];
    int sayac = 0;
    
    while (fgets(satir, sizeof(satir), dosya)) {
        Hareket hareket;
        sscanf(satir, "%[^,],%[^,],%d,%s",
               hareket.ogrenciNo, hareket.etiketNo, &hareket.islemTuru, hareket.tarih);
        
        Ogrenci* ogrenci = ogrenciListesi;
        while (ogrenci && strcmp(ogrenci->ogrenciNo, hareket.ogrenciNo) != 0)
            ogrenci = ogrenci->next;
        
        
        Kitap* kitap = kitapListesi;
        char* kitapAdi = "Bilinmiyor";
        while (kitap) {
            KitapOrnegi* ornek = kitap->ornekler;
            while (ornek) {
                if (strcmp(ornek->etiketNo, hareket.etiketNo) == 0) {
                    kitapAdi = kitap->kitapAdi;
                    break;
                }
                ornek = ornek->next;
            }
            if (strcmp(kitapAdi, "Bilinmiyor") != 0) break;
            kitap = kitap->next;
        }
        
        printf("%d. %s %s | Kitap: %s | %s | Tarih: %s\n",
               ++sayac,
               ogrenci ? ogrenci->ad : "?",
               ogrenci ? ogrenci->soyad : "?",
               kitapAdi,
               hareket.islemTuru == 0 ? "ODUNC ALDI" : "TESLIM ETTI",
               hareket.tarih);
    }
    
    if (sayac == 0)
        printf("Kayitli hareket yok.\n");
    
    fclose(dosya);
}

//ogrenci no uzerinden silme islemini gerceklestirir
void ogrenci_sil() {
    char numara[9];
    printf("Silinecek ogrenci no: ");
    scanf("%s", numara);
    
    Ogrenci* iter = ogrenciListesi;
    Ogrenci* onceki = NULL;
    
    while (iter && strcmp(iter->ogrenciNo, numara) != 0) {
        onceki = iter;
        iter = iter->next;
    }
    
    if (!iter) {
        printf("Ogrenci bulunamadi.\n");
        return;
    }
    
    FILE* dosya = fopen("Hareketler.csv", "r");
    if (dosya) {
        char satir[200];
        while (fgets(satir, sizeof(satir), dosya)) {
            Hareket hareket;
            sscanf(satir, "%[^,],%[^,],%d,%s",
                   hareket.ogrenciNo, hareket.etiketNo, &hareket.islemTuru, hareket.tarih);
            
            if (strcmp(hareket.ogrenciNo, numara) == 0 && hareket.islemTuru == 0) {
                FILE* dosya2 = fopen("Hareketler.csv", "r");
                char satir2[200];
                int teslimEdildi = 0;
                
                while (fgets(satir2, sizeof(satir2), dosya2)) {
                    Hareket hareket2;
                    sscanf(satir2, "%[^,],%[^,],%d,%s",
                           hareket2.ogrenciNo, hareket2.etiketNo, &hareket2.islemTuru, hareket2.tarih);
                    
                    if (strcmp(hareket.ogrenciNo, hareket2.ogrenciNo) == 0 &&
                        strcmp(hareket.etiketNo, hareket2.etiketNo) == 0 &&
                        hareket2.islemTuru == 1) {
                        teslimEdildi = 1;
                        break;
                    }
                }
                fclose(dosya2);
                
                if (!teslimEdildi) {
                    printf("Ogrencinin teslim etmedigi kitap var! Silinemez.\n");
                    fclose(dosya);
                    return;
                }
            }
        }
        fclose(dosya);
    }
    
    if (!onceki)
        ogrenciListesi = iter->next;
    else
        onceki->next = iter->next;
    
    if (iter->next)
        iter->next->prev = onceki;
    
    free(iter);
    ogrenci_dosya_yaz();
    printf("Ogrenci silindi.\n");
}