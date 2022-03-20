#!/bin/bash
BUILD=./build
CLIENT=./client
OUTCLI=output_client.log
OUTSUP=output_supe.log
ERRSUP=err_s.log

K=8
P=5
W=20
C=10

cd $BUILD

echo "Eseguo il supervisore"

#eseguo supervisor e ridirigo stdou e stderr su due file diversi
./supervisor $K  1>>$OUTSUP &
PID=$!

#supervisor attende due secondi e lancia 20 client
sleep 2

#elimino vecchi file per non avere dati sui vecchi client nel file output_client
rm -f $OUTCLI


echo "Eseguo i 20 client"
for((i=0;i<$C;i++)); do
	$CLIENT $P $K $W 1>>$OUTCLI &
	$CLIENT $P $K $W 1>>$OUTCLI &
	sleep 1
done


echo "Invio segnali"

	for((i=0;i<6;i++)); do
		echo "E' arrivato il segnale numero $i"
		kill -2 $PID
		sleep 10
	done

echo "Terminazione supervisor"

echo "*************************************************"
echo "TABELLA DELLE STIME:"
	kill -2 $PID
	sleep 0.5
	kill -2 $PID
	
	
	
echo "*************************************************"

bash ../test/misura.sh $OUTSUP $OUTCLI