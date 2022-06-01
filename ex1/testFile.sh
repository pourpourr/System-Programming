#!/bin/bash
#!/bin/bash


filename1=$1
filename2=$2
numLines=$3
if [[ -f "$filename1" && -f "$filename2" && "$numLines" -gt 0 ]]; then
    declare -a virus
    mapfile -t virus < $filename1
    declare -a countries
    mapfile -t countries < $filename2
    declare -A id_info
    declare -A id_virus
    letters=(q w e r t y u i o p a s d f g h j k l z x c v b n m)
    ID_RANGE=10000
    echo   > "scriptaki.txt"
    echo "Be careful id are random 4 digit nums. If numLines > 10000 and duplicates==0 there will be infinite loop and manual stop will be required "

    if [[ $4 -eq  0 ]]; then #duplicates non allowed
        n=0
        while [[ "$numLines" -gt n ]];
        do
            id=$RANDOM
            let "id %=$ID_RANGE"
            flag=0
            while [ "$flag" -eq 0 ];
            do
                if [ ${id_virus[$id]+_} ]; then
                    id=$RANDOM
                    let "id %=$ID_RANGE"
                else
                    flag=1
                fi

            done
            id_virus[$id]=1
            vir=$RANDOM
            let "vir %=${#virus[@]}"
            cou=$RANDOM
            let "cou %=${#countries[@]}"
            name_Len=$RANDOM
            let "name_Len %=9"
            let "name_Len+=3"
            name=''
            for ((i = 0 ; i < $name_Len ; i++));
            do
                lett=$RANDOM
                let "lett%=26"
                name="${name}${letters[$lett]}"
            done
            name_Len=$RANDOM
            let "name_Len %=9"
            let "name_Len+=3"
            surname=''
            for ((i = 0 ; i < $name_Len ; i++));
            do
                lett=$RANDOM
                let "lett%=26"
                surname="${surname}${letters[$lett]}"
            done
            d=$RANDOM
            let "d%=30"
            let "d+=1"
            m=$RANDOM
            let "m%=12"
            let "m+=1"
            y=$RANDOM
            let "y%=21"
            let "y+=2000"
            date="$d-$m-$y"
            rand_num=$RANDOM
            let "rand_num%=2"
            age=$RANDOM
            let "age%=120"
            let "age+=1"
            if [ $rand_num -eq 1 ]; then
                printf "%d %s %s %s %d %s YES %s\n" "$id" "$name" "$surname" "${countries[$cou]}" "$age" "${virus[$vir]}" "$date" >> "scriptaki.txt"
            else
                rand_num=$RANDOM
                let "rand_num%=3"
                if [ $rand_num -eq 1 ]; then
                    printf "%d %s %s %s %d %s NO %s\n" "$id" "$name" "$surname" "${countries[$cou]}" "$age" "${virus[$vir]}" "$date" >> "scriptaki.txt"
                else
                    printf "%d %s %s %s %d %s NO\n" "$id" "$name" "$surname" "${countries[$cou]}" "$age" "${virus[$vir]}" >> "scriptaki.txt"
                fi
            fi
            n=$((n+1))
        done

    else
        n=0
        while [[ "$numLines" -gt n ]];
        do
            id=$RANDOM
            let "id %=$ID_RANGE"
            vir=$RANDOM
            let "vir %=${#virus[@]}"
            cou=$RANDOM
            let "cou %=${#countries[@]}"
            name_Len=$RANDOM
            let "name_Len %=9"
            let "name_Len+=3"
            name=''
            for ((i = 0 ; i < $name_Len ; i++));
            do
                lett=$RANDOM
                let "lett%=26"
                name="${name}${letters[$lett]}"
            done
            name_Len=$RANDOM
            let "name_Len %=9"
            let "name_Len+=3"
            surname=''
            for ((i = 0 ; i < $name_Len ; i++));
            do
                lett=$RANDOM
                let "lett%=26"
                surname="${surname}${letters[$lett]}"
            done
            d=$RANDOM
            let "d%=30"
            let "d+=1"
            m=$RANDOM
            let "m%=12"
            let "m+=1"
            y=$RANDOM
            let "y%=21"
            let "y+=2000"
            date="$d/$m/$y"
            rand_num=$RANDOM
            let "rand_num%=2"
            age=$RANDOM
            let "age%=120"
            let "age+=1"
            if [ $rand_num -eq 1 ]; then
                printf "%d %s %s %s %d %s YES %s\n" "$id" "$name" "$surname" "${countries[$cou]}" "$age" "${virus[$vir]}" "$date" >> "scriptaki.txt"
            else
                rand_num=$RANDOM
                let "rand_num%=3"
                if [ $rand_num -eq 1 ]; then
                    printf "%d %s %s %s %d %s NO %s\n" "$id" "$name" "$surname" "${countries[$cou]}" "$age" "${virus[$vir]}" "$date" >> "scriptaki.txt"
                else
                    printf "%d %s %s %s %d %s NO\n" "$id" "$name" "$surname" "${countries[$cou]}" "$age" "${virus[$vir]}" >> "scriptaki.txt"
                fi
            fi
            n=$((n+1))
        done
    fi

    duration=$(( SECONDS - start ))
else
    echo "Wrong File name or File does not exist or non positive numLines given"
    echo "Give virusesFile countriesFile numLines duplicatesAllowed "
    echo "0 for nonDuplicates any other positive num for duplicatesAllowed "
fi
