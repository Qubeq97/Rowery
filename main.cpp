#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
using namespace std;


struct Osoba
{
	int ile_rowerow; // ile rower�w wybra�a dana osoba
	int* wybrane_rowery; // numery indeks�w rower�w wybranych przez dan� osob�
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
	// poni�sze zmienne do wykorzystania w algorytmie Hopcrofta - Karpa
	bool odwiedzony;
	int odleglosc;
	int skojarzenie; // numer roweru skojarzony z osob�, domy�lnie -1 - brak skojarzenia
};

struct Rower
{
	int skojarzenie; // numer osoby skojarzonej z rowerem, domy�lnie -1 - brak skojarzenia
	Rower()
	{
		skojarzenie = -1;
	}
};


struct KolejkaOsob
{
	Osoba** tab; // tablica os�b
	int rozmiar; // rozmiar kolejki
	int poczatek; // indeks pierwszegfo elementu kolejki
	int ile; // ile aktualnie element�w w kolejce
	KolejkaOsob(int rozmiar)
	{
		tab = new Osoba*[rozmiar]; // realizacja na tablicy o sta�ym rozmiarze - z g�ry znamy liczb� wszystkich os�b
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
			if (index >= rozmiar) // je�eli indeks danego elementu by�by wi�kszy ni� rozmiar
				index -= rozmiar; // to go zmniejszamy o rozmiar
								  // mo�e zaj�� taka sytuacja, �e:
								  // kolejka zaczyna si� dalej ni� pierwszy element tablicy
								  // wtedy mo�e by�,tak, �e dalszy element kolejki mo�e by� wcze�niejszym w tablicy
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
			poczatek++; // zmieniamy indeks pierwszego elementu kolejki na indeks nast�pnego elementu w tablicy
			if (poczatek >= rozmiar)
				poczatek = 0; // ale nie mo�e wykracza� poza tablic�
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
		osoby[i].odleglosc = -1; // najpierw oznaczmy odleg�o�� wszystkich os�b przez -1
	for (i = 0; i < ile_osob; i++)
	{
		if (osoby[i].skojarzenie == -1)
		{
			osoby[i].odleglosc = 0;
			q->Push(&osoby[i]); // wstawiamy wszystkie nieskojarzone osoby do kolejki i dajemy im odleg�o�� 0
		}
	}
	while (!q->czyPusta())
	{
		Osoba* x = q->Pop();
		for (i = 0; i < x->ile_rowerow; i++)
		{
			int skojarzenie = rowery[x->wybrane_rowery[i]].skojarzenie;
			// je�eli rower jest skojarzony z osob�, kt�ra ma przypisan� odleg�o�� -1:
			if (skojarzenie != -1 && osoby[skojarzenie].odleglosc == -1)
			{
				// to nadajemy tej osobie odleg�o�� o 1 wi�ksz� ni� osobie x
				osoby[skojarzenie].odleglosc = x->odleglosc + 1;
				q->Push(&osoby[skojarzenie]); // i wstawiamy j� do kolejki
			}
		}
	}
}

bool DFS(Osoba* osoby, Rower* rowery, int indeks_osoby)
{
	int i;
	// oznaczmy osob� o indeksie indeks_osoby jako odwiedzon�
	osoby[indeks_osoby].odwiedzony = true;
	for (i = 0; i < osoby[indeks_osoby].ile_rowerow; i++) // dla ka�dego roweru, kt�ry chce osoba o indeksie indeks_osoby
	{
		Rower* rower = &rowery[osoby[indeks_osoby].wybrane_rowery[i]];
		if (rower->skojarzenie == -1)
		{
			// je�eli rower nie jest skojarzony, kojarzymy osob� z rowerem
			osoby[indeks_osoby].skojarzenie = osoby[indeks_osoby].wybrane_rowery[i];
			rower->skojarzenie = indeks_osoby;
			return true;
		}
		else
		{
			int z = rower->skojarzenie; // osoba skojarzona z rowerem
			if (!osoby[z].odwiedzony && osoby[z].odleglosc == osoby[indeks_osoby].odleglosc + 1)
			{
				// je�eli osoba jeszcze nie jest odwiedzona i odleg�o�� osoby z jest o 1 wi�ksza od osoby indeks.osoby:
				if (DFS(osoby, rowery, z)) // je�eli DFS dla osoby z zwr�ci true:
				{
					// kojarzymy osob� indeks.osoby z rowerem
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
			if (DFS(osoby, rowery, i)) // wykonujemy w�a�ciwy DFS dla tych os�b, kt�re jeszcze nie s� skojarzone
				wynik = true;
		}
	return wynik; // je�eli kt�re� wywo�anie DFSu zwr�ci true, ta funkcja te� zwr�ci true
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
	int w, r; // w - ile os�b, r - ile rower�w
	scanf("%d", &w);
	scanf("%d", &r);
	Osoba* osoby = new Osoba[w];
	Rower* rowery = new Rower[r];
	int ile;
	int i, j;
	for (i = 0; i < w; i++)
	{
		scanf("%d", &ile); // ile dana osoba chce rower�w
		osoby[i].ile_rowerow = ile;
		osoby[i].wybrane_rowery = new int[ile];
		for (j = 0; j < ile; j++)
		{
			scanf("%d", &(osoby[i].wybrane_rowery[j]));
			(osoby[i].wybrane_rowery[j])--;
			// przedzia� indeks�w rower�w z wyj�cia wynosi <1, r>
			// a przedzia� indeks�w tablicy <0, r-1>
			// na starcie zmieniam numer indeksu
			// a algorytm Hopcrofta - Karpa operuje ju� na zmienionych indeksach
		}
	}
	HopcroftKarp(osoby, rowery, w);
	int licznik = 0; // to b�dzie licznik tych, kt�rzy dostali ten rower, co chcieli
	for (i = 0; i < w; i++) // iteruje po osobach
	{
		for (j = 0; j < osoby[i].ile_rowerow; j++) // a wewn�trz po ich wybranych rowerach
		{
			if (osoby[i].wybrane_rowery[j] == osoby[i].skojarzenie) // a je�eli osoba jest skojarzona z tym rowerem, co chcia�a
			{
				licznik++; // licznik tych os�b si� zwi�kszy
				break; // i program wyjdzie z wewn�trznej p�tli, aby iterowa� dalej po osobach
			}
		}
	}
	printf("%d", licznik);
	delete[] osoby;
	delete[] rowery;
	return 0;
}