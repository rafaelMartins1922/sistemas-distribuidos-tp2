do for [i in "1 10 100 1000"] {
    do for [j in "1,1 1,2 1,4 1,8 2,1 4,1 8,1"] {

        np = j[1:1]
        nc = j[3:3]

        datafile = sprintf("saidas/ocupacoes/ocupacao_n%s_np%s_nc%s.csv", i, np, nc)
        outputfile = sprintf("saidas/ocupacoes_graficos/ocupacao_n%s_np%s_nc%s.png", i, np, nc)

        # Set up the plot
        set term png
        set output outputfile
        set title sprintf("Ocupação - n=%s np=%s nc=%s", i, np, nc)
        set xlabel "Tempo"
        set ylabel "Ocupação"

        # Plot the data
        plot datafile using 2:1 with points

        # Reset the output
        unset output
    }
}