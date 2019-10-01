# bucketSort

Paralelná implementácia algoritmu Bucket sort s využitím knižnice Open MPI

Vstupom je počísel, ktoré majú byť náhodne vygenerované a zoradené

Výstup na stdout se skladá z dvoch častí:
1. Jednotlivé načítané nezoradené hodnoty v jednom riadku oddelené medzerou (vypísané po načítaní prvým procesorom).
2. Jednotlivé zoradené hodnoty oddelené novým riadkom (od najmenšieho po najväčšie).

Príklad spustenia:
bash test.sh <pocet_cisel_ktore_maju_byt_zoradene>
