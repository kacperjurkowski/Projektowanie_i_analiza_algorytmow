#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

template <typename T> //Template metody generowania danych
void tabGen(T* arr, int n, double percentSorted, bool reverseSort = false){
    random_device rd; //Inicjalizacja 
    mt19937 gen(rd()); //Inicjalizacja generatora liczb losowych mt19937
    uniform_int_distribution<> dis(0, 1000000); // Definicja zakresu losowanych wartości

    for(int i = 0; i < n; i++){ //Pętla przypisująca losowe wartości do tablicy
        arr[i] = dis(gen);
    }

    if(reverseSort){ // Jeżeli ma być odwrócona
        sort(arr, arr + n); // sort - Sortuje rosnąco, reverse - sortuje malejąco (gotowe metody z bibliteki "algorithm")
        reverse(arr, arr + n); // Odwracam kolejność ("arr" - wskazuje na pierwszy element | "arr + size" - wskazuje na ostatni indeks)
    } else if (percentSorted > 0){
        int elementsToSort = static_cast<int>(n * (percentSorted / 100.0)); // Ile elementów z początku ma być posortowanych
        sort(arr, arr + elementsToSort);
    }
}   

template <typename T> // Template quicksort
void quick_sort(T *tab, int start, int end){
    if(start >= end){ // Warunek bazowy rekurencji
        return;
    }

    int p = start - 1; 
    int q = end + 1; 
    T pivot = tab[(start + end) / 2]; // Wybór pivota jako elementu środkowego

    while(true){
        while(pivot > tab[++p]); // Szukanie elementu po lewej
        while(pivot < tab[--q]); // Szukanie elementu po prawej
        if(p <= q){
            swap(tab[p],tab[q]); //Podmianka
        } else {
            break;
        }
    }

    // Wywołanie rekurencji dla pozostałych podtablic
    if(q > start){
        quick_sort(tab, start, q);
    }
    
    if(p < end){
        quick_sort(tab, p, end);
    }
}

template <typename T> // Template insertion sort
void insertion_sort(T* tab, int n) {
    for (int i = 1; i < n; i++) {
        T key = tab[i]; // Wybieramy aktualny element
        int j = i - 1;
        // Przesuwamy elementy większe od klucza w prawo, robiąc dla niego miejsce
        while (j >= 0 && tab[j] > key) {
            tab[j + 1] = tab[j]; 
            j--;
        }
        tab[j + 1] = key; // Wstawiamy klucz w odpowiednie posortowane miejsce
    }
}

template <typename T> // Template kopiec
void heapify(T* tab, int n, int i) {
    int largest = i; // Zakładamy, że rodzic jest największy
    int l = 2 * i + 1; // Lewy syn
    int r = 2 * i + 2; // Prawy syn
    // Szukamy największego elementu wśród rodzica i synów
    if (l < n && tab[l] > tab[largest]) largest = l;
    if (r < n && tab[r] > tab[largest]) largest = r;
    // Jeśli rodzic nie jest największy, zamieniam go z większym synem
    if (largest != i) {
        swap(tab[i], tab[largest]);
        heapify(tab, n, largest); // Naprawiamy kopiec dalej w dół
    }
}

template <typename T> // Template sortowanie przez kopcowanie (heap sort)
void heap_sort(T* tab, int n) {
    // Budujemy kopiec początkowy
    for (int i = n / 2 - 1; i >= 0; i--) heapify(tab, n, i);
    // Wyjmujemy największy element z góry i naprawiamy kopiec dla reszty
    for (int i = n - 1; i > 0; i--) {
        swap(tab[0], tab[i]); // Największy trafia na koniec tablicy
        heapify(tab, i, 0); // Naprawiamy kopiec dla pozostałych elementów
    }
}

template <typename T> // Template sortowanie introspektywne
void intro_sort_util(T* tab, int n, int depth_limit) {
    if (n < 16) { // Poniżej 16 elementów - Instertion Sort
        insertion_sort(tab, n);
        return;
    }
    if (depth_limit == 0) { // Jeśli rekurencja jest za głęboka - Heap Sort
        heap_sort(tab, n);
        return;
    }
    
    T pivot = tab[n / 2]; // W innym wypadku dzielenie tablicy (jak w QuickSort)
    int i = 0, j = n - 1;
    while (i <= j) {
        while (tab[i] < pivot) i++;
        while (tab[j] > pivot) j--;
        if (i <= j) {
            swap(tab[i], tab[j]);
            i++; j--;
        }
    }
    // Rekurencyjne sortowanie lewej i prawej strony
    if (j > 0) intro_sort_util(tab, j + 1, depth_limit - 1);
    if (n > i) intro_sort_util(tab + i, n - i, depth_limit - 1);
}


template <typename T> // Template główny Intro Sort
void intro_sort(T* tab, int n) {
    intro_sort_util(tab, n, 2 * log2(n));
}

template <typename T> // Template łączenie tablic dla Merge Sorta
void merge(T* tab, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Kopiujemy dane do tymczasowych tablic pomocniczych
    T* L = new T[n1];
    T* R = new T[n2];

    for (int i = 0; i < n1; i++) L[i] = tab[left + i];
    for (int j = 0; j < n2; j++) R[j] = tab[mid + 1 + j];

    int i = 0, j = 0, k = left;
    
    // Porównujemy elementy z obu tablic i układamy je w kolejności w głównej tablicy
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) tab[k++] = L[i++];
        else tab[k++] = R[j++];
    }

    // Przepisujemy pozostałe elementy, jeśli któreś zostały
    while (i < n1) tab[k++] = L[i++];
    while (j < n2) tab[k++] = R[j++];

    delete[] L; // Usuwamy tablice pomocnicze L i R z pamięci
    delete[] R;
}

template <typename T> // Template Merge Sorta
void merge_sort(T* tab, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2; // Wyznaczamy środek

        // Dzielimy tablicę na pół aż do pojedynczych elementów
        merge_sort(tab, left, mid);
        merge_sort(tab, mid + 1, right);

        // Scalamy posortowane połówki w jedną całość
        merge(tab, left, mid, right);
    }
}

int main(){
    int sizes[] = {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}; //Tablica z rozmiarami
    double percentages[] = {0, 25, 50, 75, 95, 99, 99.7}; //Tablica z procentami
    
    //Tworzenie pilku csv. pod import
    ofstream file("wyniki_sortowania.csv");
    file << "Algorytm;Rozmiar;Typ_Danych;Sredni_Czas[us]\n";

    //Pętla w pętli przechodzące przez elementy tablicy sizes/percentages
    for(int n : sizes){
        for(double p : percentages){
            long long time_q = 0, time_m = 0, time_i = 0; //Zmienne przyjmujące pomiar czasu

            for(int i = 0; i < 100; i++){
                int* base_tab = new int[n];
                int* t1 = new int[n];
                int* t2 = new int[n];
                int* t3 = new int[n];

                tabGen(base_tab, n, p);
                for(int j=0; j<n; j++) {
                    t1[j] = t2[j] = t3[j] = base_tab[j];
                }
                
                // QuickSort
                auto start = chrono::high_resolution_clock::now();
                quick_sort(t1, 0, n-1);
                auto end = chrono::high_resolution_clock::now();
                time_q += chrono::duration_cast<std::chrono::microseconds>(end - start).count();

                // MergeSort
                start = chrono::high_resolution_clock::now();
                merge_sort(t2, 0, n-1);
                end = chrono::high_resolution_clock::now();
                time_m += chrono::duration_cast<std::chrono::microseconds>(end - start).count();

                // IntroSort
                start = chrono::high_resolution_clock::now();
                intro_sort(t3, n);
                end = chrono::high_resolution_clock::now();
                time_i += chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                
                // Zwalnianie pamięci
                delete[] base_tab;
                delete[] t1;
                delete[] t2; 
                delete[] t3;
            }
            string label = (p == 0) ? "Losowe" : to_string(p) + "%";
            file << "QuickSort;" << n << ";" << label << ";" << time_q / 100.0 << "\n";
            file << "MergeSort;" << n << ";" << label << ";" << time_m / 100.0 << "\n";
            file << "IntroSort;" << n << ";" << label << ";" << time_i / 100.0 << "\n";
        }
        long long rev_q = 0, rev_m = 0, rev_i = 0;

        for (int i = 0; i < 100; i++) {
            int* base_tab = new int[n];
            int* t1 = new int[n];
            int* t2 = new int[n];
            int* t3 = new int[n];

            tabGen(base_tab, n, 0, true); // true = odwrotna kolejność
            for (int j = 0; j < n; j++) {
                t1[j] = t2[j] = t3[j] = base_tab[j];
            }
            
            // QuickSort
            auto start = chrono::high_resolution_clock::now();
            quick_sort(t1, 0, n - 1);
            auto end = chrono::high_resolution_clock::now();
            rev_q += chrono::duration_cast<chrono::microseconds>(end - start).count();

            // MergeSort
            start = chrono::high_resolution_clock::now();
            merge_sort(t2, 0, n - 1);
            end = chrono::high_resolution_clock::now();
            rev_m += chrono::duration_cast<chrono::microseconds>(end - start).count();

            // IntroSort
            start = chrono::high_resolution_clock::now();
            intro_sort(t3, n);
            end = chrono::high_resolution_clock::now();
            rev_i += chrono::duration_cast<chrono::microseconds>(end - start).count();

            //Zwalnianie pamięci
            delete[] base_tab; 
            delete[] t1; 
            delete[] t2; 
            delete[] t3;
        }

        // Dane do pliku
        file << "QuickSort;" << n << ";Odwrotnie_Posortowane;" << rev_q / 100.0 << "\n";
        file << "MergeSort;" << n << ";Odwrotnie_Posortowane;" << rev_m / 100.0 << "\n";
        file << "IntroSort;" << n << ";Odwrotnie_Posortowane;" << rev_i / 100.0 << "\n";

        cout << "Zakonczono rozmiar: " << n << endl;
    }

    file.close(); // Zakończenie zapisu do pliku
    cout << "Dane zostaly zapisane do pliku wyniki_sortowania.csv" << endl;
    return 0;
}