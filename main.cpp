#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
using namespace std;


struct Osoba
{
	int ile_rowerow; // ile rowerów wybra³a dana osoba
	int* wybrane_rowery; // numery indeksów rowerów wybranych przez dan¹ osobê
	Osoba()
	{
		wybrane_rowery = nullptr;
		skojarzenie = -1;
		odleglosc = -1;
	}
	~Osoba()
	{
		delete[] wybrane_rowery;
	}
	// poni¿sze zmienne do wykorzystania w algorytmie Hopcrofta - Karpa
	bool odwiedzony;
	int odleglosc;
	int skojarzenie; // numer roweru skojarzony z osob¹, domyœlnie -1 - brak skojarzenia
};

struct Rower
{
	int skojarzenie; // numer osoby skojarzonej z rowerem, domyœlnie -1 - brak skojarzenia
	Rower()
	{
		skojarzenie = -1;
	}
};


struct KolejkaOsob
{
	Osoba** tab; // tablica osób
	int rozmiar; // rozmiar kolejki
	int poczatek; // indeks pierwszegfo elementu kolejki
	int ile; // ile aktualnie elementów w kolejce
	KolejkaOsob(int rozmiar)
	{
		tab = new Osoba*[rozmiar]; // realizacja na tablicy o sta³ym rozmiarze - z góry znamy liczbê wszystkich osób
		this->rozmiar = rozmiar;
		for (int i = 0; i < rozmiar; i++)
			tab[i] = nullptr;
		poczatek = 0;
		ile = 0;
	}
	~KolejkaOsob()
	{
		while (ile > 0)
			Pop();
		delete[] tab;
	}
	void Push(Osoba* osoba)
	{
		if (ile != rozmiar)
		{
			int index = poczatek + ile;
			if (index >= rozmiar) // je¿eli indeks danego elementu by³by wiêkszy ni¿ rozmiar
				index -= rozmiar; // to go zmniejszamy o rozmiar
								  // mo¿e zajœæ taka sytuacja, ¿e:
								  // kolejka zaczyna siê dalej ni¿ pierwszy element tablicy
								  // wtedy mo¿e byæ,tak, ¿e dalszy element kolejki mo¿e byæ wczeœniejszym w tablicy
			tab[index] = osoba;
			ile++;
		}
	}
	Osoba* Pop()
	{
		if (ile == 0)
			return nullptr;
		else
		{
			Osoba* wynik = tab[poczatek];
			tab[poczatek] = nullptr;
			ile--;
			poczatek++; // zmieniamy indeks pierwszego elementu kolejki na indeks nastêpnego elementu w tablicy
			if (poczatek >= rozmiar)
				poczatek = 0; // ale nie mo¿e wykraczaæ poza tablicê
			return wynik;
		}
	}
	bool czyPusta()
	{
		return (ile == 0);
	}
};



void BFS(Osoba* osoby, Rower* rowery, int ile_osob, KolejkaOsob* q)
{
	int i;
	for (i = 0; i < ile_osob; i++)
		osoby[i].odleglosc = -1; // najpierw oznaczmy odleg³oœæ wszystkich osób przez -1
	for (i = 0; i < ile_osob; i++)
	{
		if (osoby[i].skojarzenie == -1)
		{
			osoby[i].odleglosc = 0;
			q->Push(&osoby[i]); // wstawiamy wszystkie nieskojarzone osoby do kolejki i dajemy im odleg³oœæ 0
		}
	}
	while (!q->czyPusta())
	{
		Osoba* x = q->Pop();
		for (i = 0; i < x->ile_rowerow; i++)
		{
			int skojarzenie = rowery[x->wybrane_rowery[i]].skojarzenie;
			// je¿eli rower jest skojarzony z osob¹, która ma przypisan¹ odleg³oœæ -1:
			if (skojarzenie != -1 && osoby[skojarzenie].odleglosc == -1)
			{
				// to nadajemy tej osobie odleg³oœæ o 1 wiêksz¹ ni¿ osobie x
				osoby[skojarzenie].odleglosc = x->odleglosc + 1;
				q->Push(&osoby[skojarzenie]); // i wstawiamy j¹ do kolejki
			}
		}
	}
}

bool DFS(Osoba* osoby, Rower* rowery, int indeks_osoby)
{
	int i;
	// oznaczmy osobê o indeksie indeks_osoby jako odwiedzon¹
	osoby[indeks_osoby].odwiedzony = true;
	for (i = 0; i < osoby[indeks_osoby].ile_rowerow; i++) // dla ka¿dego roweru, który chce osoba o indeksie indeks_osoby
	{
		Rower* rower = &rowery[osoby[indeks_osoby].wybrane_rowery[i]];
		if (rower->skojarzenie == -1)
		{
			// je¿eli rower nie jest skojarzony, kojarzymy osobê z rowerem
			osoby[indeks_osoby].skojarzenie = osoby[indeks_osoby].wybrane_rowery[i];
			rower->skojarzenie = indeks_osoby;
			return true;
		}
		else
		{
			int z = rower->skojarzenie; // osoba skojarzona z rowerem
			if (!osoby[z].odwiedzony && osoby[z].odleglosc == osoby[indeks_osoby].odleglosc + 1)
			{
				// je¿eli osoba jeszcze nie jest odwiedzona i odleg³oœæ osoby z jest o 1 wiêksza od osoby indeks.osoby:
				if (DFS(osoby, rowery, z)) // je¿eli DFS dla osoby z zwróci true:
				{
					// kojarzymy osobê indeks.osoby z rowerem
					osoby[indeks_osoby].skojarzenie = osoby[indeks_osoby].wybrane_rowery[i];
					rower->skojarzenie = indeks_osoby;
					return true;
				}
			}
		}
	}
	return false;
}

bool wykonajDFS(Osoba* osoby, Rower* rowery, int ile_osob)
{
	int i;
	bool wynik = false;
	for (i = 0; i < ile_osob; i++)
		osoby[i].odwiedzony = false; // najpierw oznaczmy wszystkie osoby jako nieodwiedzone
	for (i = 0; i < ile_osob; i++)
		if (osoby[i].skojarzenie == -1)
		{
			if (DFS(osoby, rowery, i)) // wykonujemy w³aœciwy DFS dla tych osób, które jeszcze nie s¹ skojarzone
				wynik = true;
		}
	return wynik; // je¿eli któreœ wywo³anie DFSu zwróci true, ta funkcja te¿ zwróci true
}




void HopcroftKarp(Osoba* osoby, Rower* rowery, int ile_osob)
{
	KolejkaOsob* kolejka = new KolejkaOsob(ile_osob);
	bool result = true;
	do
	{
		BFS(osoby, rowery, ile_osob, kolejka);
		result = wykonajDFS(osoby, rowery, ile_osob);
	} while (result);
	delete kolejka;
}

int main()
{
	int w, r; // w - ile osób, r - ile rowerów
	scanf("%d", &w);
	scanf("%d", &r);
	Osoba* osoby = new Osoba[w];
	Rower* rowery = new Rower[r];
	int ile;
	int i, j;
	for (i = 0; i < w; i++)
	{
		scanf("%d", &ile); // ile dana osoba chce rowerów
		osoby[i].ile_rowerow = ile;
		osoby[i].wybrane_rowery = new int[ile];
		for (j = 0; j < ile; j++)
		{
			scanf("%d", &(osoby[i].wybrane_rowery[j]));
			(osoby[i].wybrane_rowery[j])--;
			// przedzia³ indeksów rowerów z wyjœcia wynosi <1, r>
			// a przedzia³ indeksów tablicy <0, r-1>
			// na starcie zmieniam numer indeksu
			// a algorytm Hopcrofta - Karpa operuje ju¿ na zmienionych indeksach
		}
	}
	HopcroftKarp(osoby, rowery, w);
	int licznik = 0; // to bêdzie licznik tych, którzy dostali ten rower, co chcieli
	for (i = 0; i < w; i++) // iteruje po osobach
	{
		for (j = 0; j < osoby[i].ile_rowerow; j++) // a wewn¹trz po ich wybranych rowerach
		{
			if (osoby[i].wybrane_rowery[j] == osoby[i].skojarzenie) // a je¿eli osoba jest skojarzona z tym rowerem, co chcia³a
			{
				licznik++; // licznik tych osób siê zwiêkszy
				break; // i program wyjdzie z wewnêtrznej pêtli, aby iterowaæ dalej po osobach
			}
		}
	}
	printf("%d", licznik);
	delete[] osoby;
	delete[] rowery;
	return 0;
}