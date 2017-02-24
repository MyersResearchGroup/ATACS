alias Top='top -d 1 -n 1'
OS_TYPE=`uname    -s`
if [ ${OS_TYPE} = HP-UX ]; then\
  alias free='(set file=/tmp/${USER}.$$;Top -f $file;cat $file;rm -f $file)';\
elif [ ${OS_TYPE} != Linux ]; then\
  alias free=Top;\
fi
memory=`free | awk '/Mem/ {print $2}' | tr K k  2>&1`
ulimit -Sv ${memory} > /dev/null 2>&1 || ulimit -Sd ${memory}
