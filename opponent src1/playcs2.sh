
PORT_NUM=54300 
WINS=0

rm "me.txt"

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <player> <port>" >&2
  exit 1
fi



while [ $PORT_NUM -le 54400 ]
do
	./servt -p $PORT_NUM & sleep 0.1
	./$1 -p $PORT_NUM  & sleep 0.1 
	./agent   -p $PORT_NUM >> me.txt
   	PORT_NUM=$((PORT_NUM + 1))

done



while IFS='' read -r line || [[ -n "$line" ]]; do
	if [ "$line" = "WE WON!!" ]
	then
   		WINS=$((WINS + 1))
	fi
done < "me.txt"


echo $WINS
