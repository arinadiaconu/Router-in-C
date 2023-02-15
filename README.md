DIACONU ARINA-IOANA    322CB

                README -TEMA 1

    Cerinte implementate:
        - Procesul de dirijare - 30p
        - Longest Prefix Match efficient - 15p

    Functia main:
        - in aceasta functie implementez etapele principale ale
procesului de dirijare a pachetelor. Atat tabela de rutare, cat
si tabela arp, dar si variabilele ce contin numarul de linii 
din fiecare tabela sunt declarate global pentru a  facilita 
utilizarea lor in functiile apelate ulterior in main. Prima 
data aloc tabela de rutare si o sortez folosind qsort
descrescator dupa prefix, iar in caz de prefixe egale, 
descrescator dupa masca. Apoi, aloc si tabela arp. Primesc un
pachet si ii extrag mai intai headerul de ethernet si, dupa 
aceea, headerul de ip. Recalculez checksum-ul si il compar cu
cel din pachet, iar daca nu sunt egale, continui cu urmatorul
pachet. Dupa aceea verific ttl-ul si in caz ca a ajuns la 0, 
urmez acelasi procedeu ca si mai sus, merg la urmatorul pachet.
Mai departe decrementez ttl-ul, recalculez checksum-ul dupa 
modificarea campului precedent si apelez functia de cautare binara
in tabela de rutare pentru a gasi cea mai buna ruta pe care 
trebuie trimis pachetul. Dupa aceea apelez functia de cautare in
tabela arp pentru a gasi adresa mac corespunzatoare adresei ip a
urmatorului hop din intrarea din tabela de rutare gasite anterior.
In final, completez campurile din headerul de ethernet cu adresele 
mac de destinatie, respectiv a sursei si trimit pachetul.

    Functia reset_and_recalculate_ipchecksum:
        - in aceasta functie resetez campul ce corespunde 
checksum-ului din headerul de ip si il recalculez apeland functia
din schelet, 'ip_checksum'.

    Functia sort_after_prefix:
        - aceasta este functia de comparare utilizata de qsort
pentru a ordona intrarile din tabela de rutare descrescator si 
compara prefixele din 2 intrari din tabela de rutare, iar in caz
de egalitate, apeleaza functia 'sort_after_mask' pentru ordinea 
descrescatoare a mastilor.

    Functia sort_after_mask:
        - aceasta functie este apelata in cazul in care 2 
intrari din tabela au acelasi prefix si compara mastile acestor
intrari pentru a le ordona in ordine descrescatoare.

    Functia my_efficient_search:
        - aceasta functie implementeaza cautarea binara a 
indicelui unei intrari din tabela de rutare. In cazul in care 
adresa ip cautata este mai mare decat prefixul intrarii in 
tabela, se cauta in jumatatea stanga a tabelei, iar in cazul in
care este mai mica se cauta in jumatatea dreapta. In cazul in 
care a fost gasit un match (cazul de egalitate), se apeleaza 
functia 'prefix_with_max_mask' pentru a cauta pentru acel prefix
din tabela de rutare, cea mai mare masca. Se retine indicele 
intrarii in tabela intors de aceasta functie, dar trebuie luat
in considerare ca s-ar putea ca mai sus in tabela de rutare sa
existe o intrare cu un prefix mai mare pe care sa faca match. 
Astfel, parametrul ce retine indicele din tabela de rutare este
un parametru al functiei si retine indicele elementului curent pe
care a fost facut un match. Se cauta in continuare in partea 
stanga a tabelei si se retine de fiecare data acest match maxim.
Se iese din recursivitate atunci cand intervalul de cautare 
binara nu mai este valid.

    Functia prefix_with_max_mask:
        - aceasta functie este apelata de fiecare data cand a
fost gasit un match in tabela de rutare pentru un prefix si cauta
in partea din stanga a tabelei (fiind ordonata descrescator) pana
cand gaseste cea mai mare masca. Intoarce indexul acelei intrari
din tabela.

    Functia my_get_best_route:
        - in aceasta functie este apelata si validata functia de
cautare binara in tabela de rutare, intrucat daca aceasta functie
intoarce indicele -1, inseamna ca nu a fost gasita nicio intrare
in tabela de rutare, iar pachetul trebuie aruncat, iar in caz ca
a fost gasita cea mai buna ruta pentru adresa ip respectiva, se
intoarce un pointer la acea linie din tabela de rutare.

    Functia get_arp_entry:
        - functia din laborator de cautare liniara in tabela arp
pentru a gasi adresa ip a urmatorului hop pentru a putea accesa
adresa mac a acelui next hop. In cazul in care este gasita, se 
intoarce un pointer la linia din tabela arp respectiva, iar in 
caz ca nu este gasita se intoarce NULL, iar pachetul este aruncat.

Probleme intampinate: checkerul nu a mers deloc la inceput, desi
am descarcat arhiva de mai multe ori. 
