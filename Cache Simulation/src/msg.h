#ifndef USAGE_H
#define USAGE_H

// Declaration and definition of usage_msg
const char* usage_msg = 
    "Usage: %s [options] <Dateiname>\n"
    "Options:\n"
    "  -c, --cycles <Zahl>            Die Anzahl der Zyklen, die simuliert werden sollen.\n"
    "      --blocksize <Zahl>         Simuliert die Groesse eines Speicherblocks in Byte.\n"
    "      --v2b-block-offset <Zahl>  Gibt an, wie virtuelle Adressen zu physischen Adressen ubersetzt werden.\n"
    "      --tlb-size <Zahl>          Die Groesse des TLBs in Eintragen.\n"
    "      --tlb-latency <Zahl>       Die Latenzzeit des TLBs in Zyklen.\n"
    "      --memory-latency <Zahl>    Die Latenzzeit des Hauptspeichers in Zyklen.\n"
    "      --tf=<Dateiname>           Ausgabedatei fur ein Tracefile mit allen Signalen.\n"
    "  -h, --help                     Zeigt diese Hilfenachricht an.\n";

// Declaration and definition of help_msg
const char* help_msg = 
    "Usage: %s [options] <Dateiname>\n"
    "Options:\n"
    "  -c, --cycles <Zahl>            Die Anzahl der Zyklen, die simuliert werden sollen. Standard value 1000.\n"
    "      --blocksize <Zahl>         Simuliert die Groesse eines Speicherblocks in Byte. Standard value 32.\n"
    "      --v2b-block-offset <Zahl>  Gibt an, wie virtuelle Adressen zu physischen Adressen ubersetzt werden. Standard value 1.\n"
    "      --tlb-size <Zahl>          Die Groesse des TLBs in Eintragen. Standard value 16.\n"
    "      --tlb-latency <Zahl>       Die Latenzzeit des TLBs in Zyklen. Standard value 1.\n"
    "      --memory-latency <Zahl>    Die Latenzzeit des Hauptspeichers in Zyklen. Standard value 50.\n"
    "      --tf=<Dateiname>           Ausgabedatei fur ein Tracefile mit allen Signalen.\n"
    "  -h, --help                     Zeigt diese Hilfenachricht an.\n";


#endif 
