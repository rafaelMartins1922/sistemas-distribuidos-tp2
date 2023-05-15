set terminal png size 800,600
set output "saidas/tempos_graficos/grafico_tempos.png"

set datafile separator ","
datafile = "saidas/tempos/tempos_medios.csv"

set xlabel "NP/NC"
set ylabel "Tempo Médio"

set xtics rotate by -60
plot datafile every ::1::7 using 3:xtic(2) with linespoints title "N=1", \
     datafile every ::8::14 using 3:xtic(2) with linespoints title "N=10", \
     datafile every ::15::21 using 3:xtic(2) with linespoints title "N=100", \
     datafile every ::22::28 using 3:xtic(2) with linespoints title "N=1000"