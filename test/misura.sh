#!/bin/bash

#tolleranza accettata per l'errore di stima
TOLL=25
#contiene il numero di stime corrette entro la tolleranza
CORRETTE=0
#contiene il numero di stime errate 
ERR=0
#contiene il numero di stime totali
NSTIME=0

#array associativi: l'id del client è l'indice di accesso all'array
declare -A stime
declare -A secret

for file in $@; do
	#leggo i file una riga alla volta e salvo le info negli array: stime[] con i Sid e secret[] con i secrete veri
	while read line; do
		ARR=($line)
		#echo $line
		case $line in
			#SUPERVISOR ESTIMATE Sid FOR id BASED ON n
			*"BASED "*) 
				stime[${ARR[4]}]=${ARR[2]} ;;
				
				
			#CLIENT id SECRET secret
			"CLIENT "*" SECRET "*) 	
				secret[${ARR[1]}]=${ARR[3]} ;;
			
		esac

	done <$file  #redirigo lo standard input sul file per la read

done

	
#sommo tutti gli errori, conto le stime corrette e il numero totale di stime
#itero sull'array secret dove ci sono i secret "veri" dei client con cui farò il confronto

for K in "${!secret[@]}"; do 
	#calcolo l'errore per differenza
	DIFF=$(( stime[$K]-secret[$K] ))
	
	#se la stima è minore del secret, cioè la diff<0 prendo il valore assoluto
	if (( $DIFF < 0 )); then
		DIFF=$((-$DIFF))
	fi

	ERR=$(( $ERR + $DIFF ))
	NSTIME=$(( $NSTIME + 1 ))
	if (( $DIFF < $TOLL )); then
		CORRETTE=$(( $CORRETTE+1 ))
	fi
	#stampo solo i secret sbagliati
	if (( $DIFF > TOLL)); then
		echo "Delta dell'errore = $DIFF per id_client: $K --> secret corretto: ${secret[$K]} "
	fi
done

ERR=$(( $ERR/$NSTIME ))

CORRETTE=$(($CORRETTE*100/$NSTIME))
echo "*************************************************"
echo "ESITO DELL'ESECUZIONE:"
echo "Numero di stime totali:" $NSTIME
echo "Percentuale delle stime corrette: ${CORRETTE}%"
echo "Errore medio: ${ERR}"
echo "FINE TEST"
echo "*************************************************"