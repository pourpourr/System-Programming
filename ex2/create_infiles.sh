#!/bin/bash
citizensFile=$1
dir=$2
numOfFiles=$3
if [[ -d "$dir" ]]; then

    echo "directory already exists exiting (and deleting it)..."
    rm -rf  "$dir"
    exit
fi

if [[ -f "$citizensFile" && "$numOfFiles" -gt 0 ]] ; then
    mkdir "$dir"
   
   declare -A countries
   cp "$citizensFile" "./$dir" 
   cd "$dir"
    while IFS= read -r line
    do

        read -ra words <<< "$line"

        if [ "${#words[@]}" -gt 0 ]; then 
            if [ ${countries[${words[3]}]+_} ]; then
                    
                if [[ "${countries[${words[3]}]}" -eq "$numOfFiles" ]]; then
                    countries[${words[3]}]=1
                else
                    let "countries[${words[3]}]=countries[${words[3]}]+1"
                fi
                count=${words[3]}
                cd "$count"
                printf "%s\n" "$line" >> "$count-${countries[${words[3]}]}.txt"
                cd ../
                
            else
                countries[${words[3]}]=1
                count=${words[3]}
                mkdir "$count"
                cd "$count"
                for ((i = 1 ; i <= $numOfFiles ; i++));
                do
                    echo > "$count-$i.txt"
                done
                printf "%s\n" "$line" >> "$count-1.txt"
                cd ../
                
                
            fi
        fi
        
    done < "$citizensFile"
    rm -f "$citizensFile"
    
else
    echo "Wrong File name or File does not exist or non positive numOfFiles given"
    echo "Give citizensFile dirName numOfFiles "
fi
