#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

template <typename T>
void tabGen(T* arr, int n, double percentSorted, bool reverseSort = false){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1000000);

    for(int i = 0; i < n; i++){
        arr[i] = dis(gen);
    }

    if(reverseSort){ // Jeżeli ma być odwrócona
        sort(arr, arr + n); // Sortuje rosnąco
        reverse(arr, arr + n); // Odwracam kolejność (arr - wskazuje na pierwszy element | arr + size - wskazuje na ostatni indeks)
    } else if (percentSorted > 0){
        int elementsToSort = static_cast<int>(n * (percentSorted / 100.0)); // Ile elementów z początku ma być posortowanych
        sort(arr, arr + elementsToSort);
    }
}   

template <typename T>
void quick_sort(T *tab, int start, int end){
    if(start >= end){ 
        return;
    }

    int p = start - 1; 
    int q = end + 1; 
    int pivot = tab[(start + end) / 2];

    while(true){
        while(pivot > tab[++p]);
        while(pivot < tab[--q]);
        if(p <= q){
            swap(tab[p],tab[q]);
        } else {
            break;
        }
    }

    if(q > start){
        quick_sort(tab, start, q);
    }
    
    if(p < end){
        quick_sort(tab, p, end);
    }
}

template <typename T>
void insertion_sort(T* tab, int n) {
    for (int i = 1; i < n; i++) {
        T key = tab[i];
        int j = i - 1;
        while (j >= 0 && tab[j] > key) {
            tab[j + 1] = tab[j];
            j--;
        }
        tab[j + 1] = key;
    }
}

template <typename T>
void heapify(T* tab, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && tab[l] > tab[largest]) largest = l;
    if (r < n && tab[r] > tab[largest]) largest = r;
    if (largest != i) {
        swap(tab[i], tab[largest]);
        heapify(tab, n, largest);
    }
}

template <typename T>
void heap_sort(T* tab, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) heapify(tab, n, i);
    for (int i = n - 1; i > 0; i--) {
        swap(tab[0], tab[i]);
        heapify(tab, i, 0);
    }
}

template <typename T>
void intro_sort_util(T* tab, int n, int depth_limit) {
    if (n < 16) {
        insertion_sort(tab, n);
        return;
    }
    if (depth_limit == 0) {
        heap_sort(tab, n);
        return;
    }
    // Uproszczona partycja dla IntroSort
    T pivot = tab[n / 2];
    int i = 0, j = n - 1;
    while (i <= j) {
        while (tab[i] < pivot) i++;
        while (tab[j] > pivot) j--;
        if (i <= j) {
            swap(tab[i], tab[j]);
            i++; j--;
        }
    }
    if (j > 0) intro_sort_util(tab, j + 1, depth_limit - 1);
    if (n > i) intro_sort_util(tab + i, n - i, depth_limit - 1);
}

template <typename T>
void intro_sort(T* tab, int n) {
    intro_sort_util(tab, n, 2 * log2(n));
}

template <typename T>
void merge(T* tab, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Tworzymy tymczasowe tablice (na stercie, bo mogą być ogromne!)
    T* L = new T[n1];
    T* R = new T[n2];

    for (int i = 0; i < n1; i++) L[i] = tab[left + i];
    for (int j = 0; j < n2; j++) R[j] = tab[mid + 1 + j];

    int i = 0, j = 0, k = left;
    
    // Scalanie dwóch tablic z powrotem do 'tab'
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) tab[k++] = L[i++];
        else tab[k++] = R[j++];
    }

    while (i < n1) tab[k++] = L[i++];
    while (j < n2) tab[k++] = R[j++];

    delete[] L;
    delete[] R;
}

template <typename T>
void merge_sort(T* tab, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(tab, left, mid);
        merge_sort(tab, mid + 1, right);

        merge(tab, left, mid, right);
    }
}

int main(){
    int sizes[] = {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    double percentages[] = {0, 25, 50, 75, 95, 99, 99.7};
    
    ofstream file("wyniki_sortowania.csv");
    file << "Algorytm;Rozmiar;Typ_Danych;Sredni_Czas[us]\n";

    for(int n : sizes){
        for(double p : percentages){
            long long time_q = 0, time_m = 0, time_i = 0;

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

            delete[] base_tab; 
            delete[] t1; 
            delete[] t2; 
            delete[] t3;
        }

        file << "QuickSort;" << n << ";Odwrotnie_Posortowane;" << rev_q / 100.0 << "\n";
        file << "MergeSort;" << n << ";Odwrotnie_Posortowane;" << rev_m / 100.0 << "\n";
        file << "IntroSort;" << n << ";Odwrotnie_Posortowane;" << rev_i / 100.0 << "\n";

        cout << "Zakonczono rozmiar: " << n << endl;
    }

    file.close();
    cout << "Dane zostaly zapisane do pliku wyniki_sortowania.csv" << endl;
    return 0;
}