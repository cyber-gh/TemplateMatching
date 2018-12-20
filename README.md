<h2>Program de criptare si decriptare a imaginilor bmp + template matching dupa cifre</h2>
Datele de intrare se citesc din fisier "input.txt" in urmatorul format:
<h4>Partea cu criptare (1-6 linie), Template matching 6-final</h4>
<ol>
<li>calea spre imaginea initiala
<li>calea unde va fi salvata imaginea criptata
<li>calea spre fisier txt cu cele 2 chei separate prin spatiu
<li>calea spre imaginea criptata
<li>calea unde va fi salvata imaginea decriptata
<li>calea spre fiser txt cu cele 2 chei separate prin spatiu
<li>calea spre imaginea asupra careia va fi aplicata template matching
<li>nr de imagini template (0<nr<=10)
<li><li>urmatoare nr linii contin drumul spre imaginile sablon
<li>calea unde va fi salvata imaginea finala dupa template matching

</ol>
A nu se lasa rand nou liber la sfarsit de input.txt
<h4>Workflow:</h4>
<ol>
<li>Se incarca o imagine initiala in memorie si se citesc cheile secrete
<li>Se generaza nr aleatorie, apoi permutarea si se permuta pixelii
<li>Se face xor dupa formula 
<li>Se salveaza in memoria externa
<li>Se incarca o imagine criptata in memorie si se citesc cheile
<li>Se genearaza tabloul de nr aleatorii, apoi permutarea si se face permutarea inversa a pixelilor
<li>Se aplica xor dupa formula
<li>Se salveaza in memoria externa
<li>Se calculeaza chi squared test cu rezultatele la consola
<li>Se incarca o imagine target
<li>Se incarca pe rand fiecare imagine sablon si se face template matching
<li>Rezultatele sunt concatenate ca detectii intr-un tabloul alocat dinamic
<li>Se sorteaza dupa acuratetea detectiilor in ordine descrescatoare
<li>Se elminia detectiile non-maxime
<li>Se coloreaza chenar la detectiile ramase
<li>Se salveaza imaginea target in memorie externa
</ol>

<h4>Explicatii</h4>
Pentru retinerea pixelilor s-a folosit o uniune cu o structura inauntru
Pentru retinerea imaginii s-a folosit o structura ce contine header dimensiuni si pointer la tabloul de pixelii cat si la matrice
Matricea de pixeli este compacta construita din tablou, acceasi memorie, nu se face copie
Ce se schimba in matrice se schimba in tablou si invers


<h4>Note:</h4>
<ul>
<li>Compilat si testat cu gcc pe Linux/Ubuntu 18
<li>A se adauga "-lm" la compilarea cu gcc in caz de eroare legata de "sqrt"
<li>Nu s-au facut validari ale input ului, a se introduce doar date corecte
<li>Memory leak check leak inclus in valgrind-out.txt, nu sunt memory leaks
<li>Comment-urile din cod sunt in format doxygen
</ul>