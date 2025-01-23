Lita Gabriel-Laurentiu 321CC
Tema 4 Protocoale de comunicatii

Pentru implementarea acestei teme am pornit de la laboratorul 9 cum a fost recomandat, doar ca in cadrul temei am adaugat 3 functii noi de request care acestea sunt GET,POST,DELETE care acestea adauga inaintea field-ului cookie si JWT Token cum a fost mentionat si in enuntul temei sub forma Authorization: Bearer %s.

Tema porneste in initializarea variabilelor care vor fii mereu tinute minte cookie, token si input pentru comanda curenta.
Codul ruleaza in ciclu infinit pana la introducerea comenzi exit.

In cadrul temei am folosit requesturile de tip get pentru enter_library, get_books, get_book si logout, in mare parte utilizarea functie de get a fost asemanatoare cu cea de la laborator diferenata a fost data de tratarea diferitelor cazuri la ce returneaza serverul si retinerea unor variabile in token pentru viitoare actiuni la functia enter_library.

Requesturile de tip post au fost folosite pentru register, login si add_book unde pentru acestea am creat obicte de tip json pentru adaugarea lor in server cu ajutorul biblioteci recomandate de enuntul temei.

Biblioteca de lucru cu tipul de date json a fost usor de folosit si de inteles am avut nevoie de aceasta biblioteca deorece serverul citiea fisier de tip json si creearea lor singuri sau cel putin formatul json ar fi fost destul de greu

Am creat o functie nou de request de tip delete care nu se regaseam in laborator pentru a sterge o carte din server.

Pentru mai multe detalii specifice si exacte am realizat si comentarii in cod.