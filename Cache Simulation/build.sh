make

echo "Main Test   "
./tlb_simulation --c 3000 --tlb-size 16 --tlb-latency 5 --memory-latency 50 --v2b-block-offset 1 --tf "traceFile" --blocksize 32 examples/Eingabe.csv
printf "\n"

echo "Test blockSize power of two "
./tlb_simulation --blocksize 7 examples/Eingabe.csv
printf "\n"

echo "help test  "
./tlb_simulation -h 
printf "\n"

echo "Edge case Test  "
./tlb_simulation -c 9223372036854775820 examples/Eingabe.csv
printf "\n"

echo "Fehler in der csv Test  "
./tlb_simulation examples/falscheEingabe.csv
printf "\n"